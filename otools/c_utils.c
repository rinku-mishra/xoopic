#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "precision.h"

/*  This function displays a double pointer type of thingy */
void SMD(double **M,int r,int c)
{
  int i,j;
  for(i=0;i<r;i++) {
	for(j=0;j<c;j++)
		printf("%12.6g ",M[i][j]);
	printf("\n");
  }
}

void SM(float **M,int r,int c)
{
  int j,k;
/*  for(i=0;i<r;i++) {
	for(j=0;j<c;j++)
		printf("%12.6g ",M[i][j]);
	printf("\n");
  } */
  for(k=c-1;k>=0;k--) {
	for(j=0;j<r;j++)
		printf("%12.6g ",M[j][k]);
	printf("\n");
  }
}

void SMV(float ***M,int com, int r,int c) /*show matix vector*/
{
  int j,k;
  for(k=c-1;k>=0;k--) {
		for(j=0;j<r;j++)
			printf("%12.6g ",M[j][k][com]);
		printf("\n");
  }
}

void SMI(int **M,int r,int c)
{
  int i,j;
  for(i=0;i<r;i++) {
	for(j=0;j<c;j++)
		printf("%10d ",M[i][j]);
	printf("\n");
  }
}

Scalar analytic_rho(int j, int k)
{
  return j+k;
}

/*  Commented out 3/3/00 by Bruhwiler
#if defined(linux) && defined(__i386)
*/

/*  wrappers for common arith functions */
Scalar SINW(double x)  { return sin(x);  }
Scalar COSW(double x)  { return cos(x);  }
Scalar LOGW(double x)  { return log(x);  }
Scalar EXPW(double x)  { return exp(x);  }
Scalar ATANW(double x) { return atan(x); }
Scalar SQRTW(double x) { return sqrt(x); }
Scalar TANHW(double x) { return tanh(x); }
Scalar POWW(double x, double y) { return pow(x, y); }
Scalar FABSW(double x) { return fabs(x); }
/*  These are some functions useful for time functions */

/*  a ramp function */
Scalar ramp(double t) {
	return t<0? 0:t;
}

/* a step function */
Scalar step(double t) {
	return t<0?0:1;
}

/* a pulse function */
Scalar pulse(double t_now, double tdelay, double trise, double tpulse, double tfall) {
	double t_tmp;
	if (t_now < tdelay) return 0.;
	if (t_now < (t_tmp=tdelay+trise) ) return (t_now-tdelay)/(trise);
	if (t_now < (t_tmp+=tpulse) ) return 1.;
	if (t_now < (t_tmp+=tfall) ) return (t_tmp-t_now)/(tfall);
	return 0.;
}

/* a gaussian profile function:  x is the position, xC is the center
   W is the spread */
Scalar gaussian(double x,double W,double xC) {
  double a = (x-xC)/W;
  return exp(-a*a);
}

/*  Commented out 3/3/00 by Bruhwiler
#else

float SINW(float x)  { return sin(x);  }
float COSW(float x)  { return cos(x);  }
float LOGW(float x)  { return log(x);  }
float EXPW(float x)  { return exp(x);  }
float ATANW(float x) { return atan(x); }
float SQRTW(float x) { return sqrt(x); }
float TANHW(float x) { return tanh(x); }
float POWW(float x, float y) { return pow(x, y); }

float ramp(float t) {
	return t<0? 0:t;
}

float step(float t) {
	return t<0?0:1;
}

float pulse(float t_now, float tdelay, float trise, float tpulse, float tfall) {
	float t_tmp;
	if (t_now < tdelay) return 0.;
	if (t_now < (t_tmp=tdelay+trise) ) return (t_now-tdelay)/(trise);
	if (t_now < (t_tmp+=tpulse) ) return 1.;
	if (t_now < (t_tmp+=tfall) ) return (t_tmp-t_now)/(tfall);
	return 0.;
}

#endif
*/


Scalar xoopic_nint(Scalar x) {
  return (Scalar)((x>=0)? (int) (x + 0.5): (int) (x -0.5));
}


/* routine from numerical recipes which does this:
  Given arrays xa[1..n] and ya[1..n], a value x, return y and
an error estimate dy */


void polint(Scalar xa[], Scalar ya[], int n, Scalar x, Scalar *y, Scalar *dy)
{
	int i,m,ns=1;
	Scalar den,dif,dift,ho,hp,w;
	Scalar *c,*d;

	dif=fabs(x-xa[1]);
	c=(Scalar*)malloc((n+1)*sizeof(Scalar));
	d=(Scalar*)malloc((n+1)*sizeof(Scalar));
	for (i=1;i<=n;i++) {
		if ( (dift=fabs(x-xa[i])) < dif) {
			ns=i;
			dif=dift;
		}
		c[i]=ya[i];
		d[i]=ya[i];
	}
	*y=ya[ns--];
	for (m=1;m<n;m++) {
		for (i=1;i<=n-m;i++) {
			ho=xa[i]-x;
			hp=xa[i+m]-x;
			w=c[i+1]-d[i];
			if ( (den=ho-hp) == 0.0) fprintf(stderr,"Error in routine polint");
			den=w/den;
			d[i]=hp*den;
			c[i]=ho*den;
		}
		*y += (*dy=(2*ns < (n-m) ? c[ns+1] : d[ns--]));
	}
	free(d);
	free(c);
	
}

