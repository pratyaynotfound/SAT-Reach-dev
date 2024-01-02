/*
 * composition.cpp
 *
 * Author: Atanu Kundu
 * Created on : 27-jun-2023
 *
 */
#include <counterExample/composition.h>


/* The variable to dimension index map */
//composition::mapper composition::var_index_mapper =
//		composition::mapper(new std::map<std::string,unsigned int>());

int composition::loc_id = 0;

composition::composition(const hybrid_automata::ptr ha_ptr, const std::list<hybrid_automata::ptr>& haPtr,  const std::list<initial_state::ptr>& init,
		const std::list<initial_state::ptr>& init_comps, const forbidden_states& forbidden, ReachabilityParameters& r_params,
		const userOptions& user_ops) : ha_ptr(ha_ptr), ha_ptrs(haPtr), init(init), init_comps(init_comps), forbidden_s(forbidden),
				reach_params(r_params), user_ops(user_ops)
{
	std::cout<<"in composition class"<<endl;
/*	//std::cout<<ha_ptr->getTotalLocations();
	std::cout<<"size of forbidden states: "<<forbidden_s.size()<<endl;
	//forbidden_s[0].second->printPoly();
	for (unsigned int i =0; i < forbidden_s.size(); i++)
	{
		std::cout<<"forbidden state: "<<forbidden_s[i].first<<endl;
		forbidden_s[i].second->printPoly();
	}
	for (auto it = ha_ptrs.begin(); it != ha_ptrs.end(); it++)
	{
		std::cout<<it->get()->getTotalLocations()<<endl;
	}
	initial_state::ptr initial_set = *(init.begin());

	for (auto it = init.begin(); it != init.end(); it++)
	{
		(*it)->getInitialSet()->printPoly();
	} */
	ha = *ha_ptr;

}
/*
void composition::insert_to_map(std::string name, unsigned int val)
{
	var_index_mapper->insert(std::pair<std::string, unsigned int>(name,val));
}

/* returns the variable name with the given index */
/*
std::string composition::get_variable_name(unsigned int index) const
{
	for(std::map<std::string, unsigned int>::iterator it = var_index_mapper.get()->begin(); it!=var_index_mapper.get()->end();it++){
		if(index == (*it).second)
			return (*it).first;
	}
	throw std::runtime_error("var to index map: get_varname(): variable index not in the map\n");
}

unsigned int composition::get_index_name(std::string var_name) const
{
	unsigned int index;
	try{
		for(std::map<std::string, unsigned int>::iterator it = var_index_mapper.get()->begin(); it!=var_index_mapper.get()->end();it++){
			if(var_name == (*it).first)
				index = (*it).second;
		}
	}catch(...)
	{
		std::cout << "index of variable not found in the variable to index map. Check the variable name.\n";
		std::cout << "varname:" << var_name << std::endl;
		exit(0);
	};
	return index;
}
void composition::print_mapper()
{
	for (auto it = var_index_mapper.get()->begin(); it != var_index_mapper.get()->end(); it++)
		{
			std::cout << "Variable = " << (*it).first;
			std::cout << " Dimension = " << (*it).second << std::endl;
		}
}
*/
void composition::make_composed_dimension()
{
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

}

