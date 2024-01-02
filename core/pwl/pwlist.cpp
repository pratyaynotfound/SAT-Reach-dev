#include <core/pwl/pwlist.h>

pwlist::pwlist() {
	;
}

pwlist::~pwlist() {
	;
}

//deletes a symbolic state from the front of the list and returns the deleted symbolic state
initial_state::ptr pwlist::WaitingList_delete_front() {
	initial_state::ptr s;

	s = waiting_list.front();
	waiting_list.pop_front();
	return s;
}

//inserts a symbolic state at the end of the passed_list
void pwlist::PassedList_insert(initial_state::ptr s) {
	passed_list.push_back(s);
}

//inserts a symbolic state at the end of the waiting_list
void pwlist::WaitingList_insert(initial_state::ptr s) {
	waiting_list.push_back(s);
}

void pwlist::setWaitingList(const std::list<initial_state::ptr>& waitingList) {
	waiting_list = waitingList;
}

const std::list<initial_state::ptr>& pwlist::getWaitingList() const {
	return waiting_list;
}

void pwlist::setPassedList(const std::list<initial_state::ptr>& passedList) {
	passed_list = passedList;
}

const std::list<initial_state::ptr>& pwlist::getPassedList() const {
	return passed_list;
}

bool pwlist::isEmpty_WaitingList() {
	if (waiting_list.empty()) //list is empty
		return true;
	else
		return false;
}

unsigned int pwlist::getWaitingListSize() const {
	return waiting_list.size();;
}
