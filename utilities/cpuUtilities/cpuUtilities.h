/*
 * cpu_utilities.h
 *
 *  Created on: 24-Apr-2015
 *    Author: amit
 *    Refrence : http://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process */

#ifndef CPU_UTILITIES_H_
#define CPU_UTILITIES_H_

/*
 * CPU currently used by current process:
 */

void init_cpu_usage();
double getCurrent_ProcessCPU_usage();


#endif /* CPU_UTILITIES_H_ */
