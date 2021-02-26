/*
 ====================================================================

 MAXWELLN.CPP

 0.99	(JohnV, 01-03-93) Separate from pic.cpp, add comments.
 0.991 (JohnV 01-27-95) Add Species.
 0.992	(JohnV 08-30-95) Fix destructor to set v_array = NULL after delete.
 0.995 (KC, 01-14-96) Major changes: separated MaxwellianFlux 
 changed some calls.
 0.996 (KC, 02-05-98) Still had some problems with the defintion of T.
       for the record E = 1/2 m v_i^2 = 1/2 k T_i where i is the direction.
1.0	(PVS 11.04.05) correct relativistic loading of maxwellian
 ====================================================================
 */

#include "maxwelln.h"
#include "ptclgrp.h"
#include "boundary.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

// Exceptions
#include <oops.h>

#ifdef _MSC_VER
#include <iomanip>
#include <iostream>
#include <fstream>
using std::cout;
using std::endl;
#else

using namespace std;
#endif

#ifndef DBL_MIN
#define DBL_MIN  1.0e-200
#endif

//--------------------------------------------------------------------
//	Create Maxwellian with all zeroes.

void Maxwellian::CreateZeroDist(Species *s)
{
	cutoffFlag = 0;
  species = s;
  v0 = vt = vcu = vcl = Vector3(0,0,0);
	gamma0 = 1;
	beta = 0;
	u0normal = v0normal = Vector3(0,0,0);
  v_array = NULL;
	
}

//--------------------------------------------------------------------
//	Deallocates memory for Maxwellian object

Maxwellian::~Maxwellian()
{
	delete[] v_array;
	v_array = NULL;
}

//--------------------------------------------------------------------
//	Set vt to the specified vector.  Default units are m/s.

void Maxwellian::setThermal(const Vector3& v, UNIT type) 
	/* throw(Oops) */ {
  vt = v;
// If the units are in eV, we must convert to m/s internally:
// note that 1/2 kT = 1/2 mv^2
  if(type==EV) {
    Scalar gg = 1.0 + 0.5*iSPEED_OF_LIGHT_SQ*vt.magnitude()*
	              fabs(ELECTRON_CHARGE)/(species->get_m());
    if(gg<1.4) { // beta < .7
      vt *= fabs(ELECTRON_CHARGE)/species->get_m();
      vt = vt.ComponentSqrt();
    } else{
	vt = SPEED_OF_LIGHT*sqrt(1.0-1.0/(gg*gg))*vt/vt.magnitude();
   }
  }
  if(vt.magnitude()>SPEED_OF_LIGHT) {
    Oops oops("Maxwellian::setThermal: Thermal velocity is superluminal, "
	"inversion in maxwellian failed.");
    throw oops;
  }

}

//--------------------------------------------------------------------
//	Set vc to the specified vector.  Default units are m/s.

void Maxwellian::setCutoff(const Vector3& vl,const Vector3& vu, UNIT type)
{
  vcl = vl;
	vcu = vu;
	if ((vcl.magnitude()==0) && (vcu.magnitude()==0)){
		cutoffFlag = 0;
	}
	else{
		cutoffFlag = 1;
		//	If the units are in eV, we must convert to m/s internally:
		if (type==EV)
			{
    			  Scalar ggl = 1.0 + iSPEED_OF_LIGHT_SQ*vcl.magnitude()*
                      			     fabs(ELECTRON_CHARGE)/(species->get_m());
    			  if(ggl<1.4) { // beta < .7
				vcl *= 2*fabs(ELECTRON_CHARGE)/species->get_m();
				vcl = vcl.ComponentSqrt();
    			  } else{
        			vcl = SPEED_OF_LIGHT*sqrt(1.0-1.0/(ggl*ggl))*vcl/vcl.magnitude();
   			  }
    			  Scalar ggu = 1.0 + iSPEED_OF_LIGHT_SQ*vcu.magnitude()*
                      			     fabs(ELECTRON_CHARGE)/(species->get_m());
    			  if(ggu<1.4) { // beta < .7
				vcu *= 2*fabs(ELECTRON_CHARGE)/species->get_m();
				vcu = vcu.ComponentSqrt();
    			  } else{
        			vcu = SPEED_OF_LIGHT*sqrt(1.0-1.0/(ggu*ggu))*vcu/vcu.magnitude();
   			  }

			}

		vcl = vcl.jvDivide(vt);
		vcu = vcu.jvDivide(vt);
		if (fabs(vcu.magnitude())>3.3)
			cout << "Upper cutoff larger than most simulation will be able to resolve." << endl;
		ArraySetUp();
	}
}

