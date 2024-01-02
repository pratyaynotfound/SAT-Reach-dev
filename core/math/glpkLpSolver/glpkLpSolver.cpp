#ifndef LP_SOLVER_CPP_
#define LP_SOLVER_CPP_

#include <core/math/glpkLpSolver/glpkLpSolver.h>
#include "core/math/matrix.h"

using namespace std;

unsigned int glpk_lp_solver::lp_count = 0;		//initialize the static variable

glpk_lp_solver::~glpk_lp_solver() {
	lp_count--;
	glp_delete_prob(mylp);
}
void glpk_lp_solver::free_glpk_lp_solver() {
	lp_count--;
	glp_delete_prob(mylp);
}
void glpk_lp_solver::delete_prob()
{
	glp_delete_prob(mylp);
}
void glpk_lp_solver::free_environment_glpk_lp_solver() {
	glp_free_env();
}
glpk_lp_solver::glpk_lp_solver() {
	mylp = glp_create_prob();
	glp_init_smcp(&param);
	Min_Or_Max = GLP_MAX;	//Setting the GLP_MAX as the default value
	lp_count++;
	dimension = 0;
	number_of_constraints = 0;
	result = 0.0;
}

void glpk_lp_solver::setDefaultObject() {		//if this is a virtual member functions, then we cannot call from constructor
	mylp = glp_create_prob();
	glp_init_smcp(&param);
	Min_Or_Max = GLP_MAX;	//Setting the GLP_MAX as the default value
	lp_count++;
	dimension = 0;
	number_of_constraints = 0;
	result = 0.0;
}

void glpk_lp_solver::displayMaxVariables() {
	for (int i = 0; i < dimension; i++)
		cout << "\t x" << i + 1 << " = " << sv[i];
}

glpk_lp_solver::glpk_lp_solver(math::matrix<double> coeff_constraints,
		std::vector<double> bounds, std::vector<int> bound_signs) {
	lp_count++;
	Min_Or_Max = GLP_MAX;	//Setting the GLP_MAX as the default value
	mylp = glp_create_prob();
	glp_init_smcp(&param);

	dimension = coeff_constraints.size2();		//.at(0).size();
	number_of_constraints = bounds.size();
	result = 0.0;
}

void glpk_lp_solver::setMin_Or_Max(int Min_Or_Max) {
	if (Min_Or_Max == 1)
		this->Min_Or_Max = GLP_MIN;
	else if(Min_Or_Max == 2)
		this->Min_Or_Max = GLP_MAX;
	else
		throw std::runtime_error("setMin_Or_Max: LP MIN MAX type error\n");
	glp_set_obj_dir(mylp,this->Min_Or_Max);
}

int glpk_lp_solver::getMin_Or_Max() {
	return Min_Or_Max;
}

/*
 *
 * InEqualitySign :	The in equalities sign of the bound values 'b'. Possible values are
 * 					0 :	for  Ax = b (b Equals to)		FOR FUTURE USE IF ANY
 * 					1 :	for  Ax <= b (b is Greater Than or Equals to)
 * 					2 :	for  Ax >= b (b is Less Than or Equals to)
 *
 * We have not the bound values as 0 for now. At present the values are either 1 or 2.
 */

void glpk_lp_solver::setConstraints(math::matrix<double> coeff_constraints,
		std::vector<double> bounds, int bound_signs) {//here bound_sign is an Integer value

	dimension = coeff_constraints.size2();	//
	number_of_constraints = bounds.size();
	result = 0.0;
	glp_set_prob_name(mylp, "Sample");
	glp_set_obj_dir(mylp, Min_Or_Max);
	glp_add_rows(mylp, number_of_constraints);

	for (int i = 0; i < number_of_constraints; i++) {
		glp_set_row_name(mylp, i + 1, "p");
		if (bound_signs == 1)		//Ax<=b
			glp_set_row_bnds(mylp, i + 1, GLP_UP, 0.0, bounds[i]);
		else
			//Ax>=b
			glp_set_row_bnds(mylp, i + 1, GLP_LO, bounds[i], 0.0);
	}

	glp_add_cols(mylp, dimension);
	for (int i = 0; i < dimension; i++) {
		glp_set_col_name(mylp, i + 1, "xi");
		glp_set_col_bnds(mylp, i + 1, GLP_FR, 0.0, 0.0);
	}

	int prod = number_of_constraints * dimension;
	int count = 1, ia[prod+1], ja[prod+1];
	double ar[prod+1];
	for (int i = 0; i < number_of_constraints; i++) {
		for (int j = 0; j < dimension; j++) {
			ia[count] = i + 1, ja[count] = j + 1, ar[count] = coeff_constraints(
					i, j);
			count++;
		}
	}
	count--;

	glp_load_matrix(mylp, count, ia, ja, ar);

}

