/**********************************************************************
  This is a dynamic ADI solution of the equation
  Del( epsilon * Del(phi) ) = rho
  in z-r coordinates.
  Neumann, Dirichlet, and symmetry boundary conditions are supported.
  Symmetry boundary condition is only applied when r0=0, and the
  neuman flag is specified there with zero slope on the electric field.

  dxdxu + dydyu =  s
  
  The function is based on a Peaceman Rachford Douglas 
  advance of the parabolic equation:  
  
  dtu = dxdxu + dydyu -s
  
  But with the time step dynamically chosen so as to speed convergence 
  to the dtu=0 steady state solution which is what we want.  It is the 
  user's responsiblity to put the initial guess of the solution stored 
  in u when the function is called.  u=0 or u=u(previous time step) 
  are possible choices.  
  
  The function sends back the finishing iteration number, the finishing
  normalized maximum error and location, and the failure code.  
  
  The failure codes are: 
    ifail=0, success actually
    ifail=1, had to discard too many times
    ifail=2, used up the iterations
    
  Send in a negative tol_test if you want to freeze the time step to 
  the initial choice.  Send in a 0 adel_t to let program pick an 
  initial del_t.   
  **********************************************************************/

#include <math.h>
#include "dadi.h"
#include "grid.h"

#ifndef MAX
#define MAX(x, y)       (((x) > (y)) ? (x) : (y))
#endif

#ifndef MIN
#define MIN(x, y)       (((x) < (y)) ? (x) : (y))
#endif

#ifndef DBL_MIN
#define DBL_MIN         1E-200
#endif


Dadi::Dadi(Scalar **_epsi,Grid *grid)
			  
{  register int i,j;
	epsi = _epsi;
   nc1=grid->getJ();
   nc2=grid->getK();
	ng1=nc1+1;
	ng2=nc2+1;  /*  These are global copies for dadi's use.  */
	u = new Scalar *[ng1];
	uwork = new Scalar *[ng1];
	ustor = new Scalar *[ng1];
	ustar = new Scalar *[ng1];
	a_x2geom = new Scalar *[ng1];
	b_x2geom = new Scalar *[ng1];
	c_x2geom = new Scalar *[ng1];
	a_x1geom = new Scalar *[ng1];
	b_x1geom = new Scalar *[ng1];
	c_x1geom = new Scalar *[ng1];


	for(i=0; i<ng1; i++)
	  {
		 u[i]= new Scalar[ng2];
		 uwork[i]=new Scalar[ng2];
		 ustor[i]=new Scalar[ng2];
		 ustar[i] = new Scalar[ng2];
		 a_x2geom[i] = new Scalar[ng2];
		 b_x2geom[i] = new Scalar[ng2];
		 c_x2geom[i] = new Scalar[ng2];
		 a_x1geom[i] = new Scalar[ng2];
		 b_x1geom[i] = new Scalar[ng2];
		 c_x1geom[i] = new Scalar[ng2];
	  }
	for(i=0;i<ng1;i++) for (j=0;j<ng2;j++)
	  { 
		 u[i][j] = 0;
		 uwork[i][j] = 0;
		 ustor[i][j] = 0;
		 ustar[i][j] = 0;
		 a_x2geom[i][j] = 0;
		 b_x2geom[i][j] = 1;  //0 is used as a flag here.
		 c_x2geom[i][j] = 0;
		 a_x1geom[i][j] = 0;
		 b_x1geom[i][j] = 0;
		 c_x1geom[i][j] = 0;
	  }

	/*******************************************************/
	/* Set up variables for tridiagonal inversion along z. */
	a_x1 = new Scalar[ng1];
	b_x1 = new Scalar[ng1];
	c_x1 = new Scalar[ng1];
	r_x1 = new Scalar[ng1];
	v_x1 = new Scalar[ng1];
	gam_x1= new Scalar[ng1];

	for(i=0;i<ng1;i++)
	  {
		 a_x1[i]=0.0;
		 b_x1[i]=0.0;
		 c_x1[i]=0.0;
		 r_x1[i]=0.0;
		 v_x1[i]=0.0;
		 gam_x1[i]=0.0;
	  }

	/*******************************************************/
	/* Set up variables for tridiagonal inversion along r. */
	a_x2 = new Scalar[ng2];
	b_x2 = new Scalar[ng2];
	c_x2 = new Scalar[ng2];
	r_x2 = new Scalar[ng2];
	v_x2 = new Scalar[ng2];
	gam_x2= new Scalar[ng2];

	for(i=0;i<ng2;i++)
	  {
		 a_x2[i]=0.0;
		 b_x2[i]=0.0;
		 c_x2[i]=0.0;
		 r_x2[i]=0.0;
		 v_x2[i]=0.0;
		 gam_x2[i]=0.0;
	  }

 }

/**********************************************************************
  Single Peaceman Rachford Douglas pass with Direchlet 0 c boundary
  conditions for the equation: 
  
  dtu = dxdxu + dydyu -s, where s is constant in time.  
  
  The Crank-Nicolson finite difference approximation to the 
  above equation leads to the fractional step or 
  ADI equations: 
  
  u*(i,j)-(del_t/2dxdx)[u*(i+1,j)-2u*(i,j)+u*(i-1,j)] 
  = un(i,j)+(del_t/2dydy)[un(i,j+1)-2un(i,j)+un(i,j-1)] - (del_t/2)s(i,j) 
  
  un+1(i,j)-(del_t/2dydy)[un+1(i,j+1)-2un+1(i,j)+un+1(i,j-1)] 
  = u*(i,j)+(del_t/2dxdx)[u*(i+1,j)-2u*(i,j)+u*(i-1,j)] - (del_t/2)s(i,j) 
  
  **********************************************************************/


/******************************************************/


Dadi::~Dadi()
{
  int i;

  for(i=0;i<ng1;i++) {
	 delete[]  u[i];
	 delete[]  uwork[i];
	 delete[]  ustor[i];
	 delete[]  ustar[i];
	 delete[]  a_x2geom[i];
	 delete[]  b_x2geom[i];
	 delete[]  c_x2geom[i];
	 delete[]  a_x1geom[i];
	 delete[]  b_x1geom[i];
	 delete[]  c_x1geom[i];
  }
  delete[]  u;
  delete[]  uwork;
  delete[]  ustar;
  delete[]  ustor;
  delete[]  a_x2geom;
  delete[]  b_x2geom;
  delete[]  c_x2geom;
  delete[]  a_x1geom;
  delete[]  b_x1geom;
  delete[]  c_x1geom;
  delete[]  a_x1;
  delete[]  b_x1;
  delete[]  c_x1;
  delete[]  r_x1;
  delete[]  v_x1;
  delete[]  gam_x1;

}

