/*
====================================================================

MISC.CPP

Defines miscellaneous functions for OOPIC, including erf(), erfc().

0.9	(JohnV 02-01-95) Original code.
0.91	(JohnV 06-28-95) Merge in math_func.*.
0.92  (PeterM 05-07-96) Added segmentate(), a tool for interpolating
                        an oblique boundary.
====================================================================
*/

#include <math.h>
#include "misc.h"
#include "species.h"

int frandseed=31207321;

//------------------------------------------------------------------
// Initialize static variables here.

int Species::idCount = 0;

//------------------------------------------------------------------
// Support for error functions and complementary error functions.
//	From Numerical Recipes in C... These have been ANSI-fied for
//	use in c++.

#define ITMAX 100
#define EPS 3.0e-7
#ifndef UNIX
void gcf(Scalar* gammcf, Scalar a, Scalar x, Scalar* gln)
{
	int n;
	Scalar gold=0.0,g,fac=1.0,b1=1.0;
	Scalar b0=0.0,anf,ana,an,a1,a0=1.0;
	Scalar gammln(Scalar x);
//	void nrerror();

	*gln=gammln(a);
	a1=x;
	for (n=1;n<=ITMAX;n++) {
		an=(Scalar) n;
		ana=an-a;
		a0=(a1+a0*ana)*fac;
		b0=(b1+b0*ana)*fac;
		anf=an*fac;
		a1=x*a0+anf*a1;
		b1=x*b0+anf*b1;
		if (a1) {
			fac=1.0/a1;
			g=b1*fac;
			if (fabs((g-gold)/g) < EPS) {
				*gammcf=exp(-x+a*log(x)-(*gln))*g;
				return;
			}
			gold=g;
		}
	}
//	nrerror("a too large, ITMAX too small in routine GCF");
}
#undef ITMAX
#undef EPS

#define ITMAX 100
#define EPS 3.0e-7
void gser(Scalar* gamser,Scalar a, Scalar x, Scalar* gln)
{
	int n;
	Scalar sum,del,ap;
	Scalar gammln(Scalar x);
//	void nrerror();

	*gln=gammln(a);
	if (x <= 0.0) {
//		if (x < 0.0) nrerror("x less than 0 in routine GSER");
		*gamser=0.0;
		return;
	} else {
		ap=a;
		del=sum=1.0/a;
		for (n=1;n<=ITMAX;n++) {
			ap += 1.0;
			del *= x/ap;
			sum += del;
			if (fabs(del) < fabs(sum)*EPS) {
				*gamser=sum*exp(-x+a*log(x)-(*gln));
				return;
			}
		}
//		nrerror("a too large, ITMAX too small in routine GSER");
		return;
	}
}
#undef ITMAX
#undef EPS

Scalar gammln(Scalar xx)
{
	double x,tmp,ser;
	static double cof[6]={76.18009173,-86.50532033,24.01409822,
		-1.231739516,0.120858003e-2,-0.536382e-5};
	int j;

	x=xx-1.0;
	tmp=x+5.5;
	tmp -= (x+0.5)*log(tmp);
	ser=1.0;
	for (j=0;j<=5;j++) {
		x += 1.0;
		ser += cof[j]/x;
	}
	return -tmp+log(2.50662827465*ser);
}

Scalar gammp(Scalar a, Scalar x)
{
	Scalar gamser,gammcf,gln;
	void gser(Scalar* gamser,Scalar a, Scalar x, Scalar* gln);
	void gcf(Scalar* gammcf, Scalar a, Scalar x, Scalar* gln);
//	void nrerror();

//	if (x < 0.0 || a <= 0.0) nrerror("Invalid arguments in routine GAMMP");
	if (x < (a+1.0)) {
		gser(&gamser,a,x,&gln);
		return gamser;
	} else {
		gcf(&gammcf,a,x,&gln);
		return 1.0-gammcf;
	}
}

Scalar gammq(Scalar a, Scalar x)
{
	Scalar gamser,gammcf,gln;
//	void gcf(),gser(),nrerror();

//	if (x < 0.0 || a <= 0.0) nrerror("Invalid arguments in routine GAMMQ");
	if (x < (a+1.0)) {
		gser(&gamser,a,x,&gln);
		return 1.0-gamser;
	} else {
		gcf(&gammcf,a,x,&gln);
		return gammcf;
	}
}

Scalar erf(Scalar x)
{
//	Scalar gammp();

	return x < 0.0 ? -gammp(0.5,x*x) : gammp(0.5,x*x);
}

Scalar erfc(Scalar x)
{
//	Scalar gammp(),gammq();

	return x < 0.0 ? 1.0+gammp(0.5,x*x) : gammq(0.5,x*x);
}

Scalar erfcc(Scalar x)
{
	Scalar t,z,ans;

	z=fabs(x);
	t=1.0/(1.0+0.5*z);
	ans=t*exp(-z*z-1.26551223+t*(1.00002368+t*(0.37409196+t*(0.09678418+
		t*(-0.18628806+t*(0.27886807+t*(-1.13520398+t*(1.48851587+
		t*(-0.82215223+t*0.17087277)))))))));
	return  x >= 0.0 ? ans : 2.0-ans;
}

#endif

