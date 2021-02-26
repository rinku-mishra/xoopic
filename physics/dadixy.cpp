/*
====================================================================

DADIXY.CPP

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

Revision/Programmer/Date
0.?	(Peterm, ??-??-94)	Conversion from C DADI.
0.9	(JohnV Peterm 08-09-95) Modify epsilon in set_coeff for dielectric
		blocks.

====================================================================
*/

#include <math.h>
#include "dadixy.h"
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

#ifdef _MSC_VER
using std::cout;
using std::cerr;
using std::endl;
#endif
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

void Dadixy::adi(Scalar **uadi, Scalar **s, Scalar del_t)
{
  register int i, j;
  Scalar dth;


  dth = .5*del_t;

  /***************************************/
  /* Do z pass.  Set up variables for    */
  /* tridiagonal inversion along z.      */

  for(j=0; j<ng2; j++)
  {
    for(i=0; i<ng1; i++)
    {
      a_x1[i] = -dth*a_x1geom[i][j];
      b_x1[i] = 1 - dth*b_x1geom[i][j];
      c_x1[i] = -dth*c_x1geom[i][j];
     }

  /*  handle the boundary conditions, neumann and dirichlet */
	  for(i=0; i<ng1; i++)
		  if(b_x2geom[i][j]!=0)  //non-dirichlet
			  r_x1[i] = uadi[i][j] +dth*(-s[i][j] 
													+((j>0)?a_x2geom[i][j]*uadi[i][j-1]:0)
													+b_x2geom[i][j]*uadi[i][j] 
													+((j<nc2)?c_x2geom[i][j]*uadi[i][j+1]:0));
		  else
			  r_x1[i] = uadi[i][j];  // metal sets the potential here.

    /* Solve tridiagonal system. */
    tridag(a_x1, b_x1, c_x1, r_x1, v_x1, gam_x1, ng1);

    /* Copy solution into ustar. */
    for(i=0; i<ng1; i++) ustar[i][j] =v_x1[i];
  }
  
  /***************************************/
  /* Do y pass.  Set up variables for    */
  /* tridiagonal inversion along y     */

  for(i=0; i<ng1; i++)
  {
    for(j=0; j<ng2; j++)
    {
      a_x2[j] = -dth*a_x2geom[i][j];
      b_x2[j] = 1 - dth*b_x2geom[i][j];
      c_x2[j] = -dth*c_x2geom[i][j];
    }

    /*  handle the boundary conditions, dirichlet or neumann */

     /*  The following code handles some special cases like corners*/
	  for(j=0; j<ng2; j++)
		  if(b_x2geom[i][j]!=0)  //non-dirichlet
			  r_x2[j] = ustar[i][j] +dth*(-s[i][j] 
													+((i>0)?a_x1geom[i][j]*ustar[i-1][j]:0)
													+b_x1geom[i][j]*ustar[i][j] 
													+((i<nc1)?c_x1geom[i][j]*ustar[i+1][j]:0));
		  else
			  r_x2[j] = ustar[i][j];  // metal sets the potential here.
          
    /* Solve tridiagonal system. */
    tridag(a_x2, b_x2, c_x2, r_x2, v_x2, gam_x2, ng2);
    
    /* Copy solution into ustar. */
    for(j=0; j<ng2; j++) uadi[i][j] =v_x2[j];

  }
  
  /*****************************************************/
  /* Dirchlet boundary conditions for i=0 and i=nc1. */
  
}


void Dadixy::init_solve(Grid *grid,Scalar **epsi)
{
  register int i, j;
  //set up freespace default coefficients everywhere.
  //outside boundaries must be set up specially, 
  //but at this point we don't know these yet.
  for(i=0;i<ng1;i++)
	  for(j=0;j<ng2;j++)
		 {
			Boundary *B = grid->GetNodeBoundary()[i][j];
			BCTypes type;
			if(B!=NULL) type = B->getBCType();
			else type = FREESPACE;
			set_coefficient(i,j,type,grid);
		 }


  /************************************************************/
  /* Determine initial step size such that 1=del_t/(dx1*dx1+dx2*dx2).  
     Chosen because this looks like stability condition for 
     an explicit scheme for the above parabolic equation.  
     Also calculate the usual constants.  */
  
  Vector2 **X = grid->getX();
  del_t0 = 0.1*(sqr(X[1][0].e1()-X[0][0].e1()) +sqr(X[0][1].e2()-X[0][0].e2()))/epsi[0][0];

  for(i=1; i<ng1; i++)
    for(j=1;j<ng2;j++)
      del_t0 = MIN(del_t0, 0.1*(sqr(X[i][j].e1()-X[i-1][j].e1()) 
				  +sqr(X[i][j].e2()-X[i][j-1].e2()))/epsi[MIN(i,nc1-1)][MIN(j,nc2-1)]);
  

}



