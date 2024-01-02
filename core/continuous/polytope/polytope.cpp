
/*
 * polytope.cpp
 *
 *  Created on: 30-Jun-2014
 *      Author: amit
 */

#include <core/continuous/polytope/polytope.h>


using namespace std;

/*
 * to create an empty polytope call the polytope(bool true) constructor
 * to make a polytope non-empty explicit call to setIsEmpty(false) is to be invoked, even if constraints are set to the
 * polytope it will not be converted into non-empty polytope
 * Universe polytope is created by default and is unbounded by default, whenever any constraints are set it automatically
 * becomes bounded universe polytope
 */

polytope::polytope() {

	// The default polytope inequality sign is <=
	InEqualitySign = 1;
	number_facets = 0;
	system_dimension = 0;
	this->IsUniverse = true; //It is a Universe polytope
	this->IsEmpty = false;
}
polytope::polytope(bool empty) {

	//polytope();
	if(empty) // make this an empty polytope
	{
			this->IsEmpty = true;
			this->IsUniverse = false;
	}
}

polytope::polytope(math::matrix<double> coeffMatrix,
		std::vector<double> columnVector, int InEqualitySign) {

	this->number_facets = coeffMatrix.size1();
	this->system_dimension = coeffMatrix.size2();
	this->coeffMatrix = coeffMatrix;
	this->columnVector.resize(this->number_facets);
	this->columnVector = columnVector;
	this->InEqualitySign = InEqualitySign;
	//Since constraints are set So it is not empty and is Universe but 'bounded'
	this->setIsEmpty(false); //Not an empty Polytope
	this->setIsUniverse(false); //Not a universe Polytope and is now a 'Bounded' polytope
	this->IsUniverse = false; // Assuming that the constraints passed as matrix, vector are meaningful
	this->IsEmpty = false; // assuming that the constraints are not unsatiafiable.

}

void polytope::setPolytope(math::matrix<double> coeffMatrix,
	std::vector<double> columnVector, int inEqualitySign) {
	this->setNumberFacets(coeffMatrix.size1());
	this->setSystemDimension(coeffMatrix.size2());
	this->coeffMatrix = coeffMatrix;
	this->columnVector.resize(this->number_facets);
	this->columnVector = columnVector;

	this->InEqualitySign = inEqualitySign;

	this->setIsUniverse(false); //Not a Universe Polytope and is now 'Bounded' polytope

}

void polytope::setIsEmpty(bool empty) {
	this->IsEmpty = empty;
}
bool polytope::getIsEmpty() const {
	return this->IsEmpty;
}
void polytope::setIsUniverse(bool universe) {
	this->IsUniverse = universe;
}
bool polytope::getIsUniverse() const {
	return this->IsUniverse;
}
bool polytope::isBounded() const
{
	if(coeffMatrix.size1()< system_dimension+1)
		return false;
	else
		return true;
	/* Note that the above is an incomplete simple check of boundedness.
	 * A true algorithm to check for boundeded is to be implemented. later.
	 */
}

const math::matrix<double>& polytope::getCoeffMatrix() const {
	return this->coeffMatrix;
}
void polytope::setCoeffMatrix(const math::matrix<double> coeffMatrix) {

	this->setNumberFacets(coeffMatrix.size1());
	this->setSystemDimension(coeffMatrix.size2());

	this->coeffMatrix = coeffMatrix;
	this->setIsUniverse(false); //Not a Universe Polytope and is now 'Bounded' polytope
}
void polytope::setMoreConstraints(std::vector<double> coeff_constraint,
		double bound_value) {
	this->setSystemDimension(coeff_constraint.size());	//or can be obtained from the map_size()
	//this->setSystemDimension(this->map_size());
	this->setIsUniverse(false); //Not a Universe Polytope and is now 'Bounded' polytope
	this->InEqualitySign = 1; // assuming that always less than ineq cons is added.
	// todo: make the impl to accept ineq sign as param or
	// change the func name to add_lt_inequalityCons().

	unsigned int row_size, col_size;
	row_size = this->getCoeffMatrix().size1();
	col_size = this->getCoeffMatrix().size2(); //dimension of the polytope
	if(col_size == 0) // The poly is currently empty
		col_size = coeff_constraint.size();
	else
		assert(col_size == coeff_constraint.size());
	this->coeffMatrix.resize(row_size + 1, col_size, true); //adding one more constraint
	this->columnVector.resize(row_size + 1); //adding one more constraint's bound value
	for (unsigned int i = 0; i < col_size; i++) {
		this->coeffMatrix(row_size, i) = coeff_constraint[i];
	}
	this->columnVector[row_size] = bound_value;
}

