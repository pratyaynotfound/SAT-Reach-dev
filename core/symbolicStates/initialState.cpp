#include <core/symbolicStates/initialState.h>

initial_state::initial_state() {
	;
}

initial_state::initial_state(int locationId, polytope::const_ptr initialSet) {
	location_id = locationId;
	initial_set = initialSet;
}

initial_state::initial_state(int locationId, polytope::const_ptr initialSet,
		symbolic_states::ptr parentPtrSymbolicState, int transitionId) {
	location_id = locationId;
	initial_set = initialSet;
	parentPtr_symbolic_state = parentPtrSymbolicState;
	transition_id = transitionId;
}

initial_state::~initial_state() {
	;
}

polytope::const_ptr initial_state::getInitialSet() const {
	return initial_set;
}

void initial_state::setInitialSet(const polytope::const_ptr initialSet) {
	initial_set = initialSet;
}

int initial_state::getLocationId() const {
	return location_id;
}

void initial_state::setLocationId(int locationId) {
	location_id = locationId;
}

int initial_state::getTransitionId() const {
	return transition_id;
}

void initial_state::setTransitionId(int transitionId) {
	transition_id = transitionId;
}

const symbolic_states::ptr initial_state::getParentPtrSymbolicState() const {
	return parentPtr_symbolic_state;
}

void initial_state::setParentPtrSymbolicState(
		const symbolic_states::ptr parentPtrSymbolicState) {
	parentPtr_symbolic_state = parentPtrSymbolicState;
}
