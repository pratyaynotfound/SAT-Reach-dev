#ifndef SATBMC_H_
#define SATBMC_H_

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

typedef std::pair<int, polytope::ptr> forbidden; //(locId,Polytope)
typedef std::vector<forbidden> forbidden_states; // vector of forbidden symb states.
typedef std::vector<std::vector<std::string>> Path;

class SATbmc {
    const std::list<hybrid_automata::ptr> ha_ptrs;
	const std::list<initial_state::ptr>& init;
	const forbidden_states& forbidden_s;
	const ReachabilityParameters& reach_params;
	const userOptions& user_ops;

    z3::context c;
	z3::expr ha_encoding;
    z3::solver S;
    int sttr_count = 0;
    

    unsigned int k; // The BMC bound on path-length, i.e., the number of edges.
	
public:
    SATbmc(const std::list<hybrid_automata::ptr>& haPtr, const std::list<initial_state::ptr>& init, const forbidden_states& forbidden,
				ReachabilityParameters& r_params, const userOptions& user_ops);

    void mk_constrains(unsigned int, hybrid_automata::ptr, int);
    void print_path(Path);
    void make_constraints(int);
    Path get_nextPath(unsigned int);
    unsigned int safe();
	void _negation(Path);

};



#endif