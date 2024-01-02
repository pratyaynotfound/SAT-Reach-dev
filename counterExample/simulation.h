/*
 * simulation.h
 *
 *  Created on: 12-Feb-2016
 *      Author: Rajarshi
 */

#ifndef SIMULATION_H_
#define SIMULATION_H_

#include <application/structures.h>
#include <vector>
#include <utility>
#include <cvode/cvode.h>
#include <nvector/nvector_serial.h>
#include <cvode/cvode.h> // prototypes for CVODE fcts., consts.
#include <nvector/nvector_serial.h>  // access to serial N_Vector
#include <sunmatrix/sunmatrix_dense.h> // access to dense SUNMatrix
#include <sunlinsol/sunlinsol_dense.h> // access to dense SUNLinearSolver
#include <cvode/cvode_direct.h> // access to CVDls interface
#include <sundials/sundials_types.h>  // defs. of realtype, sunindextype
#include <sundials/sundials_math.h>  // contains the macros ABS, SUNSQR, EXP

#include <sundials/sundials_dense.h>
#include <sundials/sundials_types.h>
#include <omp.h>
#include "core/hybridAutomata/hybridAutomata.h"
#include "core/hybridAutomata/location.h"
#include "core/hybridAutomata/transition.h"
#include "utilities/gradient.h"

/*
 * A structure to return the last point in Invariant and the
 * time of crossing the invariant
 */
struct bound_sim
{
	std::vector<double> v;
	double cross_over_time;
};
/**
 * A structure to hold the ha location and the start point for simulation
 */
typedef struct sim_start_point
{
	location::ptr locptr;
	std::vector<double> start_point;
	double cross_over_time; // the time when this new start point was created. The remaining time to simulate is Time_horizon-cross_over_time

}sim_start_point;

/**
 * A structure to hold the attributes of a ha transition which is eligible to trigger during a simulation.
 */
typedef struct eligibleTransition
{
	polytope::ptr inv_g_intersection; //Guard with Invariant intersected region, bloated with time_step
	transition::ptr trans;// the pointer to the transition
}eligibleTransition;

/**
 * This class provides methods to simulate an ODE equation for a given
 * initial value.
 */
class simulation : public var_to_index_map {

	/** The number of discrete samples to be computed
	 * in solving the ODE, in order to get the simulation. */

	unsigned int N;

	/**
	 * The dimension of the ODE system.
	 */
	unsigned int dimension;
	Dynamics D;
	double reltol;
	double abstol;
	string filename;
	unsigned int x1; // the first output dimension for plotting.
	unsigned int x2; // the second output dimension for plotting.
	typedef std::pair<double,std::vector<double>> trace_point; // A trace point is a (time,vector) pair, specifying the system state at the time instance.
	std::list<trace_point> sim_trace; // The d.s. storing the computed simulation trace

	double time_step; //the supplied time-step

public:
	typedef boost::shared_ptr<simulation> ptr;
	/** To store the end point of the simulation trajectory and its distance
	 *  from a given polytope.
	 */
//	typedef std::pair<std::vector<double>, double> simD;

	simulation();
	simulation(unsigned int dim, unsigned int steps, Dynamics Dyn, double rel_tol=1e-8, double abs_tol=1e-8){
		dimension = dim;
		N = steps;
		reltol = rel_tol;
		abstol = abs_tol;
		D = Dyn;
		filename=std::string();
		// default plotting dimension
		time_step = 0.001; //Default time step
		x1 = 0; // The default plotting of this dimension
		x2 = 1; // The default plotting of this dimension

	}
	virtual ~simulation();
	/**
	 * Prints the computed simulation trace to a file.
	 * The parameter string should be the absolute path.
	 * The simulation shall be printed to this file if filename is not
	 * empty
	 */
	void print_trace_to_outfile(std::string s);

	/**
	 * Returns the dimension of the simulation object.
	 */
	unsigned int get_system_dimension()
	{
		return dimension;
	}
	/**
	 * sets the projection dimensions to output the simulation points
	 * in a file
	 */
	void set_out_dimension(unsigned int i, unsigned int j){
		x1 = i;
		x2 = j;
	}
	/**
	 * Set the simulation time_step. Overrides the step parameter, when set.
	 */
	void set_time_step(double delta)
	{
		this->time_step = delta;
	}
	/**
	 * Get the simulation time_step. Overrides the step parameter, when set.
	 */
	double get_time_step(double delta)
	{
		return this->time_step;
	}

