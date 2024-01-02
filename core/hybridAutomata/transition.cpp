
#include <core/hybridAutomata/transition.h>

using namespace std;

transition::transition() {
	trans_id = -1;	//indicates Empty Transition
	label = "";
	source_location_id = -1; // default null value
	destination_location_id = -1; // default null value
	math::matrix<double> m;
	Guard = polytope::const_ptr(new polytope()); // universal guard, always true
	unsigned int dim = Guard->map_size();
	m.matrix_Identity(dim, Assign_T.Map);
	Assign_T.b = std::vector<double>(dim,0);
}
transition::transition(int transition_id, string label_name, int source_id,
		int dest_id, polytope::const_ptr guard, Assign assign_Trans) {
	trans_id = transition_id;
	label = label_name;
	source_location_id =source_id;
	destination_location_id = dest_id;
	Guard = guard;
	Assign_T = assign_Trans;
}

Assign& transition::getAssignT() {
	return Assign_T;
}

void transition::setAssignT(Assign assignT) {
	Assign_T = assignT;
	/*Assign_T.Map = assignT.Map;
	 Assign_T.b = assignT.b;	*/
}

polytope::const_ptr transition::getGuard() const {
	return Guard;
}

void transition::setGuard(polytope::ptr guard) {
	Guard = guard;
}

const string& transition::getLabel() const {
	return label;
}

void transition::setLabel(const string& label) {
	this->label = label;
}

int transition::getDestinationLocationId() {
	return destination_location_id;
}

void transition::setDestinationLocationId(int dest_loc_id) {
	destination_location_id = dest_loc_id;
}

int transition::getSourceLocationId() {
	return source_location_id;
}

void transition::setSourceLocationId(int source_loc_id) {
	source_location_id = source_loc_id;
}
int transition::getTransitionId() const {
	return trans_id;
}

void transition::setTransitionId(int transId) {
	trans_id = transId;
}

std::vector<double> transition::applyTransitionMap(std::vector<double> x)
{

	std::vector<double> x_prime(x.size());

	math::matrix<double> R = this->Assign_T.Map;
	std::vector<double> w = this->Assign_T.b;

	assert(x.size() == R.size2());
	assert(x.size() == w.size());

	for (unsigned int i = 0; i < R.size1(); i++) {
		double row_sum = 0.0;
		for (unsigned int j = 0; j < R.size2(); j++) {
			row_sum = row_sum + R(i, j) * x[j];
		}
		x_prime[i] = row_sum + w[i];
	}
	return x_prime;
}
