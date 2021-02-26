//  the poisson solve base class definition
#ifndef __PSOLVE_H
#define __PSOLVE_H


#include "ovector.h"
#include "boundary.h"


class Grid;

// morgan
class Domain;
class Operators;

class PoissonSolve
{
 public:

	PoissonSolve(Scalar **epsi, Grid *grid);

	// morgan
	PoissonSolve(Domain*,Operators*);

	PoissonSolve();
	 //  initialize the solver
	virtual ~PoissonSolve(){};
	virtual void init_solve(Grid *grid,Scalar **epsi)=0; 

	virtual void set_coefficient(int j, int k,BCTypes type, Grid *grid)=0;  

  virtual BCTypes get_coefficient(int j, int k)=0;		

	// actually do a solve
	virtual int solve(Scalar **x_in, Scalar **s, int itermax, Scalar tol_test=0)=0;

	// actually do a laplace_solve
	// In most cases, just calls solve
	virtual int laplace_solve(Scalar **x_in, Scalar **s, int itermax, Scalar tol_test=0)=0;

	 // tridiagonal matrix solve from Numerical Recipes
   void tridag(Scalar *a, Scalar *b, Scalar *c, Scalar *r, Scalar *utri,
					Scalar *gam, int n);


   //some solvers need to get at coefficients, interface functions:
	virtual Scalar **get_a1_coefficients()=0; 
	virtual Scalar **get_a2_coefficients()=0;
	virtual Scalar **get_b1_coefficients()=0; 
	virtual Scalar **get_b2_coefficients()=0;
	virtual Scalar **get_c1_coefficients()=0; 
	virtual Scalar **get_c2_coefficients()=0;

	virtual Scalar ***get_resCoeff()=0;

	virtual void PSolveBoltzCoeff(const Scalar & ne0, const Scalar & qbyT, Scalar** phi, const Scalar & phiMin)=0;
	
	virtual Scalar Resid(Scalar** rho, Scalar** phi)=0; 

 };

#endif  // __PSOLVE_H
