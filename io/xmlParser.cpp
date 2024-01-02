


#include "xmlParser.h"
#include <iostream>


std::map<std::string, pt::ptree > compDS; // store component name and its pointer.
std::map<std::string, std::string> new_compDS; // map the component name and its new name.
std::map<std::string, std::string> replaceMAP;
std::map<std::string, std::string> initial_strings; //
std::map<std::string, std::string> forbidden_strings; //

unsigned int xmlParser::get_number_components()
{
	return this->number_components;
}

std::string xmlParser::isWhat(std::string component_name, std::string element)
{
	for(std::list<component>::iterator comp_it = components.begin(); comp_it != components.end(); comp_it++)
	{
		if(comp_it->comp_name == component_name)
		{
			for(std::list<std::string>::iterator lab_it = comp_it->labels.begin(); lab_it != comp_it->labels.end(); lab_it++)
			{
//				std::cout<<*lab_it<<std::endl;
				if(*lab_it == element)
					return "label";
			}
			for(std::list<std::string>::iterator local_var_it = comp_it->local_vars.begin(); local_var_it != comp_it->local_vars.end(); local_var_it++)
			{
				if(*local_var_it == element)
					return "variable";
			}
			for(std::list<std::string>::iterator var_it = comp_it->vars.begin(); var_it != comp_it->vars.end(); var_it++)
			{
				if(*var_it == element)
					return "variable";
			}
		}
	}
	return 0;
}

void xmlParser::modify_xml(pt::ptree &root, std::string component_name, std::map<std::string, std::string> replaceMAP)
{
	boost::property_tree::ptree& component = root.get_child("sspaceex.component");

	std::cout<<"Printing MAP: "<<std::endl;
	for(std::map<std::string, std::string>::iterator map_it = replaceMAP.begin(); map_it != replaceMAP.end(); map_it++)
	{
		std::cout<<map_it->first<<"\t"<<map_it->second<<std::endl;
		std::string type = isWhat(component_name, map_it->first);
		std::cout<<type<<std::endl;
		if(type == "label")
		{
			for(auto it = component.begin(); it != component.end(); ++it)
			{
				if (it->first == "param" && it->second.get("<xmlattr>.name", "") == map_it->first)
				{
					it->second.put("<xmlattr>.name", map_it->second);
				}
				if (it->first == "transition")
				{
					if(it->second.get<std::string>("label") == map_it->first)
					{
						it->second.put("label", map_it->second);
					}
				}
			}
		}
		if(type == "variable")
		{
			for(auto it = component.begin(); it != component.end(); ++it)
			{
				if (it->first == "param" && it->second.get("<xmlattr>.name", "") == map_it->first)
				{
					it->second.put("<xmlattr>.name", map_it->second);
				}
				if(it->first == "location")
				{
					if(it->second.find("invariant")!= it->second.not_found())
					{
						std::cout<<it->second.get<std::string>("invariant")<<std::endl;
						std::string changeable_str = it->second.get<std::string>("invariant");
						boost::replace_all(changeable_str, map_it->first, map_it->second);
						std::cout<<changeable_str<<std::endl;
						it->second.put("invariant", changeable_str);
					}
					if(it->second.find("flow")!= it->second.not_found())
					{
						std::cout<<it->second.get<std::string>("flow")<<std::endl;
						std::string changeable_str = it->second.get<std::string>("flow");
						boost::replace_all(changeable_str, map_it->first, map_it->second);
						std::cout<<changeable_str<<std::endl;
						it->second.put("flow", changeable_str);
					}
				}
				if (it->first == "transition")
				{
					if(it->second.find("guard")!= it->second.not_found())
					{
						std::cout<<it->second.get<std::string>("guard")<<std::endl;
						std::string changeable_str = it->second.get<std::string>("guard");
						boost::replace_all(changeable_str, map_it->first, map_it->second);
						std::cout<<changeable_str<<std::endl;
						it->second.put("guard", changeable_str);
					}
					if(it->second.find("assignment")!= it->second.not_found())
					{
						std::cout<<it->second.get<std::string>("assignment")<<std::endl;
						std::string changeable_str = it->second.get<std::string>("assignment");
						boost::replace_all(changeable_str, map_it->first, map_it->second);
						std::cout<<changeable_str<<std::endl;
						it->second.put("assignment", changeable_str);
					}
				}
			}
		}
	}
}

