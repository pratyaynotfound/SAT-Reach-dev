/*
 * readCommandLine.cpp
 *
 * @Author: Atanu
 *
 */


#include <application/readCommandLine.h>

namespace po = boost::program_options;

hybrid_automata::ptr themeSelector::ha_ptr = nullptr;
std::list<hybrid_automata::ptr> themeSelector::ha_ptrs;
std::list<initial_state::ptr> themeSelector::init_components;



void readCommandLine(int argc, char *argv[], userOptions& user_options,
		std::list<initial_state::ptr>& init_state,
		ReachabilityParameters& reach_parameters,
		std::vector<forbidden>& forbidden_states) {

	bool isConfigFileAssigned = false;

	po::options_description desc("SAT-Reach options");
	po::variables_map vm;

	if (argc == 1) { //No argument
		std::cout << "Missing arguments!\n";
		exit(0);
	}
	desc.add_options()
	("help", "produce help message")

	("engine,e", po::value<std::string>()->default_value("bmc"), "set the running engine (default BMC):"
					" \n - bmc : Falsification of safety property"
					" \n - cbmc : Falsification of safety property using compositional RA."
					" \n - SATbmc : Falsification of safety property using compositional HA.")
	("directions", po::value<int>()->default_value(1), "Set the directions for template polyhedra:\n"
					"1. Box template (Set to default)\n"
					"2. Octagonal template \n"
					"n. uniformly oriented n directions \n")
	("time-horizon", po::value<double>(), "Set the local time horizon of flowpipe computation.")
	("time-step", po::value<double>(), "Set the sampling time of flowpipe computation.")
	("algo,a",po::value<int>()->default_value(1), "Set the algorithm\n"
			"1 -- Sequential FB Interpolation (default) \n")
	("depth", po::value<int>(), "Set the depth of HA exploration for Bounded Model Checking")
	("aggregate", po::value<std::string>()->default_value("thull"), "Set-aggregation (default thull): \n - thull : template hull \n - chull : convex hull \n - none : no set aggregation (precise but costly)")

	("forbidden,F", po::value<std::string>(), "forbidden location_ID and forbidden set/region within that location")
	("CE",po::value<std::string>(), "Search for counter-example to forbidden-region together with state-space exploration:\n"
			"\n - (first): search for counter-example until the first-one (if any) is found.")
	("ceproc", po::value<std::string>()->default_value("FC"), "The procedure to use for searching a counterexample to safety."
			"\n - FC: trajectory splicing with flowpipe constraints (default)"
	)
	("include-path,I", po::value<std::string>(), "include file path")
	("model-file,m", po::value<std::string>(), "include model file")
	("config-file,c", po::value<std::string>(), "include configuration file")
	("output-file,o", po::value<std::string>(), "output file name for redirecting the outputs")
	("output-variable,v", po::value<std::string>(), "variables for plotting. E.g., 'x,v' for Bouncing Ball") 
	("output-format", po::value<std::string>()->default_value("GEN"), "The type of output format, either GEN or INTV. GEN prints the vertices of the reach set and INTV prints the global bounds on the output variables (Set to GEN by default)")
;

	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	std::vector<std::string> output_vars(3); //stores the plotting variables

	std::string input;
	for (int i = 1; i < argc; i++) {

		if (std::string(argv[i]).find("-o") != string::npos || std::string(argv[i]).find("--output-file") != string::npos || std::string(argv[i]).find("-m") != string::npos || std::string(argv[i]).find("-c") != string::npos ||
				std::string(argv[i]).find("--model-file") != string::npos || std::string(argv[i]).find("--config-file") != string::npos )
			i++;
		else {
			if (std::string(argv[i]).find("-F") != string::npos){
				input.append(argv[i]);	//-F
				input.append(" ");
				i++; //move next arg ie options for -F
				input.append("\""); //starting quote
				input.append(argv[i]);
				input.append("\" "); //ending quote
			}else{
				input.append(argv[i]);
				input.append(" ");
			}
		}
	}
	//--------
	if (argc > 1) { // Options to be filled-up
		if (vm.count("help")) {
			cout << desc << "\n";
			exit(0);
		}

		// Setting the model and the cfg file

		std::string include_path = "", model_filename = "",
				config_filename = ""; //default set to empty
		if (vm.count("include-path")) {
			include_path = vm["include-path"].as<std::string>();
		}
		if (vm.count("config-file")) {
			user_options.set_configFile(vm["config-file"].as<std::string>());
		}
		if (vm.count("model-file")) {
			user_options.set_modelFile(vm["model-file"].as<std::string>());
		}
		
		// ***********Setting for Output file ****
		std::string fileName, fullPath, fileWithPath, forbidStr;
		if (vm.count("include-path")) {
			fullPath = vm["include-path"].as<std::string>();
		} else {
			fullPath = "./"; //default file path
		}
		fileWithPath.append(fullPath);
		if (vm.count("output-format")) {
			user_options.setOutputFormatType(vm["output-format"].as<std::string>());
		}

		if (vm.count("output-file")) {
			fileName = vm["output-file"].as<std::string>();
			user_options.setOutFilename(fileName);

		} else {
			fileName = "out.txt";
			user_options.setOutFilename(fileName);
		}
		fileWithPath.append(fileName);
		// ******** Setting for Output file Done *************

		if (vm.count("forbidden") && isConfigFileAssigned == false) { //Compulsory Options but set to 1 by default
			user_options.set_forbidden_set(vm["forbidden"].as<std::string>());
		}

		if (vm.count("model-file") && vm.count("config-file")
				&& (user_options.get_model()!=0) ) { // model=0 default to no model specified
			std::cout << "Invalid input. Either a model file or a pre-loaded model to be specified, but not both.\n";
			throw(new exception());
		}
		
		if (vm.count("model-file") && vm.count("config-file")
					&& ((user_options.get_model()==0)) && boost::iequals(vm["engine"].as<std::string>(),"bmc")==true) { // model=0 means no pre-built model specified

			string cmd_str = "java -jar Model-Translator.jar -t XSpeed \"\" -p flatten \"\" -i " + vm["model-file"].as<std::string>() + " " + vm["config-file"].as<std::string>() + " -o input_model.mdl";
			system(cmd_str.c_str());
			parser _parser("input_model.mdl");
			_parser.parse();
			//system("rm input_model.mdl"); // remove the mdl file
			hybrid_automata haObj = _parser.getHa();
			//hybrid_automata::ptr ha_shared_ptr = hybrid_automata::ptr(new hybrid_automata(_parser.getHa())); // assign the parsed ha
			hybrid_automata::ptr ha_shared_ptr = hybrid_automata::ptr(new hybrid_automata(haObj)); // assign the parsed ha
			themeSelector::ha_ptr = ha_shared_ptr; // initializing the static member of themeSelector

			init_state.push_back(_parser.getInitState()); // assign the parsed init
			forbidden_states = _parser.getForbidden(); //assign the forbidden states
			isConfigFileAssigned = false;// to continue taking the params from cmdline
		}

		// ********* For network components **********//
//		std::list<initial_state::ptr> init;

		if (vm.count("model-file") && vm.count("config-file") && ((user_options.get_model()==0)) && boost::iequals(vm["engine"].as<std::string>(),"cbmc")==true)
		{
			std::cout<<"Entered"<<std::endl;
			xmlParser xmlparser(vm["model-file"].as<std::string>(),vm["config-file"].as<std::string>());
			xmlparser.split_CHA();
			std::map<std::string, std::string> components = xmlparser.get_components();
			unsigned int HP_counter = 1;
			/*for(std::map<std::string, std::string>::iterator it_comp = components.begin(); it_comp != components.end(); it_comp++)
			 *
			 */
			for(auto x: components)
			{
				std::cout<<x.first<<std::endl;
				string cmd_str = "java -jar Model-Translator.jar -t XSpeed \"\"  -i " + x.first + ".xml" + " " + x.first + ".cfg" + " -o input_model_" + to_string(HP_counter) + ".mdl";
				std::cout<<cmd_str<<std::endl;
				system(cmd_str.c_str());
				parser _parser("input_model_" + to_string(HP_counter) + ".mdl");
				_parser.parse();
				hybrid_automata haObj = _parser.getHa();
//				hybrid_automata ha_object_1 = parser_1.getHa();
				hybrid_automata::ptr HA_shared_ptr_1 = hybrid_automata::ptr(new hybrid_automata(haObj)); // assign the parsed ha
//				themeSelector::ha_ptr = HA_shared_ptr_1; // initializing the static member of themeSelector. it is unnecessary here.
				themeSelector::ha_ptrs.push_back(HA_shared_ptr_1);
				themeSelector::init_components.push_back(_parser.getInitState());
//				init_state.push_back(_parser.getInitState()); // assign the parsed init
				if (!_parser.getForbidden().empty())
					forbidden_states.push_back(_parser.getForbidden()[0]); //assign the forbidden states

				HP_counter++;
			}
			std::cout<<"Number of individual components: "<<xmlparser.get_number_components()<<std::endl;
//			hybrid_automata ha = set_composed_HA(themeSelector::ha_ptrs, init_state);
//			hybrid_automata::ptr HA_shared_ptr_1 = hybrid_automata::ptr(new hybrid_automata(set_composed_HA(themeSelector::ha_ptrs, init_state)));
			themeSelector::ha_ptr = hybrid_automata::ptr(new hybrid_automata(set_composed_HA(themeSelector::ha_ptrs, themeSelector::init_components, init_state)));
			std::cout<<"Here"<<std::endl;
			exit(0);
			
		}

		if (vm.count("model-file") && vm.count("config-file") && ((user_options.get_model()==0)) && boost::iequals(vm["engine"].as<std::string>(),"SATbmc")==true)
		{
			std::cout<<"Entered"<<std::endl;
			xmlParser xmlparser(vm["model-file"].as<std::string>(),vm["config-file"].as<std::string>());
			xmlparser.split_CHA();
			std::map<std::string, std::string> components = xmlparser.get_components();
			unsigned int HP_counter = 1;
			/*for(std::map<std::string, std::string>::iterator it_comp = components.begin(); it_comp != components.end(); it_comp++)
			 *
			 */
			for(auto x: components)
			{
				std::cout<<x.first<<std::endl;
				string cmd_str = "java -jar Model-Translator.jar -t XSpeed \"\"  -i " + x.first + ".xml" + " " + x.first + ".cfg" + " -o input_model_" + to_string(HP_counter) + ".mdl";
				std::cout<<cmd_str<<std::endl;
				system(cmd_str.c_str());
				parser _parser("input_model_" + to_string(HP_counter) + ".mdl");
				_parser.parse();
				hybrid_automata haObj = _parser.getHa();
//				hybrid_automata ha_object_1 = parser_1.getHa();
				hybrid_automata::ptr HA_shared_ptr_1 = hybrid_automata::ptr(new hybrid_automata(haObj)); // assign the parsed ha
//				themeSelector::ha_ptr = HA_shared_ptr_1; // initializing the static member of themeSelector. it is unnecessary here.
				themeSelector::ha_ptrs.push_back(HA_shared_ptr_1);
				themeSelector::init_components.push_back(_parser.getInitState());
//				init_state.push_back(_parser.getInitState()); // assign the parsed init
				if (!_parser.getForbidden().empty())
					forbidden_states.push_back(_parser.getForbidden()[0]); //assign the forbidden states

				HP_counter++;
			}
			std::cout<<"Number of individual components: "<<xmlparser.get_number_components()<<std::endl;
//			hybrid_automata ha = set_composed_HA(themeSelector::ha_ptrs, init_state);
//			hybrid_automata::ptr HA_shared_ptr_1 = hybrid_automata::ptr(new hybrid_automata(set_composed_HA(themeSelector::ha_ptrs, init_state)));
			themeSelector::ha_ptr = hybrid_automata::ptr(new hybrid_automata(set_composed_HA(themeSelector::ha_ptrs, themeSelector::init_components, init_state)));
			std::cout<<"Here in SATbmc!"<<std::endl;
			
		}

		if (vm.count("directions") && isConfigFileAssigned == false) { //Compulsory Options but set to 1 by default
			user_options.set_directionTemplate(vm["directions"].as<int>());
			if (user_options.get_directionTemplate() <= 0) {
				std::cout << "Invalid Directions option specified. Expected 1 for BOX, 2 for OCT, N for uniform directions.\n";
				throw(new exception());
			}
		}
		if (vm.count("depth") && isConfigFileAssigned == false) { //Compulsory Options
			user_options.set_bfs_level(vm["depth"].as<int>());
			if (user_options.get_bfs_level() < 0) {
				std::cout<< "Invalid bfs level specified. A zero or a positive integer is expected.\n";
				throw(new exception());
			}
		}
		if (vm.count("engine")) { //Compulsory Options but set to thull by default
			user_options.setEngine((vm["engine"].as<std::string>()));
			if (boost::iequals(user_options.getEngine(),"bmc")==false && boost::iequals(user_options.getEngine(),"cbmc")==false && boost::iequals(user_options.getEngine(),"SATbmc")==false ){
				std::cout << "Invalid engine option specified. Expected  \"bmc\" or \"cbmc\" or \"SATbmc\".\n";
				throw(new exception());
			}
		}

		if (vm.count("aggregate")) { //Compulsory Options but set to thull by default
			user_options.setSetAggregation((vm["aggregate"].as<std::string>()));
			string optionVal = user_options.getSetAggregation();

			if ( boost::iequals(optionVal,"none")==false && boost::iequals(optionVal,"thull")==false && boost::iequals(optionVal,"chull")==false){
					std::cout << "Invalid aggregation option specified. Expected \"none\",\"thull\" or \"chull\".\n";
					throw(new exception());
				}
		}
		
		if (vm.count("output-variable")) {
			std::string VarStr;
			VarStr = vm["output-variable"].as<std::string>();

			typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
			boost::char_separator<char> sep(", ");
			tokenizer tokens(VarStr, sep);
			int index = 0;
			for (tokenizer::iterator tok_iter = tokens.begin();
					tok_iter != tokens.end(); ++tok_iter) {
				output_vars[index] = (std::string) (*tok_iter);
				index++;
			}
		}
		if (vm.count("forbidden") && isConfigFileAssigned == false) { //Compulsory Options but set to 1 by default
			user_options.set_forbidden_set(vm["forbidden"].as<std::string>());
		}
		if(vm.count("CE") && isConfigFileAssigned == false) {
			user_options.set_ce_path(vm["CE"].as<std::string>());
			user_options.set_ce_flag(true);
		}
//		if(vm.count("ceproc")) {
//
//			std::string userop_ceproc = vm["ceproc"].as<std::string>();
//			user_options.setCEProc(userop_ceproc);
//
//			if (boost::iequals(userop_ceproc,"FC")==false) {
//				std::cout << "Invalid CE-Procedure. Expected \"FC\".\n";
//				throw(new exception());
//			}
//		}
		
		if (vm.count("time-horizon") && isConfigFileAssigned == false) { //Compulsory Options
			user_options.set_timeHorizon(vm["time-horizon"].as<double>());
			if (user_options.get_timeHorizon() <= 0) { //for 0 or negative time-bound
				std::cout << "Invalid time-horizon option specified. A positive non-zero value expected.\n";
				throw(new exception());
			}
		} 
		else{
				std::cout << "Missing value for parameter \"time-horizon\"\n";
				throw(new exception());
		}

		if (vm.count("time-step") && isConfigFileAssigned == false) { //Compulsory Options
			user_options.set_timeStep(vm["time-step"].as<double>());
			if (user_options.get_timeStep() <= 0) {
				std::cout << "Invalid time-step option specified. A positive non-zero time-step expected.\n";
				throw(new exception());
			}
		}
		else {
			std::cout << "Missing value for parameter \"time-step\". \n";
			throw(new exception());
		}
//		//Algorithm Preference
//		if (vm.count("algo")) {
//			user_options.set_algorithm(vm["algo"].as<int>());
//			if (user_options.get_algorithm()< 0|| user_options.get_algorithm() > MAX_ALGO) {
//				std::cout << "Invalid algorithm option specified.\n";
//				throw(new exception());
//			}
//		} else {
//			std::cout << "Missing value for parameter \"algo\". \n";
//			throw(new exception());
//		}

		/*
		if (vm["model"].as<int>()!=0) { //Compulsory Options but set to 0 by default
			user_options.set_model(vm["model"].as<int>());
			// get an empty ha object
			themeSelector::ha_ptr = hybrid_automata::ptr(new hybrid_automata());
			load_ha_model(init_state, *(themeSelector::getHaInstance()), reach_parameters, user_options);
		} */

	}

	std::cout<<"Here"<<std::endl;
	if(output_vars[0].empty() && output_vars[1].empty())
	{
		std::cout<<"Output variables not specified. Two variables of the system expected.\n"<<std::endl;
		throw(new exception());
	}
 
	// Check if the ha_ptr has been initialized
	if(themeSelector::getHaInstance()==nullptr){
		std::cerr << "HA not initialized by the parser: Check for missing '-m' option in the command line, or check the specified model Id.\n";
		exit(0);
	}
	int x1, x2;
	try{

		if(themeSelector::getHaInstance()->ymap_size()!=0){
			std::cout<<"Is it in if?"<<std::endl;
			unsigned int m = themeSelector::getHaInstance()->ymap_size();
			x1 = themeSelector::getHaInstance()->get_y_index(output_vars[0]);
			if(x1==-1) // string not present in ymap.
			{
				x1 = themeSelector::getHaInstance()->get_index(output_vars[0]);
				themeSelector::getHaInstance()->insert_to_output_map(output_vars[0],m);
				location::ptr loc = themeSelector::getHaInstance()->getInitialLocation();
				Dynamics& D = loc->getSystemDynamics();
				unsigned int sysDim = themeSelector::getHaInstance()->map_size();
				math::matrix<double> rowMat(1,sysDim);
				rowMat.clear();
				rowMat(0,x1)=1;
				D.MatrixT.matrix_join(rowMat,D.MatrixT);
				x1=m; m++;
			}
			x2 = themeSelector::getHaInstance()->get_y_index(output_vars[1]);
			if(x2==-1) // string not present in ymap.
			{
				x2 = themeSelector::getHaInstance()->get_index(output_vars[1]);
				themeSelector::getHaInstance()->insert_to_output_map(output_vars[1],m);
				location::ptr loc = themeSelector::getHaInstance()->getInitialLocation();
				Dynamics& D = loc->getSystemDynamics();
				unsigned int sysDim = themeSelector::getHaInstance()->map_size();
				math::matrix<double> rowMat(1,sysDim);
				rowMat.clear();
				rowMat(0,x2)=1;
				D.MatrixT.matrix_join(rowMat,D.MatrixT);
				x2=m;
			}
		}
		else{
//			if(boost::iequals(vm["engine"].as<std::string>(),"cbmc")!=true)
//			{
				std::cout<<"No, it is in else!!!"<<std::endl;
				x1 = themeSelector::getHaInstance()->get_index(output_vars[0]);
				x2 = themeSelector::getHaInstance()->get_index(output_vars[1]);
				std::cout<<x1<<"\t"<<x2<<std::endl;
//			}
		}
	}catch(const std::out_of_range& oor)
	{
		std::cerr << "Output variables not defined in the model: " << oor.what() << '\n';
		std::cerr << "Please use two output variables from the following list of model variables\n";
		themeSelector::getHaInstance()->print_var_index_map();
		exit(0);
	}

	user_options.set_first_plot_dimension(x1);
	user_options.set_second_plot_dimension(x2);
	if (!(output_vars[2].empty())) {
		unsigned int x3 = themeSelector::getHaInstance()->get_index(output_vars[2]);
		user_options.set_third_plot_dimension(x3);
	}
	//ALL COMMAND-LINE OPTIONS are set

	/* Set the reachability options given by the user */
//	if(boost::iequals(vm["engine"].as<std::string>(),"cbmc")!=true)
//	{
		set_params(*themeSelector::getHaInstance(), init_state, user_options, reach_parameters, forbidden_states);
//	}
	if (!user_options.get_forbidden_set().empty()){
		assert(forbidden_states.size()!=0);
		try{
			forbidden forbidden_set;
			forbidden_set = forbidden_states[0]; // the first symb state
			forbidden_set.second->print2file("./bad_poly", x1, x2);
		} catch(...){
			std::cout << "Cannot print the forbidden polytope because it is unbounded in the print dimensions or may be empty\n";
		}
	}

}

