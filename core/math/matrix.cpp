/**
 * Matrix class with operations
 */

#ifndef MATRIX_CPP_
#define MATRIX_CPP_

#include "core/math/matrix.h"

#include <iostream>


using namespace std;

/** constructors */

template<typename scalar_type> math::matrix<scalar_type>::matrix() {
	ublas_matrix_impl();
}
template<typename scalar_type> math::matrix<scalar_type>::matrix(size_type r,
		size_type c) :
		ublas_matrix_impl(r, c) {
}
template<typename scalar_type> math::matrix<scalar_type>::matrix(size_type r,
		size_type c, const scalar_type init_val) : ublas_matrix_impl(r,c,init_val){

}
template<typename scalar_type> math::matrix<scalar_type>::matrix(size_type r,
		size_type c, array_type data) :	ublas_matrix_impl(r, c, data) {
}

/**
 * returns the exponentiated matrix U = exp(A*time_tau)
 */

template<typename scalar_type> void math::matrix<scalar_type>::matrix_exponentiation(
		math::matrix<scalar_type>& res, double time_tau) const {

	double *m = new double[this->size1()*this->size2()];

	for(unsigned int i=0;i<this->size1();i++){
		for(unsigned int j=0;j<this->size2();j++){
			m[i*this->size2() + j] = (*this)(i,j)*time_tau;
		}
	}

	assert(this->size1() == this->size2());
	double *expm = r8mat_expm1(this->size1(), m );
	res = math::matrix<scalar_type>(this->size1(), this->size2());

	for(unsigned int i=0;i<this->size1();i++){
		for(unsigned int j=0;j<this->size2();j++){
			res(i,j) = expm[i*this->size2() + j];
		}
	}
	delete[] m;
	delete[] expm;
}
/**
 * returns the exponentiated matrix U = exp(A)
 */

template<typename scalar_type>
void math::matrix<scalar_type>::matrix_exponentiation(
		math::matrix<scalar_type>& res) const {

	double *m = new double[this->size1()*this->size2()];

	for(unsigned int i=0;i<this->size1();i++){
		for(unsigned int j=0;j<this->size2();j++){
			m[i*this->size2() + j] = (*this)(i,j);
		}
	}

	assert(this->size1() == this->size2());
	double *expm = r8mat_expm1(this->size1(), m );
	res = math::matrix<scalar_type>(this->size1(), this->size2());

	for(unsigned int i=0;i<this->size1();i++){
		for(unsigned int j=0;j<this->size2();j++){
			res(i,j) = expm[i*this->size2() + j];
		}
	}
	delete[] m;
	delete[] expm;
}	

/**
 * Matrix Multiply routine with ublas matrix. The matrix passed as parameter is multiplied
 * with the caller matrix and the result is assigned to the caller matrix.
 */

template<typename scalar_type> void math::matrix<scalar_type>::multiply(
		const matrix& A, matrix& res) const {

	assert(this->size2() == A.size1());

	ublas_matrix_impl m1(this->size1(), this->size2(), this->data());
	ublas_matrix_impl m2(A.size1(), A.size2(), A.data());

	m1 = boost::numeric::ublas::prod(m1, m2);
	res = math::matrix<scalar_type>(m1.size1(), m1.size2(), m1.data());
}

template<typename scalar_type> void math::matrix<scalar_type>::minus(
		matrix& B, matrix& res) {

	ublas_matrix_impl m1(this->size1(), this->size2(), this->data());
	ublas_matrix_impl m2(B.size1(), B.size2(), B.data());

	m1 = m1 - m2;	//matrix subtraction
	res = math::matrix<scalar_type>(m1.size1(), m1.size2(), m1.data());
}



/**
 * Matrix vector multiplication. The result is stored in the passed argument res
 */
template<typename scalar_type> void math::matrix<scalar_type>::mult_vector(
		std::vector<scalar_type> v, std::vector<scalar_type> &res) const {
	assert(this->size2() == v.size());

	ublas_matrix_impl m(this->size1(), this->size2(), this->data());
	ublas_vector_impl uv(v.size());

	for (unsigned int i = 0; i < v.size(); i++)
		uv(i) = v[i];
	uv = prod(m, uv);
	res.resize(uv.size());
	//cout<<"\n"<<v.size()<<endl;

	for (unsigned int i = 0; i < uv.size(); i++)	//m.size1()  or uv.size()
		res[i] = uv(i);
}