Scalar bessj0(Scalar x)
{
	Scalar ax,z;
	double xx,y,ans,ans1,ans2;

	if ((ax=fabs(x)) < 8.0) {
		y=x*x;
		ans1=57568490574.0+y*(-13362590354.0+y*(651619640.7
				 +y*(-11214424.18+y*(77392.33017+y*(-184.9052456)))));
		ans2=57568490411.0+y*(1029532985.0+y*(9494680.718
				 +y*(59272.64853+y*(267.8532712+y*1.0))));
		ans=ans1/ans2;
	} else {
		z=8.0/ax;
		y=z*z;
		xx=ax-0.785398164;
		ans1=1.0+y*(-0.1098628627e-2+y*(0.2734510407e-4
         +y*(-0.2073370639e-5+y*0.2093887211e-6)));
		ans2 = -0.1562499995e-1+y*(0.1430488765e-3
           +y*(-0.6911147651e-5+y*(0.7621095161e-6
           -y*0.934935152e-7)));
		ans=sqrt(0.636619772/ax)*(cos(xx)*ans1-z*sin(xx)*ans2);
	}
	return ans;
}

Scalar bessj1(Scalar x)
{
	Scalar ax,z;
	double xx,y,ans,ans1,ans2;

	if ((ax=fabs(x)) < 8.0) {
		y=x*x;
		ans1=x*(72362614232.0+y*(-7895059235.0+y*(242396853.1
         +y*(-2972611.439+y*(15704.48260+y*(-30.16036606))))));
		ans2=144725228442.0+y*(2300535178.0+y*(18583304.74
         +y*(99447.43394+y*(376.9991397+y*1.0))));
		ans=ans1/ans2;
	} else {
		z=8.0/ax;
		y=z*z;
		xx=ax-2.356194491;
		ans1=1.0+y*(0.183105e-2+y*(-0.3516396496e-4
         +y*(0.2457520174e-5+y*(-0.240337019e-6))));
		ans2=0.04687499995+y*(-0.2002690873e-3
				 +y*(0.8449199096e-5+y*(-0.88228987e-6
         +y*0.105787412e-6)));
		ans=sqrt(0.636619772/ax)*(cos(xx)*ans1-z*sin(xx)*ans2);
		if (x < 0.0) ans = -ans;
	}
	return ans;
}

/*  This function takes two ordered pairs of integers,
which represent a line, and
returns an array of ordered pairs of integers which 
represent horizontal or vertical line segments which
will interpolate the given line onto a mesh. The array
ends when four 0 integers are given. */

int *segmentate(int j1, int k1, int j2, int k2) 
{
  int j;
  int jl,kl,jh,kh;
  int *segments;
  Scalar m,y;
  int kt;
  int j1s, k1s, j2s,k2s;  /*  the line segment coordinates */
  int segcount = 0;  /* number of segments. */
  /* reorder the input such that jl,kl is the leftmost point */
  if(j1<j2) { 
	  jl=j1;kl=k1;
	  jh=j2;kh=k2;
  }
  else {
	  jl=j2;kl=k2;
	  jh=j1;kh=k1;
  }

  m = (kh -kl)/(Scalar)(jh - jl);
  kt = kl;

  segments=new int[4*(3+ (jh-jl))];
  memset(segments,0,4*(3+(jh-jl))*sizeof(int));

  /* make sure the first point is in the array */
  segments[0]=jl;
  segments[1]=kl;

  /* get all the horizontal segments. */
  for(j=jl;j<jh;j++) {
	  y = m * (j - jl + 0.5) + kt;
	  k1s = (int) (y + 0.5);
	  k2s = k1s;
	  j1s = j;
	  j2s = j+1;
	  segments[4*segcount+2]=j1s;
	  segments[4*segcount+3]=k1s;
	  segments[4*segcount+4]=j2s;
	  segments[4*segcount+5]=k2s;
	  segcount++;
  }

  /* make sure the last point is in the array */
  segments[4*segcount+2]=jh;
  segments[4*segcount+3]=kh;
  return segments;
								  

}


Scalar normal2()
{
	/*returns a normally distributed deviate with zero mean and 1/Sqrt(2) variance.*/
	/*The way that we have defined v thermal this returns gaussian with a v thermal*/
	/* of one. */
	static int iset=0;
	static Scalar gset;
	Scalar fac,r2,v1,v2;
	
	if(iset==0){
		do{
			v1=2.0*frand()-1.0;
			v2=2.0*frand()-1.0;
			r2 = v1*v1+v2*v2;
		} while (r2>=1.0);
		fac=sqrt(-log(r2)/r2);
		gset=v1*fac;
		iset =1;
		r2=v2*fac;
	}
	else{
		iset=0;
		r2 = gset;
	}
	return(r2);
}

Scalar normal()
{
	/*returns a normally distributed deviate with zero mean and unit variance.*/
	static int iset=0;
	static Scalar gset;
	Scalar fac,r,v1,v2;
	
	if(iset==0){
		do{
			v1=2.0*frand()-1.0;
			v2=2.0*frand()-1.0;
			r = v1*v1+v2*v2;
		} while (r>=1.0);
		fac=sqrt(-2.0*log(r)/r);
		gset=v1*fac;
		iset =1;
		r=v2*fac;
	}
	else{
		iset=0;
		r = gset;
	}
	return(r);
}

Scalar LineDist(Scalar A1, Scalar A2, Scalar B1, Scalar B2, Scalar tA1, Scalar tA2) {
	Scalar dy = A2 - B2;
	Scalar dx = A1 - B1;
	return fabs( dx * tA2 - dy * tA1 +dy * A1 - dx * A2 )/ sqrt( dx*dx + dy*dy);
}  
	



