
#include <precision.h>

//  This file contains some functions used for testing the DADI poisson
//  solver.  It is temporary and should go away.

void set_analytic_rho(Scalar **rho, int ng1, int ng2, Scalar length1, Scalar length2)
{ int j,k;

    for(j=0; j< ng1; j++)
    for(k=0; k< ng2; k++)
      { b=x2_array[k];

      rho[j][k] = sin(x1_array[j]*M_PI/length1)
	*( a * ( ((b==0)?a:sin (a*b)/b) + a*cos(a*b)));
      rho[j][k] += cos(a*b) /length1/length1
	*M_PI*M_PI * sin(x1_array[j]*M_PI/length1); 

      }
}



  

