/*
 * load_model.cpp
 *
 *  Created on: 22-May-2016
 *      Author: rajarshi
 */

#include <io/loadModel.h>



/* Sets the reachability options given by the user */

void set_params(hybrid_automata& ha, std::list<initial_state::ptr>& init_state,
		userOptions& op, ReachabilityParameters& reach_parameters,
		std::vector<forbidden>& forbidden_states) {

	unsigned int row, col;
	reach_parameters.TimeBound = op.get_timeHorizon(); //Total Time Interval
	reach_parameters.Iterations = (unsigned int) (op.get_timeHorizon() / op.get_timeStep()); // number of iterations
	reach_parameters.time_step = op.get_timeStep();
	
	//Assigning Directions
	unsigned int Directions_Type = op.get_directionTemplate(); 

	unsigned int dim=ha.ymap_size();
	if(dim==0){
		for (std::list<initial_state::ptr>::iterator it=init_state.begin();it!=init_state.end();it++){
			dim = (*it)->getInitialSet()->getSystemDimension();
			std::cout<<"Dimension is set here"<<std::endl;
		}
	}

	math::matrix<double> Real_Directions; //List of all directions
	std::vector<std::vector<double> > newDirections;
	std::cout<<"Directions_Type: "<<Directions_Type<<std::endl;
	std::cout<<"Dim: "<<dim<<std::endl;
	if (Directions_Type == BOX) {
		unsigned int num_directions = 2 * dim; //Axis Directions
		newDirections = generate_axis_directions(dim);
		//if there are output variables related linearly with state variables
		if(ha.ymap_size()!=0){
			transformDirections(ha, newDirections, newDirections);
		}
		get_ublas_matrix(newDirections, Real_Directions);
		row = num_directions;
		col = dim;
		reach_parameters.Directions.resize(row, col);
		reach_parameters.Directions = Real_Directions; //Direct Assignment
	}
	if (Directions_Type == OCT) {
		unsigned int num_directions = 2 * dim * dim; // Octagonal directions
		newDirections = get_octagonal_directions(dim);
		if(ha.ymap_size()!=0){
			transformDirections(ha, newDirections,newDirections);
		}
		get_ublas_matrix(newDirections, Real_Directions);
		row = num_directions;
		col = dim;
		reach_parameters.Directions.resize(row, col);
		reach_parameters.Directions = Real_Directions; //Direct Assignment
	}
	forbidden forbidden_set;
	if (!op.get_forbidden_set().empty()) {
		string_to_poly(op.get_forbidden_set(), forbidden_set);
		forbidden_states.clear(); // erase the cfg parsed input
		forbidden_states.push_back(forbidden_set); // overwrite with cdmline parsed input
	}
//	exit(0);
}

/* for composed analysis*/
void set_comp_params(hybrid_automata& ha, std::list<initial_state::ptr>& init_state,
		const userOptions& op, ReachabilityParameters& reach_parameters,
		const std::vector<forbidden>& forbidden_states) {

	unsigned int row, col;
	reach_parameters.TimeBound = op.get_timeHorizon(); //Total Time Interval
	reach_parameters.Iterations = (unsigned int) (op.get_timeHorizon() / op.get_timeStep()); // number of iterations
	reach_parameters.time_step = op.get_timeStep();

	//Assigning Directions
	unsigned int Directions_Type = op.get_directionTemplate();

	unsigned int dim=ha.ymap_size();
	if(dim==0){
		for (std::list<initial_state::ptr>::iterator it=init_state.begin();it!=init_state.end();it++){
			dim = (*it)->getInitialSet()->getSystemDimension();
		}
	}

	math::matrix<double> Real_Directions; //List of all directions
	std::vector<std::vector<double> > newDirections;
	Directions_Type = 1; //hard coded for testing composition.
	if (Directions_Type == BOX) {
		unsigned int num_directions = 2 * dim; //Axis Directions
		newDirections = generate_axis_directions(dim);
		//if there are output variables related linearly with state variables
		if(ha.ymap_size()!=0){
			transformDirections(ha, newDirections, newDirections);
		}
		get_ublas_matrix(newDirections, Real_Directions);
		row = num_directions;
		col = dim;
		reach_parameters.Directions.resize(row, col);
		reach_parameters.Directions = Real_Directions; //Direct Assignment
	}
	if (Directions_Type == OCT) {
		unsigned int num_directions = 2 * dim * dim; // Octagonal directions
		newDirections = get_octagonal_directions(dim);
		if(ha.ymap_size()!=0){
			transformDirections(ha, newDirections,newDirections);
		}
		get_ublas_matrix(newDirections, Real_Directions);
		row = num_directions;
		col = dim;
		reach_parameters.Directions.resize(row, col);
		reach_parameters.Directions = Real_Directions; //Direct Assignment
	}
/*	forbidden forbidden_set;
	if (!op.get_forbidden_set().empty()) {
		string_to_poly(op.get_forbidden_set(), forbidden_set);
		forbidden_states.clear(); // erase the cfg parsed input
		forbidden_states.push_back(forbidden_set); // overwrite with cdmline parsed input
	} */
}
//________________________________________