void xmlParser::find_vars_labels(pt::ptree root, std::list<std::string> &vars, std::list<std::string> &local_vars, std::list<std::string> &labels)
{
	boost::property_tree::ptree& component = root.get_child("sspaceex.component");
	for (auto it = component.begin(); it != component.end(); ++it)
	{
		if(it->first == "param" && it->second.get("<xmlattr>.type", "") == "real")
		{
			std::cout<<"system variables: "<<it->second.get("<xmlattr>.name","")<<std::endl;
			if(it->second.get("<xmlattr>.local", "") == "true")
				local_vars.push_back(it->second.get("<xmlattr>.name",""));
			else
				vars.push_back(it->second.get("<xmlattr>.name",""));
		}
		if(it->first == "param" && it->second.get("<xmlattr>.type", "") == "label")
		{
			std::cout<<"system labels: "<<it->second.get("<xmlattr>.name","")<<std::endl;
			labels.push_back(it->second.get("<xmlattr>.name",""));
		}
	}
}

std::string xmlParser::retrieve_string(std::string line, std::string first_char, std::string sec_char)
{
	std::string::size_type start_position = 0;
	std::string::size_type end_position = 0;
	std::string str;

	start_position = line.find(first_char);
	if (start_position != std::string::npos)
	{
	  ++start_position; // start after the double quotes.
	  // look for end position;
	  end_position = line.find(sec_char, start_position + 1);
	  if (end_position != std::string::npos)
	  {
		 str = line.substr(start_position, end_position - start_position);
	  }
	}
	return str;
}

void xmlParser::parse_initial(std::string line)
{
	std::cout<<"Initial string: "<<line<<std::endl;
	std::string initial;
	initial = retrieve_string(line, "\"", "\"");
	std::cout<<initial<<std::endl;

	typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
	boost::char_separator<char> sep("&");
	tokenizer tokens(initial, sep);

	std::list<std::string> all_args, duplicate_args;
	std::list<std::string> all_comps;
	for (tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter)
	{
		all_args.push_back((std::string) *tok_iter);
		std::cout<<*tok_iter<<std::endl;
		std::string comp_name = retrieve_string(*tok_iter, "(", ")");
		if(!comp_name.empty())
			all_comps.push_back(comp_name);
	}
	std::cout<<"Debug: "<<std::endl;
	for(auto comps : all_comps)
	{
		std::cout<<comps<<std::endl;
	}
	// exit(0);
	duplicate_args = all_args;
//	std::list<std::string> initial_strings;
	for(std::list<std::string>::iterator it_comps = all_comps.begin(); it_comps != all_comps.end(); it_comps++)
	{
		std::cout<<*it_comps<<std::endl;
		std::string comp = *it_comps;
		std::string initial_str = "";
		std::string comp_1 = comp + ".";
		std::string comp_2 = comp + ")";
		std::cout<<"Base component: "<<new_compDS[*it_comps]<<std::endl;
		for(std::list<std::string>::iterator it_args = all_args.begin(); it_args != all_args.end(); it_args++)
		{
			std::string args = *it_args;
//			std::cout<<"args.find(comp)"<<std::endl;
//			std::cout<<"args: "<<args<<std::endl;
//			std::cout<<"comp: "<<comp<<std::endl;
			if(args.find(comp_1) != std::string::npos || args.find(comp_2) != std::string::npos) // Quick fix for the NRS and FDDI. NOt works for all the model.
			{
				std::cout<<args<<std::endl;
				duplicate_args.remove(args);
				boost::replace_all(args, ".", "_");
				std::string comp_name = retrieve_string(args, "(", ")");
				if(!comp_name.empty())
					boost::replace_all(args, comp_name, "");
				initial_str += args + " & ";
			}
		}

		initial_strings.insert(std::pair<std::string, std::string>(comp, initial_str));
	}
	for(std::list<std::string>::iterator it_comps = all_comps.begin(); it_comps != all_comps.end(); it_comps++)
	{
		std::cout<<"AK: "<<*it_comps<<std::endl;
//		std::string comp = *it_comps;
		std::string initial_str = initial_strings[*it_comps];
		for(std::list<std::string>::iterator it_args = duplicate_args.begin(); it_args != duplicate_args.end(); it_args++)
		{
//			std::cout<<*it_args<<std::endl;
			if(*it_args != duplicate_args.back())
				initial_str += *it_args + " & ";
			else
				initial_str += *it_args;
		}
		std::cout<<initial_str<<std::endl;
//		initial_strings.insert(std::pair<std::string, std::string>(comp, initial_str));
		initial_strings[*it_comps] = initial_str;
	}

}

