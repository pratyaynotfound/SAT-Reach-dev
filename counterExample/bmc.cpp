/*
 * bmc.cpp
 *
 *  Created on: 14-Sep-2020
 *      Author: rajarshi
 */

#include <counterExample/bmc.h>


bmc::~bmc() {
	// TODO Auto-generated destructor stub
} 
bmc::bmc(const hybrid_automata::ptr haPtr,  const std::list<initial_state::ptr>& init,
		const forbidden_states& forbidden, const ReachabilityParameters& r_params,
		const userOptions& user_ops) : ha_ptr(haPtr), init(init), forbidden_s(forbidden),
				reach_params(r_params), user_ops(user_ops), c(), ha_encoding(c), sol(c),s1(c){
	this->k = user_ops.get_bfs_level();
}

/*
 * Initializes the z3 solver with bounded ha encoding. 
 * It generates all the z3 constraints for bound k1 and add these constraints to the SAT solver.
 */
void bmc::init_solver(unsigned int k1)
{
	location::ptr source_ptr = ha_ptr->getInitialLocation();
	int u = source_ptr->getLocId();
	auto list_locations = ha_ptr->getAllLocations();

	// This will stores all the initial location id as per HA may have multiple initial location.
	std::vector<unsigned int> initial_locations;
	initial_locations.push_back(u); //due to Hyst does not support multiple initial location. thats why we only add one location.

	// Initial clause
	z3::expr exp1 = c.bool_const("exp1");
	string arr = "v" + to_string(initial_locations[0])+"_"+ "0";
	z3::expr exp1a = c.bool_const(arr.c_str());
	for(auto it = list_locations.begin(); it != list_locations.end(); it++)
	{
		arr = "v" + to_string(it->first)+"_"+ "0";
		if (it->first != initial_locations[0])
		{
			z3::expr x1 = c.bool_const(arr.c_str());
			exp1a = (exp1a && !(x1));
		}
	}
	exp1 = exp1a;
	for (int i = 1; i < initial_locations.size(); i++)
	{
		arr = "v" + to_string(initial_locations[i])+"_"+ "0";
		exp1a = c.bool_const(arr.c_str());
		for(auto it = list_locations.begin(); it != list_locations.end(); it++)
		{
			arr = "v" + to_string(it->first)+"_"+ "0";
			if (it->first != initial_locations[i])
			{
				z3::expr x1 = c.bool_const(arr.c_str());
				exp1a = (exp1a && !(x1));
			}
		}
		exp1 = (!(exp1) && exp1a) || (!(exp1a) && exp1); // exp1 = exp1 XOR exp1a
	}

	this->sol.add(exp1);

	//Movement clause
	std::vector<unsigned int> temp;
	std::vector<std::vector<unsigned int>> locs;
	temp.push_back(u);
	locs.push_back(temp);
	temp.clear();
	z3::expr exp2 = c.bool_const("exp2");
	z3::expr exp22 = c.bool_const("exp22");
	if(k1 == 0)
	{
		for (unsigned int i =0; i <= k1; i++)
		{

			for (auto it = list_locations.begin(); it != list_locations.end(); it++)
			{
				if (std::find(locs[i].begin(), locs[i].end(), it->first)!=locs[i].end())
				{
					auto neighbor_nodes = it->second->getOutGoingTransitions();
					arr = "v"+ to_string(it->first)+"_"+ to_string(i);
					z3::expr x2 = c.bool_const(arr.c_str());
					bool outGoingTransition = false;
					exp2 = x2;
					z3::expr exp2a = c.bool_const("exp2a");
					unsigned int count = 1;
					for (auto it2 = neighbor_nodes.begin(); it2 != neighbor_nodes.end(); it2++)
					{
						outGoingTransition = true;
						transition::ptr trans_ptr = *(it2);
						unsigned int loc_id = trans_ptr->getDestinationLocationId();
						arr = "v" + to_string(loc_id) +"_"+ to_string(i+1);
						z3::expr x2 = c.bool_const(arr.c_str());
						if(count == 1)
						{
							exp2a = x2;
						}
						if(count >= 2)
						{
							exp2a = (exp2a || x2);
						}
						count++;
						temp.push_back(loc_id);
					}
					exp22 = implies(exp2, exp2a);
					if(outGoingTransition)
						this->sol.add(exp22);
					if(!outGoingTransition)
					{
						z3:: expr exp2b = c.bool_val(false);
						this->sol.add(implies(exp2, exp2b));
					}
				}
			}
			locs.push_back(temp);
			temp.clear();
		}
	}
	if(k1 >= 1)
	{
		for (unsigned int i =0; i <= k1-1; i++)
		{
			for (auto it = list_locations.begin(); it != list_locations.end(); it++)
			{
				if (std::find(locs[i].begin(), locs[i].end(), it->first)!=locs[i].end())
				{
					auto neighbor_nodes = it->second->getOutGoingTransitions();
					arr = "v"+ to_string(it->first)+"_"+ to_string(i);
					z3::expr x2 = c.bool_const(arr.c_str());
					exp2 = x2;
					z3::expr exp2a = c.bool_const("exp2a");
					bool outGoingTransition = false;
					unsigned int count = 1;
					for (auto it2 = neighbor_nodes.begin(); it2 != neighbor_nodes.end(); it2++)
					{
						outGoingTransition = true;
						transition::ptr trans_ptr = *(it2);
						unsigned int loc_id = trans_ptr->getDestinationLocationId();
						arr = "v" + to_string(loc_id) +"_"+ to_string(i+1);
						z3::expr x2 = c.bool_const(arr.c_str());
						if(count == 1)
						{
							exp2a = x2;
						}
						if(count >= 2)
						{
							exp2a = (exp2a || x2);
						}
						count++;
						temp.push_back(loc_id);
					}
					exp22 = implies(exp2, exp2a);
					if(outGoingTransition)
						this->sol.add(exp22);
					if(!outGoingTransition)
					{
						z3:: expr exp2b = c.bool_val(false);
						this->sol.add(implies(exp2, exp2b));
					}
				}
			}
			locs.push_back(temp);
			temp.clear();
		}
	}

	//Exclusivity clause
	z3::expr exp3 = c.bool_const("exp3");
	z3::expr exp33 = c.bool_const("exp33");
	for (unsigned int i =1; i <= k1; i++)
	{
		for(auto it1 = list_locations.begin(); it1 != list_locations.end(); it1++)
		{
			if (std::find(locs[i].begin(), locs[i].end(), it1->first)!=locs[i].end())
			{
				string arr = "v" + to_string(it1->first)+"_" + to_string(i);
				z3::expr x = c.bool_const(arr.c_str());
				exp3 = x;
				z3::expr exp31 = c.bool_const("exp31");
				unsigned int count = 0;
				for(auto it2 = list_locations.begin(); it2 != list_locations.end(); it2++)
				{
					arr = "v" + to_string(it2->first)+"_"+ to_string(i);
					if (it2->first != it1->first)
					{
						z3::expr x1 = c.bool_const(arr.c_str());
						if(count == 0)
						{
							exp31 = !(x1);
						}
						if(count >= 1)
						{
							exp31 = (exp31 && !(x1));
						}
						count++;
					}
				}
				exp33 = implies(exp3, exp31);
				this->sol.add(exp33);
			}
		}
	}						//End of Exclude Constraint.


	//Destination clause
	z3::expr exp4 = c.bool_const("exp4");
	z3::expr exp4a = c.bool_const("exp4a");
	arr = "v" + to_string(forbidden_s[0].first)+"_" + to_string(k1);
	exp4 = c.bool_const(arr.c_str());;
	for (int i = 1; i < forbidden_s.size(); i++)
	{	
		int v = forbidden_s[i].first;
		arr = "v" + to_string(v)+"_" + to_string(k1);
		exp4a = c.bool_const(arr.c_str());
		exp4 = exp4 || exp4a;
	}

	//this->sol.add(exp4a || exp4b); // exp4 = exp4a || exp4b
	this->sol.add(exp4);


	
    //path purning -all retrieved paths are stored in the p_array data structure(DS). Negates each path from DS and adds them to the solver
	for (unsigned int i = 1; i < p_array.size(); i = i+2)
	{
		z3::expr exp5 = c.bool_const("exp5");
		unsigned int time_step = 0;
		for (unsigned int j = 0; j < p_array.at(i).size(); j++)
		{
			string arr = "v" + to_string(p_array.at(i).at(j))+"_" + to_string(time_step);
			z3::expr x = c.bool_const(arr.c_str());
			if (j == 0)
				exp5 = x;
			else if (j == p_array.at(i).size() - 1)
				exp5 = implies(exp5, !(x));
			else
				exp5 = (exp5 && x);
			time_step++;
		}
		this->sol.add(exp5);
	} 

}