hybrid_automata set_composed_HA(std::list<hybrid_automata::ptr>& ha_ptrs, std::list<initial_state::ptr>& init, std::list<initial_state::ptr>& initial)
{
	std::list<std::string> shared_variable;
	hybrid_automata ha;
	unsigned int new_dim = ha_ptrs.begin()->get()->getDimension();
	for (auto it = ha_ptrs.begin(); it != ha_ptrs.end(); it++)
	{
		//std::cout<<it->get()->getTotalLocations()<<endl;
		//std::cout<<"Dimension: "<<it->get()->getDimension()<<endl;
		//std::cout<<"variables: "<<it->get()->get_variable_name(0)<<endl;

		for (unsigned int dim = 0; dim < it->get()->getDimension(); dim++)
		{
			bool share_variable = false;
			std::string var_name = it->get()->get_varname(dim);
			//std::cout << "Variable auto = " << var_name<<endl;
			if(it == ha_ptrs.begin())
			{
				//std::cout<<"First automaton"<<endl;
				ha.insert_to_map(var_name, dim);
				//insert_to_map(var_name,dim);
			}
			else
			{
				//std::cout<<"else part"<<endl;
				//std::cout<<"new dim: "<<new_dim<<endl;

				for (auto it1 = ha.var_index_map_ptr.get()->begin(); it1 != ha.var_index_map_ptr.get()->end(); it1++)
				{
					//std::cout << "Variable = " << (*it1).first;
					//std::cout << " Dimension = " << (*it1).second << std::endl;
					if(it->get()->get_varname(dim) == (*it1).first)
					{
						share_variable = true;
						shared_variable.push_back((*it1).first);
						break;
					}

				}
				if(share_variable == false)
				{
					//std::cout<<"non shared variable added to the list"<<endl;
					//std::cout << "Variable = " << var_name;
					//std::cout << " Dimension = " << new_dim << std::endl;
					ha.insert_to_map(var_name, new_dim);
					//insert_to_map(var_name,new_dim);
					new_dim++;
				}
			}
		}
		ha.setDimension(new_dim);
	}
	ha.print_var_index_map();
	polytope::ptr initial_set = polytope::ptr(new polytope()); // universe;
	for (auto it = init.begin(); it != init.end(); it++)
	{
		(*it)->getInitialSet()->printPoly();
	}
	math::matrix<double> default_init = init.begin()->get()->getInitialSet()->getCoeffMatrix();
	std::vector<double> coeff = init.begin()->get()->getInitialSet()->getColumnVector();

	//initial_set->setSystemDimension(ha.getDimension());
	//initial_set->printPoly();
	math::matrix<double> mt(default_init.size1(), ha.getDimension());
	std::vector<double> cv(default_init.size1());
	mt.clear();
	//std::cout<<mt<<endl;
	/*for(unsigned int j=0;j<cv.size();j++)
		std::cout << setprecision(12) << cv[j] << "    " ;
	std::cout << "\n"; */
	//std::cout<<"default_init: "<<default_init<<endl;
	for(unsigned int i=0; i<default_init.size1();i++)
	{
		for (unsigned int j=0; j<default_init.size2(); j++)
			mt(i,j) = default_init(i,j);
		cv[i] = coeff[i];
	}
	initial_set->setPolytope(mt, cv, 1);
	//initial_set->printPoly();
	auto ha_component = ha_ptrs.begin();
	//ha_component++;
	for (auto it = init.begin(); it != init.end(); it++)
	{
		if(it != init.begin())
		{
			ha_component++;
			math::matrix<double> mat = it->get()->getInitialSet()->getCoeffMatrix();
			std::vector<double> coef = it->get()->getInitialSet()->getColumnVector();
			//std::cout<<mat<<endl;
			//print_mapper();

			//mt.resize(mt.size1()+ mat.size1(), mat.size2());
			//cv.resize(cv.size()+ coef.size());
			std::vector<double> constraint;
			constraint.resize(mt.size2());
			for(unsigned int i=0; i<mat.size1();i++)
			{
				for (unsigned int j=0; j<mat.size2(); j++)
				{
					constraint.at(ha.get_index(ha_component->get()->get_varname(j)))= mat(i,j);
				}
				//std::cout<<"Size: "<<initial_set->getCoeffMatrix().size2()<<endl;
				//std::cout<<"Size of constraints: "<<constraint.size()<<endl;
				initial_set->setMoreConstraints(constraint, coef[i]);
			}
		}
	}
	initial_state::ptr ini_ptr = initial_state::ptr(new initial_state(1, initial_set));
	initial.push_back(ini_ptr);
//	newInit.push_back(ini_ptr);



	return ha;
}


void transformDirections(hybrid_automata& ha, std::vector<std::vector<double> >& directions, std::vector<std::vector<double> >& res)
{
	// transform the directions with the MatrixT transformation matrix
	res.resize(directions.size());
	location::ptr locPtr = ha.getInitialLocation();
	Dynamics D = locPtr->getSystemDynamics();
	for(unsigned int i=0;i<directions.size();i++){
		std::vector<double> ell = directions[i];
		math::matrix<double> C;
		D.MatrixT.transpose(C);
		C.mult_vector(ell,ell);
		res[i] = ell;
	}
}
