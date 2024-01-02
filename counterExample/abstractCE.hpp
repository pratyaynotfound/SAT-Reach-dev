/*
 * AMIT: Implementing LP for initial solution point followed by Simulation for CE generation.
 */
concreteCE::ptr abstractCE::gen_concreteCE_Simulation(double tolerance, const std::list<refinement_point>& refinements){


	symbolic_states::const_ptr S = get_first_symbolic_state();
		dim = S->getContinuousSetptr()->get_dimension();

		HA = this->get_automaton();
		transList = this->get_CE_transitions();
		N = transList.size() + 1; // the length of the counter example
		bad_poly = this->forbid_poly;
		ref_pts = refinements;

		std::cout << "Length of the CE, N=" << N << std::endl;
		std::cout << "gen_concreteCE: dimension =" << dim << ", length of CE=" << N
				<< std::endl;

		// initialize the global locIdList
		locIdList.resize(N);
		std::cout << "Location ID sequence in symbolic CE: ";

		std::set<int> d;
		for (unsigned int i = 0; i < N; i++) {
			d = this->get_symbolic_state(i)->getDiscreteSet().getDiscreteElements();
			assert(d.size() == 1);
			locIdList[i] = *(d.begin());
			std::cout << locIdList[i] << " | ";
		}

		/* 1. Declare the sub-problem by fixing starting points of each location.
		 *
		 * The length of the counter example is N. When fixing the starting point of
		 * each location, the decision variables are dwell time in each location.
		 * Therefore, the dimension of the opt problem is N. This problem is supposed
		 * to be a nonlinear optimization problem.
		 */

		std::vector<double> x0[N]; // to contain the fixed start-points.
		/*
		 cout <<"What is N="<<N <<"   ";
		 x0[0].resize(3);
		 cout <<"What is x0.size="<< x0->size() <<"   ";
		 */

		std::vector<double> lb_t(N), ub_t(N);

		double max, min, start_min, start_max;

		/* build the lp where dwell times are fixed and the start points of the
		 * trajectory are variables.
		 */
		std::vector<double> dwell_times(N); // contains the fixed dwell_times

		get_first_symbolic_state()->getInitialPolytope();

		unsigned int t_index =
				get_first_symbolic_state()->getInitialPolytope()->get_index("t");

		assert((t_index >= 0) && (t_index < dim));

		std::vector<double> dmin(dim, 0), dmax(dim, 0);
		dmax[t_index] = 1;
		dmin[t_index] = -1;

		std::list<polytope::ptr> polys;
		polytope::ptr guard;

		std::list<transition::ptr>::iterator it = transList.begin();
		transition::ptr T;

		bool aggregation = true; //default is ON
		double global_time_horizon =0; //to hold the maximum global time horizon from the flowpipe computation
		// initialize values for dwell time from calculating min, max bounds.
		for (unsigned int i = 0; i < N; i++) {
			S = get_symbolic_state(i);

			unsigned int loc_id = locIdList[i];	//location ID
			location::ptr ha_loc_ptr = HA->getLocation(loc_id);
			polytope::ptr loc_inv = ha_loc_ptr->getInvariant();

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

				P = P->GetPolytope_Intersection(loc_inv);
				P = P->GetPolytope_Intersection(bad_poly);
			} else {
				// Take time projection of flowpipe \cap transition guard
				T = *(it);
				guard = T->getGuard();
				if (guard->getIsUniverse())
					std::cout << "#Guard is Universe#\n" << std::endl;

				polys = S->getContinuousSetptr()->flowpipe_intersectionSequential(
						aggregation, guard, 1);

				assert(polys.size() >= 1); // An abstract CE state must have intersection with the trans guard
				if (polys.size() > 1)
					P = get_template_hull(S->getContinuousSetptr(), 0,
							S->getContinuousSetptr()->getTotalIterations() - 1); // 100% clustering
				else
					P = polys.front();
				// Now intersect P with guard
				P = P->GetPolytope_Intersection(loc_inv);
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
				min = 0; // the min value a time can take is 0.
			}

			// we add the bounds as constraints in the nlopt

			// Get the min and max time projection of start set
			lp_solver lp1(GLPK_SOLVER);
			polytope::ptr init_of_symb = S->getInitialPolytope();

			lp1.setConstraints(init_of_symb->getCoeffMatrix(),
					init_of_symb->getColumnVector(),
					init_of_symb->getInEqualitySign());
			// Ensure that the time is positive
			start_min = -1 * lp1.Compute_LLP(dmin);
			start_max = lp1.Compute_LLP(dmax);
			ub_t[i] = max - start_min;
			if (min <= start_max)
				lb_t[i] = 0;
			else
				lb_t[i] = min - start_max;

			// We may choose to take the average time as the initial dwell time
			dwell_times[i] = (lb_t[i] + ub_t[i]) / 2;
			global_time_horizon += ub_t[i];
			// Increment the transition to the next in the symbolic path
			if (it != transList.end())
				it++;
		}

		double minf = 1e10; // a large value to start


		simulation::ptr sim = simulation::ptr(new simulation());

		sim->set_out_dimension(user_options.get_first_plot_dimension(), user_options.get_second_plot_dimension()); //output dimensions
		sim->set_time_step(user_options.get_timeStep());

		unsigned int X1 = 2 * dim * (N - 1); //list of new variables


		cout.precision(17);

		try {

					//solve the lp with fixed times
					lp_solver fixed_time_Lp = build_lp(dwell_times);

					double res = fixed_time_Lp.solve();
					std::vector<double> x = fixed_time_Lp.get_sv();
					//From this vector, get the N vectors in x0 vector
					cout << "Creating and Printing x0[i][j] (LP res = )"<< res <<" \n";
					for (unsigned int i = 0; i < N; i++) {
						x0[i].resize(dim);	//declaring each array element its size
						for (unsigned int j = 0; j < dim; j++) {
							x0[i][j] = x[X1 + i * dim + j];
								cout << x0[i][j] <<"  ";
						}
							cout<<endl;
					}


					if (res < tolerance)
						cout << "GLPK Solution res  = " << res << "\n";


					// Now do a Simulation with solution of LP by using the first segment's initial start point.


					std::vector<sim_start_point> ress;

					sim_start_point start_pts;

					start_pts.cross_over_time = 0;
					location::ptr ha_loc_ptr = HA->getLocation(locIdList[0]);
					start_pts.locptr =ha_loc_ptr;
					start_pts.start_point = x0[0];
					cout <<"Hello check x0[0]\n";
					for (unsigned int ii=0;ii<x0[0].size();ii++) {
						cout<< start_pts.start_point[ii] <<"\t";
					}

	double starting=0;
	unsigned int bfslevel = user_options.get_bfs_level();

	std::pair<int, polytope::ptr> unsafe_symb_set;

	unsafe_symb_set.first = -1; // location id; Thoughtlessly set here.
	unsafe_symb_set.second=forbid_poly;
	sim->simulateHa(start_pts, starting, global_time_horizon, getHa(), unsafe_symb_set, bfslevel);


	cout <<"global_time_horizon = "<<global_time_horizon <<"  bfslevel = " << bfslevel <<endl;


	std::ofstream myfile;
	myfile.open(user_options.getOutFilename().c_str(), std::ofstream::out);
	myfile.close();
	sim->print_trace_to_outfile("./bad_trace.o");

	string cmdStr1;
	cmdStr1.append("graph -TX -BC -m 0 ");
	cmdStr1.append("bad_trace.o");
	system(cmdStr1.c_str());


		if (res < tolerance)
				std::cout<< "Splicing with Iterative LP-Simulation: stopped successfully returning the found minimum\n"<< std::endl;

		} catch (std::exception &e) {
			std::cout << e.what() << std::endl;
		}


		concreteCE::ptr cexample = concreteCE::ptr(new concreteCE());
		cexample->set_automaton(HA);
		if (minf > tolerance) {
			std::cout << "Obtained minimum greater than " << tolerance
					<< ", with no. of refined search:" << refinements.size()
					<< std::endl;
			return cexample;
		} else {
			std::cout << "nlopt returned min : " << minf << "\n";
			std::cout << "Length of abstract counter example:" << N << "\n";

			/*if (minf > tolerance) {
			 std::cout << "Obtained minimum greater than " << tolerance
			 << ", with no. of refined search:" << refinements.size() << std::endl;
			 return cexample;
			 } else {*/
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
					y[j] = x0[i][j];
				}

				s.first = y;
				s.second = dwell_times[i];
				concreteCE::traj_segment traj;
				traj.first = locId;
				traj.second = s;
				cexample->push_back(traj);
			}
		}

		return cexample;
}