void polytope::setMoreConstraints(math::matrix<double> coeff_constraints,
		std::vector<double> bound_values) {
	// check if non-empty matrix passed. if empty, then return by doing nothing
	if(coeff_constraints.size1()==0 || coeff_constraints.size2()==0)
		return;

	this->setSystemDimension(this->map_size());
	this->setIsUniverse(false); //Not a Universe Polytope and is now 'Bounded' polytope

	unsigned int row_size, dim_size, rows_new;
	row_size = this->getCoeffMatrix().size1();
	dim_size = this->getCoeffMatrix().size2(); //dimension of the polytope
	rows_new = coeff_constraints.size1();

	assert(dim_size == coeff_constraints.size2());

	unsigned int new_total_rows = row_size + rows_new;

	this->coeffMatrix.resize(new_total_rows, dim_size, true); //adding more constraints
	this->columnVector.resize(new_total_rows); //adding more constraint's bound values

	for (unsigned int i = 0; i < rows_new; i++) {
		for (unsigned int j = 0; j < dim_size; j++) {
			this->coeffMatrix(row_size + i, j) = coeff_constraints(i, j);
		}
		this->columnVector[row_size + i] = bound_values[i];
	}

}

int polytope::getInEqualitySign() const {
	return InEqualitySign;
}
void polytope::setInEqualitySign(int inEqualitySign) {
	InEqualitySign = inEqualitySign;
}
void polytope::setColumnVector(const std::vector<double> columnVector) {
	this->columnVector.resize(this->number_facets);
	this->columnVector = columnVector;
	this->setNumberFacets(columnVector.size());
}
std::vector<double> polytope::getColumnVector() const {
	return columnVector;
}

double polytope::computeSupportFunction(const std::vector<double>& direction,
		lp_solver &lp) const {

	assert(direction.size()>0);
	double sf;

	if (this->getIsEmpty()){
	//	throw std::runtime_error("\nCompute Support Function called for an Empty Polytope.\n");
		sf = 0; //returns zero for empty polytope
	}
	else if (this->getIsUniverse())
		throw std::runtime_error("\n Cannot Compute Support Function of a Universe (un-constrained) Polytope.\n");
	else{
		sf = lp.Compute_LLP(direction); //since lp has already been created and set
		if (lp.getStatus() == GLP_UNBND){
			throw std::runtime_error("Unbounded solution to an LP problem");
		}
	}					
	return sf;
}
unsigned int polytope::getNumberFacets() const {
	return number_facets;
}

void polytope::setNumberFacets(unsigned int numberFacets) {
	number_facets = numberFacets;
}

unsigned int polytope::getSystemDimension() const {
	return system_dimension;
}

void polytope::setSystemDimension(unsigned int systemDimension) {
	system_dimension = systemDimension;
}

