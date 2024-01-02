/*
 * Template_Polytope.cpp
 *
 *  Created on: 22-Nov-2021
 *      Author: Atanu
 */

#include <core/continuous/polytope/polytope.h>
#include <omp.h>
#include <utilities/templatePolyhedra.h>
#include "boost/timer/timer.hpp"

typedef typename boost::numeric::ublas::matrix<double>::size_type size_type;

template_polyhedra::template_polyhedra() {
	total_iterations = 0;
	total_template_Directions = 0;
	total_invariant_Directions = 0;
}

template_polyhedra::template_polyhedra (
		math::matrix<double> matrix_support_function,
		math::matrix<double> template_directions) {
	this->setMatrixSupportFunction(matrix_support_function);
	this->setTemplateDirections(template_directions); //no more working with all_directions
	this->setTotalInvariantDirections(0);
}

template_polyhedra::template_polyhedra(
		math::matrix<double> matrix_support_function,
		math::matrix<double> matrix_invariant_bounds,
		math::matrix<double> template_directions,
		math::matrix<double> invariant_directions) {
	//this->setTotalIterations(matrix_support_function.size2());
	this->setMatrixSupportFunction(matrix_support_function);

	//this->setTotalTemplateDirections(template_directions.size1());
	this->setMatrix_InvariantBound(matrix_invariant_bounds);

	this->setTemplateDirections(template_directions);
	this->setInvariantDirections(invariant_directions);
}

unsigned int template_polyhedra::get_dimension(){
	assert(template_Directions.size2()!=0);
	return template_Directions.size2();
}

const math::matrix<double>& template_polyhedra::getTemplateDirections() const {
	return template_Directions;
}
void template_polyhedra::setTemplateDirections(
		math::matrix<double>& template_Directions) {
	this->template_Directions = template_Directions;
	this->setTotalTemplateDirections(template_Directions.size1());
}
const math::matrix<double>& template_polyhedra::getInvariantDirections() const {
	return invariant_Directions;
}
void template_polyhedra::setInvariantDirections(
		const math::matrix<double>& invariant_Directions) {
	this->invariant_Directions = invariant_Directions;
	this->setTotalInvariantDirections(invariant_Directions.size1());
}

const math::matrix<double>& template_polyhedra::getMatrix_InvariantBound() const {
	return Matrix_InvariantBound;
}
void template_polyhedra::setMatrix_InvariantBound(
		math::matrix<double>& matrix_invariantBound) {
	Matrix_InvariantBound = matrix_invariantBound;
}

unsigned int template_polyhedra::getTotalTemplateDirections() const {
	return total_template_Directions;
}
void template_polyhedra::setTotalTemplateDirections(
		unsigned int total_template_directions) {
	total_template_Directions = total_template_directions;
}

unsigned int template_polyhedra::getTotalInvariantDirections() const {
	return total_invariant_Directions;
}
void template_polyhedra::setTotalInvariantDirections(
		unsigned int total_invariant_directions) {
	total_invariant_Directions = total_invariant_directions;
}

std::vector<double> template_polyhedra::getInvariantBoundValue(
		int Iterations_Number) {	//need testing
	std::vector<double> bound_value(this->getInvariantDirections().size1());

	for (unsigned int i = 0; i < this->invariant_Directions.size1(); i++) {
		bound_value[i] = this->Matrix_InvariantBound(i, Iterations_Number);
	}
	return bound_value;
}

polytope::ptr template_polyhedra::getPolytope(unsigned int Iterations_Number) {
	polytope::ptr p;
	p = polytope::ptr(new polytope());
	p->setCoeffMatrix(this->getTemplateDirections());
	std::vector<double> boundValue(this->getTotalTemplateDirections());	

	for (unsigned int i = 0; i < Matrix_SupportFunction.size1(); i++) {
		boundValue[i] = Matrix_SupportFunction(i, Iterations_Number);
	}
	p->setColumnVector(boundValue);
	p->setInEqualitySign(1);	//Ax<=b		0 for = sign, 1 for <= sign and 2 for >= sign
	return p;
}

