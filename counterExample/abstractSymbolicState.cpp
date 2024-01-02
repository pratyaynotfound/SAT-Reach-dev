#include <counterExample/abstractSymbolicState.h>

abstract_symbolic_state::abstract_symbolic_state() {
	;
}
abstract_symbolic_state::abstract_symbolic_state(discrete_set& discreteSet,
	polytope::ptr continuousSet, polytope::const_ptr polyI) {
	DiscreteSet = discreteSet;
	ContinuousSet = continuousSet;
	I = polyI;
}

polytope::ptr abstract_symbolic_state::getContinuousSet() const {
	return ContinuousSet;
}

void abstract_symbolic_state::setContinuousSet(polytope::ptr continuousSet) {
	ContinuousSet = continuousSet;
}

const discrete_set& abstract_symbolic_state::getDiscreteSet() const {
	return DiscreteSet;
}

void abstract_symbolic_state::setDiscreteSet(const discrete_set& discreteSet) {
	DiscreteSet = discreteSet;
}
