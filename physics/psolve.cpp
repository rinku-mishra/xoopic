

#include <psolve.h>
#include "ovector.h"

/***********************************************************************
  Tridiagonal field solver:  
  
  | b0 c0 0                      | | u0 |     | r0 |
  | a1 b1 c1                     | | u1 |     | r1 |
  |      ............            | | .  |  =  | .  |
  |               an-2 bn-2 cn-2 | |un-2|     |rn-2|
  |               0    an-1 bn-1 | |un  |     |rn-1|                     
  
  **********************************************************************/

void PoissonSolve::tridag( Scalar *a, Scalar *b, Scalar *c, Scalar *r, Scalar *utri,
	    Scalar *gam,int n)
{
  register int i;
  Scalar bet;

  /*******************************************/
  /* Decomposition and forward substitution. */
  
  bet = b[0];
  utri[0]= r[0]/bet;
  
  for (i=1; i<n; i++)
  {
    gam[i] = c[i-1]/bet;
    bet = b[i] - a[i]*gam[i];
    utri[i] = (r[i] -a[i]*utri[i-1])/bet;
  }

  /**********************/
  /* Back substitution. */
  for(i=n-2; i>=0; i--) utri[i] -= gam[i+1]*utri[i+1];
}

PoissonSolve::PoissonSolve(void) {}
PoissonSolve::PoissonSolve(Scalar **epsi, Grid *grid) {}
