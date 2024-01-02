#ifndef  __PARSER_H
#define  __PARSER_H

#include <core/hybridAutomata/hybridAutomata.h>
#include <core/symbolicStates/initialState.h>
#include <io/flowParser/flow.tab.h>
#include "linExpParser/linexp.tab.h"

/**
 * Parser implementation of .mdl file.
 * .mdl is the XSpeed representation of
 * SpaceEx model file in .xml format.
 *
 * These files are obtained by a model
 * transformation in Hyst XSpeed printer.
 *
 * @author: Rajarshi
 */
typedef std::pair<int, polytope::ptr> forbidden;

class parser
{
	string model_file;
	hybrid_automata ha;
	initial_state::ptr ini;
	std::vector<forbidden> forbidden_states;

public:
	parser(string m_file){
		this->model_file = m_file;
	}
	~parser(){};
	/* creates a hybrid automaton model from parsing */
	void parse();

	/* returns the parsed ha*/
	hybrid_automata getHa();

	/* returns the parsed initial state */
	initial_state::ptr getInitState();

	/* return the parsed forbidden region */
	std::vector<forbidden> getForbidden();

	/* parses the ha locatios and a list of locations.*/
	void parse_loc(fstream& file, location::ptr loc);

	/* parses the variable names and creates the
 	 * var_to_id list.
 	 */
	void parse_vars(fstream& file);

	/* parses a list of linear equality expressions string to
   * create a reset map
   */
	void parse_reset(fstream& file, Assign& t_reset);

	/* parses a list of consecutive odes to create the flow matrix */
	void parse_flow(fstream& file, Dynamics& D);

	/* parses the location invariant string */
	void parse_invariant(string inv_str, polytope::ptr& Inv, polytope::ptr& U, Dynamics& D);

	/* parses transition parameters and creates a transition obj */
	void parse_transition(fstream& file, transition::ptr& t);

	/* parses the initial condition string */
	void parse_initial(std::string init_str, polytope::ptr& p, int& init_locId);

	/* parses the forbidden states string */
	void parse_forbidden(std::string forbidden_str);
};

#endif