double polytope::max_norm(int lp_solver_type,
		unsigned int dim_for_Max_Norm) const {
	//unsigned int dimension_size = this->system_dimension;
	unsigned int dimension_size = dim_for_Max_Norm;
	double Max_A, sf, Max = 0.0;
	if (this->getIsEmpty())
		sf = 0; //returns zero for empty polytope
	else if (this->getIsUniverse())
		throw("\nUniverse Unbounded Polytope!!!\n");
	else {
		//sf = lp.Compute_LLP(direction);	//since lp has already been created and set with constraints at the time of creation
		std::vector < std::vector<double> > generator_directions; //this vector-vector is used only in this function not affecting the rest of the codes
		//Generator for Positive Directions for example Right and Up
		for (unsigned int i = 0; i < dimension_size; i++) {
			std::vector<double> directions(dimension_size, 0.0);
			directions[i] = 1; //Positive Generators
			generator_directions.push_back(directions);
		}
		//Generator for Negative Directions for example Left and Down
		for (unsigned int i = 0; i < dimension_size; i++) {
			std::vector<double> directions(dimension_size, 0.0);
			directions[i] = -1; //Negative Generators
			generator_directions.push_back(directions);
		}
		int type = lp_solver_type;
		lp_solver lp1(type);
		lp1.setMin_Or_Max(2); //Setting GLP_MAX
		lp1.setConstraints(this->coeffMatrix, this->columnVector,
				this->InEqualitySign);
//Finding the maximum of all Direction : Returns the max element
		for (unsigned int i = 0; i < generator_directions.size(); i++) {
			std::vector<double> each_generator;
			each_generator = generator_directions[i];
			sf = lp1.Compute_LLP(each_generator);
			Max_A = (fabs(sf));
			if (Max_A > Max)
				Max = Max_A;
		}
	}
	return Max;
}

polytope::ptr polytope::GetPolytope_Intersection(polytope::const_ptr gPoly) const {

	assert(gPoly != NULL);

	polytope::ptr res_poly = polytope::ptr(new polytope(*gPoly));

	if(res_poly->IsUniverse)
	{
		return polytope::ptr(new polytope(this->getCoeffMatrix(), this->getColumnVector(), this->getInEqualitySign()));
	}

	if(res_poly->IsEmpty)
		return res_poly; // return empty polytope pointer

	if(this->IsUniverse) // This polytope is universe
		return res_poly; // Intersection of universe polytope with any polytope P results in P

	if(this->IsEmpty) // Intersection of an empty polytope with any polytope will result in an empty polytope.
		return polytope::ptr(new polytope(true));

	math::matrix<double> total_coeffMatrix, m1;
	m1 = this->getCoeffMatrix(); //assigning constant matrix to matrix m1 so that matrix_join function can be called
	m1.matrix_join(res_poly->getCoeffMatrix(), total_coeffMatrix);
	std::vector<double> total_columnVector;
	total_columnVector = vector_join(this->getColumnVector(), res_poly->getColumnVector());

	polytope::ptr newp = polytope::ptr(
			new polytope(total_coeffMatrix, total_columnVector, 1));

	return newp;
}

/*
 * Checks if the calling Polytope intersects with the polytope P2
 * Return True if there is an intersection, otherwise returns False
 */

bool polytope::check_polytope_intersection(polytope::const_ptr p2) const {
	// if the parameter polytope is a universe, then return true
	if(p2->getIsUniverse())
		return true;
	// if the parameter polytope is empty, return false
	if(p2->getIsEmpty())
			return false;

	bool flag = false;
	/*
	 * Process: Add all constraints of P1(the calling polytope object) and P2 to form new constraints
	 * then run lp_solver's TestConstraints function to test if the constraints have No Feasible Solution,
	 *
	 */
	polytope::ptr res = this->GetPolytope_Intersection(p2);

	glpk_lp_solver lp_res;

	lp_res.setConstraints(res->getCoeffMatrix(), res->getColumnVector(), this->InEqualitySign);

	unsigned int checkStatus = lp_res.testConstraints();

	//cout << "\n\nResult of TestConstraints = " << checkStatus << endl;
	//4 for GLP_NOFEAS; 3 for GLP_INFEAS; 6 for solution is unbounded
	if (checkStatus == GLP_INFEAS || checkStatus == GLP_NOFEAS)
		flag = false;
	else
		flag = true;

	return flag;
}

/*
 * Searches the 2D vertices of the polytope between the directions u and v
 */

