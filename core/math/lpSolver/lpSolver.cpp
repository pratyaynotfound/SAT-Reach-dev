
#include <core/math/lpSolver/lpSolver.h>


int lp_solver::lp_solver_count = 0;
lp_solver::lp_solver() {

	lp_solver_count++; //one instance of glpk is created

	lp_solver_type = GLPK_SOLVER;	//By default

	if (lp_solver_type == GLPK_SOLVER) {
		glpk_lp_problem = glpk_lp_solver::glpk_ptr(new glpk_lp_solver());
	}
	// ******* More can be added later  **************
}

lp_solver::lp_solver(int solver_type) {

	lp_solver_count++; //one instance of glpk is created

	lp_solver_type = solver_type;

	if (lp_solver_type == GLPK_SOLVER) {
		glpk_lp_problem = glpk_lp_solver::glpk_ptr(new glpk_lp_solver());
	}

	// ******* More can be added later  **************
}
lp_solver::~lp_solver() {
}

void lp_solver::add_variables(int n)
{
	if (lp_solver_type == GLPK_SOLVER) {
		glpk_lp_problem->add_new_cols(n);
	}
	else{
		throw std::runtime_error("add new variable: Cannot add new variables in the LP problem with the chosen type of LP Solver\n");
	}
}
void lp_solver::add_new_constraint(int len, int ind[], double val[])
{
	if (lp_solver_type == GLPK_SOLVER) {
		glpk_lp_problem->add_new_constraint(len, ind, val);
	}
	else{
		throw std::runtime_error("add new constraint: Cannot add new constraint in the LP problem with the chosen type of LP Solver\n");
	}
}
double lp_solver::get_col_val(int i)
{
	if (lp_solver_type == GLPK_SOLVER) {
		glpk_lp_problem->get_col_val(i);
	}
	else{
		throw std::runtime_error("get_col_val: Getting the optimal value of a LP variable not implemented for the chosen LP type\n");
	}
}
void lp_solver::setMin_Or_Max(int Min_Or_Max) {

	if (lp_solver_type == GLPK_SOLVER) {
		glpk_lp_problem->setMin_Or_Max(Min_Or_Max);
	}

	else{
		throw std::runtime_error("Set min or max: Cannot set LP problem objective function with the chosen type of LP Solver\n");
	}
	// ******* More can be added later  **************
}

void lp_solver::set_obj_coeff(unsigned int j, double val)
{
	if (lp_solver_type == GLPK_SOLVER) {
		glpk_lp_problem->set_obj_coeff(j+1, val);
	}
	else{
		throw std::runtime_error("set objective coeff: Cannot set objective function coefficient in the LP problem with the chosen type of LP Solver\n");
	}
}
double lp_solver::get_obj_coeff(unsigned int j)
{
	if (lp_solver_type == GLPK_SOLVER) {
		glpk_lp_problem->get_obj_coeff(j+1);
	}
	else{
		throw std::runtime_error("set objective coeff: Cannot set objective function coefficient in the LP problem with the chosen type of LP Solver\n");
	}
}
double lp_solver::solve()
{
	if (lp_solver_type == GLPK_SOLVER) {
			double res = glpk_lp_problem->solve();
			return res;
	}
	else {
		throw std::runtime_error("solve objective coeff: Cannot set objective function coefficient in the LP problem with the chosen type of LP Solver\n");
	}
}
void lp_solver::setConstraints(math::matrix<double> coeff_constraints,
		std::vector<double> bounds, int bound_signs) {

	if (lp_solver_type == GLPK_SOLVER) {
		glpk_lp_problem->setConstraints(coeff_constraints, bounds, bound_signs);
	}

	// ******* More can be added later  **************
}

void lp_solver::join_poly_constraints(math::matrix<double> coeff_constraints,
		std::vector<double> bounds, int bound_signs)
{
	if (lp_solver_type == GLPK_SOLVER) {
		glpk_lp_problem->join_poly_constraints(coeff_constraints, bounds, bound_signs);
	}
	else
	{
		throw std::runtime_error("join_poly_constraints: Cannot join new polytopes in the current LP problem with the chosen type of LP Solver\n");
	}

}

double lp_solver::Compute_LLP(const std::vector<double> coeff_function) {
	double res = 0.0;
	if (lp_solver_type == GLPK_SOLVER) {
		res = glpk_lp_problem->Compute_LLP(coeff_function);
		status = glpk_lp_problem->getStatus();
	}
	if(status == GLP_UNBND) {
		DEBUG_MSG("lp_solver: Unbounded solution to the objective function");
		throw(new std::exception());
	}
	return res;
}
/**
 * Return the support vector,i.e., the maximizing/minimizing
 * vector for the given objection function
 */
std::vector<double> lp_solver::get_sv() {
	std::vector<double> Maximum_point;
	if (lp_solver_type == GLPK_SOLVER) {
		Maximum_point = glpk_lp_problem->getMaximizing_Variables();
	}
	return Maximum_point;
}

int lp_solver::getMin_Or_Max() {
	int res = 2;
	if (lp_solver_type == GLPK_SOLVER) {
		res = glpk_lp_problem->getMin_Or_Max();
	}

	return res;

}

void lp_solver::setIteration_Limit(int limits) {
	if (lp_solver_type == GLPK_SOLVER) {
		glpk_lp_problem->setIteration_Limit(limits);
	}

}

void lp_solver::setInitial_SimplexControlParameters() {
	if (lp_solver_type == GLPK_SOLVER) {
		glpk_lp_problem->setInitial_SimplexControlParameters();
	}

}

unsigned int lp_solver::getStatus() {
	return status;
}
unsigned int lp_solver::testConstraints() {

	unsigned int status = 5;

	if (lp_solver_type == GLPK_SOLVER) {
		status = glpk_lp_problem->testConstraints();
	}

	return status;
}

void lp_solver::delete_prob(){
	if (lp_solver_type == GLPK_SOLVER) {
		glpk_lp_problem->delete_prob();
	}
	else
	{
		throw std::runtime_error("Delete problem: Cannot delete LP problem of the chosen type of LP Solver\n");
	}
}
void lp_solver::free_environment_glpk_lp_solver() {
	if (lp_solver_type == GLPK_SOLVER) {
		glpk_lp_problem->free_environment_glpk_lp_solver();
	}

}

int lp_solver::getLP_Solver_Type() {
	return lp_solver_type;
}

