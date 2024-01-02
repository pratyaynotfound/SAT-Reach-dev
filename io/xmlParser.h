/***
 * This file takes the hybrid automata containing several HA components,
 * and split into different component. This is required for the BMC of
 * compositional hybrid automata.
 *
 * 		Author: Atanu Kundu
 * 		Date:   31-Oct 2023
 *
 *
 *
 ***/



#ifndef  __XMLPARSER_H
#define  __XMLPARSER_H


#include <boost/property_tree/ptree.hpp> // for parsing networked components.
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/detail/file_parser_error.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/tokenizer.hpp>
#include <map>
#include <list>
#include <fstream>
#include <iostream>
namespace pt = boost::property_tree;

struct component
{
	std::string comp_name;
	std::list<std::string> vars; // for global variables.
	std::list<std::string> local_vars; //for local variable.
	std::list<std::string> labels;
//	std::string alternative_comp_name; //
};



class xmlParser
{
	std::string xml_file, config_file;
	unsigned int number_components;
	std::list<component> components;

public:
	xmlParser(std::string model, std::string config)
	{
		this->xml_file = model;
		this->config_file = config;
		//this->components = 1;
	}

	void split_CHA();
	unsigned int get_number_components();
	void find_vars_labels(pt::ptree root, std::list<std::string> &vars, std::list<std::string> &local_vars, std::list<std::string> &labels);
	void modify_xml(pt::ptree &root, std::string component_name, std::map<std::string, std::string> replaceMAP);
	std::string isWhat(std::string component_name, std::string element);

	void parse_initial(std::string line);
	void parse_forbidden(std::string line);
	std::string retrieve_string(std::string line, std::string first_char, std::string sec_char);
	std::map<std::string, std::string> get_components();
};










#endif
