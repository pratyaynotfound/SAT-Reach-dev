/*
 * nlpFunctions.cpp
 *
 *  Contains the NLP instantiation methods and the various objective functions. Each objective function
 *  is to evaluate a different trajectory splicing method.
 *
 *  Created on: 25-Sep-2016
 *      Author: rajarshi
 */

#include <application/structures.h>
#include <core/continuous/polytope/polytope.h>
#include "nlpFunctions.h"
#include "vector"

#include "core/math/analyticODESol.h"
#include "abstractCE.h"

extern std::vector<double> start_pos; // the start vectors of opt trajectory from LP solving. Useful in the context of CE_ALGO_TYPE 5

double myconstraint(const std::vector<double> &x, std::vector<double> &grad,
		void *data) {
	polyConstraints *d = (polyConstraints *)data;
	unsigned int id = d->symb_state_index;

	assert(id >= 0 && id < N);
	assert(d->a.size() == dim);

	if (!grad.empty()) {
		for (unsigned int i = 0; i < dim; i++)
			grad[i] = 0;

		for (unsigned int j = 0; j < dim; j++)
			grad[id * dim + j] = d->a[j];
	}

	double sum = 0;
	for (unsigned int j = 0; j < dim; j++) {
		sum += x[id * dim + j] * d->a[j];
	}
	sum = sum - d->b;
	return sum;
}

/* objective function */

