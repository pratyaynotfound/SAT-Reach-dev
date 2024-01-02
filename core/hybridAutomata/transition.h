/*
 * Transition.h
 *
 *  Created on: 09-Jul-2014
 *      Author: amit
 */

#ifndef TRANSITION_H_
#define TRANSITION_H_


#include <boost/shared_ptr.hpp>
#include <core/continuous/polytope/polytope.h>


/*
 * Assignment of X' = M x + b
 * X' = R X + w
 */
struct Assign {
	math::matrix<double> Map;
	std::vector<double> b;
};

class transition {
	int trans_id;
	string label;
	int source_location_id;
	int destination_location_id;
	polytope::const_ptr Guard;
	Assign Assign_T;
public:
	typedef boost::shared_ptr<transition> ptr;
	transition();
	~transition(){
	}
	transition(int trans_id, string label, int source_id, int destination_id,
			polytope::const_ptr Guard, Assign Assign_T);
	Assign& getAssignT();
	void setAssignT(Assign assignT);
	int getDestinationLocationId();
	void setDestinationLocationId(int dest_loc_id);
	polytope::const_ptr getGuard() const;
	void setGuard(polytope::ptr guard);
	const string& getLabel() const;
	void setLabel(const string& label);
	int getSourceLocationId();
	void setSourceLocationId(int source_loc_id);
	int getTransitionId() const;
	void setTransitionId(int transId);
	/**
	 * Applies the map of the transition to a start vector
	 * Returns the new mapped vector
	 */
	std::vector<double> applyTransitionMap(std::vector<double> x);

};

#endif /* TRANSITION_H_ */
