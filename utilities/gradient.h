/*
 * gradient.h
 *
 *  Created on: 18-Aug-2016
 *      Author: rajarshi
 */

#ifndef GRADIENT_H_
#define GRADIENT_H_

#include <core/continuous/polytope/polytope.h>
#include <vector>
#include <cmath>
#include "core/math/analyticODESol.h"

/*
 * Computes derivative of vector (x) to polytope distance d w.r.t x.
 * The definition of point to polytope distance is as defined in the
 * polytope.cpp class implementation
 */
std::vector<double> dist_grad(std::vector<double> x, polytope::const_ptr I);

#endif /* GRADIENT_H_ */
