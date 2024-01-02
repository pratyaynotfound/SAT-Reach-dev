/*
 * abstractCE.h
 *
 *  Created on: 20-Nov-2021
 *      Author: Atanu
 */

#ifndef ABSTRACTCE_H_
#define ABSTRACTCE_H_


#include <boost/shared_ptr.hpp>
#include <core/math/lpSolver/lpSolver.h>
#include "boost/timer/timer.hpp"
#include "nlopt.hpp"
#include "counterExample/concreteCE.h"
#include "nlpFunctions.h"
#include "core/hybridAutomata/hybridAutomata.h"
#include "core/hybridAutomata/transition.h"
#include <io/ioUtility.h>
#include "core/continuous/polytope/polytope.h"

#define DEBUG
#define VALIDATION
/**
 * This class is a data structure to store the abstract counter-example generated
 * by XSpeed when given an unsafe symbolic state. An abstract counter example
 * is an ordered list of symbolic states starting from an initial and ending in an
 * unsafe region.
 *
 * @author: Rajarshi
 */

extern unsigned int N;
extern unsigned int dim;
extern hybrid_automata* HA;
extern std::vector<int> locIdList;
extern std::list<transition::ptr> transList;
extern polytope::ptr bad_poly;
extern std::list<refinement_point> ref_pts; // a list of invariant violating points to refine the search and obtain a valid trajectory
extern std::vector<std::vector<double> > X0; // list of start points of the trajectory segments. Used only in the NLP-LP mixed program
extern std::list<symbolic_states::ptr> ce_sym_states; // list of CE abstract sym states. Used only in the NLP-LP mixed problem
//extern std::vector<double> start_pos; // the start vectors of opt trajectory from LP solving. Useful in the context of CE_ALGO_TYPE 5

class abstractCE
{

public:
	typedef boost::shared_ptr<abstractCE> ptr;
	typedef boost::shared_ptr<const abstractCE> const_ptr;
	/**empty constructor */
	abstractCE() {
	}
	;
	/* another constructor */
	abstractCE(std::list<symbolic_states::ptr> s_states,
			std::list<transition::ptr> ts, hybrid_automata* h, polytope::ptr fpoly);
	/* destructor */
	~abstractCE() {
	}
	;
	std::list<symbolic_states::ptr> get_CE_sym_states() const {
		return sym_states;
	}
	std::list<transition::ptr> get_CE_transitions() const {
		return trans;
	}

	symbolic_states::const_ptr get_first_symbolic_state() const;

	/**
	 * The semantics assumes that the last abstract_symbolic_state in the list contains the
	 * unsafe polytope
	 */
	symbolic_states::const_ptr get_last_symbolic_state() const;

	/**
	 * Returns the forbidden polytope
	 */
	polytope::ptr get_forbidden_poly(){
		return forbid_poly;
	}
	/**
	 * Returns the i-th symbolic state from the CE
	 */
	symbolic_states::const_ptr get_symbolic_state(unsigned int i) const;

	const unsigned int get_length() const {
		return length;
	}

	void set_length(unsigned int len) {
		length = len;
	}

	void set_sym_states(std::list<symbolic_states::ptr> sym);

	void set_transitions(std::list<transition::ptr> transitions) {
		trans = transitions;
	}
	/**
	 * Sets the reference to the hybrid automaton to which this CE refers.
	 */
	void set_automaton(hybrid_automata* h){
		ha = h;
	}
	/**
	 * Sets the forbidden polytope of this abstract counter example
	 */
	void set_forbid_poly(polytope::ptr fpoly){
		forbid_poly = fpoly;
	}
	hybrid_automata* get_automaton(){
		return ha;
	}
	/**
	 * returns a validated counterexample trace, a trace that satisfies the HA constraints
	 * @algo_type: mention the splicing algorithm:
	 * 	1 specifies to call splicing with FC
	 */
	concreteCE::ptr get_validated_CE(double tolerance, std::string& alg_type);
	const userOptions& getUserOptions() const;
	void setUserOptions(const userOptions& userOptions);

private:
	/**
	 * The first symbolic state is the initial symbolic state and the last one
	 * is the unsafe symbolic state
	 */
	std::list<symbolic_states::ptr> sym_states;

	/**
	 * The list of transitions taken from the initial symbolic_state to the
	 * final symbolic_state.
	 */
	std::list<transition::ptr> trans;
	/**
	 * length shows how many locations are present in the abstract counter
	 * example.
	 */
	unsigned int length;

	/**
	 * The reference to the automaton to which this is an abstract counter example
	 */
	hybrid_automata* ha;

	/**
	 * The reference to the forbidden polytope given by the user
	 */
	polytope::ptr forbid_poly;


	//kept here for temporary; for easy access
	userOptions user_options;

	/**
	 * Returns an instance of a concrete counter-example from the abstract one, using NLP with flowpipe given constraints
	 */
	concreteCE::ptr gen_concreteCE(double tolerance, const std::list<refinement_point>& refinements);



};




#endif /* ABSTRACTCE_H_ */
