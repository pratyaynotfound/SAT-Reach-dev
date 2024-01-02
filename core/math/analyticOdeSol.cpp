/*
 * analyticOdeSol.cpp
 *
 *  Created on: 06-Sep-2016
 *      Author: rajarshi
 */

#include <vector>

#include "../../core/math/analyticODESol.h"
#include "../../core/math/matrix.h"

std::vector<double> ODESol(std::vector<double> x0, const Dynamics& D, double time)
{
	/*if(!D.isEmptyMatrixB && !D.U->getIsEmpty()){
		std::cout << "File-analyticODEsol: Method-ODESol: Cannot solve ODE for dynamics with non-det input\n";
		exit(0);
	}*/
	if(D.isEmptyMatrixA && D.isEmptyMatrixB)
	{
		assert(!D.isEmptyC);
		unsigned int dim = x0.size();
		std::vector<double> res(x0);
		for(unsigned int i=0;i<dim;i++)
		{
			res[i]+=D.C[i]*time;
		}
		return res;
	}

	unsigned int dim = D.MatrixA.size2();

	assert(D.MatrixA.size1() == D.MatrixA.size2());
	assert(x0.size() == D.MatrixA.size2());

	math::matrix<double> expAt(dim,dim);
	math::matrix<double> A(D.MatrixA);
	math::matrix<double> At(A);


	At.scalar_multiply(time);

	At.matrix_exponentiation(expAt);

	std::vector<double> res1(dim), res2(dim), res3(dim);

	expAt.mult_vector(x0,res1);

	math::matrix<double> AInv(dim,dim);
	if(!A.inverse(AInv)) // meaning that the matrix A is singular,i.e., not invertible
	{
		// compute the expression A^-1 (e^At - I) alternatively as a sub-matrix of the exp(M),
		// as shown in the SpaceEx paper, page 8, phi_1.
		math::matrix<double> M(3*dim, 3*dim);
		// putting this check to see if M is initialized to 0 matrix above
		for(unsigned int i=0;i<3*dim;i++){
			for(unsigned int j=0;j<3*dim;j++)
				M(i,j) = 0;
		}
		// initialize the submatrix[0,dim-1][0,dim-1] with matrixAt
		for(unsigned int i=0;i<dim;i++)
		{
			for(unsigned int j=0;j<dim;j++)
				M(i,j) = At(i,j);
		}
		// initialize the [0,dim-1][dim,2*dim-1] with matrixAt
		for(unsigned int i=0,j=dim;i<2*dim;i++,j++)
			M(i,j)=time;
		// initialize the [dim,2*dim-1][2*dim,3*dim-1] with matrixAt
		for(unsigned int i=dim,j=2*dim;i<2*dim;i++,j++)
			M(i,j)=time;

		// compute the exponential of M
		math::matrix<double> expM(dim,dim);
		M.matrix_exponentiation(expM);
		// extract the submatrix [0,dim][dim+2*dim] into the matrix phi
		math::matrix<double> phi(dim,dim);
		for(unsigned int i=0;i<dim;i++)
		{
			for(unsigned int j=0;j<dim;j++)
				phi(i,j) = expM(i,dim+j);
		}
		phi.mult_vector(D.C,res2);
		for(unsigned int i=0;i<res1.size();i++){
			res1[i] = res1[i] + res2[i];
		}
		// debug
		//std::cout << "Implemented trace end point for non-invertible matrices\n";
		return res1;
	}
	std::vector<double> minusV(dim);

	for(unsigned int i=0;i<dim;i++)
		minusV[i] = (-1)*D.C[i];

	AInv.mult_vector(minusV,res2);

	math::matrix<double> mult(dim,dim);

	AInv.multiply(expAt,mult);
	mult.mult_vector(D.C,res3);


	for(unsigned int i=0;i<res1.size();i++){
		res1[i] = res1[i] + res2[i] + res3[i];
	}
//	std::cout << "Analytical Solution of ODE:";
//	for(unsigned int i=0;i<5;i++)
//		std::cout << res1[i] << " " << std::endl;
	return res1;
}

math::matrix<double> ODESol_homogenous_coeff(const Dynamics& D, double time){
	unsigned int dim = D.MatrixA.size2();

	if(!D.isEmptyMatrixB && !D.U->getIsEmpty()){
		std::cout << "File-analyticODEsol: Method-ODESol: Cannot solve ODE for dynamics with non-det input\n";
		exit(0);
	}
	if(D.isEmptyMatrixA)
	{
		math::matrix<double> iden(dim, dim);
		iden.matrix_Identity(dim, iden);
		return iden;
	} else {
		math::matrix<double> expAt(dim,dim);
		math::matrix<double> A(D.MatrixA);
		math::matrix<double> At(A);

		At.scalar_multiply(time);
		At.matrix_exponentiation(expAt);
		return expAt;
	}
}

