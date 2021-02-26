//  the DADI object
#ifndef __DADI_H
#define __DADI_H

#include "misc.h"
#include "ovector.h"
#include "psolve.h"

class Grid;

class Dadi : public PoissonSolve
{
  
 protected:
  /* The actual coefficients of the differencing on the mesh,
	  and the influence of the spatial variation of epsilon:  */
  Scalar **a_x1geom,**b_x1geom,**c_x1geom;  
  Scalar **a_x2geom,**b_x2geom,**c_x2geom;

  /*  The arrays used internal to DADI which contain the coefficients
		for each tridiagonal matrix solution */
  Scalar *a_x1,*b_x1,*c_x1;
  Scalar *a_x2,*b_x2,*c_x2;
  /*  Various copies of the 'answer' we're working toward */
  Scalar **u;
  Scalar **uwork,**ustor,**ustar;
  Scalar *r_x1,*v_x1,*gam_x1;
  Scalar *r_x2,*v_x2,*gam_x2;
  /*  Our fictitious time step */
  Scalar del_t0;
  /*  epsilon at the grid locations */
  Scalar **epsi;
  /*  The size of the system */
  int nc1,nc2,ng1,ng2;
  virtual  void adi(Scalar **uadi, Scalar **s, Scalar del_t)=0;
  virtual  void init_solve(Grid *grid,Scalar **epsi)=0;

 public:

  virtual int solve(Scalar **u_in, Scalar **s, int itermax,Scalar tol_test)=0;
  // laplace solve just calls solve for dadi
  virtual int laplace_solve(Scalar **u_in, Scalar **s, int itermax,Scalar tol_test)=0;

  virtual void set_coefficient(int j, int k, BCTypes type, Grid *grid)=0;
  virtual BCTypes get_coefficient(int j, int k)=0;
	Scalar **get_a1_coefficients() { return a_x1geom;};
	Scalar **get_a2_coefficients() { return a_x2geom;};
  Scalar **get_b1_coefficients() { return b_x1geom;};  
  Scalar **get_b2_coefficients() { return b_x2geom;};
	Scalar **get_c1_coefficients() { return c_x1geom;};
	Scalar **get_c2_coefficients() { return c_x2geom;}; 

	Scalar ***get_resCoeff() {return 0;};
  void PSolveBoltzCoeff(const Scalar & ne0, const Scalar & qbyT, Scalar** phi, const Scalar & minphi){};
	Scalar Resid(Scalar** rho, Scalar** phi) {return 0;};
	 
	 
  Dadi(Scalar **_epsi,Grid *grid);
  virtual ~Dadi();
};

#endif //__DADI_H
