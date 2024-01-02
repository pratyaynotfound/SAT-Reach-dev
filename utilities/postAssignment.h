/*
 * Post_Assignment.h
 *
 *  Created on: 25-Jul-2014
 *      Author: amit
 */

#ifndef POST_ASSIGNMENT_H_
#define POST_ASSIGNMENT_H_

#include <core/continuous/polytope/polytope.h>
#include "core/math/matrix.h"
#include "utilities/vectorOperations.h"



/*
 * Assignment to a constraint polyhedron newPolytope
 * Post_assign(newPolytope) = R x newPolytope + W
 * where W is a constant vector and R is the transition dynamics
 * The mapped states are computed exactly by mapping the polyhedron
 * Post_assign(newPolytope) = {x e Rn | Ap R^-1 x <= bp + Ap R^-1 w}   w e W
 *
 */
polytope::ptr post_assign_exact(polytope::ptr newPolytope, math::matrix<double> R,
		std::vector<double> w);


/*
 * Computing Transition Successors with Support Function
 * Recall proposition of support function with matrix and convex set in any directions
 *
 * This function assumes a non-deterministic input set W given in the transition assignment.
 * i.e. X' = Rx + W
 */

polytope::ptr post_assign_approx(polytope::ptr newPolytope, math::matrix<double> R,
		polytope W, math::matrix<double> Directions);


/*
 * Computing Transition Successors with Support Function
 * Recall proposition of support function with matrix and convex set in any directions
 *
 * This function assumes a vector w given in the transition assignment. Hence, deterministic.
 * i.e. X' = Rx + w
 */

polytope::ptr post_assign_approx_deterministic(polytope::ptr newPolytope, math::matrix<double> R,
		std::vector<double> w, math::matrix<double> Directions, int lp_solver_type);

#endif /* POST_ASSIGNMENT_H_ */
