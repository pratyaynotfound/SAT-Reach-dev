/*
 * concreteCE.cpp
 *
 *  Created on: 20-Jan-2016
 *      Author: rajarshi
 */

#include <counterExample/concreteCE.h>
#include <counterExample/abstractCE.h>
#include <cassert>


concreteCE::~concreteCE() {
	// TODO Auto-generated destructor stub
}

const concreteCE::traj_segment concreteCE::get_segment(unsigned int i) const {
	assert(i < T.size());
	std::list<traj_segment>::const_iterator it = T.begin();
	for (unsigned int k = 0; k != i; k++) {
		it++;
	}
	return *it;

}
const concreteCE::trans_id concreteCE::getTransitionId(unsigned int i) const {
	assert(i < jumps.size());
	std::list<trans_id>::const_iterator it = jumps.begin();
	for (unsigned int k = 0; k != i; k++) {
		it++;
	}
	return *it;
}

void concreteCE::plot_ce(std::string filename, unsigned int x1, unsigned int x2)
{
	traj_segment seg;
	unsigned int locId;
	sample simulation_sample;
	simulation::ptr sim;
	std::cout << "Inside concrete trace plotter, no. of trajectory segs:" << T.size() << std::endl;
	double steps = 10000;
	// cleaning the old contents of the file first
	std::ofstream myoutfile;
	myoutfile.open(filename.c_str(),std::fstream::out);
	myoutfile.close();

	for(trajectory::iterator it = T.begin(); it!=T.end();it++){
		seg = *it;
		locId = seg.first;
		std::cout << "plotCE: loc id = " << locId << std::endl;
		//debug print
		std::cout << "dwell time in this location = " << seg.second.second << std::endl;
		//---
		simulation_sample = seg.second;
		sim = simulation::ptr(new simulation(simulation_sample.first.size(),steps,ha->getLocation(locId)->getSystemDynamics()));
		sim->set_out_dimension(x1, x2);

		sim->simulate(simulation_sample.first, simulation_sample.second);
		sim->print_trace_to_outfile(filename); // Prints the computed simulated trace to a file
	}

}

bool concreteCE::valid(struct refinement_point& ref_pt, double valid_tol)
{
	traj_segment seg;
	unsigned int locId;
	sample simulation_sample;
	simulation::ptr sim;

	double steps = 100; // defines the precision of validation
	unsigned int seq_no = 0;

	for(trajectory::iterator it = T.begin(); it!=T.end();it++){
		seg = *it;
		locId = seg.first;
		simulation_sample = seg.second;

		polytope::const_ptr Inv;
		Inv = ha->getLocation(locId)->getInvariant();

		sim = simulation::ptr(new simulation(simulation_sample.first.size(),steps,ha->getLocation(locId)->getSystemDynamics()));
		bound_sim b; bool status=true;
		b =  sim->bounded_simulation(simulation_sample.first,simulation_sample.second, Inv, status, valid_tol);

		//debug
		if(!status)
			sim->print_trace_to_outfile("./violating_trajectory");
		//----
		if(!status){
			std::cout << "Inv violated in location id:" << locId << std::endl;
			std::cout << "Violated point is:\n";
			std::cout << b.v[0] << ", " << b.v[1] << std::endl;
			ref_pt.seq_no = seq_no;
			ref_pt.violating_pt = b.v;
			ref_pt.time = b.cross_over_time;
			return false;
		}
		seq_no++;
	}
	return true;
}
