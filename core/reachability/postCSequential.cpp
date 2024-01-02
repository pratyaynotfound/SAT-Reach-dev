/*
 * reachabilitySequential.cpp
 *
 *  Created on: 22-Nov-2021
 *      Author: Atanu
 */
#include <core/reachability/postCSequential.h>

/**
 * This method computes the postC flowpipe using the approximation model
 * of forward-backward interpolation in SpaceEx. This is the fixed time-step
 * algorithm.
 */

template_polyhedra::ptr postC_fbinterpol(const unsigned int boundedTotIteration, const Dynamics& SystemDynamics, polytope::const_ptr Initial,
		const ReachabilityParameters& ReachParameters, polytope::const_ptr invariant, bool InvariantExist) {

	
	unsigned int num_directions = ReachParameters.Directions.size1();

	unsigned int dimension = Initial->getSystemDimension();

	unsigned int num_iters = ReachParameters.Iterations;

	math::matrix<double> SFM(num_directions, num_iters); // The data structure to store the template polytopes
	//debug
	std::cout << "A-mat passed to fbinterpol\n";
	std::cout << SystemDynamics.MatrixA;
	std::cout << "B-mat passed to fbinterpol\n";
	std::cout << SystemDynamics.MatrixB;
	std::cout << "U passed to fbinterpol\n";
	SystemDynamics.U->printPoly();
	std::cout << "Initial passed to fbinterpol\n";
	Initial->printPoly();
	//---
	approx_model::ptr fbinterpol_model = approx_model::ptr(new fb_interpol(SystemDynamics.MatrixA, Initial, SystemDynamics.U, SystemDynamics.MatrixB,
			ReachParameters.time_step, num_directions, num_iters) );
	// get the direction vectors
	std::vector<std::vector<double> > direction(num_directions);
	for(unsigned int eachDirection = 0; eachDirection < num_directions; eachDirection++){
		direction[eachDirection].resize(dimension);
		for (unsigned int i = 0; i < dimension; i++) {
			assert(ReachParameters.Directions.size2() == dimension);
			direction[eachDirection][i] = ReachParameters.Directions(eachDirection, i);
		}
	}
	// create a polytope with template directions and uninitialized column vector
	//std::cout<<"printing ReachParameters.Directions: "<<endl;
	//std::cout<<ReachParameters.Directions<<endl;
	std::vector<double> bounds(ReachParameters.Directions.size1());
	polytope::ptr omega = polytope::ptr(new polytope(ReachParameters.Directions, bounds, 1));

	for(unsigned int iter = 0; iter < num_iters; iter++)
	{
		for(unsigned int eachDirection = 0; eachDirection < num_directions; eachDirection++){
			SFM(eachDirection,iter) = fbinterpol_model->omega_support(direction[eachDirection],iter);

			bounds[eachDirection] = SFM(eachDirection,iter);
		}
		//std::cout<<"printing Omega:"<<endl;
		//omega->printPoly();
		if(InvariantExist == true){
			// initialize the column vector
			omega->setColumnVector(bounds);
			bool not_empty = omega->check_polytope_intersection(invariant);
			//std::cout<<"printing Omega:"<<endl;
			//omega->printPoly();
			if(!not_empty){ // the flowpipe is completely outside the invariant
				// resize the sfm to contain omegas which are fully or partially inside the invariant
				SFM.resize(SFM.size1(), iter);
				num_iters = iter;
				break;
			}
		}
	}

	// intersect the omegas with the location invariant

	if (InvariantExist == true) { //if invariant exist. Computing
		math::matrix<double> inv_sfm;
		unsigned int num_inv_constr = invariant->getColumnVector().size(); //number of Invariant's constraints
		inv_sfm.resize(num_inv_constr, num_iters);
		for (unsigned int eachInvDirection = 0; eachInvDirection < num_inv_constr;eachInvDirection++) {
			for (unsigned int i = 0; i < num_iters; i++) {
				inv_sfm(eachInvDirection, i) =
						invariant->getColumnVector()[eachInvDirection];
			}
		}
		
		return template_polyhedra::ptr( new template_polyhedra(SFM, inv_sfm, ReachParameters.Directions, invariant->getCoeffMatrix()));
	} else {
		return template_polyhedra::ptr( new template_polyhedra(SFM, ReachParameters.Directions));
	}

}
