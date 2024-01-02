/*
 * polytope.h
 *
 *  Created on: 30-Jun-2014
 *      Author: amit
 */

#ifndef POLYTOPE_H_
#define POLYTOPE_H_

#include <vector>
#include <boost/shared_ptr.hpp>
#include<set>
#include<utility>
#include <fstream>
#include <boost/tokenizer.hpp>
#include <core/continuous/convexSet/supportFunctionProvider.h>
#include <utilities/stringParser.h>
#include "core/math/lpSolver/lpSolver.h"
#include "core/math/matrix.h"
#include <core/math/2dGeometry.h>
#include <utilities/vectorOperations.h>



using namespace std;

/*
 * If a polytope is represented using intersections of halfspaces then it is of the form Ax<=b
 * where A is the coefficients Matrix of the variables 'x' and b is the columnVector
 *
 * coeffMatrix : All facets of the Polytope in Matrix form (i.e. the coefficients of the variables).
 * number_facets : Number of faces of the defined polytope.
 * dimension :  Number of variables of the system.
 * columnVector :	The values b for each facet.
 * InEqualitySign :	The in equalities sign of the bound values 'b'. Possible values are
 * 					0 :	for  Ax = b (b Equals to)
 * 					1 :	for  Ax <= b (b is Greater or Equals to lhs)
 * 					2 :	for  Ax >= b (b is Less or Equals to lhs)
 *
 * Also include the function to compute Support Function of the defined polytope in any given direction.
 * Also include the function to return the Dimension of the defined polytope.
 */

class polytope: public supportFunctionProvider {

private:
	math::matrix<double> coeffMatrix;
	std::vector<double> columnVector;
	int InEqualitySign;
	unsigned int number_facets;
	unsigned int system_dimension;
	bool IsEmpty;
	bool IsUniverse;

public:
	typedef boost::shared_ptr<polytope> ptr;
	typedef boost::shared_ptr<const polytope> const_ptr;
	polytope();
	polytope(bool empty);
	polytope(math::matrix<double> coeffMatrix, std::vector<double> columnVector,
			int InEqualitySign);
	void setIsEmpty(bool empty);
	bool getIsEmpty() const;
	void setIsUniverse(bool universe);
	bool getIsUniverse() const;
	/* Returns true if the constraints make it bounded */
	bool isBounded() const;

	void setPolytope(math::matrix<double> coeffMatrix,
			std::vector<double> columnVector, int inEqualitySign);
	/*
	 * Adds one constraint to the existing polytope by adding the
	 * coefficient constraint with the bound value to the existing list.
	 */
	void setMoreConstraints(std::vector<double> coeff_constraint,double bound_value);

	/*
	 * Adds one or more constraints to the existing polytope by adding the
	 * coefficient_constraints with the bound_values to the existing list.
	 */
	void setMoreConstraints(math::matrix<double> coeff_constraints,std::vector<double> bound_values);

	const math::matrix<double>& getCoeffMatrix() const;
	void setCoeffMatrix(const math::matrix<double> coeffMatrix);
	void setColumnVector(const std::vector<double> columnVector);

	int getInEqualitySign() const;
	void setInEqualitySign(int inEqualitySign);
	std::vector<double> getColumnVector() const;

	unsigned int getSystemDimension() const; //returns the number of variables of the polytopes.
	void setSystemDimension(unsigned int systemDimension); 
	unsigned int getNumberFacets() const;
	void setNumberFacets(unsigned int numberFacets);

	double computeSupportFunction(const std::vector<double>& direction, lp_solver &lp) const;

	/*
	 * Returns Max norm of the polytope
	 */
	double max_norm(int lp_solver_type, unsigned int dim_for_Max_Norm) const;
	/*
	 * Returns True if polytope P1(the calling polytope object) and P2 intersects each other
	 *  i.e., True iff	P1 intersection P2 != empty set
	 */
	bool check_polytope_intersection(polytope::const_ptr P2) const ;
	/*
	 * Returns a new polytope after appending the constraints of P2
	 * which is an intersection-region
	 */
	polytope::ptr GetPolytope_Intersection(polytope::const_ptr P2) const;


	/**
	 * Enumerate all vertices of the polytope between the two vectors
	 * given as arguments
	 */
	void enum_2dVert_restrict(std::vector<double> u, std::vector<double> v,
			int i, int j, std::set<std::pair<double, double> >&pts) const;

	/**
	 * enumerate all vertices of the polytope
	 * int i is the 1st projecting variable and
	 * 	   j is the second projecting variable
	 * 	   the value/index of i and j begins with 0 to n-1
	 */
	std::set<std::pair<double, double> > enumerate_2dVertices(int i, int j) const;

	/*
	 * Returns the list of vertices of the polytope in 2d with the given inputs as
	 * i and j where i and j are the 1st and 2nd projecting variables
	 */

	math::matrix<double> get_2dVertices(int dim1, int dim2) const;

	/**
	 * Computes the distance of a point from the polytope.
	 * If the point is inside the polytope, a 0 distance
	 * is returned. Otherwise, the distance is the sum of
	 * distances of the point to all the faces.
	 */
	double point_distance(std::vector<double> v) const;

	/*
	 * Prints the vertices of the polytope on the std::cout, passed as parameter.
	 */
	void print2StdOut(unsigned int dim1, unsigned int dim2) const;

	//Prints the coefficient matrix and vector
	void printPoly() const;
	/*
	 * Prints the vertices of the polytope to a file, passed as parameter.
	 * The file could be called with any plotting utility.
	 *
	 */
	void print2file(std::string fname, unsigned int dim1, unsigned int dim2) const;


	bool contains(polytope::const_ptr poly, int lp_solver_type) const ;

};

/**
 * Creates a pair of <loc_id, poly> from the user given bad state string
 */
void string_to_poly(const std::string& bad_state, std::pair<int, polytope::ptr>& f_set);


#endif /* POLYTOPE_H_ */
