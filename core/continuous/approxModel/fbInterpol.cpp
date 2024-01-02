#include <core/continuous/approxModel/fbInterpol.h>

fb_interpol::~fb_interpol(){
}

fb_interpol::fb_interpol(const math::matrix<double>& my_A, polytope::const_ptr X0, polytope::const_ptr U, const math::matrix<double>& my_B,
		const double delta, unsigned int num_dirs, unsigned int num_iters) : approx_model(my_A, my_B, X0, U, delta)
{
	math::matrix<double> absolute_A;
	my_A.absolute(absolute_A);

	get_phi_2(absolute_A,delta).transpose(transpose_phi_2);

	math::matrix<double> A_square, AsquarePhi;
	my_A.multiply(my_A, A_square);
	A_square.transpose(transpose_A_square);
	
	my_A.transpose(transpose_A);
	this->num_iters = num_iters;
	this->num_directions = num_dirs;

	rho_psi.resize(num_directions); // the size equals num directions

	// initializing the phi list

	my_A.matrix_Identity(my_A.size1(), phi_last);

	my_A.matrix_exponentiation(expAt,delta);
	resMat = expAt;
	expAt.transpose(phi);
	my_transpose_expAt = phi;

	if(num_iters == 1){
		A_square.multiply(expAt, AsquarePhi);
	}
	else{
		A_square.multiply(phi, AsquarePhi);
	}
	
	AsquarePhi.transpose(transpose_AsquarePhi);
	dim = get_X0()->getSystemDimension();
	
	initialize_rho(); // initialize internal data-structures for memoization.

	unsigned int optD = 1;
	//myopt = new nlopt::opt(nlopt::GN_DIRECT, optD); // derivative free
	myopt = new nlopt::opt(nlopt::LD_MMA, optD); // derivative-based

	std::vector<double> ub(1),lb(1);
	lb[0]=0;
	ub[0]=1;
	myopt->set_lower_bounds(lb);
	myopt->set_upper_bounds(ub);

	//unsigned int maxeval = 1000;
	//myopt->set_maxeval(maxeval);
	myopt->set_ftol_abs(1e-10);

	last_iter = 0; d=1;
}

void fb_interpol::initialize_rho()
{
	unsigned int N = 2*dim;
	
	std::vector<double> unit_dir(dim,0), transformed_dir;

	// initialize rho_AU_list
	rho_AU_list.resize(N);	
	
	for(unsigned int i=0;i<dim;i++){
		unit_dir[i] = 1;
		transpose_A.mult_vector(unit_dir,transformed_dir);
		rho_AU_list[i*2] = rho_U(transformed_dir);
		unit_dir[i] = -1;
		transpose_A.mult_vector(unit_dir,transformed_dir);
		rho_AU_list[i*2+1] = rho_U(transformed_dir);

		unit_dir[i] = 0;		
	}
	// initialize rho_AsqrX0_list
	rho_AsqrX0_list.resize(N);

	for(unsigned int i=0;i<dim;i++){
		unit_dir[i] = 1;
		transpose_A_square.mult_vector(unit_dir,transformed_dir);
		rho_AsqrX0_list[2*i] = rho_X0(transformed_dir);
		unit_dir[i] = -1;
		transpose_A_square.mult_vector(unit_dir,transformed_dir);
		rho_AsqrX0_list[2*i+1] = rho_X0(transformed_dir);
		
		unit_dir[i] = 0;
				
	}
	// initialize rho_AsqrPhiX0_list
	rho_AsqrPhiX0_list.resize(N);
	for(unsigned int i=0;i<dim;i++){
		
		unit_dir[i] = 1;
		transpose_AsquarePhi.mult_vector(unit_dir,transformed_dir);
		rho_AsqrPhiX0_list[2*i] = rho_X0(transformed_dir);
		unit_dir[i] = -1;
		transpose_AsquarePhi.mult_vector(unit_dir,transformed_dir);
		rho_AsqrPhiX0_list[2*i+1] = rho_X0(transformed_dir);
		unit_dir[i] = 0;		
	}
	// initialize rho_symhull_AsquareX0_list, rho_symhull_AsquarePhiX0_list, rho_symhull_AU_list
	rho_symhull_AsqrX0_list.resize(N);
	rho_symhull_AsqrPhiX0_list.resize(N);
	rho_symhull_AU_list.resize(N);

	for(unsigned int i=0;i<dim;i++){
		unit_dir[i] = 1;
		transpose_phi_2.mult_vector(unit_dir,transformed_dir);
		rho_symhull_AsqrX0_list[2*i] = rho_symhull_AsquareX0(transformed_dir);
		rho_symhull_AsqrX0_list[2*i+1] = rho_symhull_AsqrX0_list[2*i];
		rho_symhull_AsqrPhiX0_list[2*i] = rho_symhull_AsquarePhiX0(transformed_dir);
		rho_symhull_AsqrPhiX0_list[2*i+1] = rho_symhull_AsqrPhiX0_list[2*i];
		rho_symhull_AU_list[2*i] = rho_symhull_AU(transformed_dir);
		rho_symhull_AU_list[2*i+1] = rho_symhull_AU_list[2*i];
		unit_dir[i] = 0;		
	}
}

