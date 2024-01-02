/*
 * parser class that instantiates a ha object
 * from MDL file.
 *
 * @Author: Rajarshi Ray
 */

#include "parser.h"
#include <fstream>

#include <boost/tokenizer.hpp>
#include <stdio.h>
#include <algorithm>

using namespace boost;

// defined externally
struct yy_buffer_state
{
	FILE *yy_input_file;

	char *yy_ch_buf;		/* input buffer */
	char *yy_buf_pos;		/* current position in input buffer */

	int yy_buf_size;
	int yy_n_chars;
	int yy_is_our_buffer;
	int yy_is_interactive;
	int yy_at_bol;

	int yy_bs_lineno; /**< The line count. */
	int yy_bs_column; /**< The column count. */
	int yy_fill_buffer;
	int yy_buffer_status;
};
	
extern yy_buffer_state* yy_scan_string ( const char *yy_str  );
extern void yy_delete_buffer ( yy_buffer_state* b  );

extern yy_buffer_state* linexp_scan_string ( const char *yy_str  );
extern yy_buffer_state* reset_scan_string ( const char *yy_str  );

extern void linexp_delete_buffer ( yy_buffer_state* b  );
extern void reset_delete_buffer ( yy_buffer_state* b  );

extern void flow_parser(Dynamics& D, hybrid_automata& ha);
extern void linexp_parser(polytope::ptr& p, polytope::ptr& U, Dynamics& D, hybrid_automata& ha);
extern void reset_parser(Assign& r_map, hybrid_automata& ha);


/* parses the variable names and creates the
 * var_to_id list.
 */
void parser::parse_vars(fstream& file)
{
	// read lines until End is read
	string var;
	
	unsigned int dim=0; // dim in ha starts from 0.
	unsigned int dimIp=0; // the number of inputs in the ha.
	unsigned int dimOp=0; // the number of outputs in the ha.

	while(getline(file,var)){
		if(var.compare("#End")==0)
			break;
		if(var.compare("")==0)
			continue;
		//std::cout << "Inserting variable " << var << std::endl;
		if(var[0]=='u' || var[0]=='U') // input variable
		{
			//std::cout << "adding " << var << "to umap" << std::endl;
			this->ha.insert_to_input_map(var,dimIp++);
		}
		else if(var[0]=='y' || var[0]=='Y') // output variable
		{
			this->ha.insert_to_output_map(var,dimOp++);
		}
		else
		{
			this->ha.insert_to_map(var,dim);
			//this->ha.insert_to_dim_map(var, dim);
			dim++;
		}
	}
	this->ha.setDimension(dim);
}

/* parses the ha locations.*/

