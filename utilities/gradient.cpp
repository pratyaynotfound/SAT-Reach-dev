/*
 * gradient.cpp
 *
 *  Created on: 18-Aug-2016
 *      Author: rajarshi
 */

#include "../utilities/gradient.h"

/*
 * Computes derivative of distance of a vector to a polytope w.r.t the vector.
 * The definition of point to polytope distance is as defined in the
 * polytope.cpp class implementation
 */
std::vector<double> dist_grad(std::vector<double> trace_end_pt, polytope::const_ptr I) {
	if(I->getIsUniverse() || I->getIsEmpty())
		return std::vector<double>(trace_end_pt.size(),0);

	std::vector<double> grad(trace_end_pt.size(),0 );

	math::matrix<double> C = I->getCoeffMatrix();
	std::vector<double> b = I->getColumnVector();

	assert(trace_end_pt.size() == C.size2());
	assert(I->getInEqualitySign() == 1);

	double facet_distance = 0;
	double coef_sq_sum = 0;
	double dist = 0;
	for(unsigned int i=0;i<C.size1();i++){
		for(unsigned int j=0;j<C.size2();j++){
			facet_distance += trace_end_pt[j]*C(i,j);
			coef_sq_sum += C(i,j)*C(i,j);
		}
		double denom = coef_sq_sum;
		facet_distance -=b[i];
		if(facet_distance > 1e-8){
			for(unsigned int k=0;k<grad.size();k++){
				grad[k] += C(i,k)/denom;
			}
			dist += facet_distance/denom;
		}
		coef_sq_sum = 0;
		facet_distance = 0;
	}
	// for experimenting with squared distance
	for(unsigned int i=0;i<grad.size();i++)
			grad[i] = 2*dist*grad[i];

	return grad;
}
