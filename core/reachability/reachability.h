/*
 * reachability.h
 *
 *  Created on: 19-Nov-2021
 *      Author: Atanu
 */

#ifndef REACHABILITY_H_
#define REACHABILITY_H_

#include <application/macroDefinitions.h>
#include "counterExample/abstractCE.h"
#include "boost/timer/timer.hpp"
#include "counterExample/abstractCE.h"
#include "core/hybridAutomata/hybridAutomata.h"
#include "core/symbolicStates/initialState.h"

typedef std::pair<int, polytope::ptr> forbidden;

// macro to choose algo_type of ce search.
// 1 enables search of ce to forbidden region using flowpipe constraints (FC).
// 2 enables the same with implicit HA constraints, requiring no flowpipe construction (WoFC).
// 3 gen_concreteCE_iterative 	: Updated by Rajarshi & Amit. Algorithm using iterative call to LP-NLP
// 4 gen_concreteCE_Simulation : Amit:- Algorithm that use LP formulation for initial start-point followed by Simulation for falsification
// 5 gen_concreteCE_LPobj: trajectory splicing with NLP using dwell-time only as vars. The objective function is the soln
// 	 of a LP formed for splicing with fixed-dwell and varying starts.
// 6 gen_concreteCE_Opt: trajectory splicing with NLP that returns a time minimal ce. (Not Functional)


#define CE_ALGO_TYPE 2

using namespace std;

class reachability {
public:
	
	/* virtual destructor */
	virtual ~reachability();
	void setReachParameter( hybrid_automata& h, const std::list<initial_state::ptr>& i, const ReachabilityParameters& reach_param,
			int lp_solver_type, const std::vector<forbidden>& forbidden_states, const userOptions& user_options);

	/*
	 * Get the number of refinements performed during the search of a counter-example.
	 * This parameter has meaning only when the counter-example search is enabled with reachability analysis.
	 */

	unsigned int get_refinements(){
		return refinements; // This is set to appropriate value by the gen_counter_examples() method.
	}
	/*
	 * Get counter-example trajectories found during reachability analysis
	 */
	std::list<concreteCE::ptr> get_counter_examples() { // making it inline
		return ce_list;
	}
	/**
	  * Returns the safety status of the model w.r.t the given
          * forbidden region
	  */
	bool isSafe(){ return safe;}
	/**
	 * Returns the time to search counter-example(s) during reachability analysis
	 */
	float get_ce_search_time(){
		return this->traj_splicing_time;
	}

	/*
	 * List of private variables now converted to public due to class inheritance framework
	 */
	std::list<initial_state::ptr> I; //converted to a list of initial state
	unsigned int bound;
	ReachabilityParameters reach_parameters;
	hybrid_automata* H;
	int lp_solver_type;
	std::vector<forbidden> forbidden_states;
	bool ce_flag; // The flag to switch ON/OFF the CE generation functionality.
	std::string ce_path; // This string can be either "all", "first" or "1,3,4,15,16" type. The last string is a comma separated list of locations to represent a path.

	const std::string& getSetAggregation() const;
	void setSetAggregation(const std::string& setAggregation);
	const userOptions& getUserOp() const;
	void setUserOp(const userOptions& userOp);
	friend class bmc;
	

private:
	bool safe;
	unsigned int Algorithm_Type;
	unsigned int Total_Partition;
	unsigned int number_of_streams;
	unsigned int refinements; // The number of refinements during the search of counter-example trajectories. This has meaning only when the counter-example generation function is enabled.
	float traj_splicing_time; // The time taken to concretize/splice abstract path(s)
	unsigned int ce_search_algo_type; // the type of search algorithm to use in order to search for a counter-example. Options are FC and WoFC

	std::string set_aggregation; // The aggregation options thull (default) and none
	std::list<concreteCE::ptr> ce_list; // the list of concrete counter-examples in the HA.

	userOptions user_op; //user options

	
	/*
	 * Manages the counter-example searching during reachability with BFS. The return status indicates whether the BFS should continue
	 * to search for further abstract paths or whether to stop. A return value of true indicates the BFS to stop. A false indicates that
	 * the BFS should continue to further explore for newer abstract counterexamples.
	 */
	bool gen_counter_example(abstractCE::ptr abs_ce_path, std::string ce_search_algo_type);
	
};

#endif /* REACHABILITY_H_ */
