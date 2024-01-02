/*
 * sf_directions.h
 *
 *  Created on: 01-May-2014
 *      Author: gurung
 */

#ifndef SF_DIRECTIONS_H_
#define SF_DIRECTIONS_H_

#define UNSOLVED 0

#include <vector>
#include <application/structures.h>
#include <list>
#include "omp.h"

/**
 * Function to get the axis directions
 * N is the dimension of the system
 */
std::vector<std::vector<double> > generate_axis_directions(unsigned int N);

//typedef std::vector<std::vector<double> > direction_list;
std::vector<std::vector<double> > get_octagonal_directions(unsigned int dim);

#endif /* SF_DIRECTIONS_H_ */