void polytope::enum_2dVert_restrict(std::vector<double> u,
		std::vector<double> v, int i, int j,
		std::set<std::pair<double, double> >& pts) const {

//	std::cout<<"Entered inside enumerateVertices_restrict()!!\n";
	std::vector<double> sv_u(getSystemDimension(), 0), sv_v(
			getSystemDimension(), 0);
//	std::cout<<"Entered inside enumerateVertices_restrict() 2 !!\n";
	std::vector<double> sv_bisect;
	lp_solver solver(1); // to choose glpk
	solver.setConstraints(getCoeffMatrix(), getColumnVector(),
			getInEqualitySign());
//	std::cout<<"Entered inside enumerateVertices_restrict() 3 !!\n";
	// get the support
	solver.setMin_Or_Max(2);
	computeSupportFunction(u, solver);
//	std::cout<<"Entered inside enumerateVertices_restrict() 4 !!\n";
	sv_u = solver.get_sv();
//	std::cout<<"Entered inside enumerateVertices_restrict() 5 !!\n";
	computeSupportFunction(v, solver);
	sv_v = solver.get_sv();
	// add the sv's to the set of points
	// make a point of 2 dimension point

	std::pair<double, double> p1, p2, p3;
	p1.first = sv_u[i];
	p1.second = sv_u[j];

	//cout << "direction: (" << u[i] << ", " << u[j] << ")" << std::endl;
	//cout << "support vector: (" << sv_u[i] << ", " << sv_u[j] << ")\n";

	p2.first = sv_v[i];
	p2.second = sv_v[j];

	//cout << "direction: (" << v[i] << ", " << v[j] << ")" << std::endl;
	//cout << "support vector: (" << sv_v[i] << ", " << sv_v[j] << ")\n";

	pts.insert(p1);
	pts.insert(p2);

	//get the bisector vector;
	std::vector<double> bisector = bisect_vector(normalize_vector(u),
			normalize_vector(v));
	computeSupportFunction(bisector, solver);
	sv_bisect = solver.get_sv();
	p3.first = sv_bisect[i];
	p3.second = sv_bisect[j];

	if (is_collinear(p1, p2, p3)) {
		return;
	} else {
		//cout << "direction: (" << bisector[i] << ", " << bisector[j] << ")"	<< std::endl;
		//cout << "support vector: (" << sv_bisect[i] << ", " << sv_bisect[j]	<< ")\n";

		pts.insert(p3);
		enum_2dVert_restrict(u, bisector, i, j, pts);
		enum_2dVert_restrict(bisector, v, i, j, pts);
	}
//	std::cout<<"Finished inside enumerateVertices_restrict()!!\n";
}

std::set<std::pair<double, double> > polytope::enumerate_2dVertices(int i,
		int j) const {
	if(this->IsUniverse)
		throw std::runtime_error("Cannot enumerate vertices of universe polytope\n");
	if(this->IsEmpty)
		throw std::runtime_error("Cannot enumerate vertices of an empty polytope\n");

	std::set < std::pair<double, double> > All_vertices;

	//enumerate the vertices in the first quadrant

	std::vector<double> u(getSystemDimension(), 0);
	u[i] = 1;
	std::vector<double> v(getSystemDimension(), 0);
	v[j] = 1;

	enum_2dVert_restrict(u, v, i, j, All_vertices);

	//enumerate vertices in the second quadrant
	u[i] = -1;
	enum_2dVert_restrict(u, v, i, j, All_vertices);

	//enumerate vertices in the third quadrant
	v[j] = -1;
	enum_2dVert_restrict(u, v, i, j, All_vertices);

	//enumerate vertices in the fourth quadrant
	u[i] = 1;
	enum_2dVert_restrict(u, v, i, j, All_vertices);
//	std::cout<<"Finished enumerateVertices()!!\n";
	return All_vertices;
}

math::matrix<double> polytope::get_2dVertices(int dim1, int dim2) const {
	std::set<std::pair<double, double> > set_vertices;
	set_vertices = enumerate_2dVertices(dim1,dim2);
	math::matrix<double> my_vertices;
	my_vertices = sort_vertices(set_vertices);
	return my_vertices;
}

