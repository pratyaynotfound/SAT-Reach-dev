/*
 * HybridAutomata.cpp
 *
 *  Created on: 09-Jul-2014
 *      Author: amit
 */

#include <core/hybridAutomata/hybridAutomata.h>
#include "z3++.h"
#include <fstream>
#include <string>

using namespace std;

hybrid_automata::hybrid_automata() {
	dimension = 0;
}

hybrid_automata::hybrid_automata(std::map<int, location::ptr>& list_locs, location::ptr init_loc,
		int dim) {
	list_locations = list_locs;	//assigning a map to another map
	initial_loc = init_loc;
	dimension = dim;
}

location::ptr hybrid_automata::getInitialLocation() const {
	return initial_loc;
}

void hybrid_automata::addInitialLocation(location::ptr& initLoc) {
	initial_loc = initLoc;
}

void hybrid_automata::setInitialLoc(int loc_id)
{
	initial_loc = list_locations[loc_id];

}

location::const_ptr hybrid_automata::getLocation(int Loc_Id) const {
	assert(list_locations.count(Loc_Id)!=0);
	return list_locations.at(Loc_Id);
}

/* returns the location from the list of locations with locName */
location::const_ptr hybrid_automata::getLocation(string locName) const {

	std::map<int, location::ptr>::const_iterator locMapIter;

	for(locMapIter = list_locations.begin();locMapIter!=list_locations.end(); locMapIter++){
		std::pair<int, location::ptr> map_elem = *locMapIter;
		location::const_ptr l = this->getLocation(map_elem.first);
		string name = l->getName();
		if(locName.compare(name)==0)
			return l;
	}
	throw std::runtime_error("hybrid_automata: getLocation: No location with the asked location name\n");
}

int hybrid_automata::getDimension() const {
	return dimension;
}

void hybrid_automata::setDimension(int dim) {
	this->dimension = dim;
}

void hybrid_automata::addMappedLocationsList(std::map<int, location::ptr>& mapped_location_list){
	list_locations = mapped_location_list;
}
void hybrid_automata::addLocation(location::ptr& loc){
	int key = loc->getLocId();
	list_locations[key] = loc;	//storing the loc with the proper loc_id as the key
}

