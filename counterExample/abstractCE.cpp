/*
 * abstractCE.cpp
 *
 *  Created on: 22-Jan-2016
 *      Author: Rajarshi
 */
#include "counterExample/abstractCE.h"


unsigned int dim;
unsigned int N;

hybrid_automata* HA;
std::vector<int> locIdList;
std::list<transition::ptr> transList;
polytope::ptr bad_poly;
std::list<refinement_point> ref_pts;
std::vector<std::vector<double> > X0; // list of start point of the trajectory segments. Used only in the NLP-LP mixed program

std::list<symbolic_states::ptr> ce_sym_states; // list of CE abstract sym states. Used only in the NLP-LP mixed problem
std::vector<double> start_pos; // the start vectors of opt trajectory from LP solving. Useful in the context of CE_ALGO_TYPE 5

unsigned int samples = 0;


//constructor method
abstractCE::abstractCE(std::list<symbolic_states::ptr> s_states,
		std::list<transition::ptr> ts, hybrid_automata* h,
		polytope::ptr fpoly) {

	sym_states = s_states;
	trans = ts;
	length = sym_states.size();
	ha = h;
	forbid_poly = fpoly;
}

symbolic_states::const_ptr abstractCE::get_first_symbolic_state() const {
	std::list<symbolic_states::ptr>::const_iterator it = sym_states.begin();
	return *it;
}

symbolic_states::const_ptr abstractCE::get_last_symbolic_state() const {
	std::list<symbolic_states::ptr>::const_iterator it = sym_states.end();
	return *it;
}

void abstractCE::set_sym_states(std::list<symbolic_states::ptr> sym) {
	sym_states = sym;
	length = sym_states.size();
}

symbolic_states::const_ptr abstractCE::get_symbolic_state(
		unsigned int i) const {
	assert(0 <= i && i < get_length());
	unsigned int j = 0;
	std::list<symbolic_states::ptr>::const_iterator it = sym_states.begin();
	while (j < i) {
		it++;
		j++;
	}
	return *it;
}

/**
 * Routine to compute concrete trajectory from given
 * abstract counter example using non-linear optimization
 * routine and using the constraints from flowpipe
 *
 * @Rajarshi
 * The approach FC in the HSCC 2019 Paper
 */
