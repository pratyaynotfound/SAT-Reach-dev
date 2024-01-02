/*
 * reachability.cpp
 *
 *  Created on: 19-Nov-2021
 *      Author: Atanu
 */


#include <core/reachability/reachability.h>


using namespace std;

void reachability::setReachParameter( hybrid_automata& h, const std::list<initial_state::ptr>& i, const ReachabilityParameters& reach_param,
		int lp_solver_type, const std::vector<forbidden>& forbidden_states, const userOptions& user_options) {
	H = &h;
	I = i;
	reach_parameters = reach_param;
	bound = user_options.get_bfs_level();	//bfs_level
	Algorithm_Type = user_options.get_algorithm();	// selects an algorithm to execute
	Total_Partition = user_options.getTotalSliceSize(); //slice size
	this->lp_solver_type= lp_solver_type;
	number_of_streams = user_options.getStreamSize();
	this->forbidden_states = forbidden_states;
	ce_flag = user_options.get_ce_flag();
	ce_path = user_options.get_ce_path();
	refinements = 0;
	traj_splicing_time = 0;
	set_aggregation = user_options.getSetAggregation();
	safe = true; // Assumed initially that the model is safe.
	setUserOp(user_options); 
}

reachability::~reachability(){}


bool reachability::gen_counter_example(abstractCE::ptr abs_path, std::string ce_search_algo_type)
{
	double splicing_error_tol = 1e-6; // A parameter particular to specify the precision of the search of ce by using trajectory splicing.
	boost::timer::cpu_timer clock; // clocks the time taken to splice a trajectory
	if(traj_splicing_time > 3600000){
		std::cout << "Search for CE TIMED-OUT (>1hrs)\n";
		return false;
	}
	if(ce_path.compare("first") == 0) // Stop the BFS search once a concrete ce has been found
	{
		clock.start();
		concreteCE::ptr ce = abs_path->get_validated_CE(splicing_error_tol,ce_search_algo_type);
		clock.stop();
		traj_splicing_time += clock.elapsed().user / 1000000;

		refinements += ce->get_refinements();
		if(!ce->is_empty()){
			this->ce_list.push_back(ce);
			this->safe = false;
			return false; 		// In order to terminate the BFS.
		}
		else
			return true; // continue for searching the first concrete trajectory
	}
}


const std::string& reachability::getSetAggregation() const {
	return set_aggregation;
}

void reachability::setSetAggregation(const std::string& setAggregation) {
	set_aggregation = setAggregation;
}


const userOptions& reachability::getUserOp() const {
	return user_op;
}

void reachability::setUserOp(const userOptions& userOp) {
	user_op = userOp;
}