void parser::parse_loc(fstream& file, location::ptr loc){
	string line;
	Dynamics D;

	while(getline(file,line)){

		if(line.compare("#End")==0)
			break;
	
		// if empty line, then continue
		if(line.compare("")==0)
			continue;

		char_separator<char> sep(":");
		tokenizer<char_separator<char> > tokens(line,sep);
		tokenizer<char_separator<char> >::iterator tok_iter=tokens.begin();

		if((*tok_iter).compare("Locname")==0){
			tok_iter++;
			string tokString = *tok_iter;
			tokString.erase(std::remove_if(tokString.begin(), tokString.end(), ::isspace), tokString.end());
			//std::cout << "Setting location name:" << *tok_iter << std::endl;
			loc->setName(tokString);
		}
		else if((*tok_iter).compare("LocId")==0){
			tok_iter++;
			string tokString = *tok_iter;
			tokString.erase(std::remove_if(tokString.begin(), tokString.end(), ::isspace), tokString.end());
			//std::cout << "Setting location Id:" << *tok_iter << std::endl;
			unsigned int locId = std::stoi(tokString);
			loc->setLocId(locId);
		}
		else if((*tok_iter).compare("Inv")==0){
			tok_iter++;
			string inv_str = *tok_iter;
			polytope::ptr inv, U;

			if(inv_str.compare(" true")==0){
				inv = polytope::ptr(new polytope()); // universe
			}

			else
			{
				//initialise inv	
				inv = polytope::ptr(new polytope()); //universe
				U = polytope::ptr(new polytope()); // universe
				D.isEmptyMatrixT = true;

				if(ha.ymap_size()!=0){ // output vars exist
					std::cout<<"Here";
					unsigned int dimOp = ha.ymap_size();
					unsigned int dim = ha.map_size();
					std::cout<<"Here";

					D.MatrixT = math::matrix<double>(dimOp,dim);
					D.isEmptyMatrixT = false;
					D.MatrixT.clear();
				}
				parse_invariant(inv_str, inv, U, D);
				//debug
				/*std::cout << "The parsed location invariant:\n";
				inv->printPoly();*/
				//debug
				//std::cout << "The parsed output transformation matrix:\n";
				//std::cout << D.MatrixT << std::endl;
				//debug
				/*std::cout << " Printing the U polytope\n";
				U->printPoly();*/

				if( U->getColumnVector().size() == 0) 
					U->setIsEmpty(true);
		
				if(!inv->getIsEmpty())
					loc->setInvariantExist(true);
			}
			D.U = U;
			loc->setInvariant(inv);
		}
		else if((*tok_iter).compare("Flow")==0){
			D.isEmptyMatrixA = true;
			D.isEmptyMatrixB = true;
			D.isEmptyC = true;

			int dim = ha.getDimension();

			D.MatrixA.resize(dim,dim);
			D.MatrixA.clear(); // all elements set to 0
			
			if(ha.umap_size()!=0)
				D.MatrixB.resize(dim,ha.umap_size());
			D.MatrixB.clear(); // all elements set to 0
			parse_flow(file, D);
			/*//debug

			std::cout << "The generated A matrix:\n";
			std::cout << D.MatrixA << std::endl;
			std::cout << "The generated B matrix:\n";
			std::cout << D.MatrixB << std::endl;
			std::cout << "The constant vector:\n";
			for(unsigned int i=0;i<D.C.size();i++)
				std::cout << D.C[i] << "\n";
				*/
			//--

			/* Setting the constants in dynamics to input when U is empty */
			
			if(D.isEmptyMatrixB && !D.isEmptyC){
				//std::cout << "B is empty and C is not empty\n";
				D.MatrixA.matrix_Identity(dim,D.MatrixB); // dim X dim Identity Mat
				D.isEmptyMatrixB = false;
				
				unsigned int row = dim*2;
				unsigned int col = dim;
				math::matrix<double> UMatrix(row, col);
				UMatrix.clear();
				for(unsigned int i=0,j=0;i<row-1;i+=2,j++){
					UMatrix(i,j)=1;
					UMatrix(i+1,j)=-1;	
				}
				std::vector<double> Ubound(row,0);
				for(int i=0,j=0;i<dim;i++,j+=2){
					Ubound[j] = D.C[i];
					Ubound[j+1] = -D.C[i];
				}
				D.U = polytope::ptr(new polytope(UMatrix, Ubound, 1));
				/*std::cout << "Row and Column size of UMatrix resp:" << row  << ", " << col << std::endl;
				std::cout << "Printing U\n";
				D.U->printPoly(); */
			}	
			/* Setting D when U as well as C is non-empty */
			else if(!D.isEmptyMatrixB && !D.isEmptyC)
			{
				//std::cout << "B is not empty and C is not empty\n";
				assert(!D.U->getIsEmpty()); // U is not empty
				math::matrix<double> coeff = D.U->getCoeffMatrix();
				std::vector<double> b = D.U->getColumnVector();
				/*
				std::cout << "size of col vector of U before expanding:" << b.size() << std::endl;
				std::cout << "elements of col vector of U before expanding:\n";
				for(unsigned int i=0;i<b.size();i++)
					std::cout << b[i] << std::endl; */

				unsigned int n = D.C.size();
				unsigned int m = D.U->getSystemDimension();
				assert(n == D.MatrixA.size2());
				assert(m == coeff.size2());
				
				math::matrix<double> newB(n,m+n);
				math::matrix<double> newCoeff(coeff.size1(), coeff.size2()+n);
				newB.clear();
				newCoeff.clear();

				//copy the old B 
				for(unsigned int i=0;i<D.MatrixB.size1();i++)
					for(unsigned int j=0;j<D.MatrixB.size2();j++)
						newB(i,j) = D.MatrixB(i,j);

				//augment n x n identity matrix
				for(unsigned int i=0;i<D.MatrixB.size1();i++)
					newB(i,m+i) = 1;
				D.MatrixB = newB; // new B assigned to dynamics

				// copy the old coeff to newCoeff
				for(unsigned int i=0;i<coeff.size1();i++)
					for(unsigned int j=0;j<coeff.size2();j++)
						newCoeff(i,j) = coeff(i,j);

				D.U->setPolytope(newCoeff,b,1);
				// add constraints from C vector
				std::vector<double> cons(n+m,0);
			
				for(unsigned int i=0;i<n;i++){
						cons[m+i] = 1;
						D.U->setMoreConstraints(cons,D.C[i]);
						cons[m+i] = -1;
						D.U->setMoreConstraints(cons,-D.C[i]);
						cons[m+i]=0;
				}
			/*	// printing the column vector of U
				std::cout << "size of col vector of U after exp:" << D.U->getColumnVector().size() << std::endl;
				std::cout << "elements of col vector are:\n";
				for(unsigned int i=0;i<D.U->getColumnVector().size();i++)
					std::cout << D.U->getColumnVector()[i] << " ";
				std::cout << std::endl;
				std::cout << "modified U to address constant:\n";
				D.U->printPoly();
				std::cout << "Modified B matrix:\n";
				std::cout << D.MatrixB; */
			}

			loc->setSystemDynamics(D);
			//std::cout<<D.MatrixA;
			//std::cout<<D.MatrixB;
			
		}
		else if((*tok_iter).compare("Transition")==0){
			transition::ptr t = transition::ptr(new transition());
			t->setSourceLocationId(loc->getLocId());
			parse_transition(file,t);
			loc->addOutGoingTransition(t);
		}
	}
	
}

