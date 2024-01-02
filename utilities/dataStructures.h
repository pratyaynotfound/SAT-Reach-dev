/*
 * UtilitiesDataStructure.h
 *
 *  Created on: 30-Jan-2017
 *      Author: amit
 */

#ifndef UTILITIESDATASTRUCTURE_H_
#define UTILITIESDATASTRUCTURE_H_
#include <core/math/lpSolver/lpSolver.h>
#include <vector>
//Data structure required for each Invariant-Boundary-Check
struct InvariantCheckData{

	std::vector<double> r_minus;	//negative_invariant directions
	std::vector<double> r1_minus;
	std::vector<double> phi_trans_dir_minus;
	double invariant_bound;
	double res1_minus;
	double term1_minus;
	double term2_minus;
	double term3_minus;
	double term3b_minus;
	double term3c_minus;
	double TempOmega_min;

	//lp_solver::lp_solver_ptr I_minus, U_minus; //using independent glpk object for each invariant for the computation of all iterations
	lp_solver I_minus, U_minus; //using independent glpk object for each invariant for the computation of all iterations

	double result1_minus;
	double res_beta_minus;

	double sVariable_min, s1Variable_min;

	//bool invariantCrossed;	//  = false;
	//unsigned int total_iter; //actual iterations to be computed. Flowpipe Cost

};

#endif /* UTILITIESDATASTRUCTURE_H_ */
