/*
 * simulation.cpp
 *
 *  Created on: 12-Feb-2016
 *      Author: Rajarshi
 */

#include <utilities/dbgMsgLogger.h>
#include "counterExample/simulation.h"
#include <fstream>
#include <iomanip>
#include "core/continuous/polytope/polytope.h"



simulation::simulation() {
	// TODO Auto-generated constructor stub
}

simulation::~simulation() {
	// TODO Auto-generated destructor stub
}

/* Private function to check function return values */

static int check_flag(void *flagvalue, const char *funcname, int opt);

static int jtv (realtype t, N_Vector y, N_Vector fy, SUNMatrix Jac,
                void *user_data, N_Vector tmp1, N_Vector tmp2, N_Vector tmp3);

static int f(realtype t, N_Vector y, N_Vector ydot, void *f_data) {
	Dynamics* D = (Dynamics *) (f_data);
	math::matrix<double> A;

	if (D->isEmptyMatrixA) {
		assert(!D->isEmptyC); // The assumption is that this function is called with non-zero dynamics.
		unsigned int dim = D->C.size();
		A = math::matrix<double>(D->C.size(), D->C.size());
		for (unsigned int i = 0; i < dim; i++)
			for (unsigned int j = 0; j < dim; j++)
				A(i, j) = 0;
	} else
		A = math::matrix<double>(D->MatrixA);

	std::vector<double> C(A.size1());

	assert(A.size1() == A.size2());
	C = D->C;

	double sum;
	// Input set not included in the dynamics.

	for (unsigned int i = 0; i < A.size1(); i++) {
		sum = 0;
		for (unsigned int j = 0; j < A.size2(); j++) {
			sum += A(i, j) * NV_Ith_S(y,j) ;
		}
		NV_Ith_S(ydot,i) = sum + C[i];
	}
	return 0;
}