std::vector<double> ODESol_inhomogenous(const Dynamics& D, double time){
	unsigned int dim = D.MatrixA.size2();

	math::matrix<double> expAt(dim,dim);
	math::matrix<double> A(D.MatrixA);
	math::matrix<double> At(A);

	At.scalar_multiply(time);

	At.matrix_exponentiation(expAt);

	std::vector<double> res2(dim), res3(dim);

	math::matrix<double> AInv(dim,dim);
	if(!A.inverse(AInv)) // meaning that the matrix A is singular, i.e., not invertible
	{
		// compute the expression A^-1 (e^At - I) alternatively as a sub-matrix of the exp(M),
		// as shown in the SpaceEx paper, page 8, phi_1.
		math::matrix<double> M(3*dim, 3*dim);
		// putting this check to see if M is initialized to 0 matrix above
		for(unsigned int i=0;i<3*dim;i++){
			for(unsigned int j=0;j<3*dim;j++)
				M(i,j) = 0;
		}
		// initialize the submatrix[0,dim-1][0,dim-1] with matrixAt
		for(unsigned int i=0;i<dim;i++)
		{
			for(unsigned int j=0;j<dim;j++)
				M(i,j) = At(i,j);
		}
		// initialize the [0,dim-1][dim,2*dim-1] with matrixAt
		for(unsigned int i=0,j=dim;i<2*dim;i++,j++)
			M(i,j)=time;
		// initialize the [dim,2*dim-1][2*dim,3*dim-1] with matrixAt
		for(unsigned int i=dim,j=2*dim;i<2*dim;i++,j++)
			M(i,j)=time;

		// compute the exponential of M
		math::matrix<double> expM(dim,dim);
		M.matrix_exponentiation(expM);
		// extract the submatrix [0,dim][dim+2*dim] into the matrix phi
		math::matrix<double> phi(dim,dim);
		for(unsigned int i=0;i<dim;i++)
		{
			for(unsigned int j=0;j<dim;j++)
				phi(i,j) = expM(i,dim+j);
		}
		phi.mult_vector(D.C,res2);
		// debug
		//std::cout << "Implemented trace end point for non-invertible matrices\n";
		return res2;
	}
	std::vector<double> minusV(dim);

	for(unsigned int i=0;i<dim;i++)
		minusV[i] = (-1)*D.C[i];

	AInv.mult_vector(minusV,res2);

	math::matrix<double> mult(dim,dim);

	AInv.multiply(expAt,mult);
	mult.mult_vector(D.C,res3);

	for(unsigned int i=0;i<res2.size();i++){
		res2[i] = res2[i] + res3[i];
	}

	return res2;
}

void extended_expm(const math::matrix<double>& A, double time, math::matrix<double>& res)
{
	unsigned int dim = A.size2();
	math::matrix<double> expAt(dim, dim);
	math::matrix<double> At(A);
	At.scalar_multiply(time);
	math::matrix<double> M(3 * dim, 3 * dim);

	for (unsigned int i = 0; i < 3 * dim; i++) {
		for (unsigned int j = 0; j < 3 * dim; j++)
			M(i, j) = 0;
	}
// initialize the submatrix[0,dim-1][0,dim-1] with matrixAt
	for (unsigned int i = 0; i < dim; i++) {
		for (unsigned int j = 0; j < dim; j++)
			M(i, j) = At(i, j);
	}
// initialize the [0,dim-1][dim,2*dim-1] with matrixAt
	for (unsigned int i = 0, j = dim; i < 2 * dim; i++, j++)
		M(i, j) = time;
// initialize the [dim,2*dim-1][2*dim,3*dim-1] with matrixAt
	for (unsigned int i = dim, j = 2 * dim; i < 2 * dim; i++, j++)
		M(i, j) = time;

	// compute the exponential of M
	M.matrix_exponentiation(res);
}
/* compute the expression A^-1 (e^At - I) alternatively as a sub-matrix of the exp(M),
 * as shown in the SpaceEx paper, page 8, phi_1.
 */
math::matrix<double> time_slice_component(const math::matrix<double>& A, double time)
{	
	unsigned int dim = A.size2();
	math::matrix<double> expM; 
	extended_expm(A,time,expM);
	// extract the submatrix [0,dim][dim+2*dim] into the matrix phi
	math::matrix<double> phi_1(dim, dim);
	for (unsigned int i = 0; i < dim; i++) {
		for (unsigned int j = 0; j < dim; j++)
			phi_1(i, j) = expM(i, dim + j);
	}
	return phi_1;
}

/*
 * compute the expression A^-2 (e^At - I - \delta.A) alternatively as a sub-matrix of the exp(M), as shown in the SpaceEx paper, page 8, phi_2.
 */
math::matrix<double> get_phi_2(math::matrix<double>& A, double time)
{
	unsigned int dim = A.size2();
	math::matrix<double> expM; 
	extended_expm(A,time,expM);
	// extract the submatrix [0,dim][2*dim+3*dim] into the matrix phi
	math::matrix<double> phi_2(dim, dim);
	for (unsigned int i = 0; i < dim; i++) {
		for (unsigned int j = 0; j < dim; j++)
			phi_2(i, j) = expM(i, 2*dim + j);
	}
	return phi_2;	
}
