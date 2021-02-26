/*
====================================================================

PORT.CPP

0.99	(NTG 12-29-93) Separated into individual module from pic.h.
0.991	(JohnV 01-03-93) Aesthetics, compile.
0.992	(JohnV 03-23-94) Streamline includes, remove pic.h.

====================================================================
*/

#include "port.h"
#include "porttm.h"
#include	"fields.h"
#include "ptclgrp.h"


#ifdef _MSC_VER
using std::cout;
using std::cerr;
using std::endl;
#endif

PortTM::PortTM(oopicList <LineSegment> *segments, int _EFFlag) : 
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
	gamma0 = 2.4048;
	dt = 0;
	tOld = 0;
}

//--------------------------------------------------------------------
//	Apply boundary conditions to fields locally.


void PortTM::applyFields(Scalar t,Scalar dt)
{
   if(dt==0) return;
	// this could be cleaned up by using a funciton pointer sin for XY and bessj1 for RX
	// also gamma1 and M_PI would have to be selected
//	cout << "dt " << dt << endl;
//	cout << "time " << t << endl;
	time = t- dt/2;
	grid = fields->get_grid();
//	EnergyOut = 0.0;
	if (fields->getSub_Cycle_Iter() ==1) EnergyOut = 0.0;
	else if (fields->getSub_Cycle_Iter() ==0) EnergyOut = 0.0;
	A1 =  get_time_value(t);
//	cout << "A1 " << A1 << endl;
	if (grid->query_geometry() == ZRGEOM)
		{
		a = grid->getMKS(0, k2).e2();
		Scalar max_bessj1 = bessj1(1.841); //scaling bessj1 by it's peak
		for (int k = k1; k < k2; k++)
		{
		rforEr = (grid->getMKS(j1, k + 1).e2() +
		 grid->getMKS(j1, k).e2())/2.0;
		Er = A1*bessj1(gamma0*rforEr/a)/max_bessj1;
		intEr = grid->dl2(j1, k)*Er;
		fields->setIntEdl(j1, k, 2, intEr);
		if(EFFlag)
		{
		if (fields->getiC()[j2+normal][k].e2())
		{
		// Er = get_time_value(time)*bessj1(gamma0*rforEr/a)/max_bessj1;
		HonBoundary = (fields->getiL()[j2+shift][k].e3())*
 		(fields->getIntBdS()[j2+shift][k].e3()) + 
		normal*.5*(fields->getIMesh(j2,k).e2() + 
	      get_time_value_deriv(time)/(fields->getiC()[j2+normal][k].e2()));
		EnergyOut += normal*Er*HonBoundary*grid->dS(j2+normal,k).e1();
			}
		}
	}
		}
	if (grid->query_geometry() == ZXGEOM)
	{
	if (alongx2())
	{
	a = (grid->getMKS(j2,k2) - grid->getMKS(j1,k1)).e2();
	for (int k = k1; k < k2; k++)
	{
	Scalar yforEy = (grid->getMKS(j1, k + 1).e2() +
	 grid->getMKS(j1, k).e2())/2.0 - grid->getMKS(j1,k1).e2() ;
	Scalar Ey = A1*cos(M_PI*yforEy/a);
	Scalar intEy = grid->dl2(j1, k)*Ey;
	fields->setIntEdl(j1, k, 2, intEy);
	if(EFFlag)
	{
	if (fields->getiC()[j2+normal][k].e2())
	{
	HonBoundary = (fields->getiL()[j2+shift][k].e3())*
	(fields->getIntBdS()[j2+shift][k].e3()) + 
	normal*.5*(fields->getIMesh(j2,k).e2() + 
	  get_time_value_deriv(time)/(fields->getiC()[j2+normal][k].e2()));
	EnergyOut += normal*Ey*HonBoundary*grid->dS(j2+shift,k).e1();
	}
	}
	}
	}
	else 
	{
	a = (grid->getMKS(j2,k2) - grid->getMKS(j1,k1)).e1();
	for (int j = j1; j < j2; j++)
	{
	Scalar xforEx = (grid->getMKS(j, k1).e1() +
	 grid->getMKS(j+1, k1).e1())/2.0 - grid->getMKS(j1,k1).e1();
	Scalar Ex = A1*cos(M_PI*xforEx/a);
	Scalar intEx = grid->dl1(j, k1)*Ex;
	fields->setIntEdl(j, k1, 1, intEx);
	if(EFFlag)
	{
	if (fields->getiC()[j][k1+normal].e1())
	{
	HonBoundary = (fields->getiL()[j][k1+shift].e3())*
	(fields->getIntBdS()[j][k1+shift].e3()) + 
	normal*.5*(fields->getIMesh(j,k1).e2() + 
	  get_time_value_deriv(time)/(fields->getiC()[j][k1+normal].e1()));
	EnergyOut += normal*Ex*HonBoundary*grid->dS(j,k1+normal).e2();
	}
	}
	}
	}
		}
	if (fields->getFieldSub() == fields->getSub_Cycle_Iter()) EnergyOut /= (fields->getFieldSub());
}

//--------------------------------------------------------------------
//	Set the passive bc for fields at the port.  Currently just a
//	copy of those for conductor.

void PortTM::setPassives()
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
ParticleList& PortTM::emit(Scalar t,Scalar dt, Species *species)
{
	while(!particleList.isEmpty())
	{
		Particle* p = particleList.pop();
		delete p;
	}
   return particleList;
}

