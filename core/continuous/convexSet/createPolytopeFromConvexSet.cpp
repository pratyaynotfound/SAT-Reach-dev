/*
 * Create_Polytope_from_ConvexSet.cpp
 *
 *  Created on: 14-Dec-2014
 *      Author: amit
 */
#include <core/continuous/convexSet/createPolytopeFromConvexSet.h>

polytope::ptr create_polytope_from_set(supportFunctionProvider::ptr Initial,
		ReachabilityParameters& ReachParameters, Dynamics& SystemDynamics, int lp_solver_type) {
	//polytope p;
	std::vector<double> columnvector(ReachParameters.Directions.size1());

	int type = lp_solver_type;
	lp_solver lp(type), lp_U(type);
	lp.setMin_Or_Max(2);
	lp_U.setMin_Or_Max(2);

	int dim = ReachParameters.Directions.size2();
	std::vector<double> direction(dim);
//	std::cout << "Dim =  " << dim;
//	std::cout << "Direction = " << ReachParameters.Directions.size1()<< std::endl;
	if (!ReachParameters.X0->getIsEmpty()) //set glpk constraints If not an empty polytope
		lp.setConstraints(ReachParameters.X0->getCoeffMatrix(),
				ReachParameters.X0->getColumnVector(),
				ReachParameters.X0->getInEqualitySign());

	if (!SystemDynamics.U->getIsEmpty()) {	//empty polytope
		lp_U.setConstraints(SystemDynamics.U->getCoeffMatrix(),
				SystemDynamics.U->getColumnVector(),
				SystemDynamics.U->getInEqualitySign());
	}
	for (unsigned int i = 0; i < ReachParameters.Directions.size1(); i++) {
		//	std::cout << "loop 1 \n";
		for (int j = 0; j < dim; j++) {
			direction[j] = ReachParameters.Directions(i, j);
			//	std::cout << "loop 2 \n";
		}
		columnvector[i] = Initial->computeSupportFunction(direction, lp);

	}
	/*p.setCoeffMatrix(ReachParameters.Directions);
	 p.setColumnVector(columnvector);
	 p.setInEqualitySign(1);*/
	polytope::ptr p = polytope::ptr(
			new polytope(ReachParameters.Directions, columnvector, 1));

	return p;
}