template<typename scalar_type> void math::matrix<scalar_type>::scalar_multiply(double c){
	for(unsigned int i=0;i<this->size1();i++){
		for(unsigned int j=0;j<this->size2();j++){
			this->at_element(i,j) = this->at_element(i,j)*c;
		}
	}
}

/**
 * Implements the transpose and assings the result to res.
 */
template<typename scalar_type> void math::matrix<scalar_type>::transpose(
		matrix& res) const {

	size_type r = this->size2();
	size_type c = this->size1();
	ublas_matrix_impl m(r, c);
	for (size_type i = 0; i < r; i++) {
		for (size_type j = 0; j < c; j++)
			m(i, j) = (*this)(j, i);
	}
	res = math::matrix<scalar_type>(m.size1(), m.size2(), m.data());
}
/**
  * Gets the absolute matrix M = (|m_{i,j}|) in res
  */
template<typename scalar_type> void math::matrix<scalar_type>::absolute(matrix& res) const {
	size_type r = this->size1();
	size_type c = this->size2();
	ublas_matrix_impl m(r, c);
	for (size_type i = 0; i < r; i++) {
		for (size_type j = 0; j < c; j++)
			m(i, j) = fabs((*this)(i, j));
	}
	res = math::matrix<scalar_type>(m.size1(), m.size2(), m.data());	
}

template<typename scalar_type> void math::matrix<scalar_type>::matrix_copy(
		math::matrix<scalar_type>& destination) const
{
	ublas_matrix_impl m(this->size1(), this->size2(), this->data());
	destination = math::matrix<scalar_type>(m.size1(), m.size2(), m.data());
}

template<typename scalar_type> void math::matrix<scalar_type>::matrix_Identity(int dimension,
		math::matrix<scalar_type>& newIdentityMatrix) const {
	//boost::numeric::ublas::identity_matrix<scalar_type> iden(dimension);
	ublas_matrix_impl m(dimension, dimension);
	for (int i=0;i<dimension;i++){
		for (int j=0;j<dimension;j++){
			if (i==j)
				m(i,j) = 1;
			else
				m(i,j) = 0;
		}
	}
	newIdentityMatrix = math::matrix<scalar_type>(m.size1(), m.size2(), m.data());
}

template<typename scalar_type> void math::matrix<scalar_type>::matrix_join(matrix mat2, matrix& joined_matrix) const {
	size_type row, col;
	row = this->size1();
	col = this->size2();
	if (mat2.size2()==0){	//second matrix is empty
		joined_matrix = matrix(row,col,this->data());
	} else if (col==0){
		joined_matrix = mat2;
	} else if (col == mat2.size2()) {
		ublas_matrix_impl m(this->size1(), this->size2(), this->data());
		matrix mat1;
		mat1 = math::matrix<scalar_type>(m.size1(), m.size2(), m.data());
		row = row + mat2.size1(); //only row will increase as the col is the system_dimension, so will not change
		matrix mat2_temp(row, col); //cout << "This is the new Rows = " << mat2_temp.size1();
		mat1.matrix_copy(mat2_temp);
		mat2_temp.resize(row, col, true); //cout << "This is the new Rows after matrix_copy and resize = " << mat2_temp.size1();
		//joined_matrix.resize(row, col);		//mat1.matrix_copy(joined_matrix);		//not working here
		for (size_type i = mat1.size1(), index_i = 0; i < mat2_temp.size1();i++, index_i++) {
			for (size_type j = 0; j < mat2_temp.size2(); j++) {
				mat2_temp(i, j) = mat2(index_i, j);
			}
		}
		joined_matrix = math::matrix<scalar_type>(mat2_temp.size1(),
				mat2_temp.size2(), mat2_temp.data());
	} else {
		cout << "Matrices are not of the same dimension:: number of columns do not match!!!";
	}
}