const std::list<template_polyhedra::ptr> template_polyhedra::polys_intersectionSequential(polytope::const_ptr G, int lp_solver_type) { //need testing due to modification

	size_type row = 0;
	size_type col = 0;
	math::matrix<double> mat_sf(row, col);
	bool is_intersected = false, intersection_started = false, intersection_ended = false;

	std::list<template_polyhedra::ptr> intersected_region;

	for (unsigned int i = 0; i < this->Matrix_SupportFunction.size2(); i++) {
		polytope::ptr p;
		p = this->getPolytope(i);
		std::vector<double> constraint_bound_values(this->getInvariantDirections().size1());
		constraint_bound_values = this->getInvariantBoundValue(i);

		/* from the reach_region that is the calling template_polyhedra add the invariant constraint and bound_value to p*/
		p->setMoreConstraints(this->getInvariantDirections(), constraint_bound_values);
		is_intersected = p->check_polytope_intersection(G);	//result of intersection

		if (is_intersected == true) {//if intersects create a new template_polyhedra subset
//#pragma omp critical
				//		{
			intersection_started = true;
			col = col + 1;//
			mat_sf.resize(p->getCoeffMatrix().size1(), col, true);//resizing to append the polytope p of the flowpipe that intersects

			for (unsigned int j = 0; j < p->getCoeffMatrix().size1(); j++) {
				mat_sf(j, col - 1) = p->getColumnVector()[j];	//	mat_sf.addColumn(p.getColumnVector()); add column in matrix class
			}
		}

		if (is_intersected == false && intersection_started == true) {
//#pragma omp critical
			//	{
			intersection_ended = true;
			//	}
		}

		if (intersection_started == true && intersection_ended == true) {
//#pragma omp critical
			//		{
			intersected_region.push_back(template_polyhedra::ptr(new template_polyhedra(mat_sf, p->getCoeffMatrix())));
			mat_sf.clear();	//reset all values to zero
			mat_sf.resize(0, 0);
			col = 0;
			intersection_started = false;
			intersection_ended = false;
			//		}
		}
	}	//end of parallel for-loop
	if (intersection_started == true && intersection_ended == false) {
		math::matrix<double> directions, templateDirs, invDirs;
		templateDirs = this->getTemplateDirections();
		invDirs = this->getInvariantDirections();
		templateDirs.matrix_join(invDirs, directions);

		intersected_region.push_back(template_polyhedra::ptr(new template_polyhedra(mat_sf, directions)));//intersected_region.push_back(template_polyhedra(mat_sf, p.getCoeffMatrix()));
	}

	return intersected_region;
}

 
std::list<std::pair<unsigned int, unsigned int> > template_polyhedra::polys_intersectionSequential_optimize(polytope::const_ptr G, int lp_solver_type) {

	// Check special case:  G is empty
	if(G->getIsEmpty()){
		std::pair<unsigned int, unsigned int> range;
		range.first = 0;
		range.second = 0;
		std::list<std::pair<unsigned int, unsigned int> > rangeList;
		rangeList.push_back(range);
		return rangeList;
	}

	// check special case: G is universe
	if(G->getIsUniverse()){
		std::pair<unsigned int, unsigned int> range;
		range.first = 0;
		range.second = this->total_iterations-1;
		std::list<std::pair<unsigned int, unsigned int> > rangeList;
		rangeList.push_back(range);
		return rangeList;
	}

	size_type row = 0;
	size_type col = 0;

	math::matrix<double> mat_sf(row, col);
	bool is_intersected = false, intersection_started = false, intersection_ended = false;
	int foundIntersection = 0;
	std::list<template_polyhedra::ptr> intersected_region;

	std::vector<bool> intersects(this->Matrix_SupportFunction.size2(), false); //all false initially
	for (unsigned int i = 0; i < this->Matrix_SupportFunction.size2(); i++) {
		polytope::ptr p;
		p = this->getPolytope(i);
		if (this->invariant_Directions.size1() != 0){
			std::vector<double> constraint_bound_values(this->getInvariantDirections().size1());
			constraint_bound_values = this->getInvariantBoundValue(i);
			p->setMoreConstraints(this->getInvariantDirections(), constraint_bound_values);
		}
		intersects[i] = p->check_polytope_intersection(G); //result of intersection
	} //end of parallel-loop :: we have the list of intersected polys

	std::list<std::pair<unsigned int, unsigned int> > intersected_range;

	std::pair<unsigned int, unsigned int> inte_range;
	for (unsigned int i = 0; i < this->Matrix_SupportFunction.size2(); i++) { //sequential reading of an boolean_array that tells intersected polys
		is_intersected = intersects[i];
		if (is_intersected == true) { //if intersects create a new template_polyhedra subset
			intersection_started = true;
			if (foundIntersection == 0) { //Intersection start
				foundIntersection++; //intersection_start = i;	//Intersection started at this position of 'i'
				inte_range.first = i; //	cout << "\nIntersection Found at = " << i << endl;
			}
		}
		if (intersection_started == true && is_intersected == false) {
			intersection_ended = true;
			inte_range.second = i;
			foundIntersection = 0;
		}
		if (intersection_ended == true && intersection_started == true) {
			intersected_range.push_back(inte_range);
			intersection_started = false;
			intersection_ended = false; //cout << "\nIntersection Ended at = " << i << "\n";
		}
	} //end of parallel for-loop

	if (intersection_started == true && intersection_ended == false) {
		inte_range.second = this->Matrix_SupportFunction.size2() - 1;
		intersected_range.push_back(inte_range);
	}
	return intersected_range;
}
/*
 * returns the template hull of the flowpipe-guard intersected template polytopes
 */