void Dadixy::set_coefficient(int i,int j, BCTypes type, Grid *grid) 
{
	Vector2 **X=grid->getX();
	int I=grid->getJ();
	int J=grid->getK();

	Scalar dxa = X[i][j].e1()  -X[MAX(0,i-1)][j].e1();
	Scalar dxb = X[MIN(I,i+1)][j].e1()-X[i][j].e1();
	Scalar dxave= 0.5*(dxa + dxb);
	Scalar dya = X[i][j].e2()-X[i][MAX(0,j-1)].e2();
	Scalar dyb = X[i][MIN(J,j+1)].e2() - X[i][j].e2();
	Scalar dyave = 0.5 * (dya + dyb);

	// epsilon coefficients, weighted appropriately for each cell side
	Scalar e1a,e1b,e2a,e2b;

//	e1a = (epsi[i][j]*dya + epsi[i][MIN(J,j+1)]*dyb)/(dya + dyb);
//	e1b = (epsi[MIN(i+1,I)][j]*dya + epsi[MIN(i+1,I)][MIN(J,j+1)]*dyb)/(dya + dyb);
//	e2a = (epsi[i][j]*dxa + epsi[MIN(I,i+1)][j]*dxa)/(dxa + dxb);
//	e2b = (epsi[i][MIN(j+1,J)]*dxa + epsi[MIN(i+1,I)][MIN(J,j+1)]*dxb)/(dxa + dxb);

	int im = MAX(i-1,0);
	int jm = MAX(j-1,0);

	e1a = (epsi[im][jm]*dya + epsi[im][MIN(j,J-1)]*dyb)/(dya + dyb);
	e1b = (epsi[MIN(i,I-1)][jm]*dya + epsi[MIN(i,I-1)][MIN(j,J-1)]*dyb)/(dya + dyb);
	e2a = (epsi[im][jm]*dxa + epsi[MIN(i,I-1)][jm]*dxb)/(dxa + dxb);
	e2b = (epsi[im][MIN(j,J-1)]*dxa + epsi[MIN(i,I-1)][MIN(j,J-1)]*dxb)/(dxa + dxb);

//	if(type==DIELECTRIC_BOUNDARY && (i!=0||i!=I||j!=J||j!=0)) type = FREESPACE;
	if((type==DIELECTRIC_BOUNDARY) && (((0<i)&&(i<I))&&((0<j)&&(j<J)))) type = FREESPACE;

	switch(type) 
		{
		 case FREESPACE:
			{
				if(b_x2geom[i][j]==0) break;  //don't override a Dirichlet
				//x finite difference
				if(i) a_x1geom[i][j] = e1a/(dxa*dxave);
				if(i!=I) c_x1geom[i][j] = e1b/(dxb*dxave);
				b_x1geom[i][j] = -( a_x1geom[i][j] + c_x1geom[i][j] );

				//y finite difference
				if(j) a_x2geom[i][j] = e2a/dyave/dya;
				if(j!=J) c_x2geom[i][j] = e2b/dyave/dyb;
				b_x2geom[i][j] = - ( a_x2geom[i][j] + c_x2geom[i][j] );
				break;
			}
		 case CONDUCTING_BOUNDARY:
			{
				a_x1geom[i][j]=0.0;
				b_x1geom[i][j]=0.0;
				c_x1geom[i][j]=0.0;
				a_x2geom[i][j]=0.0;
				b_x2geom[i][j]=0.0;
				c_x2geom[i][j]=0.0;
				break;
			}
		 case DIELECTRIC_BOUNDARY:
			{

				if(i==0&&b_x2geom[i][j]!=0) {  // left hand wall, not a conductor
					dxa = X[i+1][j].e1()-X[i][j].e1();
					a_x1geom[i][j] = 0.0;
					c_x1geom[i][j] = 2*e1b/dxa/dxa;
					b_x1geom[i][j] = -( a_x1geom[i][j] + c_x1geom[i][j] );
					if(j==0) { //neumann neumann corner
						dyb = X[i][j+1].e2()-X[i][j].e2();
						a_x2geom[i][j] = 0.0;
						c_x2geom[i][j] = e2b/dyb/dyb;
						b_x2geom[i][j] = -c_x2geom[i][j];
						return;
					} else
					if(j==J) { //neumann neumann corner
						dya = X[i][j].e2()-X[i][j-1].e2();
						c_x2geom[i][j] = 0.0;
						a_x2geom[i][j] = e1b/dya/dya;
						b_x2geom[i][j] = -a_x2geom[i][j];
						return;
					} else
						{
							a_x2geom[i][j] = e2a/dyave/dya;
							c_x2geom[i][j] = e2b/dyave/dyb;
							b_x2geom[i][j] = - ( a_x2geom[i][j] + c_x2geom[i][j] );
							return;
						}
				}

				if(i==I&&b_x2geom[i][j]!=0) {  //right hand wall, not a conductor
					dxa = X[i][j].e1()-X[i-1][j].e1();
					a_x1geom[i][j] = 2*e1a/dxa/dxa;
					c_x1geom[i][j] = 0.0;
					b_x1geom[i][j] = -( a_x1geom[i][j] + c_x1geom[i][j] );
					if(j==0) { // neumann neumann corner
						a_x2geom[i][j] = 0.0;
						c_x2geom[i][j] = e2b/dyb/dyb;
						b_x2geom[i][j] = -c_x2geom[i][j];
						return;
					} else
					if(j==J) { //neumann neumann corner
						dya = X[i][j].e2()-X[i][j-1].e2();
						c_x2geom[i][j] = 0.0;
						a_x2geom[i][j] = e1b/dya/dya;
						b_x2geom[i][j] = -a_x2geom[i][j];
						return;
					}
					else	{
						a_x2geom[i][j] = e2a/dyave/dya;
						c_x2geom[i][j] = e2b/dyave/dyb;
						b_x2geom[i][j] = - ( a_x2geom[i][j] + c_x2geom[i][j] );
						return;
					}
				}
				if(j==J&&b_x2geom[i][j]!=0) {  //we already dealt with corners, no need to here
					c_x2geom[i][j] = 0.0;
					a_x2geom[i][j] = e1b/dya/dya;
					b_x2geom[i][j] = -a_x2geom[i][j];
				}

				if(j==0&&b_x2geom[i][j]!=0) {	// already dealt with corners, no need to here
					a_x2geom[i][j] = 0.0;
					c_x2geom[i][j] = e2b/dyb/dyb;
					b_x2geom[i][j] = -c_x2geom[i][j];
				}
				break;
			}
		 case CYLINDRICAL_AXIS:
			{
				//meaningless in X-Y geometry

				break;
			 }
		 default: 
			{
			  cout << "DADI doesn't know about this boundary condition type!\n;" << endl;
			  cout << "(was either PERIODIC_BOUNDARY or SPATIAL_REGION_BOUNDARY)\n" << endl;
			  //e_xit(1);
			}
		 }
 }