double myobjfunc2(const std::vector<double> &x, std::vector<double> &grad,
		void *my_func_data) {

	// 1. Get the N start vectors and dwell times from x and call the simulation routine
	// 2. Get the N end points of the simulation trace, say, y[i].
	// 3. Compute the Euclidean distances d(y[i],y[i+1]) and sum them up.
	// Computes the L2 norm or Euclidean distances between the trace end points.
	//-----------------------------------------

	/*This is the vector of end-points of N trajectory segments */
	std::vector<std::vector<double> > y(N);

	double cost = 0;
	std::vector<double> deriv(x.size(), 0); // contains the gradient, all initialized to 0

	std::list<transition::ptr>::iterator T_iter = transList.begin();
	transition::ptr Tptr = *(T_iter);

	math::matrix<double> A, expAt, mapExpAt;
	std::vector<double> Axplusb(dim), mapAxplusb;

	std::ofstream myfile;

	polytope::const_ptr I;

	for (unsigned int i = 0; i < N; i++) {

		// dxli: v is a copy of state variables. (Get the N start vectors)
		std::vector<double> v(dim, 0);
		for (unsigned int j = 0; j < dim; j++) {
			v[j] = x[i * dim + j];
		}

		int loc_index = locIdList[i];
		Dynamics d = HA->getLocation(loc_index)->getSystemDynamics();
		I = HA->getLocation(loc_index)->getInvariant();
		assert(d.C.size() == dim);

		std::vector<double> traj_dist_grad(dim, 0); // holds the grads of the trajectories distance to invariant

		// dxli: simulate the starting point in the current location (v), for some time (x[N*dim + i]) over some dynamics (d)
		// dxli: analytic solution, rather ODE solver call. This closed form is true only when the input set is singular.
		y[i] = ODESol(v, d, x[N * dim + i]);
		// patch for constant dynamics

		if (d.isEmptyMatrixA) {
			A = math::matrix<double>(dim, dim);
			for (unsigned int i = 0; i < dim; i++) {
				for (unsigned int j = 0; j < dim; j++) {
					A(i, j) = 0;
				}
			}
		} else
			A = d.MatrixA;
		// end of patch

		assert(d.C.size() == dim);

		// dxli: some initialization for calculating derivatives
		math::matrix<double> At(A);
		At.scalar_multiply(x[N * dim + i]);
		At.matrix_exponentiation(expAt);

		A.mult_vector(y[i], Axplusb);
		for (unsigned int j = 0; j < dim; j++) {
			Axplusb[j] = Axplusb[j] + d.C[j];
		}

//		For validation, the distance of trace end points from the invariant is
//		added to the cost

		std::vector<double> inv_dist_grad(dim, 0);

		cost += I->point_distance(y[i]); // end point distance to invariant added to cost

		inv_dist_grad = dist_grad(y[i], I);

		for (unsigned int j = 0; j < dim; j++) {
			double dist_gradx_j = 0;
			for (unsigned int k = 0; k < dim; k++) {
				dist_gradx_j += inv_dist_grad[k] * expAt(k, j);
			}
			deriv[i * dim + j] += dist_gradx_j;

		}
//		 add the cost gradient w.r.t traj segment's dwell time
		double dist_gradt = 0;
		for (unsigned int j = 0; j < dim; j++) {
			dist_gradt += inv_dist_grad[j] * Axplusb[j];
		}
		deriv[N * dim + i] += dist_gradt;

		//end of validation logic
		if (i == N - 1) {
			// compute the distance of this endpoint with the forbidden polytope \cap invariant (the segment end point must lie
			// in the intersection of the bad_set and the last location invariant).

			cost += bad_poly->point_distance(y[N - 1]);

			std::vector<double> badpoly_dist_grad(dim, 0);

			badpoly_dist_grad = dist_grad(y[N - 1], bad_poly);

			for (unsigned int j = 0; j < dim; j++) {
				double dist_gradx_j = 0;
				for (unsigned int k = 0; k < dim; k++) {
					dist_gradx_j += badpoly_dist_grad[k] * expAt(k, j);
				}
				deriv[(N - 1) * dim + j] += dist_gradx_j;
			}

			//	add the cost gradient w.r.t last traj segment's dwell time

			double dist_gradt = 0;
			for (unsigned int j = 0; j < dim; j++) {
				dist_gradt += badpoly_dist_grad[j] * Axplusb[j];
			}
			deriv[N * dim + N - 1] += dist_gradt;

			break;
		} else {
			polytope::const_ptr g;
			Assign R;
			math::matrix<double> mapExpAt(expAt);
			// assign the transition pointer
			Tptr = *(T_iter);
			// assignment of the form: Rx + w

			R = Tptr->getAssignT();
			//guard as a polytope
			g = Tptr->getGuard();

			std::vector<double> mapderiv(Axplusb);

			// guard \cap invariant distance, to address Eq. (12) in CDC 13' paper
			polytope::ptr guard_intersect_inv;
			guard_intersect_inv = I->GetPolytope_Intersection(g);

			//-----------------------------------------------------
			double guard_dist = guard_intersect_inv->point_distance(y[i]);
//			double guard_dist = I->point_distance(y[i]);
			cost += guard_dist;

			std::vector<double> guard_dist_grad(dim, 0);
			guard_dist_grad = dist_grad(y[i], guard_intersect_inv);

			for (unsigned int j = 0; j < dim; j++) {
				double dist_gradx_j = 0;
				for (unsigned int k = 0; k < dim; k++) {
					dist_gradx_j += guard_dist_grad[k] * expAt(k, j);
				}
				deriv[i * dim + j] += dist_gradx_j;

			}

			// dxli: add derivative of guard \cup invariant wrt dwell time
			double dist_gradt = 0;
			for (unsigned int j = 0; j < dim; j++) {
				dist_gradt += guard_dist_grad[j] * Axplusb[j];
			}
			deriv[N * dim + i] += dist_gradt;

			assert(y[i].size() == R.Map.size2());
			std::vector<double> transform(y[i].size(), 0);
			R.Map.mult_vector(y[i], transform);
			for (unsigned int j = 0; j < transform.size(); j++)
				y[i][j] = transform[j] + R.b[j];

			R.Map.multiply(expAt, mapExpAt);
			R.Map.mult_vector(Axplusb, mapAxplusb);

			assert(y[i].size() == R.b.size());

			T_iter++; // Moving to the next transition.

			//compute the Euclidean distance between the next start point and the simulated end point
			for (unsigned int j = 0; j < dim; j++) {
				cost += (y[i][j] - x[(i + 1) * dim + j])
						* (y[i][j] - x[(i + 1) * dim + j]);

				for (unsigned int k = 0; k < dim; k++) {
					deriv[i * dim + j] += 2 * (y[i][k] - x[(i + 1) * dim + k])
							* mapExpAt(k, j);
				}
				if (i != 0) {
					deriv[i * dim + j] += -2 * (y[(i - 1)][j] - x[i * dim + j]);
				}
				deriv[N * dim + i] += 2 * (y[i][j] - x[(i + 1) * dim + j])
						* mapAxplusb[j];
			}
		}

	} // End of for-loop over i

	if (N != 1) { // The abstract CE has at least one discrete jump, and therefore at least one splicing point.
		for (unsigned int j = 0; j < dim; j++)
			deriv[(N - 1) * dim + j] = -2
					* (y[N - 2][j] - x[(N - 1) * dim + j]);
	}

#ifdef VALIDATION
// add to cost the distance of refinement points. Modify the derivatives.

	for (std::list<refinement_point>::iterator it = ref_pts.begin();
			it != ref_pts.end(); it++) {
		refinement_point p = *it;
		assert(p.seq_no < N);
		unsigned int locID = locIdList[p.seq_no];

		double dwell_time = x[N * dim + p.seq_no];
		// if the sampled time is less than the time when the refinement point violated the Inv, then ignore this refinement point
		if (dwell_time < p.time)
			continue;
		// Compute the new point v at p.time units from the new sampled start point.
		const Dynamics& d = HA->getLocation(locID)->getSystemDynamics();
		polytope::const_ptr I = HA->getLocation(locID)->getInvariant();
		assert(d.C.size() == dim);

		std::vector<double> v(dim);
		for (unsigned int i = 0; i < dim; i++)
			v[i] = x[p.seq_no * dim + i];

		v = ODESol(v, d, p.time);
		// patch for constant dynamics

		math::matrix<double> A;
		if (d.isEmptyMatrixA) {
			A = math::matrix<double>(dim, dim);
			for (unsigned int i = 0; i < dim; i++) {
				for (unsigned int j = 0; j < dim; j++) {
					A(i, j) = 0;
				}
			}
		} else
			A = d.MatrixA;

		double dist = I->point_distance(v);

		if (dist > 0) {
			cost += dist;
			math::matrix<double> expAt;
			A.matrix_exponentiation(expAt, p.time);

			std::vector<double> I_dist_grad(dim);
			I_dist_grad = dist_grad(v, I);

			for (unsigned int j = 0; j < dim; j++) {
				double I_dist_grad_j = 0;
				for (unsigned int k = 0; k < dim; k++) {
					I_dist_grad_j += I_dist_grad[k] * expAt(k, j);
				}
				deriv[p.seq_no * dim + j] += I_dist_grad_j;
			}
			// derivative wrt dwell time
			std::vector<double> Axplusb;

			A.mult_vector(v, Axplusb);
			for (unsigned int j = 0; j < dim; j++) {
				Axplusb[j] = Axplusb[j] + d.C[j];
			}

			double dist_gradt = 0;
			for (unsigned int j = 0; j < dim; j++) {
				dist_gradt += I_dist_grad[j] * Axplusb[j];
			}

			deriv[N * dim + p.seq_no] += dist_gradt;
		}
	}
#endif

	// Analytic gradients
	if (!grad.empty()) {
		for (unsigned int i = 0; i < x.size(); i++) {
			grad[i] = deriv[i];
		}
	}
	//std::cout << "cost = " << cost << std::endl;
	return cost;

}

