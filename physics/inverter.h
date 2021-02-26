#ifndef _INVERTER_H
#define _INVERTER_H

// this class and its inherited classes are implemeneted independent 
// of the particular properties of the operator, e.g., independent of
// the particular geometry and dimensionality of the coordinate system,
// the boundary conditions, order of accuracy, etc.

#include "operators.h"
#include "psolve.h"
#include "domain.h"

#define macheps 1e-16

// iterative sparse matrix inversion 
class Inverter : public PoissonSolve {

protected:

  // operator domain
  Domain *d;

  // operator
  Operators *A;

  // solution 
  Vector<Scalar> x;
  
  // rhs of linear system 
  Vector<Scalar> b;

  // iteration tolerance 
  Scalar tol;
  
  // maximum number of iterations allowed 
  int maxIter;

public:

  Inverter(Domain* dom, Operators *op)
  {
	 d = dom;
	 A = op;
	 	
	 x.redimensionalize(A->dim());
	 b.redimensionalize(A->dim());
  }

  virtual ~Inverter() {}

  // *********************************************************
  // begin function definitions required by class PoissonSolve

  int laplace_solve(Scalar** phi, Scalar** minus_rho, int maxIterations, Scalar tolerance) {
	  return solve(phi,minus_rho,maxIterations,tolerance);
  };

  // should rescale Poisson equation here into dimensionless quantities 
  int solve(Scalar** phi, Scalar** minus_rho, int maxIterations, Scalar tolerance)
  {
	 maxIter = maxIterations;
	 tol = tolerance;

	 int I = d->nc1();
	 int J = d->nc2();
	 int j;
	 for (j=0; j<=J; j++)
		for (int i=0; i<=I; i++) {
		  
		  const Grid_point_type type = d->type(i,j);
		  const int gp = d->index(i,j);
		  
		  switch (type)
			 {
			 case INTERIOR:
				x[gp] = phi[i][j];
				b[gp] = -minus_rho[i][j]; // Electrostatic_Operator (Laplacian)
			                             // must be positive definite, thus,
			                             // must solve:  -del^2(phi) = plus_rho
				break;
			 case DIRICHLET:
				x[gp] = phi[i][j];
				b[gp] = phi[i][j];
				break;
			 case NEUMANN:
				// not handled yet
				break;
			 default: {} // EXTERIOR 
			 }

		}

	 invert(x,b,tol,maxIter);
	 for (j=0; j<=J; j++) 
		for (int i=0; i<=I; i++) 
		  if (d->type(i,j)!=EXTERIOR)
			 phi[i][j] = x[d->index(i,j)];
		  
	 return 1;
  }
  
  void init_solve(Grid *grid,Scalar **epsi) {}; 
  void set_coefficient(int j, int k,BCTypes type, Grid *grid){};  
  BCTypes get_coefficient(int j, int k){ return FREESPACE;}; 

  Scalar **get_a1_coefficients() { return 0;};
  Scalar **get_a2_coefficients() { return 0;};
  Scalar **get_b1_coefficients() { return 0;};  
  Scalar **get_b2_coefficients() { return 0;};
  Scalar **get_c1_coefficients() { return 0;};  
  Scalar **get_c2_coefficients() { return 0;};
  Scalar ***get_resCoeff() {return 0;};

  void PSolveBoltzCoeff(const Scalar & ne0, const Scalar & qbyT, Scalar** phi, const Scalar & minphi){};
	Scalar Resid(Scalar** rho, Scalar** phi) {return 0;};

  // end function defintions required by class PoissonSolve
  // ******************************************************

  virtual void invert(Vector<Scalar>&, const Vector<Scalar>&, Scalar, int) = 0;

  void set_initial_solution(Vector<Scalar>& init_sol) { x = init_sol; }
  void set_rhs(Vector<Scalar>& rhs) { b = rhs; }
  void set_tolerance(Scalar tolerance) { tol = tolerance; }
  void set_max_iter(int maxNumIter) { maxIter = maxNumIter; }

  Vector<Scalar> solution() const { return x; }
 
}; 

#endif