/*
 *  This function encodes the path p as Negation clause of p and add it to SAT solver, 
 *  so that path p or extension of p is not returned further.
 */
void bmc::update_encoding(path p){

	z3::expr exp5 = c.bool_const("exp5");
	string arr = "v" + to_string(p[0])+"_" + to_string(0);
	z3::expr x13 = c.bool_const(arr.c_str());
	exp5 = !(x13);
	for (unsigned int i = 1; i < p.size(); i++)
	{
		arr = "v" + to_string(p[i])+"_" + to_string(i);
		z3::expr x14 = c.bool_const(arr.c_str());
		exp5 = (exp5 || !(x14));
	}	
	this->sol.add(exp5); 
}



/*
 * Call SAT solver to check all z3 constraints are satisfied or not. 
 *If SAT,it generates a path from the truthvalue of the model and return the path. 
 */
path bmc::getNextPath(unsigned int k1)
{
	path p;
	auto list_locations = ha_ptr->getAllLocations();

	if (this->sol.check() == z3::sat)
	{
		p.resize(k1+1);
		z3::model m = this->sol.get_model();
		for (unsigned int i = 0; i <= k1; i++)
		{
			for (auto it = list_locations.begin(); it != list_locations.end(); it++)
			{
				string loc = "v" + to_string(it->first) + "_" + to_string(i);
				z3::expr expp = c.bool_const(loc.c_str());
				if(m.eval(expp).is_true())
				{
					p.at(i) = it->first;
					break;
				}
			}
		}
	}
	else
		p.clear();
	return p; 
}