std::list<polytope::ptr> template_polyhedra::flowpipe_intersectionSequential(bool aggregation, polytope::const_ptr guard, int lp_solver_type){

	std::list<std::pair<unsigned int, unsigned int> > range_list;
	range_list = polys_intersectionSequential_optimize(guard,lp_solver_type);

	std::list<polytope::ptr> polys;
	unsigned int poly_dir_size = this->template_Directions.size1() + this->invariant_Directions.size1();
	std::vector<double> colVector(poly_dir_size);


	if (!aggregation){//OFF for each Omega that intersected it is push into the polys list. Expensive Operation
		//std::cout<<"Aggregation is Switched-OFF\n";
		for (std::list<std::pair<unsigned int, unsigned int> >::iterator range_it = range_list.begin(); range_it != range_list.end(); range_it++) {
			unsigned int start = (*range_it).first, end=(*range_it).second;
			for (unsigned int i = start; i <= end; i++) {//push in polys every polytope/Omega from start to end

				polytope::ptr p=this->getPolytope(i);//gets only the polytope with out invariant of this templeted_polyhedra
				math::matrix<double> invDirs;
				std::vector<double> invBound(this->invariant_Directions.size1());
				if (this->invariant_Directions.size1() != 0){//if invariant exists
					invDirs = this->invariant_Directions;
					invBound=this->getInvariantBoundValue(i);
					p->setMoreConstraints(invDirs,invBound);
				}
				polys.push_back(p);

			}//end of each intersected region
		}//end of multiple intersected region
	} else { //ON for all intersected Omegas create a single template-hull set
		//std::cout<<"Aggregation is Switched-ON\n";

		for (std::list<std::pair<unsigned int, unsigned int> >::iterator range_it = range_list.begin(); range_it != range_list.end();
				range_it++) {
			unsigned int start = (*range_it).first, end=(*range_it).second;

			for (unsigned int eachTemplateDir=0;eachTemplateDir<this->template_Directions.size1();eachTemplateDir++){

				double Max_sf=this->Matrix_SupportFunction(eachTemplateDir,start);
				for (unsigned int i = start+1; i <= end; i++) {
					double sf = this->Matrix_SupportFunction(eachTemplateDir,i);
					if (sf > Max_sf)
						Max_sf = sf;
				}//end of each intersected region
				colVector[eachTemplateDir] = Max_sf;
			}//end of each template direction ALSO HAVE TO PERFORM INVARIANT DIRECTION
			unsigned int total_dir = this->template_Directions.size1();
			for (unsigned int eachInvDir=0;eachInvDir<this->invariant_Directions.size1();eachInvDir++){
				double Max_sf=this->Matrix_InvariantBound(eachInvDir,start);
				for (unsigned int i = start + 1; i <= end; i++) {
					double sf = this->Matrix_InvariantBound(eachInvDir, i);
					if (sf > Max_sf)
						Max_sf = sf;
				} //end of each intersected region
				colVector[total_dir + eachInvDir] = Max_sf;
			}
			math::matrix<double> allDirs;
			this->template_Directions.matrix_join(this->invariant_Directions, allDirs);
			polytope::ptr p = polytope::ptr(new polytope(allDirs,colVector,1));
			polys.push_back(p);
		}//end of multiple intersected region
	}

	return polys;
	
}

