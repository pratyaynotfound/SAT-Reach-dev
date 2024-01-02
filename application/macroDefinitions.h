/*
 * All_PP_Definition.h
 *
 *  Created on: 11-Dec-2014
 *      Author: amit
 */

#ifndef MACRO_DEFINITION_H_
#define MACRO_DEFINITION_H_

// *********** Various Algorithms ******************

#define FB_INTERPOL	 	1	//postC with fwd-bwd interpolation model.


// *********** Various Hybrid System Model ******************
#define BBALL				1		//Bouncing Ball
#define TBBALL				2		//Timed Bouncing Ball
#define NAVIGATION_1			3		//NAVIGATION Benchmark model - NAV01 (3 x 3)
#define OSCILLATOR			4		//OSCILLATOR model without any Filter


// *********** Different Template Directions ******************
#define BOX			1
#define OCT			2
//#define UNIFORM		//values > 3

#define MAX_ALGO 	8	//Total number of existing Algorithm. This value need to be increased for new Algorithm when implemented


#endif /* MACRO_DEFINITION_H_ */
