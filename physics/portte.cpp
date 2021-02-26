/*
====================================================================

PORT.CPP

0.99	(NTG 12-29-93) Separated into individual module from pic.h.
0.991	(JohnV 01-03-93) Aesthetics, compile.
0.992	(JohnV 03-23-94) Streamline includes, remove pic.h.

====================================================================
*/

#include "port.h"
#include "portte.h"
#include	"fields.h"
#include "ptclgrp.h"


#ifdef _MSC_VER
using std::cout;
using std::cerr;
using std::endl;
#endif

PortTE::PortTE(oopicList <LineSegment> *segments, int _EFFlag) : 
        Boundary(segments)
{
  	if(segments->nItems() > 1) {
	  cout << "Warning, Port-derived boundaries can only have 1 segment.\n";
	  cout << "Check your input file.\n";
	}
	BCType=DIELECTRIC_BOUNDARY;

	EFFlag = _EFFlag;
	if(EFFlag)
	  {
		 EnergyFlux = &EnergyOut;
		 EnergyOut =0.0;
		 if (normal==1)
			shift = 0;
		 else
			shift = -1;
	  }
	gamma1 = 3.832;
	tOld = 0;
	dt = 0;
	time = 0;
}

//--------------------------------------------------------------------
//	Apply boundary conditions to fields locally.