/**
 * gen_concreteCE_LPobj: trajectory splicing with NLP using dwell-time only as vars.
 * The objective function is the soln.  of a LP formed for splicing with
 * fixed-dwell and varying starts. This routine is slow due to not having explicit
 * derivatives of cost wrt vars.
 */

concreteCE::ptr abstractCE::gen_concreteCE_LPobj(double tolerance, const std::list<refinement_point>& refinements)
{

		symbolic_states::const_ptr S = get_first_symbolic_state();
		dim = S->getInitialPolytope()->getSystemDimension();
		HA = this->get_automaton();
		transList = this->get_CE_transitions();
		N = transList.size() + 1; // the length of the counter example
		bad_poly = this->forbid_poly;
		ref_pts = refinements;

		//assert that the number of transitions equals 1 less than the length of the abstract CE path

		std::cout << "Length of the CE, N=" << N << std::endl;

		// initialize the global locIdList
		locIdList.resize(N);

		std::cout << "Location ID sequence in symbolic CE: ";
		std::set<int> d;
		for (unsigned int i = 0; i < N; i++) {
			d = this->get_symbolic_state(i)->getDiscreteSet().getDiscreteElements();
			assert(d.size() == 1);
			locIdList[i] = *(d.begin());
			std::cout << locIdList[i] << " | ";
		}
		std::cout << "\n";

		//	The dimensionality of the opt problem is N dwell times.

		unsigned int optD = N;

		std::cout << "nlopt problem dimension = " << optD << std::endl;

		//nlopt::opt myopt(nlopt::LD_MMA, optD); // local optimization routine, derivative free
		nlopt::opt myopt(nlopt::LN_COBYLA, optD); // local optimization routine, derivative free

		unsigned int maxeval = 1000; // max evaluation limit.
		myopt.set_min_objective(myobjfunc_LP, this);
		myopt.set_maxeval(maxeval);
		myopt.set_stopval(tolerance); // splicing tolerance

		//Set initial value to the optimization problem
		std::vector<double> t(optD, 0);

		std::vector<double> v(dim);

		std::vector<double> lb(optD), ub(optD);
		double max, min, start_min, start_max;

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
		polytope::ptr guard;

		std::list<transition::ptr>::iterator it = transList.begin();
		transition::ptr T;

		bool aggregation = true; //default is ON

		// get the min-max constraints over the dwell time, from the flowpipe.
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
				polys = S->getContinuousSetptr()->flowpipe_intersectionSequential(
						aggregation, guard, 1);

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
			polytope::ptr init_of_symb = S->getInitialPolytope();

			lp1.setConstraints(init_of_symb->getCoeffMatrix(),
					init_of_symb->getColumnVector(),
					init_of_symb->getInEqualitySign());
			// Ensure that the time is positive
			start_min = -1 * lp1.Compute_LLP(dmin);
			start_max = lp1.Compute_LLP(dmax);
			ub[i] = max - start_min;
			if (min <= start_max)
				lb[i] = 0;
			else
				lb[i] = min - start_max;

			// We may choose to take the average time as the initial dwell time
			t[i] = (lb[i] + ub[i]) / 2;

			// Increment the transition to the next in the symbolic path
			if (it != transList.end())
				it++;
		}
		//debug code