/*
 * This solver returns a path with unique transition Id between two locations, we called in this class
 * as run. Whereas path p was a sequence of locations, from a p this function enumerates possible set of runs.
 */
void bmc::second_solver(path p)
{
	z3::expr exp_11 = c.bool_const("exp_11");
	
	path run;
	for (unsigned int i = 0; i < p.size()-1; i++)
	{
		z3::expr exp_1 = c.bool_const("exp_1");
		unsigned int count = 1;
		unsigned int u = p[i];
		unsigned int v = p[i+1];
		
		location::const_ptr source_loc = ha_ptr->getLocation(u);
		location::const_ptr Dest_loc = ha_ptr->getLocation(v);
		std::list<transition::ptr> neighbor_nodes = source_loc->getOutGoingTransitions();
		for (auto it2 = neighbor_nodes.begin(); it2 != neighbor_nodes.end(); it2++)
		{
			transition::ptr trans_ptr = *(it2);
			unsigned int dest_loc_id = trans_ptr->getDestinationLocationId();
			if (v == dest_loc_id)
			{
				string arr = "v" + to_string(u) +"_"+ "v"+ to_string(v)+ "_" + "t" + to_string(trans_ptr->getTransitionId()) + "_" + to_string(i);
				z3::expr exp_1a = c.bool_const(arr.c_str());
				if(count == 1)
				{
					exp_1 = exp_1a;
				}
				if(count >= 2)
				{
					exp_1 = (exp_1 || exp_1a);
				}
				count++;
			}
		}
		if (i == 0)
			exp_11 = exp_1;
		else 
			exp_11 = (exp_11 && exp_1);
	}
	s1.add(exp_11);


	/*
	 * Run_array is the data structure that stores infeasible run segment which we want to not enumerate further.
	 * For this purpose we add this negated segments into the second_solver.
	 */
	for (unsigned int i = 1; i < Run_array.size(); i=i+2)
	{
		z3::expr exp_33 = c.bool_const("exp_33");
		unsigned int run_time_step = 0;
		for (unsigned int j = 0; j < Run_array.at(i).size()-1; j = j+2)
		{
			string arr = "v" + to_string(Run_array.at(i)[j]) +"_"+ "v"+ to_string(Run_array.at(i)[j+2])+ "_" + "t" + to_string(Run_array.at(i)[j+1]) + "_" + to_string(run_time_step);
			z3::expr exp_3a = c.bool_const(arr.c_str());
			if (run_time_step == 0)
				exp_33 = ! (exp_3a);
			if (run_time_step >= 1)
				exp_33 = (exp_33 || ! (exp_3a));
			run_time_step++;
		}
		s1.add(exp_33);
	}
}