double polytope::point_distance(std::vector<double> v) const{
	if(this->IsUniverse)
		return 0;
	if(this->IsEmpty){
		std::cout << "distance of a point from an empty polytope asked\n";
		return 0;
	}

	math::matrix<double> C = getCoeffMatrix();
	std::vector<double> b = getColumnVector();

	assert(v.size() == C.size2());
	assert(getInEqualitySign()==1);

	double distance = 0;
	double facet_distance = 0;
	double coef_sq_sum = 0;

	for(unsigned int i=0;i<C.size1();i++){
		for(unsigned int j=0;j<C.size2();j++){
			facet_distance += v[j]*C(i,j);
			coef_sq_sum += C(i,j)*C(i,j);
		}
		facet_distance -=b[i];

		if(facet_distance > 1e-10){
			distance += facet_distance/coef_sq_sum;
		}
		coef_sq_sum = 0;
		facet_distance = 0;
	}
	return distance*distance; // square of the distance is returned.

}

void polytope::print2file(std::string fname, unsigned int dim1, unsigned int dim2) const
{
	std::cout<<this->map_size()<<"\t"<<this->map_size()<<std::endl;
	assert(dim1 < this->map_size() && dim2 < this->map_size());
	assert(dim1 >= 0 && dim2 >= 0);
	std::ofstream myfile;
	myfile.open(fname.c_str());
	math::matrix<double> C = get_2dVertices(dim1, dim2);

	for(unsigned int i=0;i<C.size1();i++){
		for(unsigned int j=0;j<C.size2();j++)
			myfile << C(i,j) << " " ;
		myfile << "\n";
	}
	myfile.close();
	std::cout<<"Execution done!"<<std::endl;
}

void polytope::print2StdOut(unsigned int dim1, unsigned int dim2) const
{
	assert(dim1 < this->map_size() && dim2 < this->map_size());
	assert(dim1 >= 0 && dim2 >= 0);

	math::matrix<double> C = get_2dVertices(dim1, dim2);

	for(unsigned int i=0;i<C.size1();i++){
		for(unsigned int j=0;j<C.size2();j++)
			std::cout << C(i,j) << " " ;
		std::cout << "\n";
	}
}
void polytope::printPoly() const{

	std::cout<<this->coeffMatrix;
	std::cout<<"\nVector\n";
	for(unsigned int j=0;j<columnVector.size();j++)
		std::cout << setprecision(12) << this->columnVector[j] << "    " ;
	std::cout << "\n";
}

bool polytope::contains(polytope::const_ptr poly, int lp_solver_type) const {

	assert(this->getCoeffMatrix().size2() == poly->getCoeffMatrix().size2());	//same dimension
	//assert(this->getCoeffMatrix().size1() == poly->getCoeffMatrix().size1());	//same number of constraints
	lp_solver lp1(lp_solver_type), lp2(lp_solver_type);
	lp1.setMin_Or_Max(2); lp2.setMin_Or_Max(2);
	lp1.setConstraints(this->getCoeffMatrix(), this->getColumnVector(),this->getInEqualitySign());
	lp2.setConstraints(poly->getCoeffMatrix(), poly->getColumnVector(),poly->getInEqualitySign());
	double val1, val2;
	bool is_inside=false;
	for (unsigned int i=0;i<this->getCoeffMatrix().size1();i++){
		std::vector<double> dir(this->getCoeffMatrix().size1());
		for (unsigned int j=0;j<this->getCoeffMatrix().size2();j++){
			dir[j]=this->getCoeffMatrix()(i,j);
		}
		val1 = this->getColumnVector()[i];
		val2 = lp2.Compute_LLP(dir);
		if (val1 >= val2)
			is_inside = true;
		else
			is_inside = false;
		if (is_inside==false)
			break;
	}
	return is_inside;
}





/*
 * Reads the format " v1 >=2 & v1 <= 3 & v2 >=2 & v2 <=3"
 * Also, should be able to now read linear constrains of the format " 1*x + 2*y >=10 & 1*x + -1*y <=20"
 */

