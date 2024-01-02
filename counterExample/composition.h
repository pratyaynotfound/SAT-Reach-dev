/*
 *
 * composition.h
 *
 * Author: Atanu Kundu
 * Created on: 27-jun-2023
 *
 */


#ifndef SATReach_COUNTEREXAMPLE_COMPOSITION_H_
#define SATReach_COUNTEREXAMPLE_COMPOSITION_H_


#include <core/hybridAutomata/hybridAutomata.h>
#include <core/symbolicStates/initialState.h>
#include <application/userOptions.h>
#include <boost/shared_ptr.hpp>
#include <string.h>
#include <iostream>
#include <list>
#include <core/reachability/postCSequential.h>
#include <application/structures.h>
#include <core/reachability/reachability.h>
#include <io/loadModel.h>
#include <core/hybridAutomata/transition.h>
#include <boost/algorithm/string.hpp>
#include <utilities/postAssignment.h>
#include <core/pwl/pwlist.h>

//#include <utilities/templatePolyhedra.h>

struct composeLabels
{
	std::string labels; //unique labels.
	std::list<location::const_ptr> next_locations; // list of location for a unique label, which need to be composed.
	std::list<hybrid_automata::ptr> Ha_Ptrs; //list of HA object to
};

typedef std::pair<int, polytope::ptr> forbidden; //(locId,Polytope)
typedef std::vector<forbidden> forbidden_states; // vector of forbidden symb states.
typedef std::list<symbolic_states::ptr> region;



class composition
{
	hybrid_automata ha;
	const hybrid_automata::ptr ha_ptr;
	const std::list<hybrid_automata::ptr> ha_ptrs;
	const std::list<initial_state::ptr>& init;
	const std::list<initial_state::ptr>& init_comps;
	const forbidden_states& forbidden_s;
	ReachabilityParameters& reach_params;
	const userOptions& user_ops;
//	std::list<initial_state::ptr> newInit;
	reachability reach_on_the_fly;

public:
	typedef boost::shared_ptr< std::map<std::string, unsigned int> > mapper;

	/* The variable to dimension index map */
	static mapper var_index_mapper;

	static int loc_id;

	std::list<composeLabels> ComposedLabels;

	std::list<std::string> shared_variable;


	composition(const hybrid_automata::ptr ha_ptr, const std::list<hybrid_automata::ptr>& haPtr, const std::list<initial_state::ptr>& init, const std::list<initial_state::ptr>& init_comps, const forbidden_states& forbidden,
				ReachabilityParameters& r_params, const userOptions& user_ops);

	void compose();

	void make_composed_dimension();

	void insert_to_map(std::string name, unsigned int val);

	void print_mapper();

	void build_initial(polytope::ptr& initial_set);

	void build_location(location::ptr& loc, polytope::ptr& inv);

	bool is_shared(std::string str);

	transition::ptr find_trans(std::string labels, std::list<location::const_ptr> locations, std::list<hybrid_automata::ptr> Ha_Ptrs);

	void make_shared_labels(std::list<location::const_ptr> locations);

	location::ptr marged_locations(std::list<location::const_ptr> locs, std::list<hybrid_automata::ptr> Ha_Ptrs);

	location::ptr marged_next_loc(std::list<location::const_ptr> locs, std::list<hybrid_automata::ptr> Ha_Ptrs);

	std::list<symbolic_states::ptr> onTheFly_Reachability();

	/**
	 * Returns the variable name with the given index
	 */
	//std::string get_variable_name(unsigned int index) const;
	/**
	 * Returns the index of the parameter var_name
	 * in the varname to dimension index map
	 */
	//unsigned int get_index_name(std::string var_name) const;
};
















#endif /* SATReach_COUNTEREXAMPLE_COMPOSITION_H_ */

