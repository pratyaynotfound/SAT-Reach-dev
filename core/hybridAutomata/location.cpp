/*
 * Location.cpp
 *
 *  Created on: 09-Jul-2014
 *      Author: amit
 */

#include <core/hybridAutomata/location.h>
//#include "core_system/HybridAutomata/Transition.h"

using namespace std;

location::location() {
	Name = "";
}

location::location(int Loc_ID, string name, Dynamics system_dynamics, polytope::ptr invariant, bool inv_exists, std::list<transition::ptr> Out_Going_Trans) {
	loc_id = Loc_ID;
	Name = name;
	System_Dynamics = system_dynamics;
	Invariant = invariant;
	InvariantExist = inv_exists;
	Out_Going_Transitions = Out_Going_Trans;
}

const Dynamics& location::getSystemDynamics() const {
	return System_Dynamics;
}

Dynamics& location::getSystemDynamics() {
	return System_Dynamics;
}
void location::setSystemDynamics(const Dynamics& system_dynamics) {
	System_Dynamics = system_dynamics;
}

polytope::const_ptr location::getInvariant() const {
	return Invariant;
}

void location::setInvariant(polytope::ptr invariant) {
	Invariant = invariant;
}

const string& location::getName() const {
	return Name;
}

void location::setName(const string& name) {
	this->Name = name;
}

int location::getLocId() const {
	return loc_id;
}

void location::setLocId(int locId) {
	loc_id = locId;
}

const std::list<transition::ptr>& location::getOutGoingTransitions() const {
	return Out_Going_Transitions;
}
void location::addOutGoingTransition(transition::ptr t){
	this->Out_Going_Transitions.push_back(t);
		//Adj_Transitions.max_size()		//returns the size of the adjacent transitions/locations
}

transition::ptr location::getTransition(int trans_id) const {
	transition::ptr temp;
	std::list<transition::ptr>::const_iterator it;
	for (it=Out_Going_Transitions.begin(); it != Out_Going_Transitions.end(); it++){
		int transID = (*it)->getTransitionId();
		if (transID==trans_id){
			temp = (*it);
		}
	}
	return temp;
}

bool location::getInvariantExist() const {
	return InvariantExist;
}

void location::setInvariantExist(bool invariantExist) {
	this->InvariantExist = invariantExist;
}


