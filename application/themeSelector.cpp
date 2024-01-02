/*
 * themeSelector.cpp
 *
 *  Created on: 19-Sept-2021
 *      Author: Atanu
 */

#include <application/themeSelector.h>

themeSelector::themeSelector()
{
	// TODO Auto-generated constructor stub
}
void themeSelector::setUserOps(userOptions &op)
{
	this->userOps = op;
}

void themeSelector::setInit(std::list<initial_state::ptr> &init)
{
	this->init = init;
}

void themeSelector::setReachParams(ReachabilityParameters &params)
{
	this->reach_params = params;
}

void themeSelector::setForbidden(forbidden_states &forbidden)
{
	this->forbidden = forbidden;
}

userOptions &themeSelector::getUserOps()
{
	return this->userOps;
}
hybrid_automata::ptr themeSelector::getHaInstance()
{
	return ha_ptr;
}
std::list<initial_state::ptr> &themeSelector::getInit()
{
	return this->init;
}
ReachabilityParameters &themeSelector::getReachParams()
{
	return this->reach_params;
}
forbidden_states &themeSelector::getForbidden()
{
	return this->forbidden;
}

void themeSelector::selectFal()
{
	// todo: call the path-oriented falsification routine.
	boost::timer::cpu_timer timer;
	unsigned int number_of_times = 1;
	std::cout << "select fal" << endl;
	bmc bmc_fal(ha_ptr, init, forbidden, reach_params, userOps);

	timer.start();
	init_cpu_usage();

	unsigned int safe = bmc_fal.safe();
	timer.stop();
	double cpu_usage = getCurrent_ProcessCPU_usage();
	long mem_usage = getCurrentProcess_PhysicalMemoryUsed();
	print_statistics(timer, cpu_usage, mem_usage, number_of_times, "Bounded Model Checking");

	// printing the first initial polytope in the init_poly file
	polytope::const_ptr init_poly = (*init.begin())->getInitialSet();
	std::cout << userOps.get_first_plot_dimension() << std::endl;
	std::cout << userOps.get_second_plot_dimension() << std::endl;
	init_poly->printPoly();
	init_poly->print2file("./init_poly", userOps.get_first_plot_dimension(), userOps.get_second_plot_dimension());

	if (safe == 1)
		std::cout << "BMC: The model is SAFE" << std::endl;
	else if (safe == 0)
		std::cout << "BMC: The model is UNSAFE" << std::endl;
	else
		std::cout << "BMC: The safety of the model is UNKNOWN" << std::endl;
}

void themeSelector::selectCompose()
{
	// ToDo: Perform composition on the fly.
	std::cout << "In Composition Framework" << endl;
	//	userOps.set_first_plot_dimension(0);
	//	userOps.set_second_plot_dimension(1);
	//	exit(0);
	composition ha_compose(ha_ptr, ha_ptrs, init, init_components, forbidden, reach_params, userOps);
	//	ha_compose.compose();
	auto X = ha_compose.onTheFly_Reachability();
	show(X, userOps);
}
void themeSelector::selectSATbmc()
{
	// todo: call the path-oriented falsification routine.
	boost::timer::cpu_timer timer;
	unsigned int number_of_times = 1;
	std::cout << "select SATbmc" << endl;
	SATbmc SATbmc_fal(ha_ptrs, init, forbidden, reach_params, userOps);

	timer.start();
	init_cpu_usage();

	unsigned int safe = SATbmc_fal.safe();
	timer.stop();
	double cpu_usage = getCurrent_ProcessCPU_usage();
	long mem_usage = getCurrentProcess_PhysicalMemoryUsed();
	print_statistics(timer, cpu_usage, mem_usage, number_of_times, "Bounded Model Checking");

	// printing the first initial polytope in the init_poly file
	polytope::const_ptr init_poly = (*init.begin())->getInitialSet();
	std::cout << userOps.get_first_plot_dimension() << std::endl;
	std::cout << userOps.get_second_plot_dimension() << std::endl;
	init_poly->printPoly();
	init_poly->print2file("./init_poly", userOps.get_first_plot_dimension(), userOps.get_second_plot_dimension());

	if (safe == 1)
		std::cout << "SATbmc: The model is SAFE" << std::endl;
	else if (safe == 0)
		std::cout << "SATbmc: The model is UNSAFE" << std::endl;
	else
		std::cout << "SATbmc: The safety of the model is UNKNOWN" << std::endl;
}

void themeSelector::select()
{
	// select Compositional BMC engine.
	if (boost::algorithm::iequals(userOps.getEngine(), "cbmc") == true)
	{
		selectCompose();
		return;
	}

	// select BMC engine
	if (boost::algorithm::iequals(userOps.getEngine(), "bmc") == true)
	{
		selectFal();
		return;
	}

	// select SATbmc
	if (boost::algorithm::iequals(userOps.getEngine(), "SATbmc") == true)
	{
		selectSATbmc();
		return;
	}
}
themeSelector::~themeSelector()
{

	// TODO Auto-generated destructor stub
}
