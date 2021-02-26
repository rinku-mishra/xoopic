/*
====================================================================

MAXWELLN.H

The Maxwellian class parameterizes a Maxwell-Boltzmann distribution
for a particular type of particle in the units specified.

0.99	(JohnV, 01-03-93) Separate from pic.h, add comments.
0.991	(JohnV 01-27-95) Add Species.

====================================================================
*/

#ifndef	__MAXWELLN_H
#define	__MAXWELLN_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <oops.h>
#include "ovector.h"
#include "misc.h"
#include <cmath>

//#ifdef UNIX
#define	NVTS   3.3
#define	NBINS  10000
//#else
//#define	NVTS   3.0
//#define	NBINS  5000
//#endif

class Species;

enum UNIT {MKS, EV};

//--------------------------------------------------------------------
//	Maxwellian:  This class describes a Maxwellian distribution.

class Maxwellian
{
protected:
	int cutoffFlag;   // cutoffFlag = 1 cutoff distribution, 
	                  // cutoffFlag = 0 full maxwellian
	Vector3	v0;				//	drift (m/s)
	Vector3 u0;       //  drift gamma0*v0
	Scalar gamma0;    //  gamma of v0
	Scalar beta;
	Scalar iv0;       // 1/v0.magnitude();
	Vector3 u0normal;  //  normal of drift.
	Vector3 v0normal;  // normal of drift.
	Vector3	vt;				//	temperature (m/s)
	Vector3	vcu,vcl;				//	cutoff (m/s) (Normalized by vt)
	Vector3 Rvu,Rvl;  // exp(-vcu^2) and exp(-vcl^2)
	Vector3 erfvu, erfvl;
	Species*	species;
	Vector3*	v_array;       //	velocity array for a distribution
	int nvel;					//	number of velocities in the velocity array.

	Vector3 cutoff();

public:
	Maxwellian() {species = NULL;}          //      default constructor
	~Maxwellian();
	Maxwellian(Species* s) {
	  CreateZeroDist(s);
	  };
											
	Maxwellian(Vector3& vt, Species* s, UNIT type=MKS){  //      create thermal dist (m/s)
		CreateZeroDist(s);    
		setThermal(vt,type);
	};
	Maxwellian(Scalar temperature, Species* s, UNIT type=MKS){ //      create uniform thermal dist
		CreateZeroDist(s);
		setThermal(Vector3(temperature,temperature,temperature),type);
	};

	void CreateZeroDist(Species *s);

/** set thermal dist */
	void  setThermal(const Vector3& v, UNIT type=MKS) 
	 	/* throw(Oops) */;

/** isotropic thermal dist */
	void  setThermal(const Scalar v, UNIT type=MKS) 
		/* throw(Oops) */ {
	  setThermal(Vector3(v,v,v),type);
        };

	void	setCutoff(const Vector3& v1, const Vector3& v2, UNIT type=MKS);   // changes or sets cutoff to dist 
	void  setCutoff(const Scalar vl, const Scalar vu,UNIT type=MKS){setCutoff(Vector3(vl,vl,vl),Vector3(vu,vu,vu),type);};
	void	setDrift(const Vector3& v, UNIT type=MKS);
	void  ArraySetUp();  //sets up v_array

	Vector3	get_v0(UNIT type=MKS);		//	return v0 in units specified
	Vector3	get_vt(UNIT type=MKS);		//	return vt in units specified
	Vector3 get_vc(UNIT type=MKS);		//	return vc in units specified
	Vector3 get_V(); // return new velocity in a distribution
	Vector3 get_U(); // return new gamma*velocity in a distribution
	Species*	get_speciesPtr() { return species;};
	inline Scalar F(Scalar v) {return(-2*v*exp(-v*v)/sqrt(M_PI) +erf(v));}//normalized 
	// isotropic maxwellian distribution integrated from 0 to v
};

#endif	//	ifndef __MAXWELLN_H

