/*
 * userOptions.h
 *
 *  Created on: 09-May-2016
 *      Author: rajarshi
 */

#ifndef USEROPTIONS_H_
#define USEROPTIONS_H_

#include <cstring>
#include <iostream>

class userOptions {
	std::string model_filename; // filename of the automata model .xml file
	std::string config_filename; // filename of the configuration file .cfg
	std::string forbidden_state; // the string of forbidden states description
	std::string out_filename; // The output filename
	std::string output_format_type; // the string of output format description

	std::string set_aggregation; // The aggregation options thull(default), none
	std::string Engine; // The Engine options supp(default), simu
	std::string ce_procedure; // the algorithm choice for searching a counterexample

	unsigned int simu_algo;	//Algorithm choice for running Trajectory Simulation
	unsigned int simu_init_points; //Number of start-points for Simulation Algorithm

	unsigned int output_var_X; // first  dimension for plotting
	unsigned int output_var_Y; // second dimension for plotting
	unsigned int output_var_Z; // third dimension for plotting

	unsigned int model; // name of the pre-defined model to run for reachability.
	unsigned int direction_template; // template used for approximating support functions

	double time_horizon; // time horizon for reachability

	double time_step; // the time step of the support function algorithm
	unsigned int level; // the breadth level in bfs to stop reachability

	unsigned int algo;	//Common arg for all types of Algorithm
	unsigned int total_slice_size;	//total number of partition-size or number of slices
	unsigned int stream_size;	//total number of streams selected for GPU streaming
	bool ce_flag; // This flag is set to true if --CE option is given by the user.
	std::string ce_path; 	// A counter-example to forbidden state will be searched in the specified path, which is a comma separated list of location ids.

public:
	userOptions();
	virtual ~userOptions();

	void set_modelFile(std::string modefile);
	void set_configFile(std::string configfile);
	void set_first_plot_dimension(unsigned int outdim);
	void set_second_plot_dimension(unsigned int outdim);
	void set_third_plot_dimension(unsigned int outdim);
	void set_timeStep(double t);
	void set_timeHorizon(double timeHorizon);
	void set_model(unsigned int m);
	void set_directionTemplate(unsigned int d);
	void set_bfs_level(unsigned int l);
	void set_forbidden_set(std::string);
	void set_algorithm(unsigned int alg);	//assigns the Algorithm selected by the user
	void set_simu_algo(unsigned int alg);	//assigns the simulation Algorithm selected by the user
	void setStreamSize(unsigned int streamSize);
	void setTotalSliceSize(unsigned int totalSliceSize);
	void setOutputFormatType(const std::string& outputFormatType);
	void setOutFilename(const std::string& outFilename);
	void setSetAggregation(const std::string& engine);
	void setEngine(const std::string& engine);
	void setCEProc(const std::string& ceproc);
	void set_simu_init_points(unsigned int sampling_points);
	void set_ce_path(std::string path);
	/* This is the flag to trigger a search to a counter-example trajectory to a specified forbidden state */
	void set_ce_flag(bool value);


	std::string get_configFile() const;
	unsigned int get_third_plot_dimension() const;
	unsigned int get_model() const;
	unsigned int get_directionTemplate() const;
	unsigned int get_bfs_level() const;
	std::string get_forbidden_set() const;
	unsigned int get_first_plot_dimension() const;
	unsigned int get_second_plot_dimension() const;
	std::string get_modelFile() const;
	double get_timeStep() const;
	double get_timeHorizon() const;
	unsigned int get_algorithm() const;	//returns the selected Algorithm
	unsigned int get_simu_algo() const;	//returns the selected simulation Algorithm
	unsigned int getStreamSize() const;
	unsigned int getTotalSliceSize() const;
	const std::string& getOutputFormatType() const;
	const std::string& getOutFilename() const;
	const std::string& getSetAggregation() const;
	const std::string& getEngine() const;
	std::string getCEProc() const;
	unsigned int get_simu_init_points() const;

	/* This is the path to search for a concrete trajectory to a specified forbidden state */
	std::string get_ce_path() const;
	bool get_ce_flag() const;

};

#endif /* USEROPTIONS_H_ */