/*
 * It returns an exact run for a path p of same langth.
 * example of a run r = v_1 t_1 v_2 t_2 v_4.
 */
Run bmc::getARun(path p)
{
	path new_path,set_of_edges;
	Run run;
	if (s1.check() == z3::sat)
	{
		z3::model m = s1.get_model();
		new_path.resize(p.size());
		set_of_edges.resize(new_path.size()-1);
		run.resize(new_path.size() + set_of_edges.size());
		for (unsigned int i = 0; i < p.size()-1; i++)
		{
			std::list<transition::ptr> neighbor_nodes = ha_ptr->getLocation(p[i])->getOutGoingTransitions();
			for (auto it2 = neighbor_nodes.begin(); it2 != neighbor_nodes.end(); it2++)
			{
				transition::ptr trans_ptr = *(it2);
				unsigned int dest_loc_id = trans_ptr->getDestinationLocationId();
				if (p[i+1] == dest_loc_id)
				{
					string arr = "v" + to_string(p[i]) +"_"+ "v"+ to_string(p[i+1])+ "_" + "t" + to_string(trans_ptr->getTransitionId()) + "_" + to_string(i);
					z3::expr exp_1b = c.bool_const(arr.c_str());
					if(m.eval(exp_1b).is_true())
					{
						new_path[i] = p[i];
						new_path[i+1] = p[i+1];
						set_of_edges[i] = trans_ptr->getTransitionId();
						run[2*i] = p[i];
						run[(2*i)+1] = set_of_edges[i];
						run[(2*i)+2] = p[i+1]; 
						break;
					}
				}
			}
		}
	}
	else
		run.clear();

	return run;
}

/*
 *
 */
void bmc::update_run_encoding(Run run)
{
	//This functionis written for encoding negation for Run.
	z3::expr exp_22 = c.bool_const("exp_22");
	z3::model m = s1.get_model();
	unsigned int time_step = 0;
	for (unsigned int i = 0; i < run.size()-1; i=i+2)
	{
		string arr = "v" + to_string(run[i]) +"_"+ "v"+ to_string(run[i+2])+ "_" + "t" + to_string(run[i+1]) + "_" + to_string(time_step);
		z3::expr exp_1b = c.bool_const(arr.c_str());
		if (i == 0)
			exp_22 = ! (exp_1b);
		if (i >= 2)
			exp_22 = (exp_22 || ! (exp_1b));
		time_step++;
	}
	s1.add(exp_22);

}


/*
 * check a polytope is superset or not using ppl libraries and return true if the first polytope is superset.
 *
 */
/*
bool bmc::containmentChecking(polytope::const_ptr oldNextInit, polytope::const_ptr nextInit)
{
	auto matrix_nextInit = nextInit->getCoeffMatrix();
	std::vector<double> vector_nextInit = nextInit->getColumnVector();
	unsigned int row = matrix_nextInit.size1();
	//unsigned int colom = matrix_nextInit.size2(); 

	auto matrix_oldNextInit = oldNextInit->getCoeffMatrix();
	std::vector<double> vector_oldNextInit = oldNextInit->getColumnVector();
	unsigned int row1 = matrix_oldNextInit.size1();
	//unsigned int colom1 = matrix_oldNextInit.size2();
	
	NNC_Polyhedron oldNextInit_poly(ha_ptr->map_size());
	NNC_Polyhedron nextInit_poly(ha_ptr->map_size());

	for(int i = 0; i < row; i++)
	{
		Variable x(0);
		Linear_Expression e = 0*x;
		for(int j = 0; j < ha_ptr->map_size(); j++)
		{
			
			Variable v(j);
			e += matrix_nextInit(i,j)*v;
		}
		Constraint_System c(e <= vector_nextInit.at(i));
					
		nextInit_poly.add_constraints(c);
	} 
	for(int i = 0; i < row1; i++)
	{
		Variable x(0);
		Linear_Expression e = 0*x;
		for(int j = 0; j < ha_ptr->map_size(); j++)
		{
			
			Variable v(j);
			e += matrix_oldNextInit(i,j)*v;
		}
		Constraint_System c(e <= vector_oldNextInit.at(i));
					
		oldNextInit_poly.add_constraints(c);
	} 
	return(oldNextInit_poly.contains(nextInit_poly));	

} 
*/