std::vector<double> lin_interpolate(std::vector<double> x_l2,
		std::vector<double> x_l1, double factor) {
	assert(x_l1.size() == x_l2.size());
	std::vector<double> res(x_l1.size());
	for (unsigned int i = 0; i < x_l1.size(); i++)
		res[i] = factor * x_l2[i] + x_l1[i] * (1 - factor);
	return res;
}
std::vector<double> simulation::simulate(std::vector<double> x, double time) {

	int flag;
	realtype T0 = 0;
	realtype Tfinal = time;
	realtype t = 0;
	N = time / time_step;
	Dynamics *data = &D;

	N_Vector u = NULL;

	if (data->isEmptyMatrixA && data->isEmptyC) // zero dynamics
		return x; // return same initial point

	assert(x.size() == dimension);
	u = N_VNew_Serial(dimension);

	for (unsigned int i = 0; i < dimension; i++)
		NV_Ith_S(u,i) = x[i];

	void *cvode_mem;
	cvode_mem = NULL;
	/* Call CVodeCreate to create the solver memory and specify the
	 * Backward Differentiation Formula and the use of a Newton iteration */

	cvode_mem = CVodeCreate(CV_BDF);

	if (check_flag((void *) cvode_mem, "CVodeCreate", 0)) {
		throw std::runtime_error("CVODE failed\n");
	}

	/** Input user data */
	CVodeSetUserData(cvode_mem, (void *) data);
	/* Call CVodeInit to initialize the integrator memory and specify the
	 * user's right hand side function in u'=f(t,u), the inital time T0, and
	 * the initial dependent variable vector u. */

	flag = CVodeInit(cvode_mem, f, T0, u);

	if (check_flag(&flag, "CVodeInit", 1)) {
		throw std::runtime_error("CVODE failed\n");
	}

	/* Call CVodeSStolerances to specify the scalar relative tolerance
	 * and scalar absolute tolerance */
	flag = CVodeSStolerances(cvode_mem, reltol, abstol);
	if (check_flag(&flag, "CVodeSStolerances", 1)) {
		throw std::runtime_error("CVODE failed\n");
	}

	// Create Matrix Object.
	// ---------------------------------------------------------------------------
	// Need to create a dense matrix for the dense solver.
	SUNMatrix A = SUNDenseMatrix(dimension, dimension);
	if(check_flag((void *)A, "SUNDenseMatrix", 0)){
		throw std::runtime_error("CVODE failed\n");
	}
	// ---------------------------------------------------------------------------

	// 9. Create Linear Solver Object.
	// ---------------------------------------------------------------------------
	// Dense linear solver object instead of the iterative one in the original
	// simple example.
	SUNLinearSolver LS = SUNDenseLinearSolver(u, A) ;
	if(check_flag((void *)LS, "SUNDenseLinearSolver", 0)){
	  throw std::runtime_error("In simulation::simulate, SUNDenseLinearSolver: CVODE failed\n");
	}
	// ---------------------------------------------------------------------------

	// 10. Set linear solver optional inputs.

	// 11. Attach linear solver module.
	// ---------------------------------------------------------------------------
	// Call CVDlsSetLinearSolver to attach the matrix and linear solver this
	// function is different for direct solvers.
	flag = CVDlsSetLinearSolver(cvode_mem, LS, A);
	if(check_flag(&flag, "CVDlsSetLinearSolver", 1)){
	  throw std::runtime_error("CVODE failed\n");
	}
	// ---------------------------------------------------------------------------

	// 12. Set linear solver interface optional inputs.
	// ---------------------------------------------------------------------------
	// Sets the jacobian-times-vector function.
	flag = CVDlsSetJacFn(cvode_mem, jtv);
	if(check_flag(&flag, "CVDlsSetJacFn", 1)){
	  throw std::runtime_error("CVODE failed\n");
	}
	/* In loop over output points: call CVode, print results, test for errors */

	std::vector<double> last(dimension);

	trace_point simpoint;
	simpoint.first = 0;
	simpoint.second = x;
	sim_trace.push_back(simpoint); // adding the starting simpoint in the sim_trace data-structure.

	for (unsigned int k = 1; k <= N; k++) {
		double tout = (k * Tfinal) / N;
		// remember this point in the last vector
		for (unsigned int i = 0; i < dimension; i++)
			last[i] = NV_Ith_S(u,i) ;
		trace_point simpoint;
		simpoint.first = tout;
		simpoint.second = last;
		sim_trace.push_back(simpoint); // adding the simpoint in the sim_trace data-structure.

		flag = CVode(cvode_mem, tout, u, &t, CV_NORMAL);
		if (check_flag(&flag, "CVode", 1))
			break;
	}

	std::vector<double> res(dimension);
	for (unsigned int i = 0; i < dimension; i++) {
		res[i] = NV_Ith_S(u,i) ;
	}
	N_VDestroy_Serial(u); /* Free u vector */
	CVodeFree(&cvode_mem); /* Free integrator memory */
	SUNLinSolFree(LS);
	SUNMatDestroy_Dense(A);
	return res;
}

/**
 * Bounded simulation. Simulation bounded within a polytope.
 * Sets status to false if invariant violated. Otherwise,
 * status is set to true.
 * If the Inv is violated with distance < tol, tolerate the
 * violation.
 */