concreteCE::ptr abstractCE::gen_concreteCE(double tolerance,
		const std::list<refinement_point> &refinements) {


//	 1. Get the dimension of the optimization problem by
//	 getting the dimension of the continuous set of the abstract counter example

	symbolic_states::const_ptr S = get_first_symbolic_state();
	dim = S->getInitialPolytope()->getSystemDimension();
	HA = this->get_automaton();
	transList = this->get_CE_transitions();
	N = transList.size() + 1; // the length of the counter example
	bad_poly = this->forbid_poly;
	ref_pts = refinements;

	// initialize the global locIdList
	locIdList.resize(N);

	DEBUG_MSG("Searching counterexample to safety with trajectory splicing (FC)");
	DEBUG_MSG("Location ID sequence in symbolic CE: ");
	std::set<int> d;
	for (unsigned int i = 0; i < N; i++) {
		d = this->get_symbolic_state(i)->getDiscreteSet().getDiscreteElements();
		assert(d.size() == 1);
		locIdList[i] = *(d.begin());
		DEBUG_MSG(to_string(locIdList[i]) + " | ");
	}

//	 2. The dimensionality of the opt problem is N vectors, one starting point
//	 for each of the abstract sym state of the CE + N dwell times. Moreover,
//	 each starting vector is of dimension dim. Therefore, the total number of
//	 variables of the optimization problem are dim*N + N

	unsigned int optD = N * dim + N;
	nlopt::opt myopt(nlopt::LD_MMA, optD); // derivative based

	// Parameters of the optimization routine
	unsigned int maxeval = 20000;
	unsigned int maxtime = 60; // time-out of 1 min per abstract ce.

	myopt.set_min_objective(myobjfunc2, NULL);
//	myopt.set_maxeval(maxeval);
	myopt.set_maxtime(maxtime);
	myopt.set_stopval(tolerance); // search stopping value, set typically to 1e-6.

	//Set Initial value to the optimization problem
	std::vector<double> x(optD, 0);

	std::vector<double> v(dim);

	std::vector<double> lb(optD), ub(optD);
	double max, min, start_min, start_max;

	// Add constraints on the start points of the trajectory
	for (unsigned int i = 0; i < N; i++)
	{
		// dxli: the whole flowpipe is a sequence of sub-flowpipes, each of which
		// denotes \omega in each location.
		// S is the sub-flowpipe in i-th sequence; P is the first polytope in S.

		S = get_symbolic_state(i);
		polytope::const_ptr P = S->getInitialPolytope();

		// set bound constraints
		lp_solver lp(GLPK_SOLVER);

		lp.setConstraints(P->getCoeffMatrix(), P->getColumnVector(),
				P->getInEqualitySign());

		// 	we add bound constraints on the position parameters, which are required to run global opt routines.
		std::vector<double> dir(dim, 0);
		double min, max;
		for (unsigned int j = 0; j < dim; j++) // iterate over each component of the x_i start point vector
		{
			dir[j] = -1;
			try {
				min = -1 * lp.Compute_LLP(dir);
			} catch (...) {
				// assuming that the exception is caused due to an unbounded solution
				min = -999;	// an arbitrary value set as solution
			}
			dir[j] = 1;
			try {
				max = lp.Compute_LLP(dir);
			} catch (...) {
				// assuming that the exception is caused due to an unbounded solution
				max = +999; // an arbitrary value set as solution
			}
			unsigned int index = i * dim + j;

			lb[index] = min;
			ub[index] = max;

			dir[j] = 0;
			// (min+max)/2 initializaion of variables.
			x[index] = (lb[index] + ub[index]) / 2;
		}
	}

//	Set initial value to the time variables
//	Restrict dwell time within the projections of C_i in time variable

//	We assume that the time variable is named as 't' in the model.
//	We find out the min, max components of the time variable

	unsigned int t_index =
			get_first_symbolic_state()->getInitialPolytope()->get_index("t");

	assert((t_index >= 0) && (t_index < dim));

	std::vector<double> dmin(dim, 0), dmax(dim, 0);
	dmax[t_index] = 1;
	dmin[t_index] = -1;

	std::list<polytope::ptr> polys;
	polytope::const_ptr guard, inv;

	std::list<transition::ptr>::iterator it = transList.begin();
	transition::ptr T;

	bool aggregation = true; //default is ON

	for (unsigned int i = 0; i < N; i++) {
		S = get_symbolic_state(i);
		polytope::ptr P;
		if (i == N - 1) {
			// If last abstract symbolic state, then take time projection of flowpipe \cap bad_poly
			polys = S->getContinuousSetptr()->flowpipe_intersectionSequential(
					aggregation, bad_poly, 1);
			assert(polys.size() >= 0); // The last sym state of an abstract CE must intersect with the bad set

			if (polys.size() > 1)
				P = get_template_hull(S->getContinuousSetptr(), 0,
						S->getContinuousSetptr()->getTotalIterations() - 1); // 100% clustering
			else
				P = polys.front();

			P = P->GetPolytope_Intersection(bad_poly);
		} else {

			// Take time projection of flowpipe \cap transition guard
			T = *(it);

			guard = T->getGuard();
			unsigned int loc_id = *(this->get_symbolic_state(i)->getDiscreteSet().getDiscreteElements()).begin();
			inv = HA->getLocation(loc_id)->getInvariant();

			if (!guard->getIsEmpty()){
				aggregation = false;
				if(aggregation){
					polys = S->getContinuousSetptr()->flowpipe_intersectionSequential(aggregation, guard, 1);
				}
				else {
					polys = S->getContinuousSetptr()->postD_chull(guard, inv, 1);
				}

			} else { // empty guard
				DEBUG_MSG("abstractCE::gen_concreteCE: Guard Set is empty. It means that the guard condition is unsatisfiable. \n");
				exit(0);
			}

			//polys = S->getContinuousSetptr()->flowpipe_intersectionSequential(aggregation, guard, 1);

			assert(polys.size() >= 1); // An abstract CE state must have intersection with the trans guard
			if (polys.size() > 1)
				P = get_template_hull(S->getContinuousSetptr(), 0,
						S->getContinuousSetptr()->getTotalIterations() - 1); // 100% clustering
			else
				P = polys.front();
			// Now intersect P with guard
			P = P->GetPolytope_Intersection(guard);
		}

		lp_solver lp(GLPK_SOLVER);
		lp.setConstraints(P->getCoeffMatrix(), P->getColumnVector(),
				P->getInEqualitySign());
		// ensure that time is always positive
		try {
			max = lp.Compute_LLP(dmax);
		} catch (...) {
			// assuming that the exception is caused due to an unbounded solution
			max = 999; // an arbitrary large value set as solution
		}
		try {
			min = -1 * lp.Compute_LLP(dmin);
		} catch (...) {
			// assuming that the exception is caused due to an unbounded solution
			min = 0; // the min value that time can take is 0.
		}

		// we add the bounds as constraints in the nlopt

		// Get the min and max time projection of start set
		lp_solver lp1(GLPK_SOLVER);
		polytope::const_ptr init_of_symb = S->getInitialPolytope();

		lp1.setConstraints(init_of_symb->getCoeffMatrix(),
				init_of_symb->getColumnVector(),
				init_of_symb->getInEqualitySign());
		// Ensure that the time is positive
		start_min = -1 * lp1.Compute_LLP(dmin);
		start_max = lp1.Compute_LLP(dmax);

		ub[N * dim + i] = max - start_min;

		if (min <= start_max)
			lb[N * dim + i] = 0;
		else
			lb[N * dim + i] = min - start_max;

		// We may choose to take the average time as the initial dwell time
		x[N * dim + i] = (ub[N * dim + i] + lb[N * dim + i]) / 2;

		// Increment the transition to the next in the symbolic path
		if (it != transList.end())
			it++;
	}
	// set the lower and upper bounds on vars
	myopt.set_lower_bounds(lb);
	myopt.set_upper_bounds(ub);

	double minf;
	try {
		myopt.set_stopval(tolerance);
		unsigned int res = myopt.optimize(x, minf);

		if (res == NLOPT_SUCCESS)
			DEBUG_MSG("Splicing with FC: NLOPT stopped successfully");
		else if (res == NLOPT_STOPVAL_REACHED)
			DEBUG_MSG("Splicing with FC: NLOPT stopped due to stopping value (1e-6) reached");
		else if (res == NLOPT_MAXEVAL_REACHED)
			DEBUG_MSG("Splicing with FC: NLOPT stopped due to reaching maxeval = " + to_string(maxeval));
		else if (res == NLOPT_MAXTIME_REACHED)
			DEBUG_MSG("Splicing with FC: NLOPT stopped due to reaching maxtime = " + to_string(myopt.get_maxtime()));

	} catch (std::exception &e) {
		string msg = e.what();
		DEBUG_MSG(msg + "\n");
	}

	DEBUG_MSG("nlopt returned min : " + to_string(minf));
	DEBUG_MSG("Length of abstract counter example:"+ to_string(N));
	DEBUG_MSG("Number of iterations completed:" + to_string(myopt.get_maxeval()));

	concreteCE::ptr cexample = concreteCE::ptr(new concreteCE());
	cexample->set_automaton(HA);
	if (minf > tolerance) {
		DEBUG_MSG("Obtained minimum greater than " + to_string(tolerance) + ", No. of refined search:" + to_string(refinements.size()) +
				" No. of iterations completed:" + to_string(myopt.get_maxeval()));
		return cexample;
	} else {
		std::ofstream ce_trace;
		// one trajectory per symbolic state to be added in the concreteCE
		for (unsigned int i = 0; i < N; i++) {
			// create the sample
			concreteCE::sample s;
			std::set<int>::iterator dset_iter =
					get_symbolic_state(i)->getDiscreteSet().getDiscreteElements().begin();
			unsigned int locId = *dset_iter;

			std::vector<double> y(dim);
			for (unsigned int j = 0; j < dim; j++) {
				y[j] = x[i * dim + j];
			}

			double time = x[N * dim + i];
			s.first = y;
			//s.second = y[dim-1];
			s.second = time;
			concreteCE::traj_segment traj;
			traj.first = locId;
			traj.second = s;
			cexample->push_back(traj);
		}
	}

	return cexample;
}


