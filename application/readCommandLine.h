/*
 * readCommandLine.h
 *
 * @Author: Atanu
 *
 */

//**************** Hybrid Automata Definition ***********************
#include <application/macroDefinitions.h>
#include <boost/program_options/config.hpp>
#include "boost/program_options.hpp"
#include <boost/config.hpp>
#include <boost/program_options/detail/config_file.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/algorithm/string.hpp>
#include <io/ioUtility.h>
#include <io/loadModel.h>
#include <io/parser.h>
#include "counterExample/concreteCE.h"
#include "application/themeSelector.h"
#include <core/hybridAutomata/hybridAutomata.h> //for composition function
#include <io/xmlParser.h>

#ifndef APPLICATION_READCOMMANDLINE_H_
#define APPLICATION_READCOMMANDLINE_H_

typedef std::pair<int, polytope::ptr> forbidden;




void readCommandLine(int argc, char *argv[], userOptions& user_options,
		std::list<initial_state::ptr>& init_state,
		ReachabilityParameters& reach_parameters,
		std::vector<forbidden>& forbidden_states); 

void readCommandCompose(int argc, char *argv[],userOptions& user_options,
		std::list<initial_state::ptr>& init_state,
		ReachabilityParameters& reach_parameters,
		std::vector<forbidden>& forbidden_states); //For composition of two hybrid HA.

#endif /* APPLICATION_READCOMMANDLINE_H_ */