double fb_interpol::rho_symhull_AsquareX0(const std::vector<double>& l)
{
	if(get_X0()->getIsEmpty()) return 0;
	
	double res=0;
	for(unsigned int i=0;i<dim;i++){
		
		if(rho_AsqrX0_list[2*i] > rho_AsqrX0_list[2*i+1])
			res+= rho_AsqrX0_list[2*i] * fabs(l[i]);
		else
			res+= rho_AsqrX0_list[2*i+1] * fabs(l[i]);		
	}
	return res;
}

double fb_interpol::rho_omega_plus(const std::vector<double>& l)
{
	if(get_X0()->getIsEmpty()) return 0;
	
	double res=0;
	for(unsigned int i=0;i<dim;i++){
		
		if(rho_symhull_AsqrX0_list[2*i] > rho_symhull_AsqrX0_list[2*i+1])
			res+= rho_symhull_AsqrX0_list[2*i] * fabs(l[i]);
		else
			res+= rho_symhull_AsqrX0_list[2*i+1] * fabs(l[i]);		
	}
	return res;	
}

double fb_interpol::rho_symhull_AsquarePhiX0(const std::vector<double>& l)
{
	if(get_X0()->getIsEmpty()) return 0;
	
	double res=0;
	for(unsigned int i=0;i<dim;i++){
		
		if(rho_AsqrPhiX0_list[2*i] > rho_AsqrPhiX0_list[2*i+1])
			res+= rho_AsqrPhiX0_list[2*i] * fabs(l[i]);
		else
			res+= rho_AsqrPhiX0_list[2*i+1] * fabs(l[i]);		
	}
	return res;	
}

double fb_interpol::rho_omega_minus(const std::vector<double>& l)
{
	if(get_X0()->getIsEmpty()) return 0;

	double res=0;
	for(unsigned int i=0;i<dim;i++){

		if(rho_symhull_AsqrPhiX0_list[2*i] > rho_symhull_AsqrPhiX0_list[2*i+1])
			res+= rho_symhull_AsqrPhiX0_list[2*i] * fabs(l[i]);
		else
			res+= rho_symhull_AsqrPhiX0_list[2*i+1] * fabs(l[i]);		
	}
	return res;
}
/** Computes the support function of the symmetric hull of A.U */
double fb_interpol::rho_symhull_AU(const std::vector<double>& l)
{
	if(get_U()->getIsEmpty()) return 0;
	
	double res=0;
	for(unsigned int i=0;i<dim;i++){
		
		if(rho_AU_list[2*i] > rho_AU_list[2*i+1])
			res+= rho_AU_list[2*i] * fabs(l[i]);
		else
			res+= rho_AU_list[2*i+1] * fabs(l[i]);		
	}
	return res;
}

/**
  * Computes the support function of the intersection between
  * omega_plus and omega_minus w.r.t l.
  */
double fb_interpol::rho_fb_intersection(const std::vector<double>& l, double lambda,
		std::vector<double> rho_omega_plus, std::vector<double> rho_omega_minus, std::vector<double>& grad)
{
	unsigned int dim = get_X0()->getSystemDimension();
	std::vector<double> unit_dir(dim,0);
	
	double res=0;
	for(unsigned int i=0;i<dim;i++){

		double lambda_max = lambda * rho_omega_plus[i];
		double one_minus_lambda_max = (1-lambda)*rho_omega_minus[i];

		// Take the min and multiply with |l_i|
		if(lambda_max < one_minus_lambda_max){
			res+= lambda_max * fabs(l[i]);
			if(!grad.empty())
				grad[0] += rho_omega_plus[i]*fabs(l[i]);
		}
		else{
			res += one_minus_lambda_max * fabs(l[i]);
			if(!grad.empty())
				grad[0] += -fabs(l[i]) * rho_omega_minus[i];
		}
	}
	return res;
}

/** Computes the support function of the symmetric hull of 
  * \phi sum-hull(A.U), i.e. epsilon_psi (CAV'11 notation) w.r.t l. 
  */
double fb_interpol::rho_epsilon_psi(const std::vector<double>& l)
{
	if(get_U()->getIsEmpty()) return 0;
	
	double res=0;
	for(unsigned int i=0;i<dim;i++){
		
		if(rho_symhull_AU_list[2*i] > rho_symhull_AU_list[2*i+1])
			res+= rho_symhull_AU_list[2*i] * fabs(l[i]);
		else
			res+= rho_symhull_AU_list[2*i+1] * fabs(l[i]);		
	}
	return res;
}

