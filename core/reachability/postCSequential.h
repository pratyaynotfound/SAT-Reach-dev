/*
 * reachability.h
 *
 *  Created on: 16-Apr-2014
 *      Author: amit
 */

#ifndef REACHABILITY_SEQ_H_
#define REACHABILITY_SEQ_H_

#include <application/sfUtility.h>
#include <utilities/templatePolyhedra.h>
#include <fstream>
#include "core/math/lpSolver/lpSolver.h"
#include "core/math/matrix.h"
#include <core/continuous/approxModel/approxModel.h>
#include <core/continuous/approxModel/fbInterpol.h>



using namespace std;

typedef typename boost::numeric::ublas::matrix<double>::size_type size_type;

/**
 * coeffMatrix : All facets of the Polytope I
 * bMatrix : All Bound-Value of the Polytope I
 * boundBGreaterThenExpr : All Bound-Sign of the Polytope I  ... 1 for <= and 0 for >=
 *
 * VcoeffMatrix : All facets of the Polytope V
 * VbMatrix : All Bound-Value of the Polytope V
 * VboundBGreaterThenExpr : All Bound-Sign of the Polytope V  ... 1 for <= and 0 for >=
 *
 * AMatrix : is the flow matrix
 * Vector_R : is the matrix (m x n) consisting the list of 'm' directions of 'n' variables each.
 *
 * M_Matrix : is the matrix with the optimal reachability result
 * iterationNum : Number of iterations of the reachability algorithm.
 */

// A sequential postC - forward-backward interpolation model of CAV'11.
template_polyhedra::ptr postC_fbinterpol(const unsigned int NewTotalIteration, const Dynamics& SystemDynamics,
		polytope::const_ptr Initial,
		const ReachabilityParameters& ReachParameters, polytope::const_ptr invariant,
		bool InvariantExist);

//--

#endif /* REACHABILITY_SEQ_H_ */
