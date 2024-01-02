/*
 * symbolic_states_utility.h
 *
 *  Created on: 04-Dec-2015
 *      Author: amit
 */

#ifndef SYMBOLIC_STATES_UTILITY_H_
#define SYMBOLIC_STATES_UTILITY_H_

#include <core/symbolicStates/symbolicStates.h>

/*
 * Returns a symbolic state after searching in the given symbolic_list
 */
symbolic_states::ptr searchSymbolic_state(
		std::list<symbolic_states::ptr> symbolic_list,
		symbolic_states::ptr symbolic);

#endif /* SYMBOLIC_STATES_UTILITY_H_ */