//for composed automata
void readCommandCompose(int argc, char *argv[],userOptions& user_options,
		std::list<initial_state::ptr>& init_state,
		ReachabilityParameters& reach_parameters,
		std::vector<forbidden>& forbidden_states)
{


	using namespace std;
		using namespace boost;

		string fileName = "out.txt";
		user_options.setOutFilename(fileName);
		user_options.set_first_plot_dimension(0);
		user_options.set_second_plot_dimension(1);
		user_options.setOutputFormatType("GEN");
		user_options.setSetAggregation("thull");
		string path,model_file1, model_file2, config_file1, config_file2;
		/* ACC models
		path = "../testcases/ACCS03/test/";
		model_file1 = "ACCS_aut0.xml";
		model_file1 = path + model_file1;
		config_file1 = "ACCS_aut0.cfg";
		config_file1 = path + config_file1;
		model_file2 = "ACCS_aut1.xml";
		model_file2 = path + model_file2;
		config_file2 = "ACCS_aut1.cfg";
		config_file2 = path + config_file2;
		//std::cout<<"Models file: "<<model_file1<<"\t"<<model_file2<<endl;
		//std::cout<<"Config file: "<<config_file1<<"\t"<<config_file2<<endl;
		string cmd_str1 = "java -jar Model-Translator.jar -t XSpeed \"\" -i " + model_file1 + " " + config_file1 + " -o input_component_1.mdl";
		system(cmd_str1.c_str());
		string cmd_str2 = "java -jar Model-Translator.jar -t XSpeed \"\" -i " + model_file2 + " " + config_file2 + " -o input_component_2.mdl";
		system(cmd_str2.c_str());
		parser parser_1("input_component_1.mdl");
		parser_1.parse();
		hybrid_automata ha_object_1 = parser_1.getHa();
		hybrid_automata::ptr HA_shared_ptr_1 = hybrid_automata::ptr(new hybrid_automata(ha_object_1)); // assign the parsed ha
		themeSelector::ha_ptr = HA_shared_ptr_1; // initializing the static member of themeSelector
		themeSelector::ha_ptrs.push_back(HA_shared_ptr_1);
		init_state.push_back(parser_1.getInitState()); // assign the parsed init
		if (!parser_1.getForbidden().empty())
			forbidden_states.push_back(parser_1.getForbidden()[0]); //assign the forbidden states
		//std::cout<<parser_1.getForbidden()[0].first<<endl;
		//HA_shared_ptr_1->print_var_dim_map();
		std::cout<<"model file 1 parsed!\n";
		parser parser_2("input_component_2.mdl");
		parser_2.parse();
		hybrid_automata ha_object_2 = parser_2.getHa();
		hybrid_automata::ptr HA_shared_ptr_2 = hybrid_automata::ptr(new hybrid_automata(ha_object_2)); // assign the parsed ha
		themeSelector::ha_ptr = HA_shared_ptr_2; // initializing the static member of themeSelector
		//parser_2.getInitState()->getInitialSet()->printPoly();
		init_state.push_back(parser_2.getInitState()); // assign the parsed init
		themeSelector::ha_ptrs.push_back(HA_shared_ptr_2);
		//HA_shared_ptr_2->print_var_dim_map();
		if (!parser_2.getForbidden().empty())
			forbidden_states.push_back(parser_2.getForbidden()[0]); //assign the forbidden states
		std::cout<<"model file 2 parsed!\n";
		//set_params(*themeSelector::getHaInstance(), init_state, user_options, reach_parameters, forbidden_states);
		*/
		// NRS models..
		string model_file3, config_file3;
		path = "../testcases/NRS/test/";
		model_file1 = "rod_1.xml";
		model_file1 = path + model_file1;
		config_file1 = "rod_1.cfg";
		config_file1 = path + config_file1;
		model_file2 = "rod_2.xml";
		model_file2 = path + model_file2;
		config_file2 = "rod_2.cfg";
		config_file2 = path + config_file2;
		model_file3 = "controller.xml";
		model_file3 = path + model_file3;
		config_file3 = "controller.cfg";
		config_file3 = path + config_file3;
		string cmd_str1 = "java -jar Model-Translator.jar -t XSpeed \"\" -i " + model_file1 + " " + config_file1 + " -o input_component_1.mdl";
		system(cmd_str1.c_str());
		string cmd_str2 = "java -jar Model-Translator.jar -t XSpeed \"\" -i " + model_file2 + " " + config_file2 + " -o input_component_2.mdl";
		system(cmd_str2.c_str());
		string cmd_str3 = "java -jar Model-Translator.jar -t XSpeed \"\" -i " + model_file3 + " " + config_file3 + " -o input_component_3.mdl";
		system(cmd_str3.c_str());
		parser parser_1("input_component_1.mdl");
		parser_1.parse();
		hybrid_automata ha_object_1 = parser_1.getHa();
		hybrid_automata::ptr HA_shared_ptr_1 = hybrid_automata::ptr(new hybrid_automata(ha_object_1)); // assign the parsed ha
		themeSelector::ha_ptr = HA_shared_ptr_1; // initializing the static member of themeSelector
		themeSelector::ha_ptrs.push_back(HA_shared_ptr_1);
		init_state.push_back(parser_1.getInitState()); // assign the parsed init
		if (!parser_1.getForbidden().empty())
			forbidden_states.push_back(parser_1.getForbidden()[0]); //assign the forbidden states
		//std::cout<<parser_1.getForbidden()[0].first<<endl;
		//HA_shared_ptr_1->print_var_dim_map();
		std::cout<<"model file 1 parsed!\n";
		parser parser_2("input_component_2.mdl");
		parser_2.parse();
		hybrid_automata ha_object_2 = parser_2.getHa();
		hybrid_automata::ptr HA_shared_ptr_2 = hybrid_automata::ptr(new hybrid_automata(ha_object_2)); // assign the parsed ha
		themeSelector::ha_ptr = HA_shared_ptr_2; // initializing the static member of themeSelector
		themeSelector::ha_ptrs.push_back(HA_shared_ptr_2);
		init_state.push_back(parser_2.getInitState()); // assign the parsed init
		if (!parser_2.getForbidden().empty())
			forbidden_states.push_back(parser_2.getForbidden()[0]); //assign the forbidden states
		//std::cout<<parser_1.getForbidden()[0].first<<endl;
		//HA_shared_ptr_1->print_var_dim_map();
		std::cout<<"model file 2 parsed!\n";
		parser parser_3("input_component_3.mdl");
		parser_3.parse();
		hybrid_automata ha_object_3 = parser_3.getHa();
		hybrid_automata::ptr HA_shared_ptr_3 = hybrid_automata::ptr(new hybrid_automata(ha_object_3)); // assign the parsed ha
		themeSelector::ha_ptr = HA_shared_ptr_3; // initializing the static member of themeSelector
		themeSelector::ha_ptrs.push_back(HA_shared_ptr_3);
		init_state.push_back(parser_3.getInitState()); // assign the parsed init
		if (!parser_3.getForbidden().empty())
			forbidden_states.push_back(parser_3.getForbidden()[0]); //assign the forbidden states
		//std::cout<<parser_1.getForbidden()[0].first<<endl;
		//HA_shared_ptr_1->print_var_dim_map();
		std::cout<<"model file 3 parsed!\n";
}
