/*
 * Post_Assignment.cpp
 *
 *  Created on: 24-Nov-2014
 *      Author: amit
 */

#include <application/sfUtility.h>
#include <utilities/postAssignment.h>


/*
 * Assignment to a constraint polyhedron newPolytope
 * Post_assign(newPolytope) = R x newPolytope + W
 * where W is a constant vector and R is the transition dynamics
 * The mapped states are computed exactly by mapping the polyhedron
 * Post_assign(newPolytope) = {x e Rn | Ap R^-1 x <= bp + Ap R^-1 w}   w e W
 *
 */
polytope::ptr post_assign_exact(polytope::ptr newPolytope, math::matrix<double> R,
		std::vector<double> w) {
	math::matrix<double> AA, A_dash;
	math::matrix<double> R_inverse(R.size1(), R.size2());
	std::vector<double> b_p, b_dash, term2;
	bool invertible;

	/*std::cout<<"newPolytope->getCoeffMatrix()="<<newPolytope->getCoeffMatrix()<<"\n columnVector are ";
		for (int i=0;i<newPolytope->getColumnVector().size();i++)
			std::cout<<newPolytope->getColumnVector()[i]<<"\n";*/

//polytope post_assign;
	b_p = newPolytope->getColumnVector();
//	cout<<"bp size = "<<b_p.size()<<endl;
//	cout<<"Testing 1\n";
	invertible = R.inverse(R_inverse);//Size of R_inverse has to be assigned otherwise error
//	cout<<"R_inverse (rows,col) = ("<<R_inverse.size1()<<" , "<<R_inverse.size2()<<")"<<endl;
	if (invertible) {
		//cout<<"\nAmit check a\n";
		/*
		 cout << "\nAmit here\n";
		 for (int i = 0; i < R_inverse.size1(); i++) {
		 for (int j = 0; j < R_inverse.size2(); j++)
		 cout << R_inverse(i, j) << "\t";
		 cout << endl;
		 }
		 cout << "\nAmit there is Inverse Output\n";
		 */

		AA = newPolytope->getCoeffMatrix();
//		cout<<"AA (rows,col) = ("<<AA.size1()<<" , "<<AA.size2()<<")"<<endl;
		/*		cout << "\nAmit here is AA\n";
		 for (int i = 0; i < AA.size1(); i++) {
		 for (int j = 0; j < AA.size2(); j++)
		 cout << AA(i, j) << "\t";
		 cout << endl;
		 }*/
		AA.multiply(R_inverse, A_dash);
		/*		cout << "\nAmit here is A_dash = AA * R_inverse \n";
		 for (int i = 0; i < A_dash.size1(); i++) {
		 for (int j = 0; j < A_dash.size2(); j++)
		 cout << A_dash(i, j) << "\t";
		 cout << endl;
		 }*/

		A_dash.mult_vector(w, term2);
		/*cout << "\nAmit here is A' x w \n";
		 for (int j = 0; j < term2.size(); j++)
		 cout << term2[j] << "\t";
		 */
//		cout<<"Testing 2\n";
//		cout<<"bp size = "<<b_p.size()<<endl;
//		cout<<"term2 size = "<<term2.size()<<endl;
		b_dash = vector_add(b_p, term2);

		/*
		 cout << "\nAmit here is bp + A' x w \n";
		 for (int j = 0; j < b_dash.size(); j++)
		 cout << b_dash[j] << "\t";
		 */

	} else{
		std::cout << "\nThe Transition Dynamics Matrix is not Invertible!!!\n";
		//post_assign_approx(newPolytope, R, W,)
	}

	/*std::cout<<"\nnewShiftedPolytope->getCoeffMatrix()="<<A_dash<<"\n columnVector are ";
			for (int i=0;i<b_dash.size();i++)
				std::cout<<b_dash[i]<<"\n";*/

	return polytope::ptr(new polytope(A_dash, b_dash, 1));
}
/*
 * Computing Transition Successors with Support Function
 * Recall proposition of support function with matrix and convex set in any directions
 *
 */

polytope::ptr post_assign_approx(polytope::ptr newPolytope, math::matrix<double> R,
		polytope W, math::matrix<double> Directions, int lp_solver_type) {
	math::matrix<double> R_transpose;
	int max_or_min = 2;	//Maximizing
	std::vector<double> b(Directions.size1()), each_direction(
			Directions.size2()), direction_trans;
	R.transpose(R_transpose);
	//create glpk object to be used by the polytope
	int type=lp_solver_type;
	lp_solver lp(type);
	lp.setMin_Or_Max(max_or_min);
	lp.setConstraints(newPolytope->getCoeffMatrix(),newPolytope->getColumnVector(),newPolytope->getInEqualitySign());

	for (unsigned int i = 0; i < Directions.size1(); i++) {
		for (unsigned int j = 0; j < Directions.size2(); j++)
			each_direction[j] = Directions(i, j);
		R_transpose.mult_vector(each_direction, direction_trans);

		b[i] = newPolytope->computeSupportFunction(direction_trans, lp) + W.computeSupportFunction(each_direction, lp);
	}

	return polytope::ptr(new polytope(Directions, b, 1));
}



/*
 * Computing Transition Successors with Support Function
 * Recall proposition of support function with matrix and convex set in any directions
 *
 * This function assumes a vector w given in the transition assignment. Hence, deterministic.
 * i.e. X' = Rx + w
 */

polytope::ptr post_assign_approx_deterministic(polytope::ptr newPolytope, math::matrix<double> R,
		std::vector<double> w, math::matrix<double> Directions, int lp_solver_type) {
	math::matrix<double> R_transpose;
	int max_or_min = 2;	//Maximizing
	std::vector<double> b(Directions.size1()), each_direction(Directions.size2()), direction_trans;
	/*std::cout<<"newPolytope->getCoeffMatrix()="<<newPolytope->getCoeffMatrix()<<"\n columnVector are";
	for (int i=0;i<newPolytope->getColumnVector().size();i++)
		std::cout<<newPolytope->getColumnVector()[i]<<"\n";*/

	R.transpose(R_transpose);
	//create glpk object to be used by the polytope
	int type=lp_solver_type;
	lp_solver lp(type);
	lp.setMin_Or_Max(max_or_min);
	lp.setConstraints(newPolytope->getCoeffMatrix(),newPolytope->getColumnVector(),newPolytope->getInEqualitySign());
	for (unsigned int i = 0; i < Directions.size1(); i++) {
		for (unsigned int j = 0; j < Directions.size2(); j++)
			each_direction[j] = Directions(i, j);
		R_transpose.mult_vector(each_direction, direction_trans);
		try{
			b[i] = newPolytope->computeSupportFunction(direction_trans, lp) + dot_product(each_direction, w);
		} catch(...){
			b[i] = 999; // a large value set as the support function since the solution is unbounded
		}
	}

	/*std::cout<<"\nnewShiftedPolytope->getCoeffMatrix()="<<Directions<<"\n columnVector are";
		for (int i=0;i<b.size();i++)
			std::cout<<b[i]<<"\n";*/

	return polytope::ptr(new polytope(Directions, b, 1));
}
