#include <core/discrete/discreteSet/discreteSet.h>


const std::set<int>& discrete_set::getDiscreteElements() const {
	return discrete_elements;
}

void discrete_set::setDiscreteElements(
		const std::set<int>& discreteElements) {
	discrete_elements = discreteElements;
}

void discrete_set::insert_element(int element){
	discrete_elements.insert(element);	//inserted an element in the set variable
}





void discrete_set::union_set(discrete_set d){
	;
}
void discrete_set::intersection_set(discrete_set d){
	;
}
bool discrete_set::contains(discrete_set d){
	return false;
}


