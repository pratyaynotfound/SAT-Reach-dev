#include <core/continuous/approxModel/approxModel.h>

double approx_model::rho_X0(const std::vector<double>& l)
{
	if(my_X0->getIsEmpty())
	{
		return 0;
	}
	return lpX->Compute_LLP(l);
}

double approx_model::rho_U(const std::vector<double>& l)
{
	if(my_U->getIsEmpty())
	{
		return 0;
	}
	std::vector<double> trans_l;
	this->my_B_trans.mult_vector(l,trans_l);

	return lpU->Compute_LLP(trans_l);
}
