/*
 * bmc.h
 *
 *  Created on: 14-Sep-2020
 *      Author: rajarshi
 */

#ifndef SATReach_COUNTEREXAMPLE_BMC_H_
#define SATReach_COUNTEREXAMPLE_BMC_H_

#include <core/hybridAutomata/hybridAutomata.h>
#include <core/symbolicStates/symbolicStates.h>
#include <core/symbolicStates/initialState.h>
#include <core/continuous/approxModel/fbInterpol.h>
#include <core/reachability/postCSequential.h>
#include <utilities/templatePolyhedra.h>
#include <utilities/postAssignment.h>
#include <application/userOptions.h>
#include <boost/algorithm/string.hpp>
#include <io/ioUtility.h>
#include <vector>
#include <list>
#include <z3++.h>
#include <counterExample/abstractCE.h>
#include <core/symbolicStates/initialState.h>
#include <core/reachability/reachability.h>
#include "utilities/statistics.h"



/*
 * A path oriented BMC of Affine HA. Path enumeration is performed
 * with SAT solving. Path feasibility is checked with symbolic
 * RA. A feasible path is searched for concrete counterexample
 * trajectory using trajectory splicing algorithm. An unsuccessful
 * search of a ce along a path, say pi, alters the SAT encoding in
 * such a way that the next paths in the enumeration does not
 * contain the path pi within it. This reduces the candidate set of
 * paths for ce searching.
 */
typedef std::vector<unsigned int> path;
typedef std::vector<unsigned int> Run;
typedef std::list<symbolic_states::ptr> region;
typedef std::pair<int, polytope::ptr> forbidden; //(locId,Polytope)
typedef std::vector<forbidden> forbidden_states; // vector of forbidden symb states.

/* data structures used in the reachability optimization:
 *
 * let a state(v_0,x_0) of the HA can jump to (v_1,x_1) by taking a transition t_1, 
 * where v_0 is the source location, x_0 is the initial set of location v_0, and 
 * x_1= postd(postC(v_0,x_0),(v_0,v_1)).
 *
 * make a pair of locId and it's initial_set pointer <locId,Initial_set>. and it wiil be used as a key to store 
 * the flowpipe of a location. It also used as a data to store the postd(postc(),()) in the 2nd DS.
 */  
 typedef std::pair<unsigned int, polytope::const_ptr> initialInLocation;
/*
 * store flowpipe for a unique pair of location id and it's initial set {<locId,Initial_set>, flowpipe}.
 * whan path-wise reachability computation routine needs to calculate flowpipe, first it search in this data structure.
 * If flowpipe found for a key <locId,Initial_set>, retrieved it. O/W compute the flowpiipe and store it in this data structure.  
 */
typedef std::map<initialInLocation, template_polyhedra::ptr> flowForInitial;  

/*
 * make a pair between flowpipe of a location and out going transtion id from that location,<flowpipe,transId>.
 * It will be used as a key to store the inital set of the target location.  
 */
typedef std::pair<template_polyhedra::ptr, unsigned int> flowWithTid;

/*
 * store initial set of the target location for each unique combination of flow in source location 
 * and outgoing transition from that location {<flowpipe,transId>,<nextLocId,Initial_set>}. This data structure
 * skips the computation of initial set of the target location, if our algoritm finds a match of <flowpipe,transId>.
 * 
 */
typedef std::map<flowWithTid,initialInLocation> nextInitForFlow;

class bmc {

	const hybrid_automata::ptr ha_ptr;
	const std::list<initial_state::ptr>& init;
	const forbidden_states& forbidden_s;
	const ReachabilityParameters& reach_params;
	const userOptions& user_ops;

	reachability reach_for_CE;

	z3::context c;
	z3::expr ha_encoding;
	z3::solver sol;
	//z3::context cc;
	z3::solver s1; //for second solver.

	unsigned int k; // The BMC bound on path-length, i.e., the number of edges.

	unsigned int postc_counter = 0;

	/* bmc uses this private member function to initialize the ha_encoding for the given
	 * forbidden location id
	 */
	void init_solver(unsigned int k1);

	/*
	 * To store all the paths/Runs which are feasible but can't generate a concreteCE or 
	 * infeasible path segment returned by our getPathRechability routine.
	 *
	 */
	vector<path> p_array;
	std::vector<Run> Run_array;


	initialInLocation initialSetInLocation; 
	flowWithTid flowWithTrans_id;
	flowForInitial flowForInit;  
	nextInitForFlow nextInitForFlowWithT_id;
	
public:

	/* To be used for BMC when a set of ha locations together with assoc. polytopes are forbidden
	 * 
	 *
	 */
	bmc(const hybrid_automata::ptr haPtr, const std::list<initial_state::ptr>& init, const forbidden_states& forbidden,
			const ReachabilityParameters& r_params, const userOptions& user_ops);

	virtual ~bmc();
	

	/*
	 * Returns the next unexplored path in the HA from the given source
	 * to the given destination node. The already explored paths are remembered
	 * in the z3 expression ha_encoding. The implementation is a SAT solver
	 * based path enumeration algorithm.
	 */
	path getNextPath(unsigned int k1);

	/*
	 * Returns an integer 0 if the HA is found to be unsafe in the given bound.
	 * a return of non zero(positive) however, does not decide the safety of ha faithfully.
	 * This is due to the incompleteness of the ce search mechanism.
	 * return 1 if the algorithm can't find an abstractCE for the given HA with the given bound i.e
         * model is safe. when the algorithm finds at least one abstractCE but not able to find
         * a concreteCE, it returns 2 means the safety of the model is unknown.
	 */
	unsigned int safe();

	/*
	 * Updates the ha encoding with a given forbidden location in such a way
	 * that the next paths in the enumeration does not return any path containing
	 * the path p.
	 */
	void update_encoding(path p);

	/*
	 * Creates an object of abstract class if an over-approximated flowpipe intersects with the forbidden set.
	 */
	bool createAbstractCE(forbidden forbidden_set, unsigned int i, unsigned int current_locId,
			template_polyhedra::ptr reach_region, symbolic_states::ptr S,
			std::list<abstractCE::ptr> &symbolic_ce_list, abstractCE::ptr abst_ce);

	/*
	 * Checks the computed symblic states in a location is subset or not using ppl package.
	 */
	/*bool containmentChecking(polytope::const_ptr oldNextInit, polytope::const_ptr nextInit); */
	/*
	 * Return all the runs(v_0,T_1,v_1,T_2,...v_n) with different transition from a concrete path(v_0,v_1,...v_n).
	 */
	path getARun(path p);
	/*
	 * Second solver for encoding edges. It encodes possible unique run for a path P. 
	 */
	void second_solver(path p);
	/*
	 * Updates the ha encoding with a given forbidden location in such a way
	 * that the next paths in the enumeration does not return any path containing
	 * the Run run.
	 */
	 void update_run_encoding(Run p);

	 /*
	 * Computes the reachable states along with the given Run run and gets the path feasibility
	 * as a boolean flag. it uses two data structures to skip the reachable states computation 
	 * of the repeated sub path. One is used to store the flowpipe for a location and it's initial set.
	 * another one is used to store the computed symbolic states of the target location. If a location has multiple
	 * initial sets, our algorithm proceeds by taking the tightest superset of these initial sets.
	 * 
	 */
	  region getRunReachability(Run& run, bool& feasible, forbidden forbid_s, std::list<abstractCE::ptr> &symbolic_ce_list); 
	

};

#endif /* SATReach_COUNTEREXAMPLE_BMC_H_ */