//--------------------------------------------------------------------
//	Set v0 to the specified vector.  Default units are m/s.

void Maxwellian::setDrift(const Vector3& v, UNIT type)
{
  v0 = v;
	Scalar vmag = v0.magnitude();
	if (vmag != 0.0){
		v0normal = v0/(vmag);
		//	If the units are in eV, we must convert to m/s internally:
		if (type==EV)
			{
				//gamma0 = v0.magnitude()*iSPEED_OF_LIGHT_SQ*fabs(ELECTRON_CHARGE)/species->get_m()+1;
				Scalar gammam1 = v0.magnitude()*iSPEED_OF_LIGHT_SQ*fabs(ELECTRON_CHARGE)/species->get_m();
				gamma0=gammam1+1;
				beta = sqrt((Scalar)1.0-1/sqr(gamma0));
				vmag = beta*SPEED_OF_LIGHT;
				v0 = v0normal*vmag;
				/*old way*/
				/*				v0 *= iSPEED_OF_LIGHT_SQ*fabs(ELECTRON_CHARGE)/species->get_m();
									v0 +=1;
									v0 = v0.jvMult(v0);
									v0 = Vector3(1.0,1.0,1.0)-(Vector3(1.0,1.0,1.0)).jvDivide(v0);
									v0 = SPEED_OF_LIGHT*v0.ComponentSqrt();*/
			}
		else
			{
				beta = v0.magnitude()*iSPEED_OF_LIGHT;
				gamma0 = 1/sqrt(1-beta*beta);
				vmag = v0.magnitude();
			}
		u0 = gamma0*v0;
		Scalar umag = u0.magnitude();
		iv0 = 1/vmag;
		u0normal = u0/(umag);
	}
	else{ 
		u0normal = 0;
		v0normal = 0;
		beta = 0;
		gamma0 = 1;
		u0 = 0;
		v0 = 0;
		iv0 = 0;    // this could hide problems
	}
	
}

//--------------------------------------------------------------------
//	Return the drift velocity in the units requested.

Vector3 Maxwellian::get_v0(UNIT type)
{
  if (type == MKS) return v0;
  else return species->get_m()*(gamma0-1)*SPEED_OF_LIGHT_SQ/fabs(ELECTRON_CHARGE);
}

//--------------------------------------------------------------------
//	Return the thermal velocity in the units requested.

Vector3 Maxwellian::get_vt(UNIT type)
{
  if (type == MKS) return vt;
	else return species->get_m()*vt.jvMult(vt)/fabs(ELECTRON_CHARGE);
}

//--------------------------------------------------------------------
//	Return the cutoff velocity in the units requested.

Vector3 Maxwellian::get_vc(UNIT type)
{
  if (type == MKS) return vcu;
  else return 0.5*species->get_m()*vcu.jvMult(vcu)/fabs(ELECTRON_CHARGE);
}

//--------------------------------------------------------------------
//	Return a new velocity in a distribution with vt, vc, and v0
Vector3 Maxwellian::get_V()
{
	Vector3 v;
	Scalar temp1,temp2,temp3;
	if (cutoffFlag){

		temp1 = (nvel-1)*frand2();
		temp2 = (nvel-1)*frand2();
		temp3 = (nvel-1)*frand2();
		v = vt.jvMult(Vector3((1-temp1+(int)temp1)*v_array[(int)temp1].e1()+(temp1-(int)temp1)*v_array[(int)temp1+1].e1(),
													(1-temp2+(int)temp2)*v_array[(int)temp2].e2()+(temp2-(int)temp2)*v_array[(int)temp2+1].e2(),
													(1-temp3+(int)temp3)*v_array[(int)temp3].e3()+(temp3-(int)temp3)*v_array[(int)temp3+1].e3()));
		//		v = vt.jvMult(cutoff());
	}
	else{
		v = vt.jvMult(Vector3(normal(),normal(),normal()));
	}
	if (gamma0<1.4)
		return (v0+v);
	else
		{
			Scalar gammat=(Scalar)1.0/sqrt((Scalar)1.0 - v.dotprod(v)*iSPEED_OF_LIGHT_SQ);
			Scalar v02=v0.dotprod(v0);
			Vector3 ulab=gammat*(gamma0*v0 + v + ((gamma0-(Scalar)1.0)/v02)*(v.dotprod(v0))*v0);
			Vector3 vprime=ulab/sqrt((Scalar)1.0 + ulab.dotprod(ulab)*iSPEED_OF_LIGHT_SQ);

			return (vprime);
		}
}
//--------------------------------------------------------------------
//	Return a new velocity in a distribution with vt, vc, and v0
Vector3 Maxwellian::get_U()
{
	Vector3 v=get_V();
	return (sqrt((Scalar)1.0 + v.dotprod(v)*iSPEED_OF_LIGHT_SQ)*v);
}

