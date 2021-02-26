//  the Parallel Poisson object.
#ifndef __PARPOI_H
#define __PARPOI_H

// Include the PETSC parallel solver library stuff.
// MUST BE INCLUDED FIRST because of namespace issues with "Scalar"
#define MPICH_SKIP_MPICXX
extern "C" {
#include "vec.h"
#include "mat.h"
#include "sles.h"
}

/* some macros to make life easier */
#define PetscVecSetValue(v,i,va,mode) \
{ int _ierr,_row=i; double _va=va; \
_ierr = VecSetValues(v,1,&_row,&_va,mode);CHKERRQ(_ierr); \
}

#define PetscMatSetValue(v,i,j,va,mode) \
 {int _ierr,_row = i,_col = j; double _va = va; \
 _ierr = MatSetValues(v,1,&_row,1,&_col,&_va,mode);CHKERRQ(_ierr); \
 }


#include "misc.h"
#include "ovector.h"
#include "psolve.h"

#include <oops.h>


class Grid;

class ParallelPoisson : public PoissonSolve
{
  
 protected:
  Scalar **epsi;
  Grid *grid;
  Vec mpiPhi;  //  the parallel phi object used by PETSC.
  Vec mpiRho; //  the parallel rho object used by PETSC.
  Vec mpiepsi;  //  the parallel epsi object used by PETSC.
  Vec mpiNumbering;  // contains the numbering I think things
                     // should have.
  Mat mpiCoef;  //  the matrix defining the coefficients.
  SLES petsc_solver;  // the petsc solver
 public:

  virtual int solve(Scalar **u_in, Scalar **s, int itermax,Scalar tol_test);
  virtual int laplace_solve(Scalar **u_in, Scalar **s, int itermax,Scalar tol_test);

  virtual void set_coefficient(int j, int k, BCTypes type, Grid *grid) /* throw(Oops) */;
  void set_coefficient_xy(int j,int k, BCTypes type, Grid *grid) /* throw(Oops) */;
  void set_coefficient_rz(int j,int k, BCTypes type, Grid *grid);
  virtual BCTypes get_coefficient(int j, int k) { return (BCTypes) 0;};
  void init_solve(Grid *grid, Scalar **epsi);
	Scalar **get_a1_coefficients() {return 0;};
	Scalar **get_a2_coefficients() {return 0;};
   Scalar **get_b1_coefficients() {return 0;};  
   Scalar **get_b2_coefficients() {return 0;};
	Scalar **get_c1_coefficients() {return 0;};
	Scalar **get_c2_coefficients() {return 0;}; 

	Scalar ***get_resCoeff() {return 0;};
  void PSolveBoltzCoeff(const Scalar & ne0, const Scalar & qbyT, Scalar** phi, const Scalar & minphi){};
	Scalar Resid(Scalar** rho, Scalar** phi) {return 0;};
	 
	 
  ParallelPoisson(Scalar **_epsi,Grid *grid)/* throw(Oops) */;
  virtual ~ParallelPoisson();
};

#endif //__PARPOI_H