const userOptions& abstractCE::getUserOptions() const {
	return user_options;
}

void abstractCE::setUserOptions(const userOptions& userOptions) {
	user_options = userOptions;
}


concreteCE::ptr abstractCE::get_validated_CE(double tolerance, std::string& algo_type) {

	std::list<struct refinement_point> refinements;
	refinements.clear(); // No refinement point initially

	concreteCE::ptr cexample;
	bool val_res = true;
	unsigned int max_refinements = 10, ref_count = 0; // maximum limit to refinement points to be added.

	double valid_tol = 1e-5; // validation error tolerance, on invariant crossing.

	do {
		struct refinement_point pt;

		if (algo_type.compare("FC") == 0) // FC - Trajectory splicing with constraints over the search-space derived from flowpipe.
			cexample = gen_concreteCE(tolerance, refinements);
	
		else {
			DEBUG_MSG("Invalid algo type specified for trajectory splicing");
		}

		if (cexample->is_empty()) {
			cexample->set_refinement_count(ref_count);
			return cexample;
		}

		val_res = cexample->valid(pt, valid_tol);
		//putting off validation loop by refinements
		//val_res = true;
		//--

		if (!val_res) {
			refinements.push_back(pt);
			ref_count++;
			algo_type="FC";
			//debug: print the invalid trajectory in a file, in the first two dimensions
			//cexample->plot_ce("./invalid_traj.txt", 0, 1);
		} else {
			//DEBUG_MSG("Generated Trace Validated with " + to_string(ref_count) + " point Refinements");
			std::cout << "#Refinements for trace validation: " + to_string(ref_count) << std::endl;
			cexample->set_refinement_count(ref_count);
			return cexample;
		}
		DEBUG_MSG("Restarting Search with added refinement point\n");
	} while (!val_res && ref_count <= max_refinements);

	throw std::runtime_error(
			"Validation of counter example FAILED after MAX Refinements\n");
	return concreteCE::ptr(new concreteCE());
}