void xmlParser::parse_forbidden(std::string line)
{
	std::cout<<"Forbidden string: "<<line<<std::endl;
	std::string initial;
	initial = retrieve_string(line, "\"", "\"");
	std::cout<<initial<<std::endl;

	typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
	boost::char_separator<char> sep("&");
	tokenizer tokens(initial, sep);

	std::list<std::string> all_args, duplicate_args;
	std::list<std::string> all_comps;
	for (tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter)
	{
		all_args.push_back((std::string) *tok_iter);
		std::cout<<*tok_iter<<std::endl;
		std::string comp_name = retrieve_string(*tok_iter, "(", ")");
		if(!comp_name.empty())
			all_comps.push_back(comp_name);
	}
	duplicate_args = all_args;
//	std::list<std::string> initial_strings;
	for(std::list<std::string>::iterator it_comps = all_comps.begin(); it_comps != all_comps.end(); it_comps++)
	{
		std::cout<<*it_comps<<std::endl;
		std::string comp = *it_comps; //Quick fix the NRS and FDDI. NOt works for all the models.
		std::string forbid_str = "";
		std::string comp_1 = comp + ")";
		std::cout<<"Base component: "<<new_compDS[*it_comps]<<std::endl;
		for(std::list<std::string>::iterator it_args = all_args.begin(); it_args != all_args.end(); it_args++)
		{
			std::string args = *it_args;
			if(args.find(comp_1) != std::string::npos)
			{
				std::cout<<args<<std::endl;
				duplicate_args.remove(args);
				boost::replace_all(args, ".", "_");
				std::string comp_name = retrieve_string(args, "(", ")");
				if(!comp_name.empty())
					boost::replace_all(args, comp_name, "");
				forbid_str += args; //Consider only one location exist per component. No local variable involvement.
			}
		}

		forbidden_strings.insert(std::pair<std::string, std::string>(comp, forbid_str));
	}
	for(std::list<std::string>::iterator it_comps = all_comps.begin(); it_comps != all_comps.end(); it_comps++)
	{
//		std::cout<<forbidden_strings[*it_comps]<<std::endl;
		std::string forbid_str = forbidden_strings[*it_comps];
		if(duplicate_args.empty())

		for(std::list<std::string>::iterator it_args = duplicate_args.begin(); it_args != duplicate_args.end(); it_args++)
		{
//			std::cout<<*it_args<<std::endl;
			if(*it_args != duplicate_args.back())
				forbid_str += *it_args + " & ";
			else
				forbid_str += *it_args + "\n";
		}
		std::cout<<forbid_str<<std::endl;
		forbidden_strings.insert(std::pair<std::string, std::string>(*it_comps, forbid_str));
	}

}

void xmlParser::split_CHA()