/* parses a location invariant */
void parser::parse_invariant(string inv_str, polytope::ptr& Inv, polytope::ptr& U, Dynamics& D)
{
	// erase white-spaces
	inv_str.erase(std::remove_if(inv_str.begin(), inv_str.end(), ::isspace), inv_str.end());

	//std::cout << "Setting inv to:" << inv_str << std::endl;
	typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
	boost::char_separator<char> sep("&;");
	tokenizer tokens(inv_str, sep);

	std::list<std::string> all_args;				
	for (tokenizer::iterator tok_iter = tokens.begin();
			tok_iter != tokens.end(); ++tok_iter) {
		all_args.push_back((std::string) *tok_iter);
	}
	
	string tokString;
	tokenizer::iterator tok_iter;
	for(std::list<std::string>::iterator iter = all_args.begin(); iter!=all_args.end();iter++){
		tokString = *iter;	
		
	 // to satisfy linexp_parser interface.
		tokString+="\n"; // To bypass flex issue - not able to detect eos
		yy_buffer_state* my_string_buffer = linexp_scan_string(tokString.c_str());
		linexp_parser(Inv,U, D, ha); // calls bison parser
		linexp_delete_buffer(my_string_buffer);
	}	
}

/* The master parsing method */
void parser::parse()
{
	std::cout << "Parsing model ... \n";
	fstream mdlFile;
	std::map<int, location::ptr> locs_list;
	
	mdlFile.open(this->model_file,ios::in);
	string line;
	
	if(mdlFile.is_open())
	{
		while(getline(mdlFile,line))
		{
			if(line.compare("#Variables") == 0){
				parse_vars(mdlFile);
			}			
			else if(line.compare("#Location") == 0){
				location::ptr loc = location::ptr(new location());
				parse_loc(mdlFile,loc);
				ha.addLocation(loc); // initialized loc added to the list.
			}
			else if(line.compare("")==0) continue; // empty line.

			else if(line.compare("#Init")==0){
				string init_str;
				do{
					getline(mdlFile,init_str);
				}while(init_str.compare("")==0); // consume white lines
			
				polytope::ptr p = polytope::ptr(new polytope());
				p->setIsEmpty(false);
				p->setIsUniverse(true);
				
				//std::cout << "Initial String for parsing:\n";
				//std::cout << init_str << std::endl;

				int init_locId = 1; // default initial location
				parse_initial(init_str, p, init_locId);
				//debug
				/*std::cout << "Parsed initial polytope:\n";
				p->printPoly();*/
				//---
				// define the initial_state symbolic state
				initial_state::ptr ini_ptr = initial_state::ptr(new initial_state(init_locId, p));
		
				this->ini = ini_ptr;
			}
			else if(line.compare("#Forbidden")==0){
				string forbidden_str;
				do{
					getline(mdlFile,forbidden_str);
				}while(forbidden_str.compare("")==0); // consume white lines

				parse_forbidden(forbidden_str);
			}
			else{
				std::cout << "Error in model file. Expected #Variables, #Location, #Init or #forbidden\n";
				std::cout << "Line seen: " << line << std::endl;
			}
		}
		mdlFile.close();
	}
	ha.setInitialLoc(ini->getLocationId());
	std::cout << "Model parsing complete.\n";
	ha.print_var_index_map();

}