void glpk_lp_solver::join_poly_constraints(math::matrix<double> coeff_constraints,
			std::vector<double> bounds, int bound_signs){

	assert(coeff_constraints.size1() == bounds.size());

	this->dimension += coeff_constraints.size2();
	this->number_of_constraints += bounds.size();

	int r = glp_add_rows(mylp, bounds.size());

	for(unsigned int i=r,j=0;i < r+bounds.size();i++,j++)
	{
		if (bound_signs == 1)		//Ax<=b
			glp_set_row_bnds(mylp, i , GLP_UP, 0.0, bounds[j]);
		else
			//Ax>=b
			glp_set_row_bnds(mylp, i , GLP_LO, bounds[j], 0.0);
	}

	int c = glp_add_cols(mylp,coeff_constraints.size2());
	for (unsigned int i = c; i < c+coeff_constraints.size2(); i++) {
		glp_set_col_bnds(mylp, i, GLP_FR, 0.0, 0.0);
	}

	// set the new added rows and columns
	unsigned int len = coeff_constraints.size2();
	for(unsigned int i=r, row = 0;i<r+bounds.size();i++, row++)
	{
		int ind[len+1];
		double val[len+1];
		for(unsigned int j=0;j<len;j++){
			ind[j+1]= c + j; val[j+1] = coeff_constraints(row, j);
		}
		glp_set_mat_row(mylp, i, len, ind, val);
	}
}
void glpk_lp_solver::add_new_cols(int cols)
{
	this->dimension += cols;
	int c = glp_add_cols(mylp, cols);
	for (int i = c; i < c+cols; i++) {
		glp_set_col_bnds(mylp, i, GLP_LO, 0.0, 0.0);
	}
}
void glpk_lp_solver::add_new_constraint(int len, int ind[], double val[])
{
	this->number_of_constraints++;
	int r = glp_add_rows(mylp,1);

	glp_set_mat_row(mylp,r,len,ind,val);
	glp_set_row_bnds(mylp,r,GLP_UP,0.0,0.0);

}
void glpk_lp_solver::set_obj_coeff(unsigned int j, double val){
	glp_set_obj_coef(mylp, j + 1, val);	//modified to +1, since GLPK uses 1-based indexing
}

double glpk_lp_solver::get_obj_coeff(unsigned int j)
{
	return glp_get_obj_coef(mylp,j + 1);	//modified to +1, since GLPK uses 1-based indexing
}
double glpk_lp_solver::solve(){
	glp_init_smcp(&param);

	int status = glp_simplex(mylp,&param);
	if(status != 0)
	{
		std::cout << "GLPK solver failed to successfully solve the lp\n" << std::endl;
	}

	double result = glp_get_obj_val(mylp);
	/*
	 * Updated to Maximizing vector
	 */
	sv.resize(dimension, 0.0);
	for (int i = 0; i < dimension; i++)
		sv[i] = glp_get_col_prim(mylp, i + 1);

	return result;
}

double glpk_lp_solver::get_col_val(int i)
{
	return glp_get_col_prim(mylp, i);
}
void glpk_lp_solver::setIteration_Limit(int limits) {
	param.it_lim = limits;
}
void glpk_lp_solver::setInitial_SimplexControlParameters() {
	glp_init_smcp(&param);
}
unsigned int glpk_lp_solver::getStatus() {
	/*   solution status:
	 The routine glp_get_status reports the generic status of the current basic solution for
	 the specified problem object as follows:

	 #define GLP_UNDEF          1  // solution is undefined
	 #define GLP_FEAS           2  // solution is feasible
	 #define GLP_INFEAS         3  // solution is infeasible
	 #define GLP_NOFEAS         4  // no feasible solution exists
	 #define GLP_OPT            5  // solution is optimal
	 #define GLP_UNBND          6  // solution is unbounded
	 */
	unsigned int val = glp_get_status(mylp);
	return val;
}

unsigned int glpk_lp_solver::testConstraints() {

	// presolve to remove redundant constraints
	param.presolve = GLP_ON;

	//glp_simplex(mylp, &param);
	glp_exact(mylp, &param);
	int status = glp_get_status(mylp);

	return status;
}

double glpk_lp_solver::Compute_LLP(const std::vector<double> coeff_function) {//Here argument is a Vector

	for (int i = 0; i < dimension; i++) {
		glp_set_obj_coef(mylp, i + 1, coeff_function[i]);
	}
	glp_term_out(GLP_OFF);

	//glp_simplex(mylp, &param);  /* This api internally uses imprecise floating point arithmetic*/

	glp_simplex(mylp, &param);  /* This api internally used imprecise floating point arithmetic*/

	double result = glp_get_obj_val(mylp);
	int status = glp_get_status(mylp);
	if(status==GLP_UNBND)
	{
		return 999; // todo: Need to return large value
	}
	sv.resize(dimension, 0.0);
	for (int i = 0; i < dimension; i++)
		sv[i] = glp_get_col_prim(mylp, i + 1);

	return result;
}

std::vector<double> glpk_lp_solver::getMaximizing_Variables(){
	return this->sv;
}

#endif /* LP_SOLVER_CPP_ */
