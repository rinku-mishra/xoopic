#include "domain.h"
#include "grid.h"
#include "boundary.h"

#ifdef _MSC_VER
#include <iomanip>
#include <iostream>
using std::ostream;
using std::ends;
using std::cout;
using std::cerr;
using std::endl;
#endif

Domain::Domain(Grid* _grid, Scalar l1, Scalar l2, int _nc1, int _nc2, bool periodic1)
  /* throw(Oops) */{
  dx1 = (Scalar) l1/_nc1;
  dx2 = (Scalar) l2/_nc2;

  length_1 = l1;
  length_2 = l2;

  PERIODIC_1 = periodic1;

  if (PERIODIC_1)
	 number_cells_1 = _nc1-1;
  else
	 number_cells_1 = _nc1;
  number_cells_2 = _nc2;

  i=j=number_grid_points = 0;

  grid_point_type = new Grid_point_type *[number_cells_1+1];
  grid_point_index = new int *[number_cells_1+1];
  boundary_normal = new Vector<int> *[number_cells_1+1]/*(2)*/; //BROKEN

  for (i=0; i<number_cells_1+1; i++) {
	 grid_point_type[i] = new Grid_point_type[number_cells_2+1];
	 grid_point_index[i] = new int[number_cells_2+1];
	 boundary_normal[i] = new Vector<int>[number_cells_2+1]/*(2)*/;//BROKEN
  }
  
  grid = _grid;

  try{
    set_up_domain_oopic();
  }
  catch(Oops& oops){
    oops.prepend("Domain::Domain: Error: \n"); //Fields::set_up_inverter
    throw oops;
  }

  //set_up_domain();
}

Domain::~Domain()
{
  for (i=0; i<number_cells_1+1; i++) {
	 delete[] grid_point_type[i];
	 delete[] grid_point_index[i];
	 delete[] boundary_normal[i];
  }
  delete[] grid_point_type;
  delete[] grid_point_index;
  delete[] boundary_normal;
}

void Domain::set_grid_point(const Grid_point_type type, const Vector<int> &v)
{
  if (type!=EXTERIOR)
	 grid_point_index[i][j] = number_grid_points++;
  grid_point_type[i][j] = type;
  boundary_normal[i][j] = v;
}

void Domain::set_up_domain()
{
  for (j=0; j<=number_cells_2; j++)
	 for (i=0; i<=number_cells_1; i++) 
		if (j==0) {
		  set_grid_point(DIRICHLET);
		  //set_grid_point(NEUMANN,Vector<int>(0,1));
		}
		else if ((i==0)&&(!PERIODIC_1)) {
		  set_grid_point(DIRICHLET);
		  //set_grid_point(NEUMANN,Vector<int>(1,0));
		}
		else if ((i==number_cells_1)&&(!PERIODIC_1)) {
		  set_grid_point(DIRICHLET);
		  //set_grid_point(NEUMANN,Vector<int>(-1,0));
		  }
		else if (j==number_cells_2) {
		  set_grid_point(DIRICHLET);
		  //set_grid_point(NEUMANN,Vector<int>(0,-1));
		}
// 		else if (((j>=number_cells_2/4)&&
// 					 (j<=number_cells_2/2)&&
// 					 (i==number_cells_1/4))
// 					||((j>=number_cells_2/4)&&
// 						(j<=number_cells_2/2)&&
// 						(i==number_cells_1/2))
// 					||((i>=number_cells_1/4)&&
// 						(i<=number_cells_1/2)&&
// 						(j==number_cells_2/4))
// 					||((i>=number_cells_1/4)&&
// 						(i<=number_cells_1/2)&&
// 						(j==number_cells_2/2))) 
// 		  set_grid_point(DIRICHLET);
// 		else if ((j>number_cells_2/4)&&(j<number_cells_2/2)&&
// 					(i>number_cells_1/4)&&(i<number_cells_1/2)) 
// 		  set_grid_point(EXTERIOR);
		else 
		  set_grid_point(INTERIOR);
}

void Domain::set_up_domain_oopic()
  /* throw(Oops) */{
  for (j=0; j<=number_cells_2; j++)
	 for (i=0; i<=number_cells_1; i++) {

		Boundary *B = grid->GetNodeBoundary()[i][j];
		
		BCTypes type;
		if (B!=NULL) type = B->getBCType();
		else type = FREESPACE;

		Vector<int> normal(2);

		switch (type)
		  {
		  case FREESPACE:
			 set_grid_point(INTERIOR);
			 break;
		  case CONDUCTING_BOUNDARY:
			 set_grid_point(DIRICHLET);
			 break;
		  case PERIODIC_BOUNDARY:
			 if (grid->getPeriodicFlagX1()) {
				set_grid_point(INTERIOR);
				PERIODIC_1 = 1;
			 }
			 else {
         stringstream ss (stringstream::in | stringstream::out);
         ss<< "Domain::set_up_domain_oopic: Error: \n"<<
				     "X2 periodicity not handled..."
					  << "Easy though to hack...Do it sloth"
					  << "Bailing out ..." 
                 << endl;

				 std::string msg;
         ss >> msg;
         Oops oops(msg);
         throw oops;    // exit()called by Domain::Domain

			 }
			 break;
		  case DIELECTRIC_BOUNDARY:
			 int sign_normal;
			 sign_normal = B->get_normal();
			 if (B->alongx1())
				normal = Vector<int>(0,sign_normal);
			 else if (B->alongx2())
				normal = Vector<int>(sign_normal,0);
			 else {
         stringstream ss (stringstream::in | stringstream::out);
         ss<< "Domain::set_up_domain_oopic: Error: \n"<<
				   "Confused about normal vector..."
				   << "Bailing out ..." 
                 << endl;
				 std::string msg;
         ss >> msg;
         Oops oops(msg);
         throw oops;    // exit()called by Domain::Domain

			 }
			 set_grid_point(NEUMANN,normal); 
			 break;
		  default:
			 {
         stringstream ss (stringstream::in | stringstream::out);
				 ss << type << "type BCType not handled yet"
				     << endl
				     << "Bailing out of Domain::set_up_domain_oopic()" 
					  << endl;
				 std::string msg;
         ss >> msg;
         Oops oops(msg);
         throw oops;    // exit() called by Domain::Domain

			 }
		  }  
		
	 }

}



ostream& operator<<(ostream& output, const Domain& d)
{

#ifdef UNIX
  cout <<"Grid parameters: ";
  output << "(length1=" << d.length_1 << ",length2=" << d.length_2
			<< ",nc1=" << d.number_cells_1 << ",nc2="
			<< d.number_cells_2 << ",PERIODIC1=" << d.PERIODIC_1 << ")"
			<< endl;

  cout << "Grid point data:" << endl;
  for (int j=0; j<=d.number_cells_2; j++)
	 for (int i=0; i<=d.number_cells_1; i++) {

		Grid_point_type type = d.grid_point_type[i][j];
		int gp = d.grid_point_index[i][j];
 		output << "index[" << i << "][" << j << "]=";

 		switch (type)
 		  {
 		  case INTERIOR:
 			 output << gp << "," << ",INTERIOR";
  			 break;
 		  case DIRICHLET:
 			 output << gp << "," << ",DIRICHLET";
 			 break;
 		  case NEUMANN:

 			 output << gp << "," << ",NEUMANN"
 					  << ",normal=" << d.boundary_normal[i][j];

 			 break;
 		  default:
 			 output << ",EXTERIOR)" << endl;
 			 break;
 		  }

		output << endl;

	 }
#endif
  return output;
}