bound_sim simulation::bounded_simulation(std::vector<double> x, double time,
		polytope::const_ptr I, bool& status, double tol) {

	int flag;
	realtype T0 = 0;
	realtype Tfinal = time;
	realtype t = 0;
	Dynamics *data = &D;
	bound_sim b;
	N_Vector u = NULL;
	N = Tfinal / time_step;

	if (data->isEmptyMatrixA && data->isEmptyC) { // zero dynamics
		bound_sim b;
		b.v = x;
		b.cross_over_time = time; // The point remains inside the invariant for the entire time horizon
		return b; // return the initial point, time horizon

	}

	assert(x.size() == dimension);
	u = N_VNew_Serial(dimension);

	double dist = math::abs(I->point_distance(x));
	if (dist > tol) {
		//throw std::runtime_error("bounded simulation: initial point outside invariant. NLP problem constrains not set correctly\n");
		bound_sim b;
		b.v = x;
		b.cross_over_time = 0;
		status = false;
		return b;
	}
	// Add the starting point to the sim_trace otherwise here
	trace_point simpoint;
	simpoint.first = 0;
	simpoint.second = x;
	sim_trace.push_back(simpoint); // adding the starting simpoint in the sim_trace data-structure.

	for (unsigned int i = 0; i < dimension; i++)
		NV_Ith_S(u,i) = x[i];

	void *cvode_mem;
	cvode_mem = NULL;
	// Call CVodeCreate to create the solver memory and specify the
	// Backward Differentiation Formula and the use of a Newton iteration

	cvode_mem = CVodeCreate(CV_BDF);

	if (check_flag((void *) cvode_mem, "CVodeCreate", 0)) {
		throw std::runtime_error("CVODE failed\n");
	}

	// Input user data
	CVodeSetUserData(cvode_mem, (void *) data);

	// Call CVodeInit to initialize the integrator memory and specify the
	//ser's right hand side function in u'=f(t,u), the inital time T0, and
	// the initial dependent variable vector u.

	flag = CVodeInit(cvode_mem, f, T0, u);

	if (check_flag(&flag, "CVodeInit", 1)) {
		throw std::runtime_error("CVODE failed\n");
	}

	flag = CVodeSStolerances(cvode_mem, reltol, abstol);
	if (check_flag(&flag, "CVodeSStolerances", 1)) {
		throw std::runtime_error("CVODE failed\n");
	}

	// Create Matrix Object.
	// ---------------------------------------------------------------------------
	// Need to create a dense matrix for the dense solver.
	SUNMatrix A = SUNDenseMatrix(dimension, dimension);
	if(check_flag((void *)A, "SUNDenseMatrix", 0)){
		throw std::runtime_error("CVODE failed\n");
	}
	// ---------------------------------------------------------------------------

	// 9. Create Linear Solver Object.
	// ---------------------------------------------------------------------------
	// Dense linear solver object instead of the iterative one in the original
	// simple example.
	SUNLinearSolver LS = SUNDenseLinearSolver(u, A) ;
	if(check_flag((void *)LS, "SUNDenseLinearSolver", 0)){
	  throw std::runtime_error("CVODE failed\n");
	}
	// ---------------------------------------------------------------------------

	// 10. Set linear solver optional inputs.

	// 11. Attach linear solver module.
	// ---------------------------------------------------------------------------
	// Call CVDlsSetLinearSolver to attach the matrix and linear solver.
	flag = CVDlsSetLinearSolver(cvode_mem, LS, A);
	if(check_flag(&flag, "CVDlsSetLinearSolver", 1)){
	  throw std::runtime_error("CVODE failed\n");
	}
	// ---------------------------------------------------------------------------

	// 12. Set linear solver interface optional inputs.
	// ---------------------------------------------------------------------------
	// Sets the jacobian-times-vector function.
	flag = CVDlsSetJacFn(cvode_mem, jtv);
	if(check_flag(&flag, "CVDlsSetJacFn", 1)){
	  throw std::runtime_error("CVODE failed\n");
	}

	std::vector<double> v(dimension), prev_v(dimension);
	prev_v = x;

	for (unsigned int k = 1; k <= N; k++) {
		double tout = (k * Tfinal) / N;
		flag = CVode(cvode_mem, tout, u, &t, CV_NORMAL);
		if (check_flag(&flag, "CVode", 1))
			break;
		// check polytope satisfaction
		for (unsigned int i = 0; i < dimension; i++)
			v[i] = NV_Ith_S(u,i) ;

		double dist = I->point_distance(v);
		if (math::abs(dist) > tol) {
//				std::cout << "time:" << t << " ";
			std::cout << "DISTANCE TO INV= " << std::setprecision(10) << dist
					<< std::endl;
			//std::cout << "tol = " << std::setprecision(10) << tol << std::endl;
			/*
			std::cout << "Violating trajectory start point:\n";
			for(unsigned int j=0;j<dimension;j++)
				std::cout << x[j] << " ";
			std::cout << std::endl;
			*/
			status = false;
			trace_point simpoint;
			simpoint.first = tout;
			simpoint.second = v;
			sim_trace.push_back(simpoint); // adding the simpoint in the sim_trace data-structure.
			break;
		} else {
			trace_point simpoint;
			simpoint.first = tout;
			simpoint.second = v;
			sim_trace.push_back(simpoint); // adding the simpoint in the sim_trace data-structure.
			prev_v = v;
		}
	}
	b.v = v;
	b.cross_over_time = t;

	N_VDestroy_Serial(u);
	CVodeFree(&cvode_mem);
	SUNLinSolFree(LS);
	SUNMatDestroy_Dense(A);
	return b;
}

