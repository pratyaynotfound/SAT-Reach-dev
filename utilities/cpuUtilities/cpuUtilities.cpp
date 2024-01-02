/*
 * cpu_utilities.cpp
 *
 *  Created on: 24-Apr-2015
 *      Author: amit
 */

#include  <iostream>
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "sys/times.h"
#include <linux/sysctl.h>
#include <sys/param.h>
#include <unistd.h>
#include <sys/types.h>
#include <utilities/cpuUtilities/cpuUtilities.h>
//#include "sys/vtimes.h"
using namespace std;

static clock_t lastCPU, lastSysCPU, lastUserCPU;
static int numProcessors;

void init_cpu_usage() {

#if defined(__unix__)
	FILE* file;
	struct tms timeSample;
	char line[10000];

	lastCPU = times(&timeSample);
	lastSysCPU = timeSample.tms_stime;
	lastUserCPU = timeSample.tms_utime;

	file = fopen("/proc/cpuinfo", "r");
			// OS IS UBUNTU
	while (fgets(line, 128, file)!= NULL) {
	if (strncmp(line, "processor", 9) == 0)
		numProcessors++;
	}
	fclose(file);
#endif

#if defined(__MACH__)
	size_t numProcessors_size = sizeof(numProcessors);
	if(sysctlbyname("hw.logicalcpu", &numProcessors, &numProcessors_size, nullptr, 0)!=0)
					numProcessors--;
#endif

}

double getCurrent_ProcessCPU_usage() {
	struct tms timeSample;
	clock_t now;
	double percent;

	now = times(&timeSample);
	if (now <= lastCPU || timeSample.tms_stime < lastSysCPU
			|| timeSample.tms_utime < lastUserCPU) {
		//Overflow detection. Just skip this value.
		percent = -1.0;
	}
	else {
		percent = (timeSample.tms_stime - lastSysCPU)
				+ (timeSample.tms_utime - lastUserCPU);
		percent /= (now - lastCPU);
		percent /= numProcessors;
		percent *= 100;
	}
	lastCPU = now;
	lastSysCPU = timeSample.tms_stime;
	lastUserCPU = timeSample.tms_utime;

	return percent;
//	std::cout << "\nCpu percentage: \n" << percent;
}