/* parses a list of linear equality expressions string to
 * create a reset map
 */
void parser::parse_reset(fstream& file, Assign& t_reset){

	string line;
	while(getline(file,line)){

		line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());

	
		if(line.compare("#End")==0)
			return;	
	
		// if empty line, then continue
		if(line.compare("")==0)
			continue;
		
		// parse the line
		//std::cout << "reset: String sent to bison for parsing:" << line << std::endl;
		line+="\n"; // To bypass flex issue - not able to detect eos 		
		yy_buffer_state* my_string_buffer = reset_scan_string(line.c_str());
		reset_parser(t_reset, ha); // calls bison parser.
		reset_delete_buffer(my_string_buffer);
	}
}

/* parses a list of consecutive odes to create the dynamics */
void parser::parse_flow(fstream& file, Dynamics& D){
	string line;	
	while(getline(file,line)){

		if(line.compare("#End")==0)
			return;	
		//std::cout << "flow string to parse:" << line << std::endl;
		// if empty line, then continue
		if(line.compare("")==0)
			continue;
		// parse the line
		//std::cout << "String sent to bison for parsing:" << line << std::endl;
		line+="\n"; // To bypass flex issue - not able to detect eos 		
		yy_buffer_state* my_string_buffer = yy_scan_string(line.c_str());
		flow_parser(D, ha); // calls bison parser.
		yy_delete_buffer(my_string_buffer);
	}
	
}

/* parses transition parameters and creates a transition obj */
void parser::parse_transition(fstream& file, transition::ptr& t)
{
	string line;

	static int trans_id;
	t->setTransitionId(++trans_id); // giving a unique
	//std::cout<<"Transition id: "<<t->getTransitionId()<<endl;
	while(getline(file,line)){

		line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());

		if(line.compare("#End")==0)
			return;
		// if empty line, then continue
		if(line.compare("")==0)
			continue;
		
		char_separator<char> sep(":");
		tokenizer<char_separator<char> > tokens(line,sep);
		tokenizer<char_separator<char> >::iterator tok_iter=tokens.begin();

		string param = *tok_iter;
		param.erase(std::remove_if(param.begin(), param.end(), ::isspace), param.end());

		if(param.compare("Dest")==0)
			continue; // dest loc name not stored in transition obj
	
		else if(param.compare("DestId")==0){
			tok_iter++;
			t->setDestinationLocationId(std::atoi((*tok_iter).c_str()));
			//std::cout << "Setting trans dest locId:" << std::atoi((*tok_iter).c_str()) << std::endl;
		}

		else if(param.compare("Label")==0){
			tok_iter++;
			t->setLabel(*tok_iter);
			//std::cout << "Setting trans label:" << *tok_iter << std::endl;
		}

		else if(param.compare("Guard")==0){
			tok_iter++;
			//std::cout << "Setting trans guard:" << *tok_iter << std::endl;
			polytope::ptr g = polytope::ptr(new polytope());
			polytope::ptr u_dummy = polytope::ptr(new polytope());
			Dynamics D_dummy;

			if((*tok_iter).compare("true")==0)
				t->setGuard(g);
			else{
				parse_invariant(*tok_iter, g, u_dummy, D_dummy);// inv and guard are both polytope
				t->setGuard(g);
			}
		}

		else if(param.compare("Reset")==0){
			Assign t_reset;
			//reset map initialized to identity
			t_reset.Map.matrix_Identity(ha.map_size(), t_reset.Map);
			t_reset.b.resize(ha.map_size(),0);
			parse_reset(file, t_reset);
			t->setAssignT(t_reset);
			//debug
			/*
			std::cout << "Reset Map:\n";
			std::cout << t_reset.Map;	
			std::cout << "\nReset b:\n";
			for(unsigned int i=0;i<t_reset.b.size();i++)
				std::cout << t_reset.b[i] << std::endl;*/
			//--
		}
		
	}	
}
/* parses the initial condition string */
void parser::parse_initial(string init_str, polytope::ptr& p, int& init_locId)
{

	//std::cout << "Parsing initial string:" << init_str << std::endl;

	typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
	boost::char_separator<char> sep("&;:");
	tokenizer tokens(init_str, sep);

	std::list<std::string> all_args;
	for (tokenizer::iterator tok_iter = tokens.begin();
			tok_iter != tokens.end(); ++tok_iter) {
		all_args.push_back((std::string) *tok_iter);
	}

	string tokString;
	tokenizer::iterator tok_iter;
	for(std::list<std::string>::iterator iter = all_args.begin(); iter!=all_args.end();iter++){
		tokString = *iter;
		/* check if setting location id*/

		if(tokString.find("loc")!=std::string::npos){

			boost::char_separator<char> sep1("=");
			tokens = tokenizer(tokString, sep1);
			tok_iter = tokens.begin();
			string s(*tok_iter);
			assert(s.compare("loc")==0);

			tok_iter++;
			string locName = *tok_iter;
			init_locId = ha.getLocation(locName)->getLocId();
			continue;
		}
		
		/*---end of loc id setting -----*/
		polytope::ptr U = polytope::ptr(new polytope());
		Dynamics D_dummy;

		// check if constraint is true.
		if(tokString.compare("true") == 0){
			p = polytope::ptr(new polytope()); // set a universe polytope
		}
		else{
			tokString+="\n"; // To bypass flex issue - unable to detect eos
			yy_buffer_state* my_string_buffer = linexp_scan_string(tokString.c_str());
			linexp_parser(p,U,D_dummy, ha); // calls bison parser
			linexp_delete_buffer(my_string_buffer);
		}
	}
}