//		t[0] = 1.04621;
//		t[1] = 0.325437;
//		t[2] = 1.23898;
//		t[3] = 0.325143;

		//---
		myopt.set_lower_bounds(lb);
		myopt.set_upper_bounds(ub);

		double minf;
		try {
			std::cout << "Local optimization algorithm called:"
					<< myopt.get_algorithm_name() << std::endl;
			myopt.set_stopval(tolerance);
			unsigned int res = myopt.optimize(t, minf);

			if (res == NLOPT_SUCCESS)
				std::cout
						<< "Splicing with LP-OBJ: NLOPT stopped successfully returning the found minimum\n";
			else if (res == NLOPT_STOPVAL_REACHED)
				std::cout
						<< "Splicing with LP-OBJ: NLOPT stopped due to stopping value (1e-6) reached\n";
			else if (res == NLOPT_MAXEVAL_REACHED)
				std::cout
						<< "Splicing with LP-OBJ: NLOPT stopped due to reaching maxeval = "
						<< maxeval << std::endl;

		} catch (std::exception &e) {
			std::cout << e.what() << std::endl;
		}
		std::cout << "nlopt returned min : " << minf << "\n";
		std::cout << "Length of abstract counter example:" << N << "\n";

		concreteCE::ptr cexample = concreteCE::ptr(new concreteCE());
		cexample->set_automaton(HA);

		if (minf > tolerance) {
			std::cout << "Obtained minimum greater than " << tolerance
					<< ", with no. of refined search:" << refinements.size()
					<< std::endl;
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
				assert(start_pos.size() == 2*N*dim + 2 * dim * (N - 1));
				for (unsigned int j = 0; j < dim; j++) {
					y[j] = start_pos[2 * dim * (N - 1) + i * dim + j];
				}

				double time = t[i];
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

