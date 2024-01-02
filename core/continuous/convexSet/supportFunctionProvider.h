/**
 * Base class to represent a convex set whose support function can be computed. This is an ABSTRACT class.
 * Any convex set whose support function can be computed should be derived from this base class
 * @Rajarshi
 */

#ifndef __SUPPFUNC_PROV__
#define __SUPPFUNC_PROV__

#include <vector>
#include <boost/shared_ptr.hpp>
#include <core/hybridAutomata/varToIndexMap.h>
#include <core/math/lpSolver/lpSolver.h>
#include "../../../core/math/matrix.h"

class supportFunctionProvider : public var_to_index_map
{

public:
	typedef boost::shared_ptr<supportFunctionProvider> ptr;
	typedef boost::shared_ptr<const supportFunctionProvider> const_ptr;

	supportFunctionProvider() {
	}
	;
	virtual ~supportFunctionProvider() {
	}
	;
	/** Returns the dimension of the continuous set */
	virtual unsigned int getSystemDimension() const = 0;
	virtual bool getIsEmpty() const = 0;
	/**
	 * The compute support will be a function of the support function of the initial set
	 * and the input set.
	 */
	virtual double computeSupportFunction(const std::vector<double>& direction,
			lp_solver &lp) const = 0;
	virtual double max_norm(int lp_solver_type,
			unsigned int dim_size) const = 0;
};

#endif
