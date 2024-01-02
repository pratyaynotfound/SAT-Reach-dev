/*
 * sf_directions.cpp
 *
 *  Created on: 14-Dec-2014
 *      Author: amit
 */

#include <application/sfDirections.h>

std::vector<std::vector<double> > generate_axis_directions(unsigned int N) {
	std::vector<std::vector<double> > directions;
	for (unsigned int i = 0; i < N; i++) {
		std::vector<double> d1(N, 0.0);
		d1[i] = 1;
		directions.push_back(d1);
		std::vector<double> d2(N, 0.0);
		d2[i] = -1;
		directions.push_back(d2);
	}
	return directions;
}

std::vector<std::vector<double> > get_octagonal_directions(unsigned int dim) {
	std::vector<std::vector<double> > mydirs;

	for (unsigned int i = 0; i < dim; i++) {
		for (unsigned int j = i; j < dim; j++) {
			if (i == j) {
				std::vector<double> v1(dim, 0);
				v1[i] = 1;
				mydirs.push_back(v1);
				std::vector<double> v2(dim, 0);
				v2[i] = -1;
				mydirs.push_back(v2);
			} else {

				std::vector<double> v1(dim, 0);
				v1[i] = 1;
				v1[j] = 1;
				mydirs.push_back(v1);

				std::vector<double> v2(dim, 0);
				v2[i] = 1;
				v2[j] = -1;
				mydirs.push_back(v2);

				std::vector<double> v3(dim, 0);
				v3[i] = -1;
				v3[j] = 1;
				mydirs.push_back(v3);

				std::vector<double> v4(dim, 0);
				v4[i] = -1;
				v4[j] = -1;
				mydirs.push_back(v4);
			}
		}
	}
	return mydirs;
}

