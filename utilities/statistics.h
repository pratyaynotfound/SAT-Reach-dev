/*
 * statistics.h
 *
 *  Created on: 29-Nov-2019
 *      Author: rajarshi
 */

#ifndef __statistics_h
#define __statistics_h

#include <core/reachability/reachability.h>
#include "boost/timer/timer.hpp"
/**
 * The first argument is the timer for executing an algorithm, storing the total time
 * to run for num_experiments number of times. This method is used to get the average
 * execution time for the algorithm and print the stats on the console.
 *
 * In addition, an interface to print the statistics related to CounterExample
 * generation is included.
 */


void print_statistics(boost::timer::cpu_timer timer, double cpu_usage, long mem_usage, unsigned int num_exps, std::string msg);
/*
 * This function print the statistics of bounded model checking proceddure with SAT.
 *
 */
void print_bmc_ce_statistics(reachability& reachObj, std::list<abstractCE::ptr>& ce_candidates, const userOptions& user_options, std::string msg);

#endif
