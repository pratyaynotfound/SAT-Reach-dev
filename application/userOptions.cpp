/*
 * userOptions.cpp
 *
 *  Created on: 09-May-2016
 *      Author: rajarshi
 */

#include <application/userOptions.h>

userOptions::userOptions() {
	// TODO Auto-generated constructor stub
	model = 0; //no model specified by this value
	direction_template = 0; //default directions, box template
	output_var_X = 0; // default first dimension of plot
	output_var_Y = 1; // default second dimension of plot
	output_var_Z = 0; //default third plot dimension
	simu_init_points = 1; // Number of initial points set to 1 by default.
	ce_flag = false; // Counter-example generation switched-off by default
	ce_path = "all"; // all paths set by default. This implies all possible flowpipe defined paths to a forbidden state will be explored
//	automata_exploration_algorithm = 12; // sequential BFS
//	flow_algorithm = 1;	// SEQ

	stream_size =1;	//default set to 1 streams
}
std::string userOptions::get_modelFile() const
{
	return model_filename;
}
void userOptions::set_modelFile(std::string modelfile)
{
	model_filename = modelfile;
}
std::string userOptions::get_configFile() const
{
	return config_filename;
}
void userOptions::set_configFile(std::string configfile)
{
	config_filename = configfile;
}
unsigned int userOptions::get_first_plot_dimension() const
{
	return output_var_X;
}
void userOptions::set_first_plot_dimension(unsigned int outdim)
{
	output_var_X = outdim;
}
unsigned int userOptions::get_second_plot_dimension() const
{
	return output_var_Y;
}
void userOptions::set_second_plot_dimension(unsigned int outdim)
{
	output_var_Y = outdim;
}

unsigned int userOptions::get_third_plot_dimension() const
{
	return output_var_Z;
}
void userOptions::set_third_plot_dimension(unsigned int outdim)
{
	output_var_Z = outdim;
}

double userOptions::get_timeStep() const
{
	return time_step;
}
void userOptions::set_timeStep(double t)
{
	time_step = t;
}
double userOptions::get_timeHorizon() const
{
	return time_horizon;
}
void userOptions::set_timeHorizon(double timeHorizon)
{
	time_horizon = timeHorizon;
}
unsigned int userOptions::get_model() const
{
	return model;
}
void userOptions::set_model(unsigned int m)
{
	model = m;
}
unsigned int userOptions::get_directionTemplate() const
{
	return direction_template;
}
void userOptions::set_directionTemplate(unsigned int d)
{
	 direction_template = d;
}
unsigned int userOptions::get_bfs_level() const
{
	return level;
}
void userOptions::set_bfs_level(unsigned int l)
{
	level = l;
}
/*unsigned int userOptions::get_flow_algorithm()
{
	return flow_algorithm;
}
void userOptions::set_flow_algorithm(unsigned int alg)
{
	flow_algorithm = alg;
}
unsigned int userOptions::get_automata_exploration_algorithm()
{
	return automata_exploration_algorithm;
}
void userOptions::set_automata_exploration_algorithm(unsigned int exp_alg)
{
	automata_exploration_algorithm = exp_alg;
}*/

std::string userOptions::get_forbidden_set() const
{
	return forbidden_state;
}
//void userOptions::set_forbidden_state(std::__cxx11::string forbid_s){ //creates some other error if use -std=c++11 options
void userOptions::set_forbidden_set(std::string forbid_s){

	forbidden_state = forbid_s;
}
userOptions::~userOptions() {
	// TODO Auto-generated destructor stub
}


unsigned int userOptions::get_algorithm() const {
	return algo;	//returns the selected Algorithm
}

void userOptions::set_algorithm(unsigned int alg) {
	algo =alg;	//assigns the Algorithm selected by the user
}

unsigned int userOptions::get_simu_algo() const {
	return simu_algo;	//returns the selected simulation Algorithm
}

void userOptions::set_simu_algo(unsigned int alg) {
	simu_algo =alg;	//assigns the simulation Algorithm selected by the user
}

unsigned int userOptions::getStreamSize() const {
	return stream_size;
}

void userOptions::setStreamSize(unsigned int streamSize) {
	stream_size = streamSize;
}

unsigned int userOptions::getTotalSliceSize() const {
	return total_slice_size;
}

void userOptions::setTotalSliceSize(unsigned int totalSliceSize) {
	total_slice_size = totalSliceSize;
}

const std::string& userOptions::getOutputFormatType() const {
	return output_format_type;
}

void userOptions::setOutputFormatType(const std::string& outputFormatType) {
	output_format_type = outputFormatType;
}

const std::string& userOptions::getOutFilename() const {
	return out_filename;
}

const std::string& userOptions::getSetAggregation() const {
	return set_aggregation;
}

void userOptions::setSetAggregation(const std::string& setAggregation) {
	set_aggregation = setAggregation;
}

const std::string& userOptions::getEngine() const {
	return Engine;
}

void userOptions::setEngine(const std::string& engine) {
	Engine = engine;
}

void userOptions::setCEProc(const std::string& ceproc) {
	ce_procedure = ceproc;
}

std::string userOptions::getCEProc() const {
	return ce_procedure;
}

unsigned int userOptions::get_simu_init_points() const{
	return simu_init_points;
}
void userOptions::set_simu_init_points(unsigned int sampling_points){
	simu_init_points = sampling_points;
}

void userOptions::setOutFilename(const std::string& outFilename){
	out_filename = outFilename;
}

std::string  userOptions::get_ce_path() const {
	return this->ce_path;
}

void userOptions::set_ce_path(std::string path)
{
	this->ce_path = path;
}

bool userOptions::get_ce_flag() const
{
	return this->ce_flag;
}
void userOptions::set_ce_flag(bool value)
{
	this->ce_flag = value;
}
