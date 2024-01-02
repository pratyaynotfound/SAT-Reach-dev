/*
 * themeSelector.h
 *
 *  Created on: 19-Sept-2021
 *      Author: Atanu
 */

#ifndef SATReach_THEMESELECTOR_H_
#define SATReach_THEMESELECTOR_H_

#include <core/hybridAutomata/hybridAutomata.h>
#include <core/symbolicStates/initialState.h>
#include <core/continuous/polytope/polytope.h>
#include <application/userOptions.h>
#include <utilities/cpuUtilities/cpuUtilities.h>	//cpu usage functions
#include <utilities/memUtilities/memUsage.h> //memory usage functions
#include <boost/algorithm/string/predicate.hpp>
#include "utilities/statistics.h"
#include <counterExample/bmc.h>
#include <counterExample/composition.h> //for composition class
#include <counterExample/SATbmc.h>

/*
 * Call BMC procedure of the tool:
 * 1. selectfal()
 */

typedef std::pair<int, polytope::ptr> forbidden; //(locId,Polytope1)
typedef std::vector<forbidden> forbidden_states; // vector of forbidden symb states.

class themeSelector {

public:
	themeSelector();
	void setUserOps(userOptions& op);
	void setInit(std::list<initial_state::ptr>& init);
	void setReachParams(ReachabilityParameters& params);
	void setForbidden(forbidden_states& forbidden);
	userOptions& getUserOps();
	static hybrid_automata::ptr getHaInstance();
	std::list<initial_state::ptr>& getInit();
	ReachabilityParameters& getReachParams();
	forbidden_states& getForbidden();

	/*
	 * Selects BMC based on user option
	 */
	void select();

	/* selects BMC routine */
	void selectFal();

	/* selects BMC routine */
	void selectCompose();
	void selectSATbmc();

	virtual ~themeSelector();
	// static member
	static hybrid_automata::ptr ha_ptr;
	static std::list<hybrid_automata::ptr> ha_ptrs;
	static std::list<initial_state::ptr> init_components;

private:
	std::list<initial_state::ptr> init;
	forbidden_states forbidden;
	userOptions userOps;
	ReachabilityParameters reach_params;
};

//definition of a static member variable

#endif /* SATReach_THEMESELECTOR_H_ */