/* parses the forbidden string */
void parser::parse_forbidden(string forbidden_str)
{
	// erase white-spaces
	forbidden_str.erase(std::remove_if(forbidden_str.begin(), forbidden_str.end(), ::isspace), forbidden_str.end());

	typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
	boost::char_separator<char> sep(",");
	tokenizer tokens(forbidden_str, sep);

	// segregate the forbidden states separated by location
	std::list<std::string> all_states;

	for (tokenizer::iterator tok_iter = tokens.begin();
			tok_iter != tokens.end(); ++tok_iter) {
		all_states.push_back((std::string) *tok_iter);
	}

	string tokString;
	tokenizer::iterator tok_iter;
	for(std::list<std::string>::iterator iter = all_states.begin(); iter!=all_states.end();iter++){
		tokString = *iter;

		// extract the location name
		string locName;
		boost::char_separator<char> delim(":");
		tokens = tokenizer(tokString, delim);
		tokenizer::iterator tok_iter = tokens.begin();
		locName = *tok_iter;

		int locId = ha.getLocation(locName)->getLocId();
		//std::cout << "Location Id = " << locId << std::endl;
		tok_iter++;
		string constraints = *tok_iter;

		// tokenize constraints with logical OR op (|)

		boost::char_separator<char> dl("|");
		tokens = tokenizer(constraints, dl);
		for (tok_iter = tokens.begin();tok_iter != tokens.end(); ++tok_iter) {
			string and_constraints = *tok_iter;
			/*std::cout << "Printing constraint string:\n";
			std::cout << and_constraints << std::endl;*/
			polytope::ptr bad_poly = polytope::ptr(new polytope());
			parse_initial(and_constraints, bad_poly, locId);

			/*std::cout << "forbidden poly is\n";
			bad_poly->printPoly();*/

			forbidden fset;
			fset.first = locId; fset.second = bad_poly;
			this->forbidden_states.push_back(fset);
		}
	}
}

/* returns the parsed ha*/
hybrid_automata parser::getHa()
{
	return this->ha;
}

/* returns the parsed initial state */
initial_state::ptr parser::getInitState()
{
	return this->ini;
}

/* return the parsed forbidden region */
std::vector<forbidden> parser::getForbidden()
{
	return this->forbidden_states;
}
