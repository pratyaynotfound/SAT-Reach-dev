#include <core/symbolicStates/symbolicStates.h>

symbolic_states::symbolic_states() {
	this->transition_id  = 0;

}
/*
 symbolic_states::symbolic_states(discrete_set DiscreteSet, polytope::ptr ContinuousSet){
 this->DiscreteSet = DiscreteSet;
 this->ContinuousSetptr = ContinuousSet;
 }

 polytope::ptr symbolic_states::getContinuousSet() {
 return ContinuousSetptr;
 }

 void symbolic_states::setContinuousSet(const polytope::ptr continuousSet) {
 this->ContinuousSetptr = continuousSet;
 }
 */
symbolic_states::symbolic_states(discrete_set DiscreteSet,
		template_polyhedra::ptr ContinuousSet) {
	this->DiscreteSet = DiscreteSet;
	this->ContinuousSetptr = ContinuousSet;
}

symbolic_states::symbolic_states(discrete_set DiscreteSet,
		template_polyhedra::ptr ContinuousSet,
		symbolic_states::ptr parentPtr_symbolic_state, int transition_id) {
	this->DiscreteSet = DiscreteSet;
	this->ContinuousSetptr = ContinuousSet;
	this->parentPtr_symbolic_state = parentPtr_symbolic_state;
	this->transition_id = transition_id;
}

const discrete_set& symbolic_states::getDiscreteSet() const {
	return DiscreteSet;
}

template_polyhedra::ptr symbolic_states::getContinuousSetptr() const {
	return ContinuousSetptr;
}

void symbolic_states::setDiscreteSet(const discrete_set& discreteSet) {
	this->DiscreteSet = discreteSet;
}

void symbolic_states::setContinuousSetptr(
		template_polyhedra::ptr continuousSetptr) {
	ContinuousSetptr = continuousSetptr;
}

symbolic_states::ptr symbolic_states::getParentPtrSymbolicState() const {
	return parentPtr_symbolic_state;
}

void symbolic_states::setParentPtrSymbolicState(
		symbolic_states::ptr parentPtrSymbolicState) {
	parentPtr_symbolic_state = parentPtrSymbolicState;
}

int symbolic_states::getTransitionId() const {
	return transition_id;
}

void symbolic_states::setTransitionId(int transitionId) {
	transition_id = transitionId;
}

polytope::const_ptr symbolic_states::getInitialPolytope() const {
	return initial_polytope;
}
