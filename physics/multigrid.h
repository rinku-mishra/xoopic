//  the multigrid object
#ifndef __MULTIGRID_H
#define __MULTIGRID_H

#include "misc.h"
#include "ovector.h"
#include "psolve.h"
class Grid;

#define NORTH 0
#define SOUTH 1
#define WEST 2
#define EAST 3
#define SOURCE 4

class Multigrid: public PoissonSolve
{
 protected:
  Scalar*** res; 			/* pointer to residual. */
  int size1,size0; 		/* size of soln, {rhs,res} arrays*/
	int BadnessFactor;
  Grid** MultiGrid; 			/* a Grid for each level */
  int level;
	int PeriodicFlagX1, PeriodicFlagX2;
  int tree_size;   /*the number of level in the grid */
	Scalar ****resCoeff, ****GSRBCoeff;
	Scalar ***epsi_local;
  void mgrelax();// Recursive relaxation step.
	void MGSetCoeff(int j, int k, BCTypes type, int level);
	Vector2** GridCoarsen(Grid *, int ratio1, int ratio2);
	int BoltzmannFlag;
	Scalar BoltzmannShift;
	Scalar** oldphi;
#ifdef MGRID_DEBUG
	Scalar **x1_array, **x2_array;
  int *Jmax, *Kmax;
#endif

public:
	Multigrid();// Default constructor.
	Multigrid(Scalar ** epsi, Grid *grid);// Sets up storage for multigrid routine.
  ~Multigrid();// Destructor.
	Scalar errorest(); // compute L^2 error of the residual.
	Scalar*** rhs; 			/* pointer to right-hand side. */
	intVector2* length;		/* length of sides */
	Scalar*** soln; 			/* pointer to solution values. */
	virtual int solve(Scalar **phi, Scalar **rho, int itermax,Scalar tol_test);
  virtual int laplace_solve(Scalar **phi, Scalar **rho, int itermax, Scalar tol_test) {
	  return solve(phi,rho,itermax,tol_test);
  }
	void setBoltzmannFlag(int Flag) {BoltzmannFlag=Flag;};
	void setBoltzmannShift(Scalar Shift) {BoltzmannShift = Shift;};
	virtual void set_coefficient(int j, int k, BCTypes type, Grid *grid);
  virtual BCTypes get_coefficient(int j, int k) { return FREESPACE;};
	Scalar **get_a1_coefficients() { return 0;};  
  Scalar **get_a2_coefficients() { return 0;};
	Scalar **get_b1_coefficients() { return 0;};  
  Scalar **get_b2_coefficients() { return 0;};
	Scalar **get_c1_coefficients() { return 0;};  
  Scalar **get_c2_coefficients() { return 0;};
	Scalar ***get_resCoeff() {return resCoeff[0];};
	void PSolveBoltzCoeff(const Scalar& ne0, const Scalar& qbyT, Scalar** phi, const Scalar& PhiMin);
	Scalar Resid(Scalar** rho, Scalar** phi);
	void init_solve(Grid *grid,Scalar **epsi) {};
};

#endif //__MULTIGRID_H
