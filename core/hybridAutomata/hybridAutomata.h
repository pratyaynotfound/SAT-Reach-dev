/*
 * Hybrid_Automata.h
 *
 *  Created on: 09-Jul-2014
 *      Author: amit
 */

#ifndef HYBRID_AUTOMATA_H_
#define HYBRID_AUTOMATA_H_

#include <list>
#include <map>
#include <boost/shared_ptr.hpp>
#include <bits/stdc++.h>
#include "core/hybridAutomata/location.h"
//#include <core/symbolicStates/initialState.h>

using namespace std;

class hybrid_automata : public var_to_index_map {
	std::map<int, location::ptr> list_locations;		//mapping a list of locations based on the key=loc_id
	location::ptr initial_loc;
	int dimension;
	std::map<std::string, unsigned int> var_to_dim;//fix: dimension issues.
public:
	typedef boost::shared_ptr<hybrid_automata> ptr;
	hybrid_automata();

	hybrid_automata(std::map<int, location::ptr>& list_locs, location::ptr init_loc,
			int dim);
	/* returns a boost pointer */
	ptr getSharedPtr(){
		return ptr(this);
	}
	void addInitialLocation(location::ptr& initLoc);

	/* sets the initial location from its id. Silently does nothing
     * if the init_id is not in the id to location map.
	 */
	void setInitialLoc(int init_id);
	
	location::ptr getInitialLocation() const;

	//returns the location from the list of locations with location_ID as the input parameter
	location::const_ptr getLocation(int Loc_ID) const;

	/* returns the location from the list of locations with locName */
	location::const_ptr getLocation(string locName) const;

	void addMappedLocationsList(std::map<int, location::ptr>& mapped_location_list);

	void addLocation(location::ptr& loc);	//inserts location into its correctly mapped key
	int getDimension() const;
	void setDimension(int dimension);

	/*
	 * Returns the total number of Locations in the hybrid automata with ID = 1 to returned size
	 */
	int getTotalLocations() const {
		return list_locations.size();
	}
	
	std::map<int, location::ptr> getAllLocations() const {
		return list_locations;
	}
	
	/**
	 * A sat-based path enumeration procedure.
	 * Returns the number of paths enumerated from src to dst of length bounded by depthBound
	 */
	unsigned int satEnumPaths(unsigned int forbidden_loc_id, unsigned int depth);


	void printPath(std::vector<int>& path);

	//Quick fix: dimesion's issue fixed by storing dimension index in a map for HA ptr.
	void insert_to_dim_map(std::string name, unsigned int val);
	void print_var_dim_map();
	/**
	 * Returns the variable name with the given index
	 */
	std::string get_variable_name(unsigned int index) const;
	/**
	 * Returns the index of the parameter var_name
	 * in the varname to dimension index map
	 */
	unsigned int get_index_name(std::string var_name) const;

};

#endif /* HYBRID_AUTOMATA_H_ */
