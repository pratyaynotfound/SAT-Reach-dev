/*
 * transMinkPoly.cpp
 *
 *  Created on: 16-Nov-2014
 *      Author: amit
 */

#include <application/sfUtility.h>
#include <core/continuous/convexSet/transMinkPoly.h>


transMinkPoly::transMinkPoly(polytope::const_ptr myX0, polytope::ptr myU,
		math::matrix<double> myTRANS, math::matrix<double> myB_TRANS,
		double mytime, double mybeta) {
	X0 = myX0;
	U = myU;
	TRANS = myTRANS;
	B_TRANS = myB_TRANS;
	time = mytime;
	beta = mybeta;
	Cempty = true;	//C is empty here as not supplied
}
transMinkPoly::transMinkPoly(polytope::const_ptr myX0, polytope::ptr myU, std::vector<double> c,
			math::matrix<double> myTRANS, math::matrix<double> myB_TRANS, double mytime, double mybeta){
	X0 = myX0;
	if (myU==NULL)
		U=polytope::ptr(new polytope(true));
	else
		U = myU;

	TRANS = myTRANS;
	B_TRANS = myB_TRANS;
	time = mytime;
	beta = mybeta;
	Cempty = false;	//C is NOT empty here as it is supplied
	C = c;
}
transMinkPoly::transMinkPoly(polytope::const_ptr myX0, math::matrix<double> myTRANS) {
	X0 = myX0; //This is POINTER COPY
	U = polytope::ptr(new polytope(true));
	TRANS = myTRANS;
	time = 0;
	beta = 0;
	Cempty = true;	//C is empty here as not supplied
}
unsigned int transMinkPoly::getSystemDimension() const {
	return X0->getSystemDimension();
}

bool transMinkPoly::getIsEmpty() const {
	if (X0->getIsEmpty() && U->getIsEmpty())
		return true;
	else
		return false;
}

double transMinkPoly::computeSupportFunction(const std::vector<double>& direction, lp_solver &lp) const {
//this function is also called from compute_beta, compute_alfa, etc
	std::vector<double> dprime;

	TRANS.mult_vector(direction, dprime);
	double res1 = 0;
	if (!X0->getIsEmpty()) {	
		res1 = X0->computeSupportFunction(dprime, lp);
	}
	
	double res2 = 0.0;
	if (!U->getIsEmpty()) {
		
		lp_solver lp_U(GLPK_SOLVER);
		lp_U.setMin_Or_Max(2);//Maximizing
		lp_U.setConstraints(U->getCoeffMatrix(), U->getColumnVector(), U->getInEqualitySign());

		B_TRANS.mult_vector(direction, dprime);
		res2 = U->computeSupportFunction(dprime, lp_U);
	}
	double res3 = 0.0;	//for C
	if (!Cempty){

		B_TRANS.mult_vector(direction,dprime);
		res3 = dot_product(dprime, C);
	}
	double res = res1 + time * res2 + time * res3;

	if (beta != 0) {
		double dir_norm = support_unitball_infnorm(direction);
		return res + beta * dir_norm;
	} else
		return res;
}

double transMinkPoly::max_norm(int lp_solver_type,
		unsigned int dim_for_Max_norm) const {
	unsigned int dimension_size = dim_for_Max_norm;
	double Max_A, sf, Max = 0.0;

	if (this->getIsEmpty()) {
		sf = 0.0; //returns zero for empty polytope
	}
	/*else if (this->getIsUniverse())
	 throw("\nUniverse Unbounded Polytope!!!\n");
	 */
	else {

		std::vector<std::vector<double> > generator_directions; //this vector is used only in this function not affecting the rest of the codes
		//Generator for Positive Directions for example Right and Up
		for (unsigned int i = 0; i < dimension_size; i++) {
			std::vector<double> directions(dimension_size, 0.0);
			directions[i] = 1; //Positive Generators
			generator_directions.push_back(directions);
		}
		//Generator for Negative Directions for example Left and Down
		for (unsigned int i = 0; i < dimension_size; i++) {
			std::vector<double> directions(dimension_size, 0.0);
			directions[i] = -1; //Negative Generators
			generator_directions.push_back(directions);
		}
		int type = lp_solver_type;
		lp_solver lp(type);

		if (!X0->getIsEmpty()) {
			lp.setMin_Or_Max(2); //Setting GLP_MAX
			lp.setConstraints(X0->getCoeffMatrix(), X0->getColumnVector(), X0->getInEqualitySign());

		}

		//Finding the maximum of all Direction : Returns the max element

		for (unsigned int i = 0; i < generator_directions.size(); i++) {
			std::vector<double> each_generator;
			each_generator = generator_directions[i];
			sf = computeSupportFunction(each_generator, lp);
			Max_A = (abs(sf));
			if (Max_A > Max)
				Max = Max_A;
		}
	}
	return Max;
}
