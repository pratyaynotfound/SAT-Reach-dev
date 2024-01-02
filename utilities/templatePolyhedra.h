/*
 * Template_polytope.h
 *
 *  Created on: 10-July-2014
 *      Author: amit
 */

#ifndef TEMPLATE_POLYHEDRA_H_
#define TEMPLATE_POLYHEDRA_H_

#include <list>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <core/continuous/polytope/polytope.h>
#include <utilities/templatePolyhedra.h>
#include <core/hybridAutomata/hybridAutomata.h>
#include <application/sfDirections.h>
#include <application/sfUtility.h>
#include <omp.h>

#include "core/math/matrix.h"

using namespace std;
/*
 * Combination of the matrices All_Directions and
 * Matrix_SupportFunction consists of all the reachable set of only one flow-pipe.
 */

class template_polyhedra {
public:

	typedef boost::shared_ptr<template_polyhedra> ptr;
	template_polyhedra();
	template_polyhedra(math::matrix<double> matrix_support_function,
			math::matrix<double> template_directions);
	template_polyhedra(math::matrix<double> matrix_support_function,
			math::matrix<double> matrix_invariant_bounds,
			math::matrix<double> template_directions,
			math::matrix<double> invariant_directions);

	/* returns the dimension of the template polyhedra/flowpipe*/
	unsigned int get_dimension();

	const math::matrix<double>& getMatrixSupportFunction() const;
	void setMatrixSupportFunction(math::matrix<double>& matrixSupportFunction);
	const math::matrix<double>& getTemplateDirections() const;
	void setTemplateDirections(math::matrix<double>& template_Directions);
	const math::matrix<double>& getInvariantDirections() const;
	void setInvariantDirections(const math::matrix<double>& invariant_Directions);

	const math::matrix<double>& getMatrix_InvariantBound() const;
	void setMatrix_InvariantBound(math::matrix<double>& matrix_invariantBound);

	/*
	 * Returns a single polytope set Omega(i) where i represent the Iteration number or the Column number of the Matrix_SupportFunction.
	 * Note:: the Index of i begins from 0 to i-1 just like the concept of Array.
	 */
	polytope::ptr getPolytope(unsigned int Iterations_Number);

	/*
	 * From the calling template_polyhedra or the entire reachable set of a single trajectory of a continuous system,
	 * a subset of the reachable set is returned which intersects with the guard region.
	 * Thus it returns a template_polyhedra consisting of a subset of the reachable set which intersects with the guard region.
	 * As there could be multiple intersected region, so it returns a list of intersected region, each
	 * elements of the list is a intersected region.
	 * Note::If the size of the returning list is 0 indicates that polytope-guard does not intersects with the calling template_polyhedra
	 */
	const std::list<template_polyhedra::ptr> polys_intersectionSequential(polytope::const_ptr guard, int lp_solver_type);//, int & point_of_intersection);

	/*
	 * Less Expensive function
	 * Returns a list of pairs with each pair of element as (start,end) where start and end are index at which intersection start and end.
	 */
	std::list<std::pair<unsigned int, unsigned int> > polys_intersectionSequential_optimize(polytope::const_ptr guard, int lp_solver_type);//, int & point_of_intersection);

	/*
	 * Less Expensive function
	 * Returns the list of polytopes with each polytope as the template_approximation of the intersected region
	 * the intersected region is computed as range of indices of SFM using the function polys_intersectionSequential_optimize()
	 *
	 * If aggregation is TRUE/ON, it indicate a single template-hull convex set is returned for the set of omegas that intersect with the guard (for each
	 * intersection region. Note there can be more than one set of omegas that intersect with the same guard, in this case list of convex sets can be returned).
	 * If aggregation is FALSE/OFF, it indicate for each omega that intersect with the guard it will be considered as individual convex set.
	 */
	std::list<polytope::ptr> flowpipe_intersectionSequential(bool aggregation, polytope::const_ptr guard, int lp_solver_type);

	/*
	 * Returns a list of polytopes as the convex_hull approximation of intersection of guard and flowpipe
	 */
	std::list<polytope::ptr> postD_chull(polytope::const_ptr guard, polytope::const_ptr inv, int lp_solver_type);
	unsigned int getTotalIterations() const;
	unsigned int getTotalTemplateDirections() const;
	unsigned int getTotalInvariantDirections() const;
	std::vector<double> getInvariantBoundValue(int iterations_number);

private:
	/*polytope*/
	math::matrix<double> Matrix_SupportFunction;//Note if it has invariants_dirs then Matrix_SupportFunction will also have bound_value
	math::matrix<double> template_Directions;	//only the template directions
	math::matrix<double> Matrix_InvariantBound;	//Note now Matrix_SupportFunction will NOT have the bound_value
	math::matrix<double> invariant_Directions;	//only the invariant directions
	unsigned int total_template_Directions;	//total number of template directions
	unsigned int total_invariant_Directions;//total number of invariant directions
	unsigned int total_iterations;//Number of Columns or the number of Convex Set/Polytope Omega's
	void setTotalIterations(unsigned int totalIterations);
	void setTotalTemplateDirections(unsigned int total_template_directions);
	void setTotalInvariantDirections(unsigned int total_invariant_directions);

};

#endif /* TEMPLATE_POLYHEDRA_H_ */