/*
 * Performs symbolic reachability analysis on a given run r
 * and returns the result as a list of symbolic states.
 * the feasible flag is set false if the path is found not infeasible
 * in the ha dynamics. If the path is feasible, call generate_counterexample
 * routine to get a concreteCE.
 * 
 */
region bmc::getRunReachability(Run& run, bool& feasible, forbidden forbid_s, std::list<abstractCE::ptr>& symbolic_ce_list)
{
	region reach_region;

	assert(run.size()>=1); // The given path must not be empty

	template_polyhedra::ptr flowpipe = template_polyhedra::ptr(new template_polyhedra());
	initial_state::ptr initial_set = *(init.begin());

	polytope::const_ptr init_poly = initial_set->getInitialSet();
	reach_region.clear();

	path smallest_inf_run;
	smallest_inf_run.push_back(run[0]);

	polytope::const_ptr nextInit;
	polytope::ptr shift_polytope;
	
	for(unsigned int i = 0; i < run.size(); i = i+2)
	{
		unsigned int locId = run[i];
		unsigned int transId = run[i+1];
		unsigned int nextLocId = run[i+2];

		location::const_ptr current_location = ha_ptr->getLocation(locId);

		polytope::const_ptr initial_polytope = initial_set->getInitialSet();
		if(i == run.size()-1)
		{
			//feasible = true;

			flowpipe = postC_fbinterpol(reach_params.Iterations, current_location->getSystemDynamics(), initial_polytope, reach_params,
				current_location->getInvariant(), current_location->getInvariantExist());
			postc_counter++;
			discrete_set d;
			d.insert_element(locId);
			symbolic_states::ptr symb = symbolic_states::ptr(new symbolic_states(d,flowpipe));
			reach_region.push_back(symb);
			symb->setInitialPolytope(initial_polytope);
			symb->setParentPtrSymbolicState(initial_set->getParentPtrSymbolicState()); //keeps track of parent pointer to symbolic_states
			symb->setTransitionId(initial_set->getTransitionId()); //keeps track of originating transition_ID

			abstractCE::ptr abst_ce = abstractCE::ptr(new abstractCE());
			bool safety_violation = false;
			safety_violation = createAbstractCE(forbid_s, i, locId, flowpipe, symb, symbolic_ce_list, abst_ce);
			if (safety_violation) {	//Safety violation found with the over-approximated flowpipe with the forbidden set
				feasible = true;
				abst_ce->setUserOptions(user_ops);
				bool continue_search = reach_for_CE.gen_counter_example(abst_ce,user_ops.getCEProc());
				if (continue_search == false) {
					cout<<"Concrete Counter Example Found Successfully!!"<<endl;
					return reach_region;
				}
				else //When path is feasiable but can't find CE, then go to the solver to check another path
				{
					std::cout<<"can't find CE"<<endl;
					//std::cout << "Time to search CE:" << reach_for_CE.get_ce_search_time() / (double) 1000 << std::endl;
					feasible = false;
				}
					
			}

			
			break;
		}

		initialSetInLocation = make_pair(locId,initial_polytope); // Initial set in locId. 
		auto it = flowForInit.find(initialSetInLocation);
		if(it == flowForInit.cend())
		{
			flowpipe = postC_fbinterpol(reach_params.Iterations, current_location->getSystemDynamics(), initial_polytope, reach_params,
				current_location->getInvariant(), current_location->getInvariantExist());
			postc_counter++;
			flowForInit.insert({initialSetInLocation, flowpipe});	// store flow for specific intial set.
		}
		else
			flowpipe = (*it).second;   // If Inittial set for locId is already computed.	

		transition::ptr current_transition = current_location->getTransition(transId);
	
		discrete_set d;
		d.insert_element(locId);
		symbolic_states::ptr symb = symbolic_states::ptr(new symbolic_states(d,flowpipe));
		reach_region.push_back(symb);
		symb->setInitialPolytope(initial_polytope);
		symb->setParentPtrSymbolicState(initial_set->getParentPtrSymbolicState()); //keeps track of parent pointer to symbolic_states
		symb->setTransitionId(initial_set->getTransitionId()); //keeps track of originating transition_ID

		assert(current_transition!=nullptr);

		flowWithTrans_id = make_pair(flowpipe, transId); // make a pair flow with out going trans_id.
		auto iterator = nextInitForFlowWithT_id.find(flowWithTrans_id);
		// Found a initial set for a specific flow with outgoing transition. 
		if (iterator != nextInitForFlowWithT_id.cend())
		{
			nextInit = (*iterator).second.second;
			initial_state::ptr newState = initial_state::ptr(new initial_state(nextLocId, nextInit));  //shift_polytope
			newState->setTransitionId(transId); // keeps track of the transition_ID
			newState->setParentPtrSymbolicState(symb);
			initial_set = newState;
			smallest_inf_run.push_back(run[i+1]);
			smallest_inf_run.push_back(run[i+2]);
			continue;
		}
		else
		{

			location::const_ptr next_location = ha_ptr->getLocation(nextLocId);

			polytope::const_ptr trans_guard = current_transition->getGuard();

			std::string postd_aggregation = user_ops.getSetAggregation();
			std::list<polytope::ptr> polys;
			polytope::const_ptr inv = current_location->getInvariant();

			bool aggregation=true; // set aggregation is set to true by default.

			if (boost::iequals(postd_aggregation,"thull") || boost::iequals(postd_aggregation,"chull")){
				aggregation=true;

			} 
			else if (boost::iequals(postd_aggregation,"none"))
			{
				aggregation=false;
			}

			assert(trans_guard!=nullptr);

			if (!trans_guard->getIsEmpty()){

				if(boost::iequals(postd_aggregation,"thull") || boost::iequals(postd_aggregation,"none")){
					polys = flowpipe->flowpipe_intersectionSequential(aggregation, trans_guard, 1);
				}
				else if(boost::iequals(postd_aggregation,"chull")){
					polys = flowpipe->postD_chull(trans_guard, inv, 1);
				}

			} 
			else 
			{ // empty guard
				DEBUG_MSG("bmc::getRunReachability: Guard Set is empty. It means that the guard condition is unsatisfiable. \n");
				exit(0);
			}
			// apply transition map
			assert(polys.size()<=1);

			smallest_inf_run.push_back(run[i+1]);
			smallest_inf_run.push_back(run[i+2]);
			
			if (polys.size() == 0)
			{
				feasible = false;
				break;
			}
			// Intersect with guard
			polytope::ptr g_flowpipe_intersect;
			polytope::ptr hull_poly = *(polys.begin());

			if(boost::iequals(postd_aggregation,"thull") || boost::iequals(postd_aggregation,"none")){

				if(!trans_guard->getIsUniverse()){
					g_flowpipe_intersect = hull_poly->GetPolytope_Intersection(trans_guard);
				} 
				else
				{
					g_flowpipe_intersect = hull_poly;
				}
			}
			else // chull aggregation. no need to intersect with guard.
				g_flowpipe_intersect = hull_poly;

			Assign& t_map = current_transition->getAssignT();
			

			if (t_map.Map.isInvertible()) {
				shift_polytope = post_assign_exact(g_flowpipe_intersect, t_map.Map, t_map.b);
			} else {
				shift_polytope = post_assign_approx_deterministic(g_flowpipe_intersect,
						t_map.Map, t_map.b, reach_params.Directions,1);
			}
			// The newShifted must satisfy the destination location invariant
			if (next_location->getInvariant()!=NULL) { // ASSUMPTION IS THAT NULL INV=> UNIVERSE INV
				shift_polytope = shift_polytope->GetPolytope_Intersection(next_location->getInvariant());
			}

			if(shift_polytope->getIsEmpty()){
				feasible = false;
				break; // test for other initial regions
			}

			nextInit = shift_polytope;

			initialSetInLocation = make_pair(nextLocId,nextInit); // Initial set in locId. 
			auto it1 = flowForInit.find(initialSetInLocation);
			if(it1 == flowForInit.cend())
			{
				unsigned int count = 0;
				bool found_superset = false;
				polytope::const_ptr oldNextInit, temp_poly;
				for(auto it = flowForInit.cbegin(); it != flowForInit.cend(); ++it)
				{
					if((*it).first.first == nextLocId)
					{
						oldNextInit = (*it).first.second;
						//bool contain = containmentChecking(oldNextInit,nextInit); // checking oldNextInit is superset or not using ppl.
						bool contain = oldNextInit->contains(nextInit,1);  //use the previous function.
						if(contain)
						{
							found_superset = true;
							if(count==0)
								temp_poly = oldNextInit; 
							if(count >= 1)
							{
								//bool contain1 = containmentChecking(temp_poly, oldNextInit);
								bool contain1 = oldNextInit->contains(nextInit,1);  //use the previous function.
								if(contain1)
								{
									//std::cout<<"\033[32mfound tightest superset\033[0m"<<endl; 
									temp_poly = oldNextInit;
								}
							}
							count = count + 1;
							//std::cout<<"\033[32mfound superset\033[0m"<<endl; 
						}
					}
				}
				if(found_superset)
					nextInit = temp_poly;
			}

			initial_state::ptr newState = initial_state::ptr(new initial_state(nextLocId, nextInit));
			newState->setTransitionId(transId); // keeps track of the transition_ID
			newState->setParentPtrSymbolicState(symb);
			initial_set = newState;

			// store initial set of the next location for flow with incoming trans-id.
			flowWithTrans_id = make_pair(flowpipe, transId);
			initialSetInLocation = make_pair(nextLocId,nextInit);
			nextInitForFlowWithT_id.insert({flowWithTrans_id, initialSetInLocation}); 
		}

	}

	run = smallest_inf_run;
	return reach_region;
}