BCTypes Dadixy::get_coefficient(int j, int k) {
  return (b_x2geom[j][k]==0)? CONDUCTING_BOUNDARY:FREESPACE;
}


/**********************************************************************/

/*dadi(u_in, s, itermax, tol_test, u_x0, u_xlx, u_y0, u_yly)
int itermax;
Scalar tol_test;
Scalar **u_in, **s, *u_x0, *u_xlx, *u_y0, *u_yly;
*/
int Dadixy::solve(Scalar **u_in, Scalar **s, int itermax,Scalar tol_test)
{
  register int i, j;
  int iter, ndiscard;
  static Scalar del_t=0.0;
  Scalar del_td=0, tptop=0, tpbot=0, ratio=0;
  Scalar rnorm=0, rsum=0, res=0, errchk=0, dxdxutrm=0, dydyutrm=0;

  rnorm=rsum=0.0;
  for(i=0; i< ng1; i++)
    for(j=0; j< ng2; j++) {

		/* Residual normalization.  */
		// dirichlet conditions don't add to rnorm
		rnorm +=( (b_x2geom[i][j]==0)? 0:s[i][j]*s[i][j]) ;
      
  /*  copy u_in to u for working purposes.  */  
		u[i][j]=(Scalar)u_in[i][j];

		//calculate an initial estimate of the residual
	/* Use the residual as the absolute error and if it is bigger 
	   than the stored maximum absolute error, record new maximum 
	   absolute error and location.  */

		if(i>0&&j>0&&i<nc1&&j<nc2) {
		  dxdxutrm=a_x1geom[i][j]*u_in[i-1][j] +b_x1geom[i][j]*u_in[i][j] +
			 c_x1geom[i][j]*u_in[i+1][j];
		  dydyutrm=a_x2geom[i][j]*u_in[i][j-1] +b_x2geom[i][j]*u_in[i][j] +
			 c_x2geom[i][j]*u_in[i][j+1];
		}

  		/* only include points which are not in structures. */
		errchk = ((b_x2geom[i][j]==0)? 0:dxdxutrm +dydyutrm -s[i][j]);
	
		/* Residual sums. */
		rsum += errchk*errchk;
	 }

  // If rnorm is zero, we must deal with it...
  if (rnorm == 0.0) {

	 // check dirichlet conditions
	 for(i=0;i<ng1;i++) for(j=0;j<ng2;j++) {
		rnorm += sqr(((i>0&&b_x2geom[i-1][j]!=0)?c_x1geom[i-1][j]*u[i][j]:0));
		// check right
		rnorm += sqr(((i<nc1&&b_x2geom[i+1][j]!=0)?a_x1geom[i+1][j]*u[i][j]:0));
		// check up
		rnorm += sqr(((j>0&&b_x2geom[i][j-1]!=0)?c_x2geom[i][j-1]*u[i][j]:0));
		// check right
		rnorm += sqr(((j<nc2&&b_x2geom[i][j+1]!=0)?c_x2geom[i][j+1]*u[i][j]:0));

	 }

	 if(rnorm == 0) { //still zero, we don't need to iterate
		for(i=0;i<ng1;i++) for(j=0;j<ng2;j++) u_in[i][j]=0;
		return 0;
	 }
  }
  rnorm=sqrt(rnorm);
  res = sqrt(rsum)/rnorm;
#ifdef DADI_DEBUG
  printf("dadi: res = %g\n",res);
  printf("dadi: rnorm= %g\n", rnorm);
#endif

  if(res<tol_test) return 0;  // no need to iterate

  /*************************************************/
  if(del_t==0.0) del_t=del_t0; else del_t/=4;
  del_td= 2.0*del_t;
  ndiscard=0;

  /********************/
  /* Begin iteration. */
  
  for(iter=0; iter<itermax; iter++)
  {
    /*************************************************/
    /* Copy u into the work array and storage array. */
    
    for(i=0; i< ng1; i++)
      for(j=0; j< ng2; j++) uwork[i][j] = ustor[i][j] = u[i][j];
    
    /************************************/
    /* Two advances of u via ADI at del_t. */

    adi(u, s, del_t);
    adi(u, s, del_t);
    
    /*****************************************/
    /* One advance of uwork via ADI at 2*del_t. */

    adi(uwork, s, del_td);
    
    /*******************************************************/
    /* Calculate test parameter and normalized error.
       For Dirichlet BCs, no need to worry about boundary
       points since u,uwork, and ustor should be the same. */
    
    tptop= tpbot= rsum = 0.0;

    for(i=1; i< nc1; i++)
      for(j=1; j< nc2; j++)
      {
		  /* Test paramter sums. */
		  tptop += (u[i][j]-uwork[i][j])*(u[i][j]-uwork[i][j]);
		  tpbot += (u[i][j]-ustor[i][j])*(u[i][j]-ustor[i][j]);
	
	/* Residual terms. */

		  dxdxutrm=a_x1geom[i][j]*u[i-1][j] +b_x1geom[i][j]*u[i][j] +c_x1geom[i][j]*u[i+1][j];
		  dydyutrm=a_x2geom[i][j]*u[i][j-1] +b_x2geom[i][j]*u[i][j] +c_x2geom[i][j]*u[i][j+1];

	/* Use the residual as the absolute error and if it is bigger 
	   than the stored maximum absolute error, record new maximum 
	   absolute error and location.  */
		  /* only include points which are not in structures. */
		  errchk = ((b_x2geom[i][j]==0)? 0:dxdxutrm +dydyutrm -s[i][j]);
	
	/* Residual sums. */
		  rsum += errchk*errchk;
      }

    /* Calculate normalized residual. */
    res = sqrt(rsum)/rnorm;
#ifdef DADI_DEBUG
    //printf("dadi: iter= %d, res = %lg del_t=%le\n", iter, res,del_t);
		printf("dadi: iter= %d, res = %g del_t=%e\n", iter, res,del_t);
#endif // DADI_DEBUG
    /* If the residual is less than the tolerance, SUCCESS! */
    if ((res < tol_test) && (iter))
		{
#ifdef DADI_DEBUG
		  printf("dadi: iter=%d\n", iter);
#endif// DADI_DEBUG
		  for(i=0;i<ng1;i++)
			 for(j=0;j<ng2;j++)
				u_in[i][j]=(Scalar)u[i][j];

		  return(0);
		}

    /* Determine ratio used to find the time step change.  If tpbot 
       is zero but tptop is finite, consider this a case of a large 
       ratio and act accordingly.  DWH does about the same thing 
       except he does NOT discard the solution if tpbot=0. */
    
    if (tpbot > 0.0) ratio=tptop/tpbot;
    if (tpbot== 0.0) ratio=1.0;
#ifndef NO_STEP_ADJUST    
    /* Get next time step. */
    if (ratio < 0.02) del_t *= 8.0;
    if (ratio >=0.02 && ratio < 0.05) del_t *= 4.0;
    if (ratio >=0.05 && ratio < 0.10) del_t *= 2.0;
    if (ratio >=0.10 && ratio < 0.30) del_t *= 0.80;
    if (ratio >=0.30 && ratio < 0.40) del_t *= 0.50;
    if (ratio >=0.40 && ratio < 0.60) del_t *= 0.25; 
	 for(i=0;i<ng1;i++)
		for(j=0;j<ng2;j++)
		  u_in[i][j]=(Scalar)u[i][j];
#endif   
    
	 /* Ratio is too large. */
    if (ratio >=0.60)
    { 
      ndiscard++;
			iter--;
#ifdef DADI_DEBUG
	    //  printf("ndiscard= %d, iter=%d step=%lf\n", ndiscard, iter,del_t); 
			printf("ndiscard= %d, iter=%d step=%f\n", ndiscard, iter,del_t); 
#endif //DADI_DEBUG
      
      /* Check if too many discards. */
      if (ndiscard > 20)
      {
		  for(i=0;i<ng1;i++)
			 for(j=0;j<ng2;j++)
				u_in[i][j]=(Scalar)u[i][j];
		  del_t = del_t0;
//		  if(solve(u_in,s,itermax,tol_test))
		  printf("Poisson solve FAILURE: dadi: iter= %d, ndiscard>20\n", iter);
		  return 1;
      }	
      /* Discard by replacing u with what we started with. */
      for(i=0; i< ng1; i++)
		  for(j=0; j< ng2; j++) u[i][j] = ustor[i][j];
      
      /* Reduce del_t. */
      del_t /= 8.0;
		//del_t = del_t0;
    }
    del_td=2*del_t;
  }
  /* Fail if used up the maximum iterations. */

  printf("Poisson solve FAILURE: dadi:  iter>= %d\n",itermax);

  for(i=0;i<ng1;i++)
	 for(j=0;j<ng2;j++)
		u_in[i][j]=(Scalar)u[i][j];
 
  return(2);
}


Dadixy::~Dadixy()
{
}