/**
 * returns the infinity norm of the matrix. inf norm of a matrix m is defined as
 * maximum absolute row sum.
 */
template<typename scalar_type> scalar_type math::matrix<scalar_type>::norm_inf() const {
	scalar_type norm = 0;
	scalar_type sum;
	for (size_type i = 0; i < this->size1(); i++) {
		sum=0;
		for (size_type j = 0; j < this->size2(); j++) {
			sum = sum + fabs((*this)(i,j));
		}
		if (sum > norm)
			norm = sum;
	}
	return norm;
}

/**
 * returns the Max norm of the matrix. Max norm of a matrix m is defined as
 * max_norm = max(a_{i,j}), 0<=i<r, 0<=j<c
 *
 */
template<typename scalar_type> scalar_type math::matrix<scalar_type>::norm_max() const {
	scalar_type norm = 0;
	for (size_type i = 0; i < this->size1(); i++) {
		for (size_type j = 0; j < this->size2(); j++) {
			if (abs((*this)(size_type(i), size_type(j))) > norm)
				norm = abs((*this)(size_type(i), size_type(j)));
		}
	}
	return norm;
}

/**
 * Computes the inverse of the matrix.
 *
 * \return The inverse matrix if it exists. If the matrix is singular, then the parameter singular is set to true.
 * singular is set to false otherwise. The result is put in the argument res.
 * @notice This function works doesn't work for Rational type
 */
/* Matrix inversion routine.
 Uses lu_factorize and lu_substitute in uBLAS to invert a matrix */
template<typename scalar_type>
bool math::matrix<scalar_type>::inverse(math::matrix<scalar_type>& inverse) const {
	if(!isInvertible())
		return false;
	using namespace boost::numeric::ublas;
	typedef permutation_matrix<std::size_t> pmatrix;

	// create identity matrix of "inverse"
	inverse.assign(identity_matrix<scalar_type>(this->size1()));
	ublas_matrix_impl A(this->size1(), this->size2(), this->data());
	pmatrix pm(A.size1());
	lu_factorize(A, pm);
	// backsubstitute to get the inverse
	lu_substitute(A, pm, inverse);
	return true;	//	NonSingular=true;
}

template<typename scalar_type>
bool math::matrix<scalar_type>::isInvertible() const
{
	assert(this->size1() == this->size2());
	using namespace boost::numeric::ublas;
	typedef permutation_matrix<std::size_t> pmatrix;
	// create a working copy of the input
	ublas_matrix_impl A(this->size1(), this->size2(), this->data());
	// create a permutation matrix for the LU-factorization
	pmatrix pm(A.size1());
	// perform LU-factorization
	int res = lu_factorize(A, pm);
	if (res != 0) {
		//NonSingular=false;
		return false;
	}
	else return true;
}

template<typename scalar_type>
bool math::matrix<scalar_type>::isIdentity() const
{
	if(this->size1() != this->size2())
		return false;
	
	using namespace boost::numeric::ublas;

	for(unsigned int i=0;i<this->size1();i++){
		for(unsigned int j=0;j<this->size2();j++){
			if(i==j && (*this)(size_type(i), size_type(j))!=1)
				return false;
			if(i!=j && (*this)(size_type(i), size_type(j))!=0)
				return false;
		}
	}
	return true;
}

template<typename scalar_type>
bool math::matrix<scalar_type>::operator==(const math::matrix<scalar_type>& M)
{
	if(this->size1() != M.size1() || this->size2() != M.size2()) return false;

	for(unsigned int i=0;i<this->size1();i++)
		for(unsigned int j=0;j<this->size2();j++)
			if(this->at_element(size_t(i),size_t(j))!=M(i,j) )
				return false;
	return true;
}

template<class T>
std::ostream& operator<<(std::ostream &os, const math::matrix<T> &t)
{
	os << std::endl;

	for(unsigned int i=0;i<t.size1();i++){
		for(unsigned int j=0;j<t.size2();j++){
			os << t(i,j) << " ";
		}
		os << std::endl;
	}

	os << std::endl;
	return os;
}
#endif