const math::matrix<double>& template_polyhedra::getMatrixSupportFunction() const {
	return Matrix_SupportFunction;
}

void template_polyhedra::setMatrixSupportFunction(
		math::matrix<double>& matrixSupportFunction) {

	Matrix_SupportFunction = matrixSupportFunction;

	this->setTotalIterations(matrixSupportFunction.size2());
}

unsigned int template_polyhedra::getTotalIterations() const {
	return total_iterations;
}

void template_polyhedra::setTotalIterations(unsigned int totalIterations) {
	total_iterations = totalIterations;
}

std::list<polytope::ptr> template_polyhedra::postD_chull(polytope::const_ptr guard, polytope::const_ptr inv, int lp_solver_type)
{
	std::list<std::pair<unsigned int, unsigned int> > range_list;
	range_list = polys_intersectionSequential_optimize(guard,lp_solver_type);

	std::list<polytope::ptr> polys;
	unsigned int poly_dir_size = this->template_Directions.size1() + this->invariant_Directions.size1();
	std::vector<double> colVector(poly_dir_size);

	for (std::list<std::pair<unsigned int, unsigned int> >::iterator range_it = range_list.begin(); range_it != range_list.end(); range_it++) {
		unsigned int start = (*range_it).first, end=(*range_it).second;

		// intersect the omegas with guard.
		std::vector<polytope::ptr> omega_intersect_g;
		for(unsigned int i=start;i<=end;i++){
			polytope::ptr p = this->getPolytope(i);
			p = p->GetPolytope_Intersection(guard);
			p = p->GetPolytope_Intersection(inv);
			omega_intersect_g.push_back(p);
		}
			
		//compute the template hull of the polys in omega_intersect_g
			 
		for (unsigned int eachTemplateDir=0;eachTemplateDir<this->template_Directions.size1();eachTemplateDir++){
				
			double max_sf;
			std::vector<double> dir(template_Directions.size2());
			lp_solver lp;
			//get directions from template
			for(unsigned int j=0;j<dir.size();j++)
				dir[j] = template_Directions(eachTemplateDir, j);
			
			for(unsigned int j=0;j<omega_intersect_g.size();j++){
				polytope::ptr p = omega_intersect_g[j];
				lp.setConstraints(p->getCoeffMatrix(),p->getColumnVector(),1);
				double sf = p->computeSupportFunction(dir,lp);
				
				if(j==0){
					max_sf = sf; continue;
				}
				if(sf>max_sf)
					max_sf = sf;					
			}	
			colVector[eachTemplateDir] = max_sf;
			
		}//end of each template direction ALSO HAVE TO PERFORM INVARIANT DIRECTION

		unsigned int total_dir = this->template_Directions.size1();
		for (unsigned int eachInvDir=0;eachInvDir<this->invariant_Directions.size1();eachInvDir++){
	
			double max_sf;
			std::vector<double> dir(invariant_Directions.size2());
			lp_solver lp;
			
			//get invariant normal directions
			for(unsigned int j=0;j<dir.size();j++)
				dir[j] = invariant_Directions(eachInvDir,j);

			for(unsigned int j=0;j<omega_intersect_g.size();j++){
				polytope::ptr p = omega_intersect_g[j];
				lp.setConstraints(p->getCoeffMatrix(),p->getColumnVector(),1);
				double sf = p->computeSupportFunction(dir,lp);
				if(j==0){
					max_sf = sf; continue;
				}
				if(sf>max_sf)
					max_sf = sf;					
			}	
			colVector[total_dir + eachInvDir] = max_sf;
		}
		math::matrix<double> allDirs;
		this->template_Directions.matrix_join(this->invariant_Directions, allDirs);
		polytope::ptr p = polytope::ptr(new polytope(allDirs,colVector,1));
		// intersect with guard again for precision
		p = p->GetPolytope_Intersection(guard);
		polys.push_back(p);
	} //end of multiple intersected region

	return polys;
}

