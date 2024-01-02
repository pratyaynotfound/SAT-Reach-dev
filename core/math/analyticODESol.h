/*
 * analyticODESol.h
 *
 *  Created on: 06-Sep-2016
 *      Author: rajarshi
 */

#ifndef ANALYTICODESOL_H_
#define ANALYTICODESOL_H_

#include <application/structures.h>
#include <vector>

/*
 * Returns the solution of an ODE of the form X' = A*X+b at some time
 */
std::vector<double> ODESol(std::vector<double> x0, const Dynamics& D, double time);
math::matrix<double> ODESol_homogenous_coeff(const Dynamics& D, double time);
std::vector<double> ODESol_inhomogenous(const Dynamics& D, double time);

/**
  * A utility method for the following two methods.
  */
void extended_expm(const math::matrix<double>& A, double time, math::matrix<double>& res);

/*
 * compute the expression A^-1 (e^At - I) alternatively as a sub-matrix of the
 * expanded_exp(M), as shown in the SpaceEx paper, page 8, phi_1.
 */
math::matrix<double> time_slice_component(const math::matrix<double>& A, double time);

/*
 * compute the expression A^-2 (e^At - I - \delta.A) alternatively as a 
 * sub-matrix of the extended_exp(M), as shown in the SpaceEx paper, 
 * page 8, phi_2.
 */
math::matrix<double> get_phi_2(math::matrix<double>& A, double time);

#endif /* ANALYTICODESOL_H_ */
