/*j
 ====================================================================
 VMAXWELLN.CPP
 0.99 (KC, 01-14-96) birth of a new class
(Selma Cetiner, 06-29-2005) Change get_V to implement eqn. 16.2.1 in Birdsall and Langdon, 
				   "Plasma physics via computer simulation", pg 388. 
 ====================================================================
*/


#include "vmaxwelln.h"
#include "ptclgrp.h"
#include "boundary.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

using namespace std;

#ifndef DBL_MIN
#define DBL_MIN         1E-200
#endif

//--------------------------------------------------------------------
void MaxwellianFlux::CreateZeroDist()
{
	FluxArray = NULL;
}
//--------------------------------------------------------------------
//	Set vt to the specified vector.  Default units are m/s.

void MaxwellianFlux::setThermal(const Vector3& v, UNIT type)
{
	Maxwellian::setThermal(v,type);
}

//--------------------------------------------------------------------
//	Set v0 to the specified vector.  Default units are m/s.

void MaxwellianFlux::setDrift(const Vector3& v, UNIT type)
{
    DriftFlag = 0; //default flag, used if length of V is 0
                   // David, please verify that it is correct m.g.
	Maxwellian::setDrift(v,type); 
	
	
}


//--------------------------------------------------------------------
//	Set vc to the specified vector.  Default units are m/s.



void MaxwellianFlux::setCutoff(const Vector3& vl,const Vector3& vu, UNIT type)
{
	
	Vector3 sign1,sign2;
	
	
	vcl=vl;
	vcu=vu;

	//get the sign of the lower and upper cut-off velocities since this information is lost when
	//converting from EV units to (m/s)

	sign1 = Vector3( ((vcl.e1()+1.0)/fabs(vcl.e1()+1.0)) ,((vcl.e2()+1.0)/fabs(vcl.e2()+1.0)) ,
										((vcl.e3()+1.0)/fabs(vcl.e3()+1.0)) );
	sign2 = Vector3( ((vcu.e1()+1.0)/fabs(vcu.e1()+1.0)) ,((vcu.e2()+1.0)/fabs(vcu.e2()+1.0)) ,
										((vcu.e3()+1.0)/fabs(vcu.e3()+1.0)) );

		
	//	If the units are in eV, we must convert to m/s internally and apply the correct sign
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

	vLlimit = Vector3(vcl.e1(),vcl.e2(),vcl.e3());
	vUlimit = Vector3(vcu.e1(),vcu.e2(),vcu.e3());

	//keep the original lower and upper cut-offs for use in Maxwellian::get_V();
	vcl = vcl.jvDivide(vt);
	vcu = vcu.jvDivide(vt);
	Maxwellian::ArraySetUp();
	
}


//--------------------------------------------------------------------
//	Return a new velocity in a distribution with vt, vc, and v0 for 
// 	Maxwellian flux