void PortTE::applyFields(Scalar t,Scalar dt)
{
   if(dt==0) return;
	// this could be cleaned up by using a funciton pointer sin for XY and bessj1 for RX
	// also gamma1 and M_PI would have to be selected
   time = t - dt/2;
   grid = fields->get_grid();
   A1 = get_time_value(t);
   EnergyOut = 0.0;
   if (grid->query_geometry() == ZRGEOM)
   {
     a = grid->getMKS(0, k2).e2();	
     for (int k = k1; k < k2; k++)
     {
       rforEphi = grid->getMKS(j1, k).e2();
       Ephi = A1*bessj1(gamma1*rforEphi/a);
       intEphi = grid->dl3(j1, k)*Ephi;
       fields->setIntEdl(j1, k, 3, intEphi);
       if(EFFlag)
       {
	 if (fields->getiC()[j2+normal][k].e3())
	 {
	   HonBoundary = (fields->getiL()[j2+shift][k].e2())*
	     (fields->getIntBdS()[j2+shift][k].e2()) +normal*( 
	       (fields->getiL()[j2][k].e1())*(fields->getIntBdS()[j2][k].e1()) -
	       (fields->getiL()[j2][k-1].e1())*fields->getIntBdS()[j2][k-1].e1() +
	       fields->getIMesh(j2,k).e3() +
	       get_time_value_deriv(time)/(fields->getiC()[j2+normal][k].e3()));
	   EnergyOut += normal*Ephi*HonBoundary*grid->dS(j2+normal,k).e1();
	 }
       }
     }
   }
   if (grid->query_geometry() == ZXGEOM)
   {
     if (alongx2())
     {
       a = grid->getMKS(j2, k2).e2() - grid->getMKS(j1,k1).e2();	
       for (int k = k1; k < k2; k++)
       {
	 Scalar yforEz = grid->getMKS(j1, k).e2() - grid->getMKS(j1,k1).e2();
//	Scalar Ez = A1*sin(M_PI*yforEz/a);
// Gauss
//
	 Scalar gauss_w = 0.001/11.;
	 Scalar omegag = 18.e12;
	 Scalar gauss_rl = gauss_w*gauss_w*omegag*0.5*iSPEED_OF_LIGHT;
	 Scalar zf = 0.001*0.5;
	 Scalar zb = grid->getMKS(j1,k1).e2();
	 Scalar zp02 = gauss_rl*gauss_rl;
	 Scalar zp2 = (zb - zf)*(zb - zf);
	 Scalar wz = gauss_w*sqrt(1. + zp2/zp02);
	 Scalar wz2 = wz*wz;
	 Scalar Rz = (zb - zf) + zp02/(zb - zf);
	 Scalar phig = atan((zb-zf)/gauss_rl);
	 Scalar kg = omegag*iSPEED_OF_LIGHT;
	 Scalar Agw = 1.e2;
	 
	 Scalar rt2 = (yforEz - 0.5*a)*(yforEz - 0.5*a);
	 Scalar Ez = (A1*Agw*gauss_w*exp(-rt2/wz2)/wz)*(
	   cos(0.5*kg*rt2/Rz + kg*(zb) - phig)*cos(omegag*t) + 
	   sin(0.5*kg*rt2/Rz + kg*(zb) - phig)*sin(omegag*t));
	 
	 Scalar intEz = grid->dl3(j1, k)*Ez;
	 fields->setIntEdl(j1, k, 3, intEz);
	 if(EFFlag)
	 {
	   if (fields->getiC()[j2+normal][k].e3())
	   {
	     HonBoundary = (fields->getiL()[j2+shift][k].e2())*
	       (fields->getIntBdS()[j2+shift][k].e2()) +normal*( 
		 (fields->getiL()[j2][k].e1())*(fields->getIntBdS()[j2][k].e1()) -
		 (fields->getiL()[j2][k-1].e1())*fields->getIntBdS()[j2][k-1].e1() +
		 fields->getIMesh(j2,k).e3() +
		 get_time_value_deriv(time)/(fields->getiC()[j2+normal][k].e3()));
	     EnergyOut += normal*Ez*HonBoundary*grid->dS(j2+normal,k).e1();
	   }
	 }
       }	
     }
     else
     {
       a = grid->getMKS(j2, k2).e1() -  grid->getMKS(j1, k1).e1();	
       for (int j = j1; j < j2; j++)
       {
	 Scalar xforEz = grid->getMKS(j, k1).e1() - grid->getMKS(j1, k1).e1();
	 Scalar Ez = A1*sin(M_PI*xforEz/a);
	 Scalar intEz = grid->dl3(j, k1)*Ez;
	 fields->setIntEdl(j, k1, 3, intEz);
	 if(EFFlag)
	 {
	   if (fields->getiC()[j][k1+normal].e3())
	   {
	     HonBoundary = (fields->getiL()[j][k1+shift].e2())*
	       (fields->getIntBdS()[j][k1+shift].e2()) +normal*( 
		 (fields->getiL()[j][k1].e1())*(fields->getIntBdS()[j][k1].e1()) -
		 (fields->getiL()[j-1][k1].e1())*fields->getIntBdS()[j][k1].e1() +
		 fields->getIMesh(j,k1).e3() +
		 get_time_value_deriv(time)/(fields->getiC()[j][k1+normal].e3()));
	     EnergyOut += normal*Ez*HonBoundary*grid->dS(j,k1+normal).e2();
	   }
	 }
       }	
     }
   }
}

//--------------------------------------------------------------------
//	Set the passive bc for fields at the port.  Currently just a
//	copy of those for conductor.

void PortTE::setPassives()
{
	if (alongx2())								//	vertical
	{
		for (int k=MIN(k1,k2); k<MAX(k1,k2); k++)
		{
			fields->set_iC2(j1, k, 0);       	
			fields->set_iC3(j1, k, 0);
		}
		fields->set_iC3(j1, k2, 0);
	}
	else											//	horizontal
	{
		for (int j=MIN(j1,j2); j<MAX(j1,j2); j++)
      {
			fields->set_iC1(j, k1, 0);
			fields->set_iC3(j, k1, 0);
		}
		fields->set_iC3(j2, k1, 0);
	}
}

//--------------------------------------------------------------------
//	Port::emit() simply deletes Particles in its stack.  May add some
//	diagnostics for particles collected in the future.
#if !defined __linux__ && !defined _WIN32
#pragma argsused
#endif
ParticleList& PortTE::emit(Scalar t,Scalar dt, Species *species)
{
	while(!particleList.isEmpty())
	{
		Particle* p = particleList.pop();
		delete p;
	}
   return particleList;
}