void string_to_poly(const std::string& bad_state, std::pair<int, polytope::ptr>& f_set)
{
	std::list<std::string> all_args;

	//polytope::ptr p = polytope::ptr(new polytope());
	polytope::ptr p = polytope::ptr(new polytope());
	p->setIsEmpty(false);
	p->setIsUniverse(true);

	typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
	boost::char_separator<char> sep("&");
	tokenizer tokens(bad_state, sep);

	for (tokenizer::iterator tok_iter = tokens.begin();
			tok_iter != tokens.end(); ++tok_iter) {
		all_args.push_back((std::string) *tok_iter);
	}
	
	std::string varname, tokString;
	tokenizer::iterator tok_iter;
	unsigned int i, init_locId = 1; // initial loc Id set to 1 by default
		
	for(std::list<std::string>::iterator iter = all_args.begin(); iter!=all_args.end();iter++){
		tokString = *iter;
		if (tokString.find("<=")!=std::string::npos ){ // less than equal to constraint
			sep = boost::char_separator<char>("<=");
			tokens = tokenizer(tokString,sep);
			tok_iter = tokens.begin(); // tok_iter contains the lhs of a linear exp
			std::list<std::pair<double, std::string> > coeff_var_pairs = linexp_parser(*tok_iter);
			std::vector<double> cons(p->map_size(), 0);
			for(std::list<std::pair<double, std::string> >::iterator it = coeff_var_pairs.begin(); it!=coeff_var_pairs.end();++it){
				double coeff = (*it).first;
				varname = (*it).second;
 				i = p->get_index(varname);
				cons[i] = coeff;
			}
			tok_iter++;
			double bound = std::atof((*tok_iter).c_str());
			p->setMoreConstraints(cons,bound);
		}
		else if(tokString.find(">=")!=std::string::npos){ // greater than equal to constraint
			sep = boost::char_separator<char>(">=");
			tokens = tokenizer(tokString,sep);
			tok_iter = tokens.begin();
			std::list<std::pair<double, std::string> > coeff_var_pairs = linexp_parser(*tok_iter);
			std::vector<double> cons(p->map_size(), 0);
			for(std::list<std::pair<double, std::string> >::iterator it = coeff_var_pairs.begin(); it!=coeff_var_pairs.end();++it){
				double coeff = (*it).first;
				varname = (*it).second;
				i = p->get_index(varname);
				cons[i] = -coeff;
			}
			tok_iter++;
			double bound = std::atof((*tok_iter).c_str());
			p->setMoreConstraints(cons,-bound);
		}
		else if(tokString.find("=")!=std::string::npos){
			/* check if setting location id*/
				
			if(tokString.find("loc")!=std::string::npos){
				
				boost::char_separator<char> sep1("=; ");
				tokens = tokenizer(tokString, sep1);
				tok_iter = tokens.begin();
				assert(*tok_iter == "loc");
				
				tok_iter++;
				init_locId = std::atoi((*tok_iter).c_str());
				continue;
			}
			
			/*---end of loc id setting -----*/

			sep = boost::char_separator<char>("=");
			tokens = tokenizer(tokString,sep);
			tok_iter = tokens.begin(); // tok_iter contains the lhs of a linear exp
			std::list<std::pair<double, std::string> > coeff_var_pairs = linexp_parser(*tok_iter);
			std::vector<double> cons(p->map_size(), 0);
			for(std::list<std::pair<double, std::string> >::iterator it = coeff_var_pairs.begin(); it!=coeff_var_pairs.end();++it){
				double coeff = (*it).first;
				varname = (*it).second;
 				i = p->get_index(varname);
				cons[i] = coeff;
			}
			tok_iter++;
			double bound = std::atof((*tok_iter).c_str());
			p->setMoreConstraints(cons,bound);
			//-----------------------
			sep = boost::char_separator<char>(">=");
			tokens = tokenizer(tokString,sep);
			tok_iter = tokens.begin();
			coeff_var_pairs = linexp_parser(*tok_iter);
			std::vector<double> cons1(p->map_size(), 0);
			for(std::list<std::pair<double, std::string> >::iterator it = coeff_var_pairs.begin(); it!=coeff_var_pairs.end();++it){
				double coeff = (*it).first;
				varname = (*it).second;
				i = p->get_index(varname);
				cons1[i] = -coeff;
			}
			tok_iter++;
			bound = std::atof((*tok_iter).c_str());
			p->setMoreConstraints(cons1,-bound);
		}
		else{
			std::cout << "Error in string to poly conversion\n";
			throw std::runtime_error("forbidden state string improper\n");
		}
	}
	f_set.first = init_locId;
	f_set.second=p;
};

