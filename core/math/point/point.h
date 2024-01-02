/*
 * point.h
 *
 *  Created on: 09-Jun-2016
 *      Author: amit
 */

#ifndef POINT_H_
#define POINT_H_

#include <iostream>
#include <vector>
#include <boost/shared_ptr.hpp>

#include "../../../core/math/numeric/comp.h"

class MyPoint {
	double X;
	double Y;
public:
	typedef boost::shared_ptr<MyPoint> ptr;

	MyPoint();
	MyPoint(double x, double y);
	MyPoint(std::vector<double> pt);

	MyPoint::ptr operator=(const MyPoint::ptr& p2);


	void setPoint(double x, double y);
	void setPoint_fromVector(std::vector<double> pt);
	std::vector<double> getPoint_asVector();
	//compare point p with the calling point object
	bool isEqual(MyPoint::ptr p);
	void printPoint();
};

#endif /* POINT_H_ */
