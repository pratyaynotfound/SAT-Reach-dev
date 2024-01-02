/*
 * pwlist.h
 *
 *  Created on: 03-Dec-2015
 *      Author: amit
 */

#ifndef PWLIST_H_
#define PWLIST_H_

#include <boost/shared_ptr.hpp>
#include <core/symbolicStates/initialState.h>

/*
 * Passed-Waiting-List of initial_states. A class that handles Queue data_structure of
 *  initial_states to be processed or that has already been processed
 */

class pwlist {
public:
	typedef boost::shared_ptr<pwlist> ptr;

	pwlist();

	~pwlist();


	//deletes an initial state from the front of the list and returns the deleted initial state
	initial_state::ptr WaitingList_delete_front();

	//inserts a initial state at the end of the passed_list
	void PassedList_insert(initial_state::ptr s);

	//inserts an initial state at the end of the waiting_list
	void WaitingList_insert(initial_state::ptr s);



	unsigned int getWaitingListSize() const;
	//return TRUE if the waiting_list is empty otherwise FALSE
	bool isEmpty_WaitingList();
	const std::list<initial_state::ptr>& getPassedList() const;
	void setPassedList(const std::list<initial_state::ptr>& passedList);
	const std::list<initial_state::ptr>& getWaitingList() const;
	void setWaitingList(const std::list<initial_state::ptr>& waitingList);

private:
	std::list<initial_state::ptr> waiting_list;
	std::list<initial_state::ptr> passed_list;
	unsigned int waiting_list_size;	//number of elements currently present in the waiting_list
};

#endif /* PWLIST_H_ */