	/**
	 * Generates a simulation trace for time duration, starting at start_time.
	 * The initial state is given by the first parameter. The last point of the
	 * simulation trace is returned.
	 */
	std::vector<double> simulate(std::vector<double>, double time);

	/**
	 * Returns the computed simulation trace. This should be called after calling one of the
	 * simulate methods of the class where the member is initialized.
	 */
	std::list< trace_point> get_sim_trace(){
		return sim_trace;
	}

	/**
	 * Generates a simulation trace for time duration, starting at start_time.
	 * The time instant, within the simulation time, when the polytope I is
	 * violated by the trace is returned and with the first simulation point
	 * that violated I, as a struct object. status is set to false if invariant
	 * is violated. If the Inv violation distance is < tol, then it is tolerated.
	 * Returns the last point of the simulation trace.
	 */
	bound_sim bounded_simulation(std::vector<double>, double time, polytope::const_ptr I, bool &status, double tol);

	/**
	 * Simulate and also compute the distance of the trajectory with a polytope,
	 * gradient of the distance of the trace to the given polytope invariant I w.r.t
	 * the trace end point and the gradient of the distance of the trace
	 * to the invariant w.r.t time. Returns the last point of the simulation trace.
	 */
	std::vector<double> metric_simulate(std::vector<double> x, double time, double& distance, polytope::ptr Inv,
			std::vector<double>& grad);

	/**
	 * Simulate the location dynamics mentioned in the start_point (symbolic initial point), for the initial point passed in the parameter.
	 * Returns a collection of valid new start points for further simulation of ha.
	 */
	std::vector<sim_start_point> simulateHaLocation(sim_start_point start_point, double start_time,
			double tot_time, hybrid_automata& ha, std::pair<int, polytope::ptr>& forbidden_set,
			bool& safety_status);

	/**
	 * Simulates the Hybrid Automaton from the given start point for the time_duration.
	 * The result is stored in the sim_trace data-structure. The simulation follows "urgent"
	 * semantics of HA. According to this, a transition is taken as soon as the transition
	 * guard constraint is satisfied. A simulation with "max_jumps" transitions is computed.
	 * Returns true when the simulated trajectory is safe, and return false otherwise.
	 */
	bool simulateHa(sim_start_point start_point, double start_time, double tot_time, hybrid_automata& ha,
			std::pair<int, polytope::ptr>& forbidden_set, unsigned int max_jumps);

	/**
	 * Parallel Simulation of Ha with N uniformly distributed random start points from the initial set.
	 * Returns true when the simulated trajectory is safe, and return false otherwise.
	 */
	bool parSimulateHa(unsigned int N, polytope::const_ptr initial_set, double start_time, double tot_time,
			hybrid_automata& ha, std::pair<int, polytope::ptr>& forbidden_set, unsigned int max_jumps);

	/**
	 * Generates N simulation from uniformly distributed start points from the initial set. Monitors how many
	 * of these intersect with the unsafe_region.
	 * Returns the number of trajectory(ies) that intersects with the unsafe region.
	 */
	unsigned int SafetyTestingSimHa(unsigned int N, polytope::ptr initial_set, polytope::ptr unsafe_set,
			double start_time, double tot_time, hybrid_automata& ha, unsigned int max_jumps=10);

	std::vector<sim_start_point> get_start_points(unsigned int n, polytope::const_ptr initialset, location::ptr locptr);
	void Hyperplane_to_Halfspace(math::matrix<double>& M, std::vector<double>& Bounds, std::vector<double> x);

	/**
	 * Returns a vector of start points for initiating simulation from the given location of a ha.
	 * The parameter n is the number of start points to get, the initial_set specifies the region from
	 * where to create the initial points (assumption is initial_set is subsumed by the location invariant) and locptr
	 * is the ha location to start simulations. The initial_set is assumped to be type hyperbox
	 */
};

#endif /* SIMULATION_H_ */