{
	std::string xml_content;
	std::getline(std::ifstream(this->xml_file), xml_content, '\0');
	// std::cout<<xml_content<<std::endl;

	// Create an empty property tree object
	pt :: ptree xml_tree;

	try {
		std :: stringstream ss;
		ss << xml_content;
		read_xml(ss, xml_tree, boost::property_tree::xml_parser::trim_whitespace);
	} catch (pt :: xml_parser_error &e) {
		std::cout << "Failed to parse the xml string." << e.what();
	} catch (...) {
		std::cout << "Failed !!!";
	}
	unsigned int i = 1;
	std::string name, bind_comp;
	for(const pt::ptree::value_type& p : xml_tree.get_child("sspaceex"))
	{
//				 std::cout<<p.first<<std::endl;
		if(p.first == "component")
		{
			pt::ptree sub_ptr, new_tree, rt_tree;
			std::tie(name, sub_ptr) = p;
			// std::cout <<name<<std::endl;
			sub_ptr.put("<xmlattr>.id", p.second.get<std::string>("<xmlattr>.id"));
			new_tree.add_child("component", sub_ptr);
			new_tree.put("<xmlattr>.version", "0.2");
			new_tree.put("<xmlattr>.math", "Spaceex");
			rt_tree.add_child("sspaceex", new_tree);
			std::cout<<p.second.get<std::string>("<xmlattr>.id")<<std::endl;
			component c1;
			c1.comp_name = p.second.get<std::string>("<xmlattr>.id");
			std::list<std::string> vars, local_vars, labels;
			find_vars_labels(rt_tree, vars, local_vars, labels);
			c1.vars = vars;
			c1.labels = labels;
			c1.local_vars = local_vars;
			this->components.push_back(c1);
			compDS.insert(std::pair<std::string, pt::ptree>(p.second.get<std::string>("<xmlattr>.id"), rt_tree));
			write_xml(p.second.get<std::string>("<xmlattr>.id") + ".xml",rt_tree, std::locale(), boost::property_tree::xml_writer_make_settings<std::string>(' ', 1));
			bind_comp = p.second.get<std::string>("<xmlattr>.id");
			i++;
		}

	}
	this->number_components = 0;
	for(const pt::ptree::value_type& p : xml_tree.get_child("sspaceex"))
	{
		if(p.first == "component")
		{
			for(const pt::ptree::value_type& c: p.second)
			{
				pt::ptree sub_pt;
				std::tie(name, sub_pt) = c;
//
				if(name == "bind")
				{
					this->number_components++;
					std::cout<<sub_pt.get<std::string>("<xmlattr>.component")<<std::endl;
					pt::ptree xyz = compDS[sub_pt.get<std::string>("<xmlattr>.component")];
//					write_xml(sub_pt.get<std::string>("<xmlattr>.as") + ".xml", xyz, std::locale(), boost::property_tree::xml_writer_make_settings<std::string>(' ', 1));
					i++;
					std::string source, dest;

					for(const pt::ptree::value_type& d : c.second)
					{
						// std::cout << "\t" << sub_pt.get<std::string>("<xmlattr>.bind.map.key") << "\t";
						std::string name1;
						pt::ptree sub_ptr;
						std::tie(name1, sub_ptr) = d;

//						std::cout<<"Atanu: "<<name1<<std::endl;
						if(name1 == "map")
						{
//							std::cout << "\t" << sub_ptr.get<std::string>("<xmlattr>.key") << "\t";
							source = sub_ptr.get<std::string>("<xmlattr>.key");


						}
						if(d.first != "<xmlattr>")
						{
							// std::cout <<d.first.data()<<"\t";
//							std::cout <<d.second.data()<<std::endl;
							dest = d.second.data();
							if(source != dest)
							{
								std::cout<<source<<"\t"<<dest<<std::endl;
								replaceMAP.insert(std::pair<std::string, std::string>(source,dest));
							}
						}

					}
					for(std::list<component>::iterator comp_it = components.begin(); comp_it != components.end(); comp_it++)
					{
						if(comp_it->comp_name == sub_pt.get<std::string>("<xmlattr>.component"))
						{
							for(std::list<std::string>::iterator local_var_it = comp_it->local_vars.begin(); local_var_it != comp_it->local_vars.end(); local_var_it++)
							{
								std::cout<<"local_variable: "<< *local_var_it<<sub_pt.get<std::string>("<xmlattr>.as")<<"_"<<std::endl;
								std::string new_local_var = sub_pt.get<std::string>("<xmlattr>.as") + "_" + *local_var_it;
								replaceMAP.insert(std::pair<std::string, std::string>(*local_var_it,new_local_var));
							}
//							std::cout<<"Atanu alternative comp_name: "<<sub_pt.get<std::string>("<xmlattr>.as")<<std::endl;
//							comp_it->alternative_comp_name = sub_pt.get<std::string>("<xmlattr>.as");
							new_compDS.insert(std::pair<std::string, std::string>(sub_pt.get<std::string>("<xmlattr>.as"), sub_pt.get<std::string>("<xmlattr>.component")));
						}
					}
					modify_xml(xyz, sub_pt.get<std::string>("<xmlattr>.component"), replaceMAP);
					write_xml(sub_pt.get<std::string>("<xmlattr>.as") + ".xml", xyz, std::locale(), boost::property_tree::xml_writer_make_settings<std::string>(' ', 1));
					std::cout<<"complete one component"<<std::endl;
					replaceMAP.clear();
				}
			}
		}
	}

	// for spliting cfg file.
	std::string cfg_content;
	std::getline(std::ifstream(this->config_file), cfg_content, '\0');
	std::fstream cfgFile;
	cfgFile.open(this->config_file,std::ios::in);
	std::string line;
	if(cfgFile.is_open())
	{

		while(getline(cfgFile,line))
		{
		//	std::cout<<line<<std::endl;

			if(line.find("initially") != std::string::npos)
			{
				std::cout<<"Inital string: "<<std::endl;
				parse_initial(line);
				// exit(0);
			}
			if(line.find("forbidden") != std::string::npos)
			{
				std::cout<<"Forbidden string: "<<std::endl;
				parse_forbidden(line);
			}
		}
		// std::cout<<"Debug ##########################################"<<std::endl;
	}
	std::cout<<"To be written in individual cfg file...."<<std::endl;
	for(std::map<std::string, std::string>::iterator it_map = new_compDS.begin(); it_map != new_compDS.end(); it_map++)
	{
		std::cout<<it_map->first<<std::endl;
		std::cout<<initial_strings[it_map->first]<<std::endl;
		std::cout<<forbidden_strings[it_map->first]<<std::endl;
		std::ofstream cfg_file;
		cfg_file.open(it_map->first + ".cfg");
		cfg_file << "system = \"" + new_compDS[it_map->first] + "\"\n";
		cfg_file <<"initially = \"" + initial_strings[it_map->first] + "\"";
		cfg_file <<"\nforbidden = \"" + forbidden_strings[it_map->first] + "\"";
	}
}

std::map<std::string, std::string> xmlParser::get_components()
{
	return new_compDS;
}