Vector3 MaxwellianFlux::get_V(const Vector3& wall_normal)
{
	Vector3 v;
	Vector3 vLf, vUf, expvu2, expvl2, vFdist, ifvt ;
	
	Scalar defaultUpperV = SPEED_OF_LIGHT*gamma0; //(S.C.)//if no upper velocity limit specified 
							//this forces exp(-0.5*vcu^2/vt^2) to zero

	Scalar f = frand();

	v = Maxwellian::get_V();


	//account for no thermal velocity 
	//if use jvDivide(vt) and thermal v is zero get incorect results for MaxwellianFlux::get_V

	ifvt = Vector3( 0.0, 0.0 , 0.0);
	if (vt.e1() != 0.0)
		ifvt.set_e1(1.0/vt.e1());
	if (vt.e2() != 0.0)
		ifvt.set_e2(1.0/vt.e2());
	if (vt.e3() != 0.0)
		ifvt.set_e3(1.0/vt.e3());
	

	//set lower cut-off: (Vcut-off - Vdrift) for use in eqn 16.2.1
	
	vLf = Vector3(0.0,0.0,0.0);
		
	if ( (fabs(vLlimit.e1()) - fabs(v0.e1())) > 0.0 )
		vLf.set_e1(fabs(vLlimit.e1()) - fabs(v0.e1()));

	if ( (fabs(vLlimit.e2()) - fabs(v0.e2())) > 0.0 )
		vLf.set_e2( fabs(vLlimit.e2()) - fabs(v0.e2()));

	if ( (fabs(vLlimit.e3()) - fabs(v0.e3())) > 0.0 )
		vLf.set_e3( fabs(vLlimit.e3()) - fabs(v0.e3()));

	

	//set upper cut-off : (Vcut-off - Vdrift) for use in eqn 16.2.1

	
	if (vUlimit.e1() == 0.0 ){
		vUlimit.set_e1( defaultUpperV);
	}
	if (vUlimit.e2() == 0.0){
		vUlimit.set_e2(defaultUpperV);
	}
	if (vUlimit.e3() == 0.0){
		vUlimit.set_e3(defaultUpperV);
	}

	// Scott R; 2017
	// Aldan --- Initialization on vUf needs to be done here:
	vUf = Vector3(vUlimit.e1(),vUlimit.e2(),vUlimit.e3());

	if(vUlimit.e1() < defaultUpperV)		
	vUf.set_e1(fabs(vUlimit.e1()) - fabs(v0.e1()));

	if(vUlimit.e2()  < defaultUpperV)
	vUf.set_e2(fabs(vUlimit.e2()) - fabs(v0.e2())) ;

	if(vUlimit.e3()  < defaultUpperV)	
	vUf.set_e3(fabs(vUlimit.e3()) - fabs(v0.e3())) ;
			
	vLf = vLf.jvMult(ifvt);
	vUf = vUf.jvMult(ifvt);
	


	//implement equation 16.2.1 as in reference at start of file
		
	expvu2 = Vector3(exp(-0.5*sqr(vUf.e1())), exp(-0.5*sqr(vUf.e2())), exp(-0.5*sqr(vUf.e3())));
	expvl2 = Vector3(exp(-0.5*sqr(vLf.e1())), exp(-0.5*sqr(vLf.e2())), exp(-0.5*sqr(vLf.e3())));         
	
	vFdist= Vector3(fabs(v0.e1()) + vt.e1()*sqrt(-2.0*log((1.0-f)*expvl2.e1() + f*expvu2.e1())), 
				fabs(v0.e2()) + vt.e2()*sqrt(-2.0*log((1.0-f)*expvl2.e2() + f*expvu2.e2())),
			 	fabs(v0.e3()) + vt.e3()*sqrt(-2.0*log((1.0-f)*expvl2.e3() + f*expvu2.e3())));
	
	
	
	// since the velocities in a maxwellian flux disrtibution are forwards directed use the wall normals
	// to ensure the particles are directed into the simulation region from the defined surface

	if (wall_normal.e1()==1)
		v.set_e1(vFdist.e1());
	else if (wall_normal.e1()==-1)
		v.set_e1(-vFdist.e1());
	else if (wall_normal.e2()==1)
		v.set_e2(vFdist.e2());
	else if (wall_normal.e2()==-1)
		v.set_e2(-vFdist.e2());
		
		
	return v;
	
}

//--------------------------------------------------------------------
//	Return a new velocity in a distribution with vt, vc, and v0 for 
// 	Maxwellian flux

Vector3 MaxwellianFlux::get_U(const Vector3& wall_normal)
{
	Vector3 v = get_V(wall_normal);
	return (gamma0*v);
	
}

