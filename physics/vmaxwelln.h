/*
	 ====================================================================

	 VMAXWELLN.H

	 The MaxwellianFlux class parameterizes the Flux of a Maxwell-Boltzmann distribution
	 for a particular type of particle in the units specified.

	 0.99	(kc, 01-14-97) Separate from Maxwellian and maxwelln.*


	 ====================================================================
	 */

#include "maxwelln.h"
#ifndef	__VMAXWELLN_H
#define	__VMAXWELLN_H

class Vector3;
//--------------------------------------------------------------------
//	MaxwellianFlux:  This class describes the flux of a Maxwellian distribution.

class MaxwellianFlux:public Maxwellian
{
protected:
	Vector3 vLlimit, vUlimit;
	Vector3 vcl,vcu;
	Vector3* FluxArray;
	int DriftFlag;
public:
	MaxwellianFlux() {
	  species = NULL;
	  DriftFlag = 0;
	}          //      default constructor
	MaxwellianFlux(Species *s):Maxwellian(s){
	  DriftFlag = 0;
	  CreateZeroDist();
	};
	MaxwellianFlux(const Vector3& vt, Species* s, UNIT type=MKS)
	  :Maxwellian(s){ // create thermal dist
	  DriftFlag = 0;
	  CreateZeroDist();
	  setThermal(vt,type);
	};

//create isotropic thermal dist
	MaxwellianFlux(const Scalar temperature, Species* s, UNIT type=MKS){
	  DriftFlag = 0;
	  Vector3 temp2 = Vector3(temperature,temperature,temperature);
	  MaxwellianFlux(temp2,s,type);
	};  
 
	void CreateZeroDist();
	void	setThermal(const Vector3& v, UNIT type=MKS);  // changes or sets thermal dist 
	void  setThermal(const Scalar v, UNIT type=MKS){setThermal(Vector3(v,v,v),type);};  //creates isotropic thermal dist 
	void	setCutoff(const Vector3& vl,const Vector3& vu, UNIT type=MKS);   // changes or sets cutoff on dist
	void setCutoff(const Scalar vl, const Scalar vu, UNIT type=MKS){setCutoff(Vector3(vl,vl,vl),Vector3(vu,vu,vu),type);};
	void ArraySetUp();  // sets up FluxArray

	void	setDrift(const Vector3& v, UNIT type=MKS);   // changes or sets drift velocity
	Vector3 get_V(const Vector3& normal); // return new velocity in a distribution
	Vector3 get_U(const Vector3& normal); // return new gamma*velocity in a distribution
	Species*	get_speciesPtr() { return species;};
};

#endif	//	ifndef __VMAXWELLN_H