bool bmc::createAbstractCE(forbidden forbidden_set, unsigned int i, unsigned int current_locId,
		template_polyhedra::ptr reach_region, symbolic_states::ptr S,
		std::list<abstractCE::ptr> &symbolic_ce_list, abstractCE::ptr abst_ce) {

	bool safety_violation = false;
	int locID = current_locId;
	
	std::list < symbolic_states::ptr > list_sym_states;
	std::list < transition::ptr > list_transitions; // list of transitions leading to the unsafe set



	if (forbidden_set.first==-1 || locID == forbidden_set.first) { // forbidden locID matches. loc id of -1 means any location
		polytope::ptr forbid_poly = forbidden_set.second;
		std::list <template_polyhedra::ptr > forbid_intersects;
		//forbid_intersects.push_back(reach_region);
		forbid_intersects = reach_region->polys_intersectionSequential(forbid_poly, 1);
		//std::cout<<"intersect:"<<forbid_intersects.size()<<endl;
		
		if (forbid_intersects.size() != 0) {
			safety_violation = true;
			symbolic_states::ptr symb_state_in_abst_ce; // This is a pointer to the current symbolic state in the abstract ce.
			symb_state_in_abst_ce = S;
			int symbolic_ce_length = 0;
			do {
				unsigned int locationID2=0;
				discrete_set ds, ds2;
				int transID = symb_state_in_abst_ce->getTransitionId();
				list_sym_states.push_front(symb_state_in_abst_ce); //pushing the unsafe symbolic_state first
				if (symb_state_in_abst_ce->getParentPtrSymbolicState() != NULL) { //searching only if not NULL
					symb_state_in_abst_ce = symb_state_in_abst_ce->getParentPtrSymbolicState();
					ds2 = symb_state_in_abst_ce->getDiscreteSet();
					for (std::set<int>::iterator it = ds2.getDiscreteElements().begin(); it != ds2.getDiscreteElements().end(); ++it)
						locationID2 = (*it);
					location::const_ptr object_location;
					object_location = ha_ptr->getLocation(locationID2);
					transition::ptr temp = object_location->getTransition(transID);
					list_transitions.push_front(temp); //pushing the transition in the stack
				}
				symbolic_ce_length++;
			} while (symb_state_in_abst_ce->getParentPtrSymbolicState()!= NULL);
			if ((symbolic_ce_length >= 1) && (symb_state_in_abst_ce->getParentPtrSymbolicState()== NULL)) { //root is missed
				list_sym_states.push_front(symb_state_in_abst_ce); //1) pushing this new symb state at the beginning
			}

			abst_ce->set_length(symbolic_ce_length);
			abst_ce->set_sym_states(list_sym_states);
			abst_ce->set_transitions(list_transitions);
			abst_ce->set_automaton(&(*ha_ptr));
			abst_ce->set_forbid_poly(forbidden_set.second);
			symbolic_ce_list.push_back(abst_ce); // This abstract counter-example path is added to the list of all such identified paths.
		}
	}
	return safety_violation;
}