void composition::build_initial(polytope::ptr& initial_set)
{
	for (auto it = init_comps.begin(); it != init_comps.end(); it++)
	{
		(*it)->getInitialSet()->printPoly();
	}
	math::matrix<double> default_init = init_comps.begin()->get()->getInitialSet()->getCoeffMatrix();
	std::vector<double> coeff = init_comps.begin()->get()->getInitialSet()->getColumnVector();

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
	for (auto it = init_comps.begin(); it != init_comps.end(); it++)
	{
		if(it != init_comps.begin())
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
	//initial_state::ptr ini_ptr = initial_state::ptr(new initial_state(1, initial_set));
	//init.push_back(ini_ptr);
	//init.push_back(ini_ptr);


}

bool composition::is_shared(std::string str)
{
	for(std::list<std::string>::iterator k = shared_variable.begin(); k != shared_variable.end(); k++)
	{
		if(str.compare(*k)== 0)
			return true;
	}
	return false;

}

void composition::build_location(location::ptr& loc, polytope::ptr& inv)
{
	std::cout<<"In build location....."<<endl;

	loc->setLocId(++loc_id);
	std::string loc_name;
	Dynamics D;
	Dynamics newD;
	newD.MatrixA.resize(ha.getDimension(), ha.getDimension(), false);
	newD.MatrixB.resize(ha.getDimension(), ha.getDimension(), false);
	newD.C.resize(ha.getDimension());
	math::matrix<double> first_inv_poly;
	std::vector<double> first_inv_coeff;
	double sum = 0.0;
	newD.MatrixA.clear();
	newD.MatrixB.clear();
	for (std::list<hybrid_automata::ptr>::const_iterator it = ha_ptrs.begin(); it != ha_ptrs.end(); it++)
	{
		D = it->get()->getInitialLocation()->getSystemDynamics();
		std::cout<<D.MatrixA;
		std::cout<<D.MatrixB;
		std::cout << "The constant vector:\n";
		for(unsigned int i=0;i<D.C.size();i++)
			std::cout << D.C[i] << "\n";
		//std::vector<double> constraint;
		//constraint.resize(ha.getDimension());
		std::cout<<"U matrix:"<<endl;
		D.U->printPoly();
		math::matrix<double> old_mt = D.U->getCoeffMatrix();

		if(it == ha_ptrs.begin())
		{
			loc_name = it->get()->getInitialLocation()->getName();

			for(unsigned int i = 0; i<D.MatrixA.size1(); i++)
			{
				for(unsigned int j = 0; j<D.MatrixA.size1(); j++)
				{
					newD.MatrixA(i,j) = D.MatrixA(i,j);
					newD.MatrixB(i,j) = D.MatrixB(i,j);
				}
				newD.C[i] = D.C[i];
			}
			math::matrix<double> mt(D.U->getCoeffMatrix().size1(), ha.getDimension());
			std::vector<double> cv(D.U->getCoeffMatrix().size1());
			mt.clear();
			//std::cout<<mt<<endl;
			for(unsigned int i=0; i < old_mt.size1(); i++)
			{
				for(unsigned int j=0; j < old_mt.size2(); j++)
				{
					mt(i,j) = old_mt(i,j);
				}
				cv[i]= D.U->getColumnVector()[i];
			}
			newD.U = polytope::ptr(new polytope(mt, cv, 1));
			newD.U->printPoly();
		}
		else
		{
			loc_name = loc_name + "_" + it->get()->getInitialLocation()->getName();

			for(unsigned int i = 0; i<D.MatrixA.size1(); i++)
			{
				if(is_shared(it->get()->get_varname(i)))
				{
					//std::cout<<"shared variable :"<<it->get()->get_varname(i)<<endl;
					for(unsigned int j = 0; j<D.MatrixA.size2(); j++)
					{
						if(newD.MatrixA(ha.get_index(it->get()->get_varname(i)), ha.get_index(it->get()->get_varname(j))) == D.MatrixA(i,j)
								&& newD.MatrixB(ha.get_index(it->get()->get_varname(i)), ha.get_index(it->get()->get_varname(j))) == D.MatrixB(i,j))
						{
							//std::cout<<"Dimension same"<<endl;
						}

						else if(newD.MatrixA(ha.get_index(it->get()->get_varname(i)), ha.get_index(it->get()->get_varname(j))) == 0 && D.MatrixA(i,j) != 0)
						{
							//Update Dynamics to the new value.
							newD.MatrixA(ha.get_index(it->get()->get_varname(i)), ha.get_index(it->get()->get_varname(j))) = D.MatrixA(i,j);
							//newD.MatrixB(ha.get_index(it->get()->get_varname(i)), ha.get_index(it->get()->get_varname(j))) = D.MatrixB(i,j);

						}

						else if(newD.MatrixA(ha.get_index(it->get()->get_varname(i)), ha.get_index(it->get()->get_varname(j))) != 0 && D.MatrixA(i,j) == 0)
						{

						}
						else
						{
							std::cout<<"Found conflict"<<endl;
							break;
						}

					}

					if(newD.C[ha.get_index(it->get()->get_varname(i))] == D.C[i])
					{

					}

					else if(newD.C[ha.get_index(it->get()->get_varname(i))] == 0 && D.C[i] != 0)
					{
						newD.C[ha.get_index(it->get()->get_varname(i))] == D.C[i];
					}

					else if(newD.C[ha.get_index(it->get()->get_varname(i))] != 0 && D.C[i] == 0)
					{
						//newD.C[ha.get_index(it->get()->get_varname(i))] == D.C[i];
					}
					//ToDo: Have to fixed the dimension of shared variable.
					//Now, I am pass the constant C checking.
					//in ACC model, shared variable X0 has empty dynamics, while
					// in other component, x0 has some dynamics.
					// what we have to do for this case?
					else
					{
						std::cout<<"Found conflict in column vector"<<endl;
						break;
					}
					/*std::cout<<D.C[i]<<endl;
					std::cout<<newD.C[ha.get_index(it->get()->get_varname(i))]<<endl;
					if(D.C[i] == 0 && newD.C[ha.get_index(it->get()->get_varname(i))] != 0)
						newD.C[ha.get_index(it->get()->get_varname(i))] == D.C[i]; */
				}
				else
				{
					//std::cout<<"variable :"<<it->get()->get_varname(i)<<endl;
					for(unsigned int j = 0; j<D.MatrixA.size2(); j++)
					{
						newD.MatrixA(ha.get_index(it->get()->get_varname(i)), ha.get_index(it->get()->get_varname(j))) = D.MatrixA(i,j);
						newD.MatrixB(ha.get_index(it->get()->get_varname(i)), ha.get_index(it->get()->get_varname(j))) = D.MatrixB(i,j);

					}
					newD.C[ha.get_index(it->get()->get_varname(i))] = D.C[i];
				}
			}
			std::vector<double> u_const, next_u_const;
			u_const.resize(ha.getDimension());
			next_u_const.resize(ha.getDimension());
			//std::cout<<"Old mt: "<<old_mt<<endl;
			for(unsigned int i=0; i < old_mt.size2(); i++)
			{
				if(is_shared(it->get()->get_varname(i)))
				{
					//std::cout<<it->get()->get_varname(i)<<endl;
					//std::cout<<newD.C[ha.get_index(it->get()->get_varname(i))]<<"\t"<<D.U->getColumnVector()[2*i]<<endl;
					for(unsigned int i=0;i<newD.C.size();i++)
						std::cout << newD.C[i] << "\n";
					D.U->printPoly();
					if(newD.C[ha.get_index(it->get()->get_varname(i))] == D.U->getColumnVector()[2*i])
					{

					}
					else if(newD.C[ha.get_index(it->get()->get_varname(i))] == 0 && D.U->getColumnVector()[(2*i)] != 0)
					{
						for(unsigned int j=0; j < old_mt.size2(); j++)
						{
							u_const.at(ha.get_index(it->get()->get_varname(j))) = old_mt((2*i),j);
							next_u_const.at(ha.get_index(it->get()->get_varname(j))) = old_mt((2*i)+1,j);
						}
						newD.U->setMoreConstraints(u_const, D.U->getColumnVector()[(2*i)]);
						newD.U->setMoreConstraints(next_u_const, D.U->getColumnVector()[(2*i)+1]);

					}
					else if(newD.C[ha.get_index(it->get()->get_varname(i))] != 0 && D.U->getColumnVector()[2*i] == 0)
					{

					}
					else if(newD.C[ha.get_index(it->get()->get_varname(i))] == 0 && D.U->getColumnVector()[2*i] == 0)
					{

					}
					else
					{
						std::cout<<"Found conflict in column vector while making set U."<<endl;
						break;
					}

				}
				else
				{
					for(unsigned int j=0; j < old_mt.size2(); j++)
					{
						u_const.at(ha.get_index(it->get()->get_varname(j))) = old_mt((2*i),j);
						next_u_const.at(ha.get_index(it->get()->get_varname(j))) = old_mt((2*i)+1,j);
					}
					newD.U->setMoreConstraints(u_const, D.U->getColumnVector()[(2*i)]);
					newD.U->setMoreConstraints(next_u_const, D.U->getColumnVector()[(2*i)+1]);

				}
			}

		}

		first_inv_poly = it->get()->getInitialLocation()->getInvariant()->getCoeffMatrix();
		first_inv_coeff = it->get()->getInitialLocation()->getInvariant()->getColumnVector();
		std::vector<double> constraint;
		constraint.resize(ha.getDimension());
		for(unsigned int i=0; i<first_inv_poly.size1();i++)
		{
			for (unsigned int j=0; j<first_inv_poly.size2(); j++)
			{
				constraint.at(ha.get_index(it->get()->get_varname(j)))= first_inv_poly(i,j);
			}
			inv->setMoreConstraints(constraint, first_inv_coeff[i]);
		}

	}
	if(newD.isEmptyMatrixB && !newD.isEmptyC)
		std::cout << "B is empty and C is not empty\n";
	if(!newD.isEmptyMatrixB && !newD.isEmptyC)
		std::cout << "B is not empty and C is not empty\n";
	//std::cout<<loc_name<<endl;
	loc->setName(loc_name);
	loc->setInvariant(inv);
	/*inv->printPoly();
	std::cout<<newD.MatrixA;
	std::cout<<newD.MatrixB;
	std::cout << "The constant vector:\n";
	for(unsigned int i=0;i<newD.C.size();i++)
		std::cout << newD.C[i] << "\n"; */
	newD.U->printPoly();
	loc->setSystemDynamics(newD);
}


transition::ptr composition::find_trans(std::string label, std::list<location::const_ptr> locations, std::list<hybrid_automata::ptr> Ha_Ptrs)
{
//	list<std::string> labels; //list of unique labels for the set of initial locations of different component.
//	std::list<transition::ptr> transitions;
//	std::list<location::const_ptr> next_location; //list of next locations
//	for (std::list<hybrid_automata::ptr>::const_iterator it1 = ha_ptrs.begin(); it1 != ha_ptrs.end(); it1++)
//	{
//		//std::cout<<"for automata: "<<endl;
//		list<transition::ptr> abc = it1->get()->getInitialLocation()->getOutGoingTransitions();
//		for(list<transition::ptr>::iterator it = abc.begin(); it != abc.end();it++)
//		{
//			list<std::string>::iterator itt = std::find(labels.begin(), labels.end(), it->get()->getLabel());
//			if(itt == labels.end())
//				labels.push_back(it->get()->getLabel());
////			std::cout<<it->get()->getSourceLocationId()<<"\t";
////			std::cout<<it->get()->getLabel()<<"\t";
////			std::cout<<it->get()->getTransitionId()<<"\t";
////			it->get()->getGuard()->printPoly();
////			std::cout<<it->get()->getAssignT().Map<<endl;
////			for(unsigned int i=0;i<it->get()->getAssignT().b.size();i++)
////				std::cout << it->get()->getAssignT().b[i] << "\n";
////			std::cout<<it->get()->getDestinationLocationId()<<endl;
////			location::const_ptr new_loc = it1->get()->getLocation(it->get()->getDestinationLocationId());
////			new_loc->getInvariant()->printPoly();
////			std::cout<<"Loc name: "<<new_loc->getName()<<endl;
//
//		}
//	}
	//std::cout<<"Size of labels: "<<labels.size()<<endl;
//	for(list<std::string>::iterator itLab = labels.begin(); itLab != labels.end(); itLab++)
//	{
//		std::cout<<label<<endl;
		bool flag = true;

		//math::matrix<double> guard_mat;
		//std::vector<double> guard_coeff;
		transition::ptr t = transition::ptr(new transition());
		polytope::ptr guard_set = polytope::ptr(new polytope());
		Assign t_reset;
		static int trans_id;
		math::matrix<double> reset_map(ha.getDimension(), ha.getDimension());
		std::vector<double> reset_coeff(ha.getDimension());
		reset_map.matrix_Identity(ha.getDimension(), reset_map);
		reset_coeff.resize(ha.getDimension(),0);
		std::list<hybrid_automata::ptr>::const_iterator it_ha = ha_ptrs.begin();
		for (std::list<location::const_ptr>::const_iterator it_loc = locations.begin(); it_loc!= locations.end(); it_loc++)
		{
			std::cout<<"location name: "<<it_loc->get()->getName()<<endl;
			list<transition::ptr> abc = it_loc->get()->getOutGoingTransitions();
			for(list<transition::ptr>::iterator it = abc.begin(); it != abc.end(); it++)
			{
				if(it->get()->getLabel() == label)
				{
					math::matrix<double> guard_mat(it->get()->getGuard()->getCoeffMatrix().size1(), ha.getDimension());
					std::vector<double> guard_coeff(it->get()->getGuard()->getCoeffMatrix().size1());
					guard_mat.clear();

					//Assigning Guard.
					if(flag == true)
					{
						//std::cout<<it->get()->getAssignT().Map<<endl;
						flag = false;
						for(unsigned int i = 0; i < it->get()->getGuard()->getCoeffMatrix().size1(); i++)
						{
							for(unsigned int j =0; j < it->get()->getGuard()->getCoeffMatrix().size2(); j++)
							{
								guard_mat(i, ha.get_index(it_ha->get()->get_varname(j))) = it->get()->getGuard()->getCoeffMatrix()(i,j);
							}
							guard_coeff[i] = it->get()->getGuard()->getColumnVector()[i];
						}
						guard_set->setPolytope(guard_mat, guard_coeff, 1);

						/*for(unsigned int i = 0; i< it->get()->getAssignT().Map.size1(); i++)
						{
							for(unsigned int j = 0; j < it->get()->getAssignT().Map.size2(); j++)
							{
								reset_map(i, ha.get_index(it_ha->get()->get_varname(j))) = it->get()->getAssignT().Map(i,j);
							}
							reset_coeff[i] = it->get()->getAssignT().b[i];
						}
						std::cout<<reset_map<<endl;
						for(unsigned int i=0;i<reset_coeff.size();i++)
							std::cout << reset_coeff[i] << "\n"; */
					}
					else
					{
						//std::cout<<it->get()->getAssignT().Map<<endl;
						std::vector<double> constraint;
						constraint.resize(ha.getDimension());
						for(unsigned int i=0; i<it->get()->getGuard()->getCoeffMatrix().size1();i++)
						{
							for (unsigned int j=0; j<it->get()->getGuard()->getCoeffMatrix().size2(); j++)
							{
								constraint.at(ha.get_index(it_ha->get()->get_varname(j)))= it->get()->getGuard()->getCoeffMatrix()(i,j);
							}
							//std::cout<<"Size: "<<initial_set->getCoeffMatrix().size2()<<endl;
							//std::cout<<"Size of constraints: "<<constraint.size()<<endl;
							guard_set->setMoreConstraints(constraint, it->get()->getGuard()->getColumnVector()[i]);
						}
					}
					//Assigning reset map.
					for(unsigned int i = 0; i< it->get()->getAssignT().Map.size1(); i++)
					{
						unsigned int position;
						bool found_reset = false;
						for(unsigned int j = 0; j < it->get()->getAssignT().Map.size2(); j++)
						{
							if(it->get()->getAssignT().Map(i,j) == 0 && i == j)
							{
								found_reset = true;
								position = ha.get_index(it_ha->get()->get_varname(j));
								reset_map(position, ha.get_index(it_ha->get()->get_varname(j))) = it->get()->getAssignT().Map(i,j);
							}
						}
						if(found_reset)
							reset_coeff[position] = it->get()->getAssignT().b[i];
					}
				}
			}
			it_ha++;
		}
		/*guard_set->printPoly();
		std::cout<<reset_map<<endl;
		for(unsigned int i=0;i<reset_coeff.size();i++)
			std::cout << reset_coeff[i] << "\n"; */
		t->setLabel(label);
		t->setTransitionId(++trans_id);
		t->setGuard(guard_set);
		t_reset.Map = reset_map;
		t_reset.b = reset_coeff;
		t->setAssignT(t_reset);
//		t->setSourceLocationId(loc->getLocId());
//		transitions.push_back(t);
//	}
		return t;
//	return transitions;
}

void composition::make_shared_labels(std::list<location::const_ptr> locations)
{
	list<std::string> labels; //list of unique labels for the set of initial locations of different component.
//	std::cout<<"size of locations: "<<locations.size()<<endl;
//	for(std::list<location::const_ptr>::const_iterator loc = locations.begin(); loc != locations.end(); loc++)
//	{
//		std::cout<<loc->get()->getName()<<endl;
//	}
	//std::list<hybrid_automata::ptr>::const_iterator it1 = ha_ptrs.begin();
	for(std::list<location::const_ptr>::const_iterator loc = locations.begin(); loc != locations.end(); loc++)
	{
		//std::cout<<"for automata: "<<endl;
		list<transition::ptr> abc = loc->get()->getOutGoingTransitions();
		for(list<transition::ptr>::iterator it = abc.begin(); it != abc.end();it++)
		{
			list<std::string>::iterator itt = std::find(labels.begin(), labels.end(), it->get()->getLabel());
			if(itt == labels.end())
				labels.push_back(it->get()->getLabel());
		}
		//it1++;
	}
	for(list<std::string>::iterator itLab = labels.begin(); itLab != labels.end(); itLab++)
	{
//		std::cout<<*itLab<<endl;
		composeLabels clabels;
		clabels.labels = *itLab;
		std::list<location::const_ptr> next_location;
		std::list<hybrid_automata::ptr> Ha_Ptrs;
		std::list<hybrid_automata::ptr>::const_iterator it1 = ha_ptrs.begin();
		for(std::list<location::const_ptr>::const_iterator loc = locations.begin(); loc != locations.end(); loc++)
		{
			bool label_exist = false;
			list<transition::ptr> abc = loc->get()->getOutGoingTransitions();
			for(list<transition::ptr>::iterator it = abc.begin(); it != abc.end(); it++)
			{
				if(it->get()->getLabel() == *itLab)
				{
					label_exist = true;
//					it1->get()->print_var_index_map();
					next_location.push_back(it1->get()->getLocation(it->get()->getDestinationLocationId()));
					Ha_Ptrs.push_back(*(it1));
					//std::cout<<it->get()->getDestinationLocationId()<<endl;
				}
			}
			if(!label_exist)
			{
				next_location.push_back(*(loc));
				Ha_Ptrs.push_back(*(it1));
			}
			it1++;
		}
		clabels.next_locations = next_location;
		clabels.Ha_Ptrs = Ha_Ptrs;
		ComposedLabels.push_back(clabels);
	}
}


location::ptr composition::marged_locations(std::list<location::const_ptr> locs, std::list<hybrid_automata::ptr> Ha_Ptrs)
{
	std::cout<<"In marged location...."<<endl;
	location::ptr new_loc = location::ptr(new location());
//	static int loc_id;
	new_loc->setLocId(++loc_id);
	std::string loc_name;
	Dynamics D;
	Dynamics newD;
	newD.MatrixA.resize(ha.getDimension(), ha.getDimension(), false);
	newD.MatrixB.resize(ha.getDimension(), ha.getDimension(), false);
	newD.C.resize(ha.getDimension());
//	newD.U = polytope::ptr(new polytope());
//	newD.U->printPoly();
	math::matrix<double> first_inv_poly;
	std::vector<double> first_inv_coeff;
	polytope::ptr inv = polytope::ptr(new polytope()); //universe
	newD.MatrixA.clear();
	newD.MatrixB.clear();
//	std::cout << "The constant vector:\n";
//	for(unsigned int i=0;i<newD.C.size();i++)
//		std::cout << newD.C[i] << "\n";
//	std::cout<<newD.MatrixA;
	std::list<hybrid_automata::ptr>::iterator it_ha = Ha_Ptrs.begin();
	for(std::list<location::const_ptr>::iterator it_loc = locs.begin(); it_loc != locs.end(); it_loc++)
	{
		std::cout<<"Locations marging...."<<endl;
		std::cout<<it_loc->get()->getName()<<endl;
		D = it_loc->get()->getSystemDynamics();
//		std::cout<<D.MatrixA;
//		std::cout<<D.MatrixB;
//		std::cout << "The constant vector:\n";
//		for(unsigned int i=0;i<D.C.size();i++)
//			std::cout << D.C[i] << "\n";
//		//std::vector<double> constraint;
//		//constraint.resize(ha.getDimension());
//		std::cout<<"U matrix:"<<endl;
//		D.U->printPoly();
		math::matrix<double> old_mt = D.U->getCoeffMatrix();
		if(it_loc == locs.begin())
		{
			loc_name = it_loc->get()->getName();
			math::matrix<double> mt(D.U->getCoeffMatrix().size1(), ha.getDimension());
			std::vector<double> cv(D.U->getCoeffMatrix().size1());
			mt.clear();
			//std::cout<<mt<<endl;
			for(unsigned int i=0; i < old_mt.size1(); i++)
			{
				for(unsigned int j=0; j < old_mt.size2(); j++)
				{
					mt(i,j) = old_mt(i,j);
				}
				cv[i]= D.U->getColumnVector()[i];
			}
			newD.U = polytope::ptr(new polytope(mt, cv, 1));
//			newD.U->printPoly();
		}
		else
		{
			loc_name = loc_name + "_" + it_loc->get()->getName();
			std::vector<double> u_const, next_u_const;
			u_const.resize(ha.getDimension());
			next_u_const.resize(ha.getDimension());
			//std::cout<<"Old mt: "<<old_mt<<endl;
	//		math::matrix<double> mt(2*ha.getDimension(), ha.getDimension());
	//		std::vector<double> cv(ha.getDimension());
	//		mt.clear();
	//		newD.U = polytope::ptr(new polytope(mt, cv, 1));
	//		newD.U->printPoly();
			for(unsigned int i=0; i < D.U->getCoeffMatrix().size2(); i++)
			{

				if(is_shared(it_ha->get()->get_varname(i)))
				{
					//std::cout<<it->get()->get_varname(i)<<endl;
					//std::cout<<newD.C[ha.get_index(it->get()->get_varname(i))]<<"\t"<<D.U->getColumnVector()[2*i]<<endl;
//					for(unsigned int i=0;i<newD.C.size();i++)
//						std::cout << newD.C[i] << "\n";
//					D.U->printPoly();
					if(newD.C[ha.get_index(it_ha->get()->get_varname(i))] == D.U->getColumnVector()[2*i])
					{

					}
					else if(newD.C[ha.get_index(it_ha->get()->get_varname(i))] == 0 && D.U->getColumnVector()[(2*i)] != 0)
					{
						for(unsigned int j=0; j < old_mt.size2(); j++)
						{
							u_const.at(ha.get_index(it_ha->get()->get_varname(j))) = old_mt((2*i),j);
							next_u_const.at(ha.get_index(it_ha->get()->get_varname(j))) = old_mt((2*i)+1,j);
						}
						newD.U->setMoreConstraints(u_const, D.U->getColumnVector()[(2*i)]);
						newD.U->setMoreConstraints(next_u_const, D.U->getColumnVector()[(2*i)+1]);

					}
					else if(newD.C[ha.get_index(it_ha->get()->get_varname(i))] != 0 && D.U->getColumnVector()[2*i] == 0)
					{

					}
					else if(newD.C[ha.get_index(it_ha->get()->get_varname(i))] == 0 && D.U->getColumnVector()[2*i] == 0)
					{

					}
					else
					{
						std::cout<<"Found conflict in column vector while making set U."<<endl;
						break;
					}

				}
				else
				{
					for(unsigned int j=0; j < old_mt.size2(); j++)
					{
						u_const.at(ha.get_index(it_ha->get()->get_varname(j))) = old_mt((2*i),j);
						next_u_const.at(ha.get_index(it_ha->get()->get_varname(j))) = old_mt((2*i)+1,j);
					}
					newD.U->setMoreConstraints(u_const, D.U->getColumnVector()[(2*i)]);
					newD.U->setMoreConstraints(next_u_const, D.U->getColumnVector()[(2*i)+1]);

				}

			}
		}
		for(unsigned int i = 0; i < D.MatrixA.size1(); i++)
		{

			if(is_shared(it_ha->get()->get_varname(i)))
			{
//				std::cout<<"shared variable :"<<it_ha->get()->get_varname(i)<<endl;
				for(unsigned int j = 0; j<D.MatrixA.size2(); j++)
				{
//					std::cout<<D.MatrixA(i,j)<<endl;
//					std::cout<<newD.MatrixA(ha.get_index(it_ha->get()->get_varname(i)), ha.get_index(it_ha->get()->get_varname(j)))<<endl;
					if(newD.MatrixA(ha.get_index(it_ha->get()->get_varname(i)), ha.get_index(it_ha->get()->get_varname(j))) == D.MatrixA(i,j))
					{
						//std::cout<<"Dimension same"<<endl;
					}

					else if(newD.MatrixA(ha.get_index(it_ha->get()->get_varname(i)), ha.get_index(it_ha->get()->get_varname(j))) == 0 && D.MatrixA(i,j) != 0)
					{
						//Update Dynamics to the new value.
						newD.MatrixA(ha.get_index(it_ha->get()->get_varname(i)), ha.get_index(it_ha->get()->get_varname(j))) = D.MatrixA(i,j);
						//newD.MatrixB(ha.get_index(it->get()->get_varname(i)), ha.get_index(it->get()->get_varname(j))) = D.MatrixB(i,j);

					}

					else if(newD.MatrixA(ha.get_index(it_ha->get()->get_varname(i)), ha.get_index(it_ha->get()->get_varname(j))) != 0 && D.MatrixA(i,j) == 0)
					{

					}
					else
					{
						std::cout<<"Found conflict"<<endl;
						break;
					}
					newD.MatrixB(ha.get_index(it_ha->get()->get_varname(i)), ha.get_index(it_ha->get()->get_varname(j))) = D.MatrixB(i,j);

				}
//				std::cout<<"D C value: "<<D.C[i]<<endl;
//				std::cout<<"newD C value: "<<ha.get_index(it_ha->get()->get_varname(i))<<endl;
				if(newD.C[ha.get_index(it_ha->get()->get_varname(i))] == D.C[i])
				{

				}

				else if(newD.C[ha.get_index(it_ha->get()->get_varname(i))] == 0 && D.C[i] != 0)
				{
					newD.C[ha.get_index(it_ha->get()->get_varname(i))] = D.C[i];
				}

				else if(newD.C[ha.get_index(it_ha->get()->get_varname(i))] != 0 && D.C[i] == 0)
				{
					//newD.C[ha.get_index(it->get()->get_varname(i))] == D.C[i];
				}

				else
				{
					std::cout<<"Found conflict in column vector"<<endl;
					break;
				}

			}
			else
			{
				for(unsigned int j = 0; j<D.MatrixA.size2(); j++)
				{
//					std::cout<<"Non shared variable: "<<it_ha->get()->get_varname(i)<<endl;
//					std::cout<<ha.get_index(it_ha->get()->get_varname(i))<<endl;
					newD.MatrixA(ha.get_index(it_ha->get()->get_varname(i)), ha.get_index(it_ha->get()->get_varname(j))) = D.MatrixA(i,j);
					newD.MatrixB(ha.get_index(it_ha->get()->get_varname(i)), ha.get_index(it_ha->get()->get_varname(j))) = D.MatrixB(i,j);

				}
				newD.C[ha.get_index(it_ha->get()->get_varname(i))] = D.C[i];
			}

		}
		first_inv_poly = it_loc->get()->getInvariant()->getCoeffMatrix();
		first_inv_coeff = it_loc->get()->getInvariant()->getColumnVector();
		std::vector<double> constraint;
		constraint.resize(ha.getDimension());
		for(unsigned int i=0; i<first_inv_poly.size1();i++)
		{
			for (unsigned int j=0; j<first_inv_poly.size2(); j++)
			{
				constraint.at(ha.get_index(it_ha->get()->get_varname(j)))= first_inv_poly(i,j);
			}
			inv->setMoreConstraints(constraint, first_inv_coeff[i]);
		}
		it_ha++;
	}
	std::cout<<loc_name<<endl;
//	std::cout<<newD.MatrixA;
//	std::cout<<newD.MatrixB;
//	std::cout << "The constant vector:\n";
//	for(unsigned int i=0;i<newD.C.size();i++)
//		std::cout <<newD.C[i] << "\n";
//	newD.U->printPoly();
//	inv->printPoly();
	new_loc->setInvariant(inv);
	new_loc->setName(loc_name);
	new_loc->setSystemDynamics(newD);
	make_shared_labels(locs);
//	std::cout<<"In the DS\n";
	for(std::list<composeLabels>::iterator cl = ComposedLabels.begin(); cl != ComposedLabels.end(); cl++)
	{
//		std::cout<<cl->labels<<endl;
//		for(std::list<location::const_ptr>::iterator it = cl->next_locations.begin(); it != cl->next_locations.end(); it++)
//		{
//			std::cout<<it->get()->getLocId()<<endl;
//		}
//		std::cout<<"size of locations: "<<cl->next_locations.size()<<"\t"<< cl->Ha_Ptrs.size()<<endl;
		assert(cl->next_locations.size() == cl->Ha_Ptrs.size());
//		location::ptr next_location = marged_locations(cl->next_locations, cl->Ha_Ptrs);
		transition::ptr trans = find_trans(cl->labels, locs, cl->Ha_Ptrs);
		trans->setSourceLocationId(new_loc->getLocId());
		trans->getGuard()->printPoly();
		std::cout<<"label: "<< trans->getLabel()<<endl;
		std::cout<<"t_id: "<<trans->getTransitionId()<<endl;
		new_loc->addOutGoingTransition(trans);
//		location::ptr next_loc = marged_next_loc(cl->next_locations, cl->Ha_Ptrs);
//		trans->setDestinationLocationId(next_loc->getLocId());
	}
	//std::list<transition::ptr> all_trans = find_trans(locs);
	ha.addLocation(new_loc);
	return new_loc;
}

location::ptr composition::marged_next_loc(std::list<location::const_ptr> locs, std::list<hybrid_automata::ptr> Ha_Ptrs)
{
	std::cout<<"In marged next location...."<<endl;
		location::ptr new_loc = location::ptr(new location());
//		static int loc_id;
		new_loc->setLocId(++loc_id);
		std::string loc_name;
		Dynamics D;
		Dynamics newD;
		newD.MatrixA.resize(ha.getDimension(), ha.getDimension(), false);
		newD.MatrixB.resize(ha.getDimension(), ha.getDimension(), false);
		newD.C.resize(ha.getDimension());
	//	newD.U = polytope::ptr(new polytope());
	//	newD.U->printPoly();
		math::matrix<double> first_inv_poly;
		std::vector<double> first_inv_coeff;
		polytope::ptr inv = polytope::ptr(new polytope()); //universe
		newD.MatrixA.clear();
		newD.MatrixB.clear();
	//	std::cout << "The constant vector:\n";
	//	for(unsigned int i=0;i<newD.C.size();i++)
	//		std::cout << newD.C[i] << "\n";
	//	std::cout<<newD.MatrixA;
		std::list<hybrid_automata::ptr>::iterator it_ha = Ha_Ptrs.begin();
		for(std::list<location::const_ptr>::iterator it_loc = locs.begin(); it_loc != locs.end(); it_loc++)
		{
			std::cout<<"Destinations loc marging..."<<endl;
			std::cout<<it_loc->get()->getName()<<endl;
			D = it_loc->get()->getSystemDynamics();
	//		std::cout<<D.MatrixA;
	//		std::cout<<D.MatrixB;
	//		std::cout << "The constant vector:\n";
	//		for(unsigned int i=0;i<D.C.size();i++)
	//			std::cout << D.C[i] << "\n";
	//		//std::vector<double> constraint;
	//		//constraint.resize(ha.getDimension());
	//		std::cout<<"U matrix:"<<endl;
	//		D.U->printPoly();
			math::matrix<double> old_mt = D.U->getCoeffMatrix();
			if(it_loc == locs.begin())
			{
				loc_name = it_loc->get()->getName();
				math::matrix<double> mt(D.U->getCoeffMatrix().size1(), ha.getDimension());
				std::vector<double> cv(D.U->getCoeffMatrix().size1());
				mt.clear();
				//std::cout<<mt<<endl;
				for(unsigned int i=0; i < old_mt.size1(); i++)
				{
					for(unsigned int j=0; j < old_mt.size2(); j++)
					{
						mt(i,j) = old_mt(i,j);
					}
					cv[i]= D.U->getColumnVector()[i];
				}
				newD.U = polytope::ptr(new polytope(mt, cv, 1));
	//			newD.U->printPoly();
			}
			else
			{
				loc_name = loc_name + "_" + it_loc->get()->getName();
				std::vector<double> u_const, next_u_const;
				u_const.resize(ha.getDimension());
				next_u_const.resize(ha.getDimension());
				//std::cout<<"Old mt: "<<old_mt<<endl;
		//		math::matrix<double> mt(2*ha.getDimension(), ha.getDimension());
		//		std::vector<double> cv(ha.getDimension());
		//		mt.clear();
		//		newD.U = polytope::ptr(new polytope(mt, cv, 1));
		//		newD.U->printPoly();
				for(unsigned int i=0; i < D.U->getCoeffMatrix().size2(); i++)
				{

					if(is_shared(it_ha->get()->get_varname(i)))
					{
						//std::cout<<it->get()->get_varname(i)<<endl;
						//std::cout<<newD.C[ha.get_index(it->get()->get_varname(i))]<<"\t"<<D.U->getColumnVector()[2*i]<<endl;
	//					for(unsigned int i=0;i<newD.C.size();i++)
	//						std::cout << newD.C[i] << "\n";
	//					D.U->printPoly();
						if(newD.C[ha.get_index(it_ha->get()->get_varname(i))] == D.U->getColumnVector()[2*i])
						{

						}
						else if(newD.C[ha.get_index(it_ha->get()->get_varname(i))] == 0 && D.U->getColumnVector()[(2*i)] != 0)
						{
							for(unsigned int j=0; j < old_mt.size2(); j++)
							{
								u_const.at(ha.get_index(it_ha->get()->get_varname(j))) = old_mt((2*i),j);
								next_u_const.at(ha.get_index(it_ha->get()->get_varname(j))) = old_mt((2*i)+1,j);
							}
							newD.U->setMoreConstraints(u_const, D.U->getColumnVector()[(2*i)]);
							newD.U->setMoreConstraints(next_u_const, D.U->getColumnVector()[(2*i)+1]);

						}
						else if(newD.C[ha.get_index(it_ha->get()->get_varname(i))] != 0 && D.U->getColumnVector()[2*i] == 0)
						{

						}
						else if(newD.C[ha.get_index(it_ha->get()->get_varname(i))] == 0 && D.U->getColumnVector()[2*i] == 0)
						{

						}
						else
						{
							std::cout<<"Found conflict in column vector while making set U."<<endl;
							break;
						}

					}
					else
					{
						for(unsigned int j=0; j < old_mt.size2(); j++)
						{
							u_const.at(ha.get_index(it_ha->get()->get_varname(j))) = old_mt((2*i),j);
							next_u_const.at(ha.get_index(it_ha->get()->get_varname(j))) = old_mt((2*i)+1,j);
						}
						newD.U->setMoreConstraints(u_const, D.U->getColumnVector()[(2*i)]);
						newD.U->setMoreConstraints(next_u_const, D.U->getColumnVector()[(2*i)+1]);

					}

				}
			}
			for(unsigned int i = 0; i < D.MatrixA.size1(); i++)
			{

				if(is_shared(it_ha->get()->get_varname(i)))
				{
	//				std::cout<<"shared variable :"<<it_ha->get()->get_varname(i)<<endl;
					for(unsigned int j = 0; j<D.MatrixA.size2(); j++)
					{
	//					std::cout<<D.MatrixA(i,j)<<endl;
	//					std::cout<<newD.MatrixA(ha.get_index(it_ha->get()->get_varname(i)), ha.get_index(it_ha->get()->get_varname(j)))<<endl;
						if(newD.MatrixA(ha.get_index(it_ha->get()->get_varname(i)), ha.get_index(it_ha->get()->get_varname(j))) == D.MatrixA(i,j))
						{
							//std::cout<<"Dimension same"<<endl;
						}

						else if(newD.MatrixA(ha.get_index(it_ha->get()->get_varname(i)), ha.get_index(it_ha->get()->get_varname(j))) == 0 && D.MatrixA(i,j) != 0)
						{
							//Update Dynamics to the new value.
							newD.MatrixA(ha.get_index(it_ha->get()->get_varname(i)), ha.get_index(it_ha->get()->get_varname(j))) = D.MatrixA(i,j);
							//newD.MatrixB(ha.get_index(it->get()->get_varname(i)), ha.get_index(it->get()->get_varname(j))) = D.MatrixB(i,j);

						}

						else if(newD.MatrixA(ha.get_index(it_ha->get()->get_varname(i)), ha.get_index(it_ha->get()->get_varname(j))) != 0 && D.MatrixA(i,j) == 0)
						{

						}
						else
						{
							std::cout<<"Found conflict"<<endl;
							break;
						}
						newD.MatrixB(ha.get_index(it_ha->get()->get_varname(i)), ha.get_index(it_ha->get()->get_varname(j))) = D.MatrixB(i,j);

					}
	//				std::cout<<"D C value: "<<D.C[i]<<endl;
	//				std::cout<<"newD C value: "<<ha.get_index(it_ha->get()->get_varname(i))<<endl;
					if(newD.C[ha.get_index(it_ha->get()->get_varname(i))] == D.C[i])
					{

					}

					else if(newD.C[ha.get_index(it_ha->get()->get_varname(i))] == 0 && D.C[i] != 0)
					{
						newD.C[ha.get_index(it_ha->get()->get_varname(i))] = D.C[i];
					}

					else if(newD.C[ha.get_index(it_ha->get()->get_varname(i))] != 0 && D.C[i] == 0)
					{
						//newD.C[ha.get_index(it->get()->get_varname(i))] == D.C[i];
					}

					else
					{
						std::cout<<"Found conflict in column vector"<<endl;
						break;
					}

				}
				else
				{
					for(unsigned int j = 0; j<D.MatrixA.size2(); j++)
					{
	//					std::cout<<"Non shared variable: "<<it_ha->get()->get_varname(i)<<endl;
	//					std::cout<<ha.get_index(it_ha->get()->get_varname(i))<<endl;
						newD.MatrixA(ha.get_index(it_ha->get()->get_varname(i)), ha.get_index(it_ha->get()->get_varname(j))) = D.MatrixA(i,j);
						newD.MatrixB(ha.get_index(it_ha->get()->get_varname(i)), ha.get_index(it_ha->get()->get_varname(j))) = D.MatrixB(i,j);

					}
					newD.C[ha.get_index(it_ha->get()->get_varname(i))] = D.C[i];
				}

			}
			first_inv_poly = it_loc->get()->getInvariant()->getCoeffMatrix();
			first_inv_coeff = it_loc->get()->getInvariant()->getColumnVector();
			std::vector<double> constraint;
			constraint.resize(ha.getDimension());
			for(unsigned int i=0; i<first_inv_poly.size1();i++)
			{
				for (unsigned int j=0; j<first_inv_poly.size2(); j++)
				{
					constraint.at(ha.get_index(it_ha->get()->get_varname(j)))= first_inv_poly(i,j);
				}
				inv->setMoreConstraints(constraint, first_inv_coeff[i]);
			}
			it_ha++;
		}
		std::cout<<loc_name<<endl;
	//	std::cout<<newD.MatrixA;
	//	std::cout<<newD.MatrixB;
	//	std::cout << "The constant vector:\n";
	//	for(unsigned int i=0;i<newD.C.size();i++)
	//		std::cout <<newD.C[i] << "\n";
	//	newD.U->printPoly();
	//	inv->printPoly();
		new_loc->setInvariant(inv);
		new_loc->setName(loc_name);
		new_loc->setSystemDynamics(newD);
		ha.addLocation(new_loc);
		return new_loc;
}

void composition::compose()
{
	//std::cout<<"what's the problem"<<endl;
	make_composed_dimension();
	ha.print_var_index_map();
//	polytope::ptr initial_set = polytope::ptr(new polytope()); // universe;
//	location::ptr loc = location::ptr(new location());
	polytope::ptr inv = polytope::ptr(new polytope()); //universe
	//print_mapper();
	polytope::const_ptr initial_set = init.begin()->get()->getInitialSet();

//	build_initial(initial_set);
	initial_set->printPoly();
	//build_location(loc, inv);
	std::list<location::const_ptr> locations;
	std::list<hybrid_automata::ptr>::const_iterator ha_comps = ha_ptrs.begin();
	for (std::list<initial_state::ptr>::const_iterator it = init_comps.begin(); it != init_comps.end(); it++)
	{
//		(*it)->getInitialSet()->printPoly();
//		std::cout<<it->get()->getLocationId()<<endl;
		locations.push_back(ha_comps->get()->getLocation(it->get()->getLocationId()));
		ha_comps++;
	}
	location::ptr loc = marged_locations(locations, ha_ptrs);
	std::cout<<"location_id :"<<loc->getLocId()<<endl;
	loc->getInvariant()->printPoly();



//	ha.addLocation(loc);
	//ha.print_var_index_map();
	//make a const pointer type initial for calling reachability routine.
//	polytope::const_ptr initial_set1;
//	initial_set1 = initial_set;
//	set_comp_params(ha, newInit, user_ops, reach_params, forbidden_s);

	reach_on_the_fly.setReachParameter(ha, init, reach_params, 1, forbidden_s, user_ops);


	template_polyhedra::ptr flowpipe = template_polyhedra::ptr(new template_polyhedra());
	flowpipe = postC_fbinterpol(reach_params.Iterations, loc->getSystemDynamics(), initial_set, reach_params,
					loc->getInvariant(), loc->getInvariantExist());
//	std::cout<<flowpipe->getTotalIterations()<<endl;
	//flowpipe->getPolytope(0)->printPoly();
	std::cout<<"Flowpipe Computed!!"<<std::endl;
	region reach_region;
	discrete_set d;
	//std::cout<<"loc_Id: "<<loc->getLocId()<<endl;
	d.insert_element(loc->getLocId());
	symbolic_states::ptr symb = symbolic_states::ptr(new symbolic_states(d,flowpipe));
	reach_region.push_back(symb);
	symb->setInitialPolytope(initial_set);
	symb->setParentPtrSymbolicState(init.begin()->get()->getParentPtrSymbolicState()); //keeps track of parent pointer to symbolic_states
	symb->setTransitionId(init.begin()->get()->getTransitionId()); //keeps track of originating transition_ID
	show(reach_region, user_ops);
	std::cout<<"Before checking the transitions..."<<std::endl;
	for (std::list<transition::ptr>::const_iterator t = loc->getOutGoingTransitions().begin(); t != loc->getOutGoingTransitions().end(); t++)
	{
		// get each destination_location_id and push into the pwl.waiting_list
		int transition_id = (*t)->getTransitionId();
		std::cout<<"trans id: "<<transition_id<<endl;
		location::const_ptr current_destination;
		Assign current_assignment;
		polytope::const_ptr guard_polytope;
		discrete_set ds;
		current_destination = ha.getLocation((*t)->getDestinationLocationId());
		string locName = current_destination->getName();
		std::cout<<"Destination locname: "<<locName<<endl;
		std::list<polytope::ptr> polys; // list of template hull of flowpipe-guard intersections.
		guard_polytope = (*t)->getGuard();
		guard_polytope->printPoly();
		polytope::const_ptr inv = loc->getInvariant();
		inv->printPoly();


//		std::cout<<"I have to make the destination location. (pending...)"<<endl;
//		std::cout<<it->get()->getSourceLocationId()<<"\t";
//		std::cout<<it->get()->getLabel()<<"\t";
//		std::cout<<it->get()->getTransitionId()<<"\t";
//		it->get()->getGuard()->printPoly();
//		std::cout<<it->get()->getAssignT().Map<<endl;
//		for(unsigned int i=0;i<it->get()->getAssignT().b.size();i++)
//			std::cout << it->get()->getAssignT().b[i] << "\n";
//		std::cout<<it->get()->getDestinationLocationId()<<endl;

		polytope::const_ptr trans_guard = t->get()->getGuard();
		std::string postd_aggregation = user_ops.getSetAggregation();
//		std::list<polytope::ptr> polys;
//		polytope::const_ptr inv = loc->getInvariant();

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

		if (polys.size() == 0)
		{
			std::cout<<"guard not satisfied"<<endl;
			continue;
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

		Assign& t_map = t->get()->getAssignT();
		polytope::ptr shift_polytope;

		if (t_map.Map.isInvertible()) {
			shift_polytope = post_assign_exact(g_flowpipe_intersect, t_map.Map, t_map.b);
		} else {
			shift_polytope = post_assign_approx_deterministic(g_flowpipe_intersect,
					t_map.Map, t_map.b, reach_params.Directions,1);
		}
		// The newShifted must satisfy the destination location invariant
		if (current_destination->getInvariant()!=NULL) { // ASSUMPTION IS THAT NULL INV=> UNIVERSE INV
			shift_polytope = shift_polytope->GetPolytope_Intersection(current_destination->getInvariant());
		}
		shift_polytope->printPoly();
		if(shift_polytope->getIsEmpty()){
			std::cout<<"New polytope in the next target location is empty."<<endl;
			continue; // test for other initial regions
		}

	}



}



std::list<symbolic_states::ptr> composition::onTheFly_Reachability()
{
	reach_on_the_fly.setReachParameter(*(ha_ptr), init, reach_params, 1, forbidden_s, user_ops);
	ha_ptr->print_var_index_map();
	region Reachability_Region;
	template_polyhedra::ptr reach_region;
	unsigned int bfslevel = 0;
	std::list<int> queue; // data structure to keep track of the number of transitions
	pwlist pw_list; //list of initial states

	for (auto i : init)	{
		pw_list.WaitingList_insert(i);
		queue.push_back(bfslevel); //insert at REAR, first Location
	}

	bool safety_violation = false;

	polytope::const_ptr initial_polytope;

	unsigned int num_flowpipe_computed=0;

	std::list<location::const_ptr> locations;
	std::list<hybrid_automata::ptr>::const_iterator ha_comps = ha_ptrs.begin();
	for (std::list<initial_state::ptr>::const_iterator it = init_comps.begin(); it != init_comps.end(); it++)
	{
//		(*it)->getInitialSet()->printPoly();
//		std::cout<<it->get()->getLocationId()<<endl;
		locations.push_back(ha_comps->get()->getLocation(it->get()->getLocationId()));
		ha_comps++;
	}
	location::ptr loc = marged_locations(locations, ha_ptrs);
	//ha_ptr->addLocation(loc);

	while(!pw_list.isEmpty_WaitingList())
	{
		initial_state::ptr U;
		U = pw_list.WaitingList_delete_front();
		bfslevel = queue.front();
		queue.pop_front();

		if (bfslevel > user_ops.get_bfs_level())
			break; //stopping since the number of jumps exceeds the bound

		int location_id;
		location_id = U->getLocationId();
		std::cout<<"Location Id: "<<location_id<<std::endl;
		location::const_ptr current_location;
		current_location = ha.getLocation(location_id);
		discrete_set discrete_state;
		discrete_state.insert_element(location_id); //creating discrete_state

		initial_polytope = U->getInitialSet();
		reach_params.X0 = initial_polytope;
//		symbolic_states::ptr S = symbolic_states::ptr(new symbolic_states());
//		S->setDiscreteSet(discrete_state);
//		S->setInitialPolytope(initial_polytope);
//		S->setParentPtrSymbolicState(U->getParentPtrSymbolicState()); //keeps track of parent pointer to symbolic_states
//		S->setTransitionId(U->getTransitionId()); //keeps track of originating transition_ID

		pw_list.PassedList_insert(U);
		template_polyhedra::ptr flowpipe = template_polyhedra::ptr(new template_polyhedra());
			flowpipe = postC_fbinterpol(reach_params.Iterations, current_location->getSystemDynamics(), initial_polytope, reach_params,
					current_location->getInvariant(), current_location->getInvariantExist());

		discrete_set d;
		d.insert_element(loc->getLocId());
		symbolic_states::ptr symb = symbolic_states::ptr(new symbolic_states(d,flowpipe));
		Reachability_Region.push_back(symb);
		symb->setInitialPolytope(initial_polytope);
		symb->setParentPtrSymbolicState(init.begin()->get()->getParentPtrSymbolicState()); //keeps track of parent pointer to symbolic_states
		symb->setTransitionId(init.begin()->get()->getTransitionId()); //keeps track of originating transition_ID
//		show(Reachability_Region,user_ops);
		for(auto t : loc->getOutGoingTransitions())
		{
			int transition_id = t->getTransitionId();
			std::cout<<"trans id: "<<transition_id<<endl;
			location::const_ptr current_destination;
			Assign current_assignment;
			polytope::const_ptr guard_polytope;
			discrete_set ds;
//			current_destination = ha.getLocation(t->getDestinationLocationId());
//			string locName = current_destination->getName();
//			current_destination->getInvariant()->printPoly();
//			std::cout<<"Destination locname: "<<locName<<endl;
//			std::cout<<"Transition Label: "<<t->getLabel()<<std::endl;
//			for(auto cl : ComposedLabels)
//			{
//				if(boost::iequals(cl.labels,t->getLabel()))
//				{
//					std::cout<<"Label: "<<cl.labels<<std::endl;
//					for(auto it : cl.next_locations)
//						std::cout<<it->getName()<<std::endl;
//				}
//			}
			std::list<polytope::ptr> polys; // list of template hull of flowpipe-guard intersections.
			guard_polytope = t->getGuard();
			guard_polytope->printPoly();
			polytope::const_ptr inv = loc->getInvariant();
			inv->printPoly();
			polytope::const_ptr trans_guard = t->getGuard();
			std::string postd_aggregation = user_ops.getSetAggregation();
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

			if (polys.size() == 0)
			{
				std::cout<<"guard not satisfied"<<endl;
				continue;
			}
			for(auto cl : ComposedLabels)
			{
				if(boost::iequals(cl.labels,t->getLabel()))
				{
					std::cout<<"Label: "<<cl.labels<<std::endl;
					for(auto it : cl.next_locations)
						std::cout<<it->getName()<<std::endl;
					location::ptr next_loc = marged_next_loc(cl.next_locations, cl.Ha_Ptrs);
					t->setDestinationLocationId(next_loc->getLocId());
				}
			}
			current_destination = ha.getLocation(t->getDestinationLocationId());
			string locName = current_destination->getName();
			current_destination->getInvariant()->printPoly();
			std::cout<<"Destination locname: "<<locName<<endl;
			std::cout<<"Transition Label: "<<t->getLabel()<<std::endl;
			for(auto cl : ComposedLabels)
			{
				if(boost::iequals(cl.labels,t->getLabel()))
				{
					std::cout<<"Label: "<<cl.labels<<std::endl;
					for(auto it : cl.next_locations)
						std::cout<<it->getName()<<std::endl;
				}
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

			Assign& t_map = t->getAssignT();
			polytope::ptr shift_polytope;

			if (t_map.Map.isInvertible()) {
				shift_polytope = post_assign_exact(g_flowpipe_intersect, t_map.Map, t_map.b);
			} else {
				shift_polytope = post_assign_approx_deterministic(g_flowpipe_intersect,
						t_map.Map, t_map.b, reach_params.Directions,1);
			}
			// The newShifted must satisfy the destination location invariant
			if (current_destination->getInvariant()!=NULL) { // ASSUMPTION IS THAT NULL INV=> UNIVERSE INV
				shift_polytope = shift_polytope->GetPolytope_Intersection(current_destination->getInvariant());
			}
			shift_polytope->printPoly();
			if(shift_polytope->getIsEmpty()){
				std::cout<<"New polytope in the next target location is empty."<<endl;
				continue; // test for other initial regions
			}
			initial_state::ptr newState = initial_state::ptr(new initial_state(t->getDestinationLocationId(), shift_polytope));
			newState->setTransitionId(transition_id); // keeps track of the transition_ID
			newState->setParentPtrSymbolicState(symb);
			pw_list.WaitingList_insert(newState);
			queue.push_back(bfslevel+1);

		}
	}
	return Reachability_Region;

}