//Objective function for solving max opt. problem
double myobjfun(const std::vector<double> &x, std::vector<double> &grad, void *my_func_data)
{
	assert(x.size() == 1);
	struct terms * my_terms = reinterpret_cast<struct terms *>(my_func_data);
	std::vector<double> dir = my_terms->direction;
	double lambda = x[0];
	double res;

	res = (1-lambda) * my_terms->sup_X0;
	res += lambda * my_terms->sup_phiX0;
	res += lambda * my_terms->sup_deltaU;
	res += lambda * lambda * my_terms->sup_epsilon_psi;
	if(!grad.empty()){
		 grad[0] = -my_terms->sup_X0;
		 grad[0] += my_terms->sup_phiX0;
		 grad[0] += my_terms->sup_deltaU;
		 grad[0] += 2*lambda*my_terms->sup_epsilon_psi;
	}
	res += my_terms->fb_interpol_ptr->rho_fb_intersection(dir,lambda,my_terms->rho_omega_plus,my_terms->rho_omega_minus,grad);

	return res;
}

double fb_interpol::first_omega_support(const std::vector<double>& l, double time_step)
{
	double res;
	double sup_X0;
	// This is term1 
	sup_X0 = rho_X0(l);
	// This term to compute rho_(X0) in the direction transpose(expAt).l
	double term2;

	if(time_step != get_delta() || num_iters==1){
		math::matrix<double> trans_expAt;
		get_A().matrix_exponentiation(expAt,time_step);
		expAt.transpose(my_transpose_expAt);
	}
	
	// transform l to transpose(exp^{At}).l
	 
	std::vector<double> transformed_l;
	
	my_transpose_expAt.mult_vector(l,transformed_l);
	term2 = rho_X0(transformed_l);
	
	// This term to compute time_step * rho_U(l)
	double term3 = time_step *  rho_U(l);
	
	// This term to compute rho_{sym-hull(\phi. sym-hull(AU)}
	
	double term5 = rho_epsilon_psi(l);

	// creating a structure to pass on calculated terms to the obj of nlopt.
	terms my_terms;
	my_terms.sup_X0 = sup_X0;
	my_terms.sup_phiX0 = term2;
	my_terms.sup_deltaU = term3;
	my_terms.sup_epsilon_psi = term5;
	my_terms.direction = l;
	my_terms.fb_interpol_ptr = this;

	unsigned int dim = get_X0()->getSystemDimension();
	std::vector<double> unit_dir(dim,0);

	my_terms.rho_omega_plus = std::vector<double>(dim,0);
	my_terms.rho_omega_minus = std::vector<double>(dim,0);

	// some book-keeping
	for(unsigned int i=0;i<dim;i++){
		unit_dir[i] = 1;
		my_terms.rho_omega_plus[i] = rho_omega_plus(unit_dir);
		my_terms.rho_omega_minus[i] = rho_omega_minus(unit_dir);
		unit_dir[i] = 0;
	}



	// Now, we create an nlopt obj for solving the maximization problem
	// over lambda.


	// set the objective function
	std::vector<double> opt_lambda(1);
	myopt->set_max_objective(myobjfun, &my_terms);
	myopt->set_ftol_abs(1e-18);
	myopt->optimize(opt_lambda,res);
	
	return res;
}

double fb_interpol::omega_support(const std::vector<double>& l, unsigned int iter)
{
	double res;
	if(iter != last_iter){
		d=0; // reset direction count to beginning
		if(iter>=2){
			phi_last=phi;
			resMat.multiply(expAt,resMat);
			resMat.transpose(phi);
		}
	}
	// transform l to transpose(exp^{At}).l	 
	std::vector<double> transformed_l(l.size());
	if(iter==0)
		transformed_l = l;
	else
		phi.mult_vector(l,transformed_l);

	double first_omega_sup = first_omega_support(transformed_l, get_delta());

	res = first_omega_sup + psi_support(l,iter) ;
	last_iter=iter; d++;
	return res;
}

/**
  * Implementing with the limitation that it is expected to be called
  * with iters following the sequence 0,1, ..., num_iters.
  */
double fb_interpol::psi_support(const std::vector<double>& l, unsigned int iter)
{
	if(iter == 0) return 0;

	std::vector<double> transformed_l;

	phi_last.mult_vector(l,transformed_l);

	double term1 = get_delta() * rho_U(transformed_l);
	double term2 = rho_epsilon_psi(transformed_l);

	double sup_psi_delta = term1 + term2;

	rho_psi[d] = rho_psi[d] + sup_psi_delta;
	return rho_psi[d];
}