unsigned int bmc::safe(){
	int cnt = 0, run_counter = 0;
	region r, R;
	std::list<abstractCE::ptr> symbolic_ce_list;
	reach_for_CE.setReachParameter(*(ha_ptr), init, reach_params, 1, forbidden_s, user_ops);
	forbidden forbid_s;
	if (forbidden_s.size() == 0)
	{
		std::cout<<"forbidden set is empty"<<endl;
		return 1;
	}
	boost::timer::cpu_timer timer1;
	timer1.start();
	// iterate over the bound.
	for(unsigned int k1=0;k1 <= k; k1++)
	{
		init_solver(k1); // initialize the ha_encoding for this forbidden location
		path p = getNextPath(k1);
		while(p.size()!=0){ // A path is returned above
			if(timer1.elapsed().user/1000000 <= 3600000)
			{
				//Assume that all forbidden locations are distinct.
				unsigned int forbid_loc = p[p.size()-1];
				for (auto it = forbidden_s.begin(); it != forbidden_s.end(); it++)
					if(forbid_loc = (*it).first)
						forbid_s = (*it);

				second_solver(p);
				Run run = getARun(p);
				cnt++;
				unsigned int feasiable_tracker = 1; // track a feasible path for a sequence of locations. 0--> feasible run found. 1--> no feasible run.

			
				path smallest_inf_run_segment; //keep track of smallest inf path to prune all the extension from the search space.
				while(run.size() != 0)
				{
					run_counter++;
					
					bool feasible = false;
					path run_before_reach;
					run_before_reach = run;
					R = getRunReachability(run,feasible, forbid_s, symbolic_ce_list);	
					if (!feasible)
					{
						update_run_encoding(run);
						if (run.size() == run_before_reach.size())
							feasiable_tracker *= 0;
						if (run.size() < run_before_reach.size())
						{
							Run_array.resize(Run_array.size()+1);
							Run_array.push_back(run);
							feasiable_tracker *= 1;
							smallest_inf_run_segment = (smallest_inf_run_segment.size() < run.size()) ? run : smallest_inf_run_segment;
						}
							

					}	
					else
					{
						print_bmc_ce_statistics(reach_for_CE, symbolic_ce_list, user_ops, "Safety Analysis");
						std::cout<< "\nTotal paths visited by our old Algorithm: "<<cnt<<endl;
						std::cout<< "\nTotal paths visited by our new Algorithm: "<<run_counter<<endl;
						std::cout<<"\nNumber of postC operation done: "<<postc_counter<<endl;
						show(R, user_ops);
						return 0; // the model is unsafe
					}
					run = getARun(p);
				}
				s1.reset(); 
				bool feasible = false;
				if (feasiable_tracker)
				{
					if(!smallest_inf_run_segment.empty())
					{
						p.clear();
						p.resize((smallest_inf_run_segment.size()/2)+1);
						for (unsigned int i = 0; i <= (smallest_inf_run_segment.size()/2); i++)
							p.at(i) = smallest_inf_run_segment.at(2*i);
					}
					
					p_array.resize(p_array.size()+1);
					p_array.push_back(p);
				} 
				
				if(!feasible)
					update_encoding(p); // update the encoding such that no path containing p is returned further.

				p = getNextPath(k1);
			}
			else
				break;		
		}
		this->sol.reset();
	}
	
	print_bmc_ce_statistics(reach_for_CE, symbolic_ce_list, user_ops, "Safety Analysis");
	std::cout<< "\nTotal paths visited by our old Algorithm: "<<cnt<<endl;
	std::cout<< "\nTotal paths visited by our new Algorithm: "<<run_counter<<endl;
	std::cout<<"\nNumber of postC operation done: "<<postc_counter<<endl;
	show(R, user_ops);
	if(symbolic_ce_list.size()==0 && reach_for_CE.get_counter_examples().size()==0)
	{
		return 1; //model is safe
	}
	if(symbolic_ce_list.size()>0 && reach_for_CE.get_counter_examples().size()==0)
	{
		return 2; // The safety of the model is unknown
	}
}
