/*
 * omega_abstract_model.h
 *
 *  Created on: 26-Apr-2018
 *      Author: rajarshi
 */

#ifndef APPROX_MODEL_H_
#define APPROX_MODEL_H_

#include <core/continuous/polytope/polytope.h>
#include <core/math/lpSolver/lpSolver.h>
#include <vector>
#include "core/math/matrix.h"

/**
 * Creating the abstract factory class for abstract model in XSpeed. The
 * derived classes implement the various approximation models such as that
 * of SpaceEx.
 *
 * Omega is the approximation of Reach_{0,\delta}(X_0,U), that is the reachable
 * states in the time interval [0,\delta].
 *
 * Psi is the approximation of Reach_{\delta,\delta}({0},U). This is the
 * reachable states at time \delta, due to the effect of the input U on the
 * origin {0}. The principle of super-position says that:
 *
 * Reach_{t,t+\delta}(X_0,U) = e^At * Reach_{0,\delta}(X_0,U) \oplus Reach_{\delta,\delta}({0},U)
 *
 * It says that the reachable states in the time interval [t, t+\delta] can be
 * obtained by applying the linear transformation on Reach_{0,\delta}(X_0,U) to
 * and then bloating this with Reach_{\delta,\delta}({0},U), the reachable
 * states at time \delta under the influence of inputs on the origin {0} .
 */

class approx_model
{

public:

	typedef boost::shared_ptr<approx_model> ptr;

	/** Initializing constructor */
	approx_model(const math::matrix<double>& A, const math::matrix<double>& B, polytope::const_ptr X0,
			polytope::const_ptr U, double delta) : my_A(A), my_B(B), my_X0(X0), my_U(U) {
		this->my_delta = delta;
		my_B.transpose(this->my_B_trans);

		lpX = lp_solver::lp_solver_ptr(new lp_solver());
		lpX->setConstraints(my_X0->getCoeffMatrix(),my_X0->getColumnVector(),1);
		lpU = lp_solver::lp_solver_ptr(new lp_solver());
		lpU->setConstraints(my_U->getCoeffMatrix(),my_U->getColumnVector(),1);
	}
	/** virtual destructor */
	virtual ~approx_model(){};

	/** Returns the time-step */
	inline double get_delta() const {return my_delta;}
	
	/** Returns the dynamics matrix A*/
	inline const math::matrix<double>& get_A() const
	{return my_A;}

	/** Returns the input-transformation matrix B*/
	inline const math::matrix<double>& get_B() const
	{return my_B;}

	/** Returns the initial_set */
	inline polytope::const_ptr get_X0() const{return my_X0;}

	/** Returns the input set */
	inline polytope::const_ptr get_U() const {return my_U;}
	
	/** Computes the support of X0 w.r.t. direction l. */
	virtual double rho_X0(const std::vector<double>& l);

	/** Compute the support of U w.r.t. direction l. */
	virtual double rho_U(const std::vector<double>& l);
	
	/** computes the support function of Omega_0 w.r.t l */
	virtual double first_omega_support(const std::vector<double>& l, double time_step) = 0;
	
	/** Computes the support function of Omega_{i*\delta} w.r.t l
	 *  This is an interface method to be implemented
	 *  by the deriving class.
	 */	
	virtual double omega_support(const std::vector<double>& l, unsigned int iter)  = 0;

private:
	const math::matrix<double>& my_A;
	const math::matrix<double>& my_B;
	math::matrix<double> my_B_trans; // transpose of my_B
	double my_delta;
	polytope::const_ptr my_X0;
	polytope::const_ptr my_U;
	lp_solver::lp_solver_ptr lpX;
	lp_solver::lp_solver_ptr lpU;

	/** Computes the support function of Psi w.r.t l
	 */
	virtual double psi_support(const std::vector<double>& l, unsigned int iter) = 0;

};

#endif /* APPROX_MODEL_H_ */
