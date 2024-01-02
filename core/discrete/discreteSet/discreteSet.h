/*
 * discrete_set.h
 *
 *  Created on: 12-Nov-2014
 *      Author: amit
 */

#include <set>
#ifndef DISCRETE_SET_H_
#define DISCRETE_SET_H_


class discrete_set
{
public:
	void union_set(discrete_set d);
	void intersection_set(discrete_set d);
	bool contains(discrete_set d);

	const std::set<int>& getDiscreteElements() const;
	void setDiscreteElements(const std::set<int>& discreteElements);

	void insert_element(int element);
private:
	std::set<int> discrete_elements;
};



#endif /* DISCRETE_SET_H_ */