void MaxwellianFlux::ArraySetUp()
{
	nvel = 1000;
	FluxArray = new Vector3[nvel];
	Scalar v0local, vcllocal, vculocal;
	Scalar t1,t2,t3;
	Scalar dvi,dv,f,vtemp;
	Scalar flower,fupper,vlower,vupper,fmid;

	if ((vcl.magnitude() ==0)&&(vcu.magnitude()==0))
		{
			FluxArray[0] = Vector3(0,0,0);
			for (int e=1; e<=3; e++){
				v0local = v0.e(e)/vt.e(e);
				dvi = 3.3/((Scalar)(nvel-1));
				t2 = sqrt(M_PI)*v0local;
				t1 = exp(-sqr(v0local))+t2*erf(v0local);
				
				dv = dvi;
				fmid = 0;
				f = 0;
				vlower = 0;
				vtemp = vlower-v0local;
				flower = (1-f)*t1-f*t2-exp(-sqr(vtemp))+t2*erf(vtemp);
				vupper = vlower + dv;
				vtemp = vupper-v0local;
				fupper = (1-f)*t1-f*t2-exp(-sqr(vtemp))+t2*erf(vtemp);
				for (int n=1; n<nvel; n++){
					f = (Scalar)n/(Scalar)(nvel-1);
					vtemp = vlower-v0local;
					flower = (1-f)*t1-f*t2-exp(-sqr(vtemp))+t2*erf(vtemp);
					vtemp = vupper-v0local;
					fupper = (1-f)*t1-f*t2-exp(-sqr(vtemp))+t2*erf(vtemp);
					while ((flower*fupper)>0.0){
						dv*=2;
						vupper = vlower + dv;
						vtemp = vupper-v0local;
						fupper = (1-f)*t1-f*t2-exp(-sqr(vtemp))+t2*erf(vtemp);
					}
					while (dv>1e-8){
						dv /=2;
						vtemp = vlower+dv-v0local;
						fmid = (1-f)*t1-f*t2-exp(-sqr(vtemp))+t2*erf(vtemp);
						if (fmid<=0) 
							vlower+=dv;
						else
							vupper-=dv;
					}
					FluxArray[n].set(e,vlower); 
					dv = dvi;
				}
			}
		}
	else
		{
			for (int e=1; e<=3; e++){
				v0local = v0.e(e)/vt.e(e);
				vcllocal = vcl.e1()+v0local;
				vculocal = vcu.e1()+v0local;
				
				if (vcllocal<0)
					vcllocal=0;
				if (vculocal<0)
					vculocal=0;
				
				FluxArray[0].set(e,vcllocal);
				FluxArray[nvel-1].set(e,vculocal);
				dvi = (vculocal-vcllocal)/((Scalar)(nvel-1));
				t3 = sqrt(M_PI)*v0local;
				t1 = exp(-sqr(vcllocal-v0local))-t3*erf(vcllocal-v0local);
				t2 = -exp(-sqr(vculocal-v0local))+t3*erf(vculocal-v0local);
				
				dv = dvi;
				fmid = 0;
				vlower = vcllocal;
				f = 0;
				vtemp = vlower-v0local;
				flower = (1-f)*t1-f*t2-exp(-sqr(vtemp))+t3*erf(vtemp);
				vupper = vlower + dv;
				vtemp = vupper-v0local;
				fupper = (1-f)*t1-f*t2-exp(-sqr(vtemp))+t3*erf(vtemp);
				// for the 1 direction
				for (int n=1; n<nvel; n++){
					f = (Scalar)n/((Scalar)(nvel-1));
					vtemp = vlower-v0local;
					flower = (1-f)*t1-f*t2-exp(-sqr(vtemp))+t3*erf(vtemp);
					vtemp = vupper-v0local;
					fupper = (1-f)*t1-f*t2-exp(-sqr(vtemp))+t3*erf(vtemp);
					while ((flower*fupper)>0.0){
						dv*=2;
						vupper = vlower + dv;
						vtemp = vupper-v0local;
						fupper = (1-f)*t1-f*t2-exp(-sqr(vtemp))+t3*erf(vtemp);
					}
					while (dv>1e-8){
						dv /=2;
						vtemp = vlower+dv-v0local;
						fmid = (1-f)*t1-f*t2-exp(-sqr(vtemp))+t3*erf(vtemp);
						if (fmid<=0) 
							vlower+=dv;
						else
							vupper-=dv;
					}
					FluxArray[n].set(e,vlower);
					dv=dvi;
				}
			}
		}		
}