/*
 * objective function for the nlp of Alt-Min algorithm.
 */
double myobjfuncIterativeNLP(const std::vector<double> &t,
		std::vector<double> &grad, void *my_func_data) {

// 1. Get the N start vectors (fixed-constants) and dwell times (variables) and get the endpoints, say y[i] by ODE solving.
// 2. Get the N end points of the simulation trace, say, y[i].
// 3. Compute the Euclidean distances d(y[i],y[i+1]) and sum them up.
// Computes the L2 norm or Euclidean distances between the splicing points.
//-----------------------------------------

	assert(t.size() == N);
	std::vector<double> *x = reinterpret_cast<std::vector<double>*>(my_func_data);

	/*This is the vector of end-points of N trajectory segments to be computed*/
	std::vector<std::vector<double> > y(N);
	double cost = 0;
	std::vector<double> deriv(N, 0); // contains the gradient, all initialized to 0.

	std::list<transition::ptr>::iterator T_iter = transList.begin();
	transition::ptr Tptr = *(T_iter);
	math::matrix<double> A;
	std::vector < double > myAxPlusb(dim), mapAxplusb;

	polytope::const_ptr I;

	for (unsigned int i = 0; i < N; i++) {

		std::vector<double> v(dim, 0);
		for (unsigned int j = 0; j < dim; j++) {
			v[j] = x[i][j];
		}

		int loc_index = locIdList[i]; //global variable populated already
		Dynamics d = HA->getLocation(loc_index)->getSystemDynamics();
		I = HA->getLocation(loc_index)->getInvariant();
		assert(d.C.size() == dim);

		std::vector<double> traj_dist_grad(dim, 0); // holds the grads of the trajectories distance to invariant

		//dxli: analytic solution, rather ODE solver call. This closed form is true only when the input set is singular.
		y[i] = ODESol(v, d, t[i]);
		// patch for constant dynamics

		if (d.isEmptyMatrixA) {
			A.resize(dim,dim);
			A.clear(); //Amit: writes zero for all elements
		} else
			A = d.MatrixA;
		// end of patch
		assert(d.C.size() == dim);

		A.mult_vector(y[i], myAxPlusb);
		for (unsigned int j = 0; j < dim; j++) {
			myAxPlusb[j] = myAxPlusb[j] + d.C[j];
		}

		if (i == N - 1) {
			// compute the distance of this endpoint with the forbidden polytope \cap invariant (the segment end point must lie
			// in the intersection of the bad_set and the last location invariant).

			cost += bad_poly->point_distance(y[N - 1]); // end point distance to bad-set added to cost;

			std::vector<double> badpoly_dist_grad(dim, 0);

			badpoly_dist_grad = dist_grad(y[N - 1], bad_poly);
			// add the cost gradient w.r.t last traj segment's dwell time
			double dist_gradt = 0;
			for (unsigned int j = 0; j < dim; j++) {
				dist_gradt += badpoly_dist_grad[j] * myAxPlusb[j];
			}

			deriv[i] = dist_gradt;

			break;
		} else {
			polytope::const_ptr g;
			Assign R;

			// assign the transition pointer
			Tptr = *(T_iter);
			// assignment of the form: Rx + w

			R = Tptr->getAssignT();

			//guard as a polytope
			g = Tptr->getGuard();
			// guard \cap invariant distance, to address Eq. (12) in CDC 13' paper
			polytope::ptr guard_intersect_inv;
			guard_intersect_inv = I->GetPolytope_Intersection(g);

			cost += guard_intersect_inv->point_distance(y[i]);

			std::vector<double> guard_dist_grad(dim, 0);
			guard_dist_grad = dist_grad(y[i], guard_intersect_inv);

			// Add derivative of guard \cap invariant wrt dwell time
			double dist_gradt = 0;
			for (unsigned int j = 0; j < dim; j++) {
				dist_gradt += guard_dist_grad[j] * myAxPlusb[j];
			}
			deriv[i] = dist_gradt;

			assert(y[i].size() == R.Map.size2());
			std::vector<double> transform(y[i].size(), 0);

			R.Map.mult_vector(y[i], transform);
			for (unsigned int j = 0; j < transform.size(); j++)
				y[i][j] = transform[j] + R.b[j];

			R.Map.mult_vector(myAxPlusb, mapAxplusb);

			assert(y[i].size() == R.b.size());

			T_iter++; // Moving to the next transition.

			//compute the Euclidean distance between the next start point and the simulated end point
			for (unsigned int j = 0; j < dim; j++) {
				cost += (y[i][j] - x[(i + 1)][j]) * (y[i][j] - x[(i + 1)][j]);
				deriv[i] += 2 * (y[i][j] - x[(i + 1)][j]) * mapAxplusb[j];
			}
		}

	} // End of for-loop over i

	// Analytic gradients
	if (!grad.empty()) {
		for (unsigned int i = 0; i < t.size(); i++) {
			grad[i] = deriv[i];
		}
	}
	//std::cout << "cost = " << cost << std::endl;
	return cost;
}