Vector3 Maxwellian::cutoff()
{
	/*returns a normally distributed deviate with zero mean and 1/Sqrt(2) variance.*/
	/*that has a cutoff above vcu and below vcl, Rvu=exp(-vcu^2) and Rvl=exp(-vcl^2)*/
	/*The way that we have defined v thermal this returns gaussian with a v thermal*/
	/*of one. */
	static int iset1=0;
	static Scalar gset1;
	static int iset2=0;
	static Scalar gset2;
	static int iset3=0;
	static Scalar gset3;
	Scalar fac,r2,v1,v2;
	Scalar out1,out2,out3;
	
	if(iset1==0){
		do{
			v1=2.0*frand()-1.0;
			v2=2.0*frand()-1.0;
			r2 = v1*v1+v2*v2;
		} while (r2>=1.0);
		fac=sqrt(-log(r2*(1-Rvu.e1())+Rvu.e1())/r2);
		gset1=v1*fac;
		iset1 =1;
		out1=v2*fac;
	}
	else{
		iset1=0;
		out1 = gset1;
	}

	if(iset2==0){
		do{
			v1=2.0*frand()-1.0;
			v2=2.0*frand()-1.0;
			r2 = v1*v1+v2*v2;
		} while (r2>=1.0);
		fac=sqrt(-log(r2*(1-Rvu.e2())+Rvu.e2())/r2);
		gset2=v1*fac;
		iset2 = 1;
		out2=v2*fac;
	}
	else{
		iset2=0;
		out2 = gset2;
	}
	
	if(iset3==0){
		do{
			v1=2.0*frand()-1.0;
			v2=2.0*frand()-1.0;
			r2 = v1*v1+v2*v2;
		} while (r2>=1.0);
		fac=sqrt(-log(r2*(1-Rvu.e3())+Rvu.e3())/r2);
		gset3=v1*fac;
		iset3 = 1;
		out3=v2*fac;
	}
	else{
		iset3=0;
		out3 = gset3;
	}
	return (Vector3(out1,out2,out3));
}

void Maxwellian::ArraySetUp()
{

	//needs a little more work--look at pdp1

	Scalar f, dv, dvi, fmid;
	Scalar vlower, flower, vupper, fupper;
	Scalar diff_erf, erfvl;

	nvel = 1000;
	v_array = new Vector3[nvel];
	v_array[0] = vcl;
	v_array[nvel-1] = vcu;

	for (int e=1; e<=3; e++){
		dvi = (vcu.e(e)-vcl.e(e))/((Scalar)nvel);
		dv = dvi;
		erfvl = erf(vcl.e(e));
		diff_erf = erf(vcu.e(e))-erfvl;
		fmid = 0;
		vlower = vcl.e(e);
		f = 0;
		flower = erf(vlower) - f*diff_erf - erfvl;
		vupper = vlower + dv;
		fupper = erf(vupper) - f*diff_erf - erfvl;
		for (int n=1; n<(nvel-1); n++){
			f = (Scalar)n/(Scalar)(nvel-1);
			flower = erf(vlower) - f*diff_erf - erfvl;
			fupper = erf(vupper) - f*diff_erf - erfvl;
			while ((flower*fupper)>0.0){
				dv*=2;
				vupper = vlower + dv;
				fupper = erf(vupper) - f*diff_erf - erfvl;
			}
			while (dv>1e-8){
				dv /=2;
				fmid = erf(vlower+dv) - f*diff_erf - erfvl; 
				if (fmid<=0) 
					vlower+=dv;
				else
					vupper-=dv;
			}
			v_array[n].set(e,vlower);  
		}
	}
}
