/*
 * Create_Polytope_from_ConvexSet.h
 *
 *  Created on: 14-Dec-2014
 *      Author: amit
 */

#ifndef CREATE_POLYTOPE_FROM_CONVEXSET_H_
#define CREATE_POLYTOPE_FROM_CONVEXSET_H_

#include <application/structures.h>
#include "core/continuous/polytope/polytope.h"

/*
 * Creates a polytope from a specified Convex Set
 */
polytope::ptr create_polytope_from_set(supportFunctionProvider::ptr Initial,
		ReachabilityParameters& ReachParameters, Dynamics& SystemDynamics, int lp_solver_type);



#endif /* CREATE_POLYTOPE_FROM_CONVEXSET_H_ */
