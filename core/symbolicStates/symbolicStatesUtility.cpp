/*
 * symbolic_states_utility.cpp
 *
 *  Created on: 04-Dec-2015
 *      Author: amit
 */
#include <core/symbolicStates/symbolicStatesUtility.h>

symbolic_states::ptr searchSymbolic_state(
		std::list<symbolic_states::ptr> symbolic_list,
		symbolic_states::ptr symbolic) {
	symbolic_states::ptr found_symbolic_state;
	std::list<symbolic_states::ptr>::iterator it;
	bool notFound=true;
	for (it = symbolic_list.begin(); it != symbolic_list.end(); it++) {
		//found_symbolic_state = (*it)->getParentPtrSymbolicState();
		found_symbolic_state = (*it);// each (*it) is a symbolic_state from the list
		if (found_symbolic_state == symbolic) {	//need to verify address comparison
			return found_symbolic_state;
		}
	}
	if (notFound){
		std::cout<<"Symbolic state search:: Not Found!!! Issue with Memory_Address_Corroption!!!\n";
		return symbolic;	//should not return ACTUALLY this condition should not arise
	}
}
