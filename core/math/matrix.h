/*
 * matrix.h
 *
 *  Created on: 22-Jul-2014
 *      Author: amit
 */

#ifndef MATRIX_H_
#define MATRIX_H_

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/triangular.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <core/math/matrixExponential.hpp>
#include "../../core/math/r8lib.hpp"

namespace math {

template<typename scalar_type> class matrix: public boost::numeric::ublas::matrix<
		scalar_type> {
public:
	typedef boost::numeric::ublas::matrix<scalar_type> ublas_matrix_impl;
	typedef typename boost::numeric::ublas::matrix<scalar_type>::size_type size_type;
	typedef boost::numeric::ublas::vector<scalar_type> ublas_vector_impl;
	typedef typename boost::numeric::ublas::matrix<scalar_type>::array_type array_type;

	matrix();
	matrix(size_type r, size_type c);
	matrix(size_type r, size_type c, array_type data);
	/**
	 * This creates a matrix with the given dimension and assigns init_val to
	 * every member of the matrix.
	 */
	matrix(size_type r, size_type c, const scalar_type init_val);
	void matrix_exponentiation(math::matrix<scalar_type>& res, double time_tau) const;
	void matrix_exponentiation(math::matrix<scalar_type>& res) const;
	void multiply(const matrix& A, matrix& res) const;

	//any matrix object can call this function to create another identity matrix
	void matrix_Identity(int dimension, math::matrix<scalar_type>& newIdentityMatrix) const;

	//calling matrix as minuend, passed as subtrahend and result as difference (minuend − subtrahend =	difference)
	void minus(matrix& B, matrix& res);
	void mult_vector(std::vector<scalar_type> v, std::vector<scalar_type> &res) const;
	// multiplies each entry of the matrix by c
	void scalar_multiply(double c);

	/**
    	  * Gets the transpose of the current matrix in res
	  */
	void transpose(matrix& res) const;

	/**
	  * Gets the absolute matrix M = (|m_{i,j}|) in res
	  */
	void absolute(matrix& res) const;
	/*
	 * Appends a column vector to the end of the calling matrix and returns the new resized matrix
	 */
//	void addColumn(std::vector <scalar_type> columnVector, math::matrix<scalar_type>& resized_matrix);
	scalar_type norm_inf() const;

	scalar_type norm_max() const;

	void matrix_copy(math::matrix<scalar_type>& destination) const;
	void matrix_join(math::matrix<scalar_type> mat2,
			math::matrix<scalar_type>& joined_matrix) const;
	//void matrix_Identity(math::matrix<scalar_type>& newIdentityMatrix);
	/*
	 * inverse of a matrix : Returns True if Inverse Exists otherwise returns False
	 */
	bool inverse(math::matrix<scalar_type>& res) const;
	/* Returns the boolean status of invertibility */
	bool isInvertible() const;
	/**
	 * Returns true if identity, false othwerwise
	 */
	bool isIdentity() const;

	/*private:
	 ublas_matrix_impl my_matrix;*/

	// overloaded operators
	bool operator==(const math::matrix<scalar_type>& M);

	/** Overloaded << operator */
	template<class U>
	friend std::ostream& operator << (std::ostream &os, const math::matrix<U> &t);

};

#include "../../core/math/matrix.cpp"

} // end of math namespace

#endif /* MATRIX_H_ */
