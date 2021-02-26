/*
====================================================================

MISC.H

Defines miscellaneous constants and macros for OOPIC.

0.99	(JohnV, NTG 12-28-93) Separated into individual module from
		pic.h.
0.991	(JohnV 03-22-94) Improve portability, define RAND_MAX for UNIX.
1.01   (JohnV 09Sep2007) modified frand() to handle singleprecision truncation
       properly so that the return value is never 1.

====================================================================
*/

#ifndef	__MISC_H
#define	__MISC_H

#include	<stdlib.h>
#include	<cstdio>
#include	<math.h>
#include	"oopiclist.h"
#include "define.h"
#include "precision.h"


//--------------------------------------------------------------------
//	Define classes for forward references.

class SpatialRegion;
class	Fields;
class	Grid;
class	Boundary;
class ParticleGroup;    
class	Particle;
class Species;
class Diag;

//--------------------------------------------------------------------
//	typedefs

// JRC 29 Jan 00 - Move to C++ standards
// typedef int	BOOL;

#ifdef UNIX
typedef bool	BOOL;
#else
typedef int BOOL;
#endif

typedef oopicList<Species> SpeciesList;
typedef oopicList<ParticleGroup> ParticleGroupList;
typedef oopicList<Boundary> BoundaryList;
typedef oopicList<Diag> DiagList;
typedef oopicList<SpatialRegion> SpatialRegionList;

//--------------------------------------------------------------------
//	constants

// JRC 29 Jan 00 - Move to C++ standards
// #ifndef	FALSE
// #define	FALSE 0
// #define	TRUE  !FALSE
// #endif

#ifndef	FALSE
#define	FALSE false
#define	TRUE  true
#endif

//	Definitions for XOOPIC under g++
#ifndef	RAND_MAX
#define	RAND_MAX								2147483647
#define	rand()								random()
#endif


//const	double	PI = 3.14159265358979323846; use M_PI -> portable

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
const double	TWOPI = 2*M_PI;

const double  SPEED_OF_LIGHT = 299792458.;
const double iSPEED_OF_LIGHT = 1. / SPEED_OF_LIGHT;

const double  SPEED_OF_LIGHT_SQ = SPEED_OF_LIGHT * SPEED_OF_LIGHT;
const double iSPEED_OF_LIGHT_SQ = 1. / SPEED_OF_LIGHT_SQ;

const double  MU0 = 4.0e-07 * M_PI;
const double iMU0 = 1. / MU0;

const double iEPS0 = MU0 * SPEED_OF_LIGHT_SQ;
const double  EPS0 = 1/iEPS0;

const double ELECTRON_MASS    = 9.1093897e-31;   // in kg
const double ELECTRON_MASS_EV = 510999.06;       // in eV

const double PROTON_MASS    = ELECTRON_MASS    / 5.44617013e-04;
const double PROTON_MASS_EV = ELECTRON_MASS_EV / 5.44617013e-04;;

// const double PROTON_CHARGE   =  1.60206e-19;
const double PROTON_CHARGE   =  ELECTRON_MASS * 1.75881962e+11;
const double ELECTRON_CHARGE = -PROTON_CHARGE;

//	macros
#define MIN(a,b)			((a<b) ? (a) : (b))
#define MAX(a,b)			((a>b) ? (a) : (b))
#define sqr(a)	      ((a)*(a))
#define cube(a)       ((a)*(a)*(a))

//--------------------------------------------------------------------
//	General service functions
#ifdef KCC
extern "C" {
double erf(double x);
double erfc(double x);
};
#endif

#ifndef UNIX
Scalar erf(Scalar x);
Scalar erfc(Scalar x);
#endif

Scalar bessj0(Scalar r);
Scalar bessj1(Scalar r);
int *segmentate(int j1,int k1,int j2, int k2);

//	return a random number between 0 and 1
inline Scalar frand2() {Scalar f = (Scalar)rand(); return f/((Scalar)RAND_MAX+1.0f);}

/************************************************************************/
/* frand() returns values 0 through 1.                                  */
/* From "Random number generators: good ones are hard to find", S. Park */
/* and K. Miller, Communications of ACM, October 1988, pp 1192-1201.    */
/* This is from page 1195, and is to work on any system for which       */
/* maxint is 2**31-1 or larger. Due earlier to Schrage, as cited by P&M.*/
/*                                                                      */
/* Note: OK values for iseed are 1 through 2**31-2. Give it 0 or 2*31-1 */
/* and it will return the same values thereafter!                       */
/*                                                                      */
/* C version 6/91, Bruce Langdon.                                       */
/*                                                                      */
/* Algorithm replaces seed by mod(a*seed,m). First represent            */
/* seed = q*hi + lo.  Then                                              */
/* a*seed = a*q*hi + a*lo = (m - r)*hi + a*lo = (a*lo - r*hi) + m*hi,     */
/* and new seed = a*lo - r*hi unless negative; if so, then add m.       */

extern int frandseed;

inline Scalar frand()
{
  int a = 16807, m = 2147483647, q = 127773, r = 2836;
  int hi, lo;
  Scalar fnumb;
//  static long seed=31207321; 

	// moved seed to frandseed
  //  fnumb = 2;
  // the random number returned is 0=<fnumb<1
  do {
    hi = frandseed/q;
    lo = frandseed - q*hi;
    frandseed = a*lo - r*hi;
    /* "seed" will always be a legal integer of 32 bits (including sign). */
    // if(seed <= 0) seed = seed + m;
    if(frandseed < 0) frandseed = frandseed + m;
#if (ScalarInt == 4)    
    fnumb = (frandseed & 0x7fffff80)/2147483647.0f;
#else
    fnumb = frandseed/2147483647.0;
#endif
  } while (fnumb-1.0 >= 0); // johnv: modified to handle single precision properly
  return(fnumb);
}

Scalar normal();
Scalar normal2();


//	return a randomly signed 1
inline int rsign() {int i = rand()%2; if (i == 0) return -1; else return 1;}

// return a bit reversed number
inline Scalar revers(int num, int n)
{
  Scalar power, rev;
  int inum, iquot, irem;
  
  rev = 0.;
  inum = num;
  power = 1.;
  
  do
	 {
		iquot = inum/n;
		irem = inum - n*iquot;
		power /= n;
		rev += irem*power;
		inum = iquot;
	 } 
  while (inum > 0);
  
  return (rev);
}

/* Returns bit reversed num in base 2 */

inline Scalar revers2(unsigned int num)
{
  double f=0.5, sum=0.0;
  
  while(num)
		{
			if (num & 1) sum += f;     /* is 1st bit set? */
			f *= 0.5;
			num >>= 1;		       /* fast divide by 2 */
		}
  return (Scalar)(sum);
}

/* returns the distance between the line determined by (A1,A2), (B1,B2), and
the point, (tA1,tA2) */
Scalar LineDist(Scalar A1, Scalar A2, Scalar B1, Scalar B2, Scalar tA1, Scalar tA2);

#endif	//	ifndef __MISC_H