/**
 * Routine to print the simulation trace to a output file.
 */
void simulation::print_trace_to_outfile(std::string s) {
	std::ofstream myoutfile;
	myoutfile.open(s.c_str(),std::fstream::app);
	for(std::list<trace_point>::const_iterator it = sim_trace.begin();
			it != sim_trace.end(); it++) {
		myoutfile << (*it).second[this->x1] << "  " << (*it).second[this->x2] << "\n";
	}
	myoutfile.close();
}

/* Check function return value...
 opt == 0 means SUNDIALS function allocates memory so check if
 returned NULL pointer
 opt == 1 means SUNDIALS function returns a flag so check if
 flag >= 0
 opt == 2 means function allocates memory so check if returned
 NULL pointer */

static int check_flag(void *flagvalue, const char* funcname, int opt) {
	int *errflag;

	/* Check if SUNDIALS function returned NULL pointer - no memory allocated */

	if (opt == 0 && flagvalue == NULL) {
		fprintf(stderr,
				"\nSUNDIALS_ERROR: %s() failed - returned NULL pointer\n\n",
				funcname);
		return (1);
	}

	/* Check if flag < 0 */

	else if (opt == 1) {
		errflag = (int *) flagvalue;
		if (*errflag < 0) {
			fprintf(stderr, "\nSUNDIALS_ERROR: %s() failed with flag = %d\n\n",
					funcname, *errflag);
			return (1);
		}
	}

	/* Check if function returned NULL pointer - no memory allocated */

	else if (opt == 2 && flagvalue == NULL) {
		fprintf(stderr,
				"\nMEMORY_ERROR: %s() failed - returned NULL pointer\n\n",
				funcname);
		return (1);
	}

	return (0);
}
static int jtv (realtype t, N_Vector y, N_Vector fy, SUNMatrix Jac,
                void *user_data, N_Vector tmp1, N_Vector tmp2, N_Vector tmp3)
{
	Dynamics* D = (Dynamics *) (user_data);
	math::matrix<double> A;

	if (D->isEmptyMatrixA) {
		assert(!D->isEmptyC); // The assumption is that this function is called with non-zero dynamics.
		unsigned int dim = D->C.size();
		A = math::matrix<double>(D->C.size(), D->C.size());
		for (unsigned int i = 0; i < dim; i++)
			for (unsigned int j = 0; j < dim; j++)
				A(i, j) = 0;
	} else
		A = math::matrix<double>(D->MatrixA);

	std::vector<double> C(A.size1());

	assert(A.size1() == A.size2());
	C = D->C;

	// Input set not included in the dynamics.

	for (unsigned int i = 0; i < A.size1(); i++) {
		for (unsigned int j = 0; j < A.size2(); j++) {
			SM_ELEMENT_D(Jac, i, j) = A(i, j);
		}
	}
	return 0;
}
