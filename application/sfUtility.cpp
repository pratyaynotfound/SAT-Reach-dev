/*
 * sf_utility.cpp
 *
 *  Created on: 14-Dec-2014
 *      Author: amit
 */

#include <application/sfUtility.h>


/* Change to remove matrix_exponentiation. Pass as Parameter structure */
double Omega_Support(const ReachabilityParameters& ReachParameters,
		std::vector<double> direction, supportFunctionProvider::ptr Initial_X0,
		Dynamics& system_dynamics, lp_solver &lp, lp_solver &lp_U, int Min_Or_Max) {

	double res1;

	res1 = Initial_X0->computeSupportFunction(direction, lp);

	std::vector<double> trans_dir;
	math::matrix<double> A, B, B_trans;
	math::matrix<double> phi_tau_Transpose;
	std::vector<double> r(direction.size(),0);

	//system_dynamics.MatrixA.matrix_exponentiation(phi_tau,tau);
	phi_tau_Transpose = ReachParameters.phi_trans;
	phi_tau_Transpose.mult_vector(direction, r);	//is this not repeated ? Sir


	double term1, term2, term3, term3a, term3b, res2;

	// todo: changed the above line to below to avoid runtime error
	term1 = Initial_X0->computeSupportFunction(direction, lp);


	// transpose to be done once and stored in the structure of parameters */
	B_trans = ReachParameters.B_trans;

	// todo: changed the above line to below to avoid runtime error
	term2 = ReachParameters.time_step* system_dynamics.U->computeSupportFunction(direction, lp_U);

	// compute alpha once and put it inside reach parameter
	term3a = ReachParameters.result_alfa;//compute_alfa(ReachParameters.time_step,system_dynamics,Initial_X0);

	term3b = support_unitball_infnorm(direction);
	term3 = term3a * term3b;


	res2 = term1 + term2 + term3;

	if (res1 > res2)
		return res1;
	else
		return res2;
}

