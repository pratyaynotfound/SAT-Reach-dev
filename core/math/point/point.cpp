#include "../../../core/math/point/point.h"

MyPoint::MyPoint() {
	X = -999; //default value
	Y = -999; //default value
}

MyPoint::MyPoint(double x, double y) {
	X = x;
	Y = y;
}

MyPoint::MyPoint(std::vector<double> pt) {
	X = pt[0];
	Y = pt[1];
}

MyPoint::ptr MyPoint::operator=(const MyPoint::ptr& p2) {

	MyPoint::ptr p;
	p->X = p2->X;
	p->Y = p2->Y;

	return p;
}

void MyPoint::setPoint(double x, double y) {
	X = x;
	Y = y;
}

void MyPoint::setPoint_fromVector(std::vector<double> pt) {
	X = pt[0];
	Y = pt[1];
}

std::vector<double> MyPoint::getPoint_asVector() {
	std::vector<double> v(2);
	v[0] = X;
	v[1] = Y;
	return v;
}

//compare point p with the calling point object
bool MyPoint::isEqual(MyPoint::ptr p) {
	//bool itisEqual=false;
	if (math::numeric::is_MEQ(this->X, p->X)
			&& math::numeric::is_MEQ(this->Y, p->Y))
		return true;
	else
		return false;
}

void MyPoint::printPoint() {
	std::cout << " (" << X << " , " << Y << " ) \n";
}