unsigned int hybrid_automata::satEnumPaths(unsigned int forbidden_loc_id, unsigned int depth)
{

	location::ptr source_ptr = getInitialLocation();
	int u = source_ptr->getLocId();
	unsigned int v = forbidden_loc_id;
	unsigned int bound = depth;    // bound = # locations in a path.

	z3::context c;
	unsigned int count1 = 0;
	
	for (unsigned int k = 0; k < bound; k++)    
	{						

		// INIT Step
		z3::expr exp1 = c.bool_const("exp1");
		string arr = "v" + to_string(u)+"_"+ "0";
		unsigned int l = arr.length();
		char array[l];
		for (unsigned int i = 0 ; i < l; i++)
			array[i] = char(arr[i]);
		z3::expr x = c.bool_const(array);
		exp1 = x;
		for(auto it = list_locations.begin(); it != list_locations.end(); it++)
		{
			arr = "v" + to_string(it->first)+"_"+ "0";
			l = arr.length();
			char array1[l];
			for (unsigned int i = 0 ; i < l; i++)
				array1[i] = char(arr[i]);
			if (it->first != u)
			{
				z3::expr x1 = c.bool_const(array1);
				exp1 = (exp1 && !(x1));
			}
		}				   					
		z3::solver s(c);
		s.add(exp1);

		// NEXT Step

		z3::expr exp2 = c.bool_const("exp2");
		z3::expr exp22 = c.bool_const("exp22");
		if(k == 0)   
		{
			for (unsigned int i =0; i <= k; i++)   
			{
			
				for (auto it = list_locations.begin(); it != list_locations.end(); it++)
				{
					auto neighbor_nodes = it->second->getOutGoingTransitions();			
					arr = "v"+ to_string(it->first)+"_"+ to_string(i);
					l = arr.length();
					char array2[l];
					for (unsigned int j = 0 ; j < l; j++)
						array2[j] = char(arr[j]);
					z3::expr x2 = c.bool_const(array2);
					exp2 = x2;
					z3::expr exp2a = c.bool_const("exp2a");
					unsigned int count = 1;
					for (auto it2 = neighbor_nodes.begin(); it2 != neighbor_nodes.end(); it2++)
					{
						transition::ptr trans_ptr = *(it2);
						unsigned int loc_id = trans_ptr->getDestinationLocationId();
						arr = "v" + to_string(loc_id) +"_"+ to_string(i+1);
						l = arr.length();
						char array2[l];
						for (unsigned int j = 0 ; j < l; j++)
							array2[j] = char(arr[j]);
						z3::expr x2 = c.bool_const(array2);
						if(count == 1)
						{
							exp2a = x2;
						}
						if(count >= 2)
						{	
							exp2a = (exp2a || x2);
						}
						count++;
					}
					exp22 = implies(exp2, exp2a);
					s.add(exp22);
				}
			} 						
		}
		if(k >= 1)   
		{
			for (unsigned int i =0; i <= k-1; i++)   
			{		
				for (auto it = list_locations.begin(); it != list_locations.end(); it++)
				{
					auto neighbor_nodes = it->second->getOutGoingTransitions();			
					arr = "v"+ to_string(it->first)+"_"+ to_string(i);
					l = arr.length();
					char array2[l];
					for (unsigned int j = 0 ; j < l; j++)
						array2[j] = char(arr[j]);
					z3::expr x2 = c.bool_const(array2);
					exp2 = x2;
					z3::expr exp2a = c.bool_const("exp2a");
					unsigned int count = 1;
					for (auto it2 = neighbor_nodes.begin(); it2 != neighbor_nodes.end(); it2++)
					{
						transition::ptr trans_ptr = *(it2);
						unsigned int loc_id = trans_ptr->getDestinationLocationId();
						arr = "v" + to_string(loc_id) +"_"+ to_string(i+1);
						l = arr.length();
						char array2[l];
						for (unsigned int j = 0 ; j < l; j++)
							array2[j] = char(arr[j]);
						z3::expr x2 = c.bool_const(array2);
						if(count == 1)
						{
							exp2a = x2;
						}
						if(count >= 2)
						{	
							exp2a = (exp2a || x2);
						}
						count++;
					}
					exp22 = implies(exp2, exp2a);
					s.add(exp22);
				}
			} 	
		}						

		//EXCLUDE
		z3::expr exp3 = c.bool_const("exp3");
		z3::expr exp33 = c.bool_const("exp33");
		for (unsigned int i =0; i <= k; i++)
		{
			for(auto it1 = list_locations.begin(); it1 != list_locations.end(); it1++)
			{
				string arr = "v" + to_string(it1->first)+"_" + to_string(i);
				unsigned int l = arr.length();
				char array[l];
				for (unsigned int ii = 0 ; ii < l; ii++)
					array[ii] = char(arr[ii]);
				z3::expr x = c.bool_const(array);
				exp3 = x;
				z3::expr exp31 = c.bool_const("exp31");
				unsigned int count = 0;
				for(auto it2 = list_locations.begin(); it2 != list_locations.end(); it2++)
				{
					arr = "v" + to_string(it2->first)+"_"+ to_string(i);
					l = arr.length();
					char array1[l];
					for (unsigned int ii = 0 ; ii < l; ii++)
						array1[ii] = char(arr[ii]);
					if (it2->first != it1->first)
					{
						z3::expr x1 = c.bool_const(array1);
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
				s.add(exp33);
			}
		}						//End of Exclude Constraint.

		//TARGET
		z3::expr exp4 = c.bool_const("exp4");
		arr = "v" + to_string(v)+"_" + to_string(k);
		l = arr.length();
		char array13[l];
		for (unsigned int i = 0 ; i < l; i++)
			array13[i] = char(arr[i]);
		z3::expr x13 = c.bool_const(array13);
		exp4 = x13;						
		s.add(exp4);
							

		
		//Negation
		int count = 1;
		while(true)
		{
			if (s.check() == z3::sat)
			{	
				z3::model m = s.get_model();
				unsigned int w = m.size();
				z3::expr exp = c.bool_const("exp");
				int co = 0;
				for (unsigned int i = 0; i < w; i++)
				{
					z3::func_decl v1 = m[i];
					assert(v1.arity() == 0);
					arr = v1.name().str();
					l = arr.length();
					char array16[l];
					for (unsigned int j = 0 ; j < l; j++)
						array16[j] = char(arr[j]);
					z3::expr x15 = c.bool_const(array16);
					if(co == 0)
					{
						exp = (x15 != m.get_const_interp(v1));
					}
					if(co >= 1)
					{	
						exp = (exp || (x15 != m.get_const_interp(v1)));
					}
					co++;
				}
				s.add(exp);
				count++;
			}
			else
				break;
		}	
	count1 += --count;

	}
	return count1;
}

void hybrid_automata::printPath(vector<int>& path) {
	int size = path.size();
	for (int i = 0; i < size; i++)
		cout << path[i] << " ";
	cout << endl;
}

// For composed HA.
void hybrid_automata::insert_to_dim_map(std::string name, unsigned int val)
{
		this->var_to_dim.insert(std::pair<std::string, unsigned int>(name,val));
}

void hybrid_automata::print_var_dim_map()
{
	unsigned int i = 0;
	std::cout << "The variable to index map is:\n";
	for(std::map<std::string, unsigned int>::iterator it = this->var_to_dim.begin(); it!=this->var_to_dim.end();it++)
	{
		std::cout << "Variable = " << (*it).first;
		std::cout << " Dimension = " << (*it).second << std::endl;
		i++;
	}
}

/* returns the variable name with the given index */
std::string hybrid_automata::get_variable_name(unsigned int index) const
{
	for(std::map<std::string, unsigned int>::const_iterator it = this->var_to_dim.begin(); it!=this->var_to_dim.end();it++){
		if(index == (*it).second)
			return (*it).first;
	}
	throw std::runtime_error("var to index map: get_varname(): variable index not in the map\n");
}

unsigned int hybrid_automata::get_index_name(std::string var_name) const
{
	unsigned int index;
	try{
		for(std::map<std::string, unsigned int>::const_iterator it = this->var_to_dim.begin(); it!=this->var_to_dim.end();it++){
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
