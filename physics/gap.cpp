/*
====================================================================

Gap

Started spring 94 for a 290q project.
Using Surface Impedance Boundary Conditions (SIBC)
This work is following the paper  Finite-Difference Time-Domain
Implementation of Surface Impedance Boundary Conditions  by J.H. Beggs,
Luebbers, Yee, and Kunz in IEEE Transactions on Antennas
and Propagation, Vol. 40, No. 1, January 1992.

Changes from the paper:
	In the paper a new H is found on the boundary.  This is changed
so that E is given on the boundary.

The boundary condition that is discussed on page 9 of 2D electronmagnetics
and PIC on a quadrilateral mesh by Bruce Langdon is a special case of
the boundary condtion programed here.  If L=0 and R=mu0*c = 120PI ohms.

0.99	(KC 06-94) Initial code.
0.991	(JohnV 11-17-94) Aesthetics, add additional comments, generalize
		for either orientation.

====================================================================
*/

#include	"fields.h"
#include "gap.h"
#ifdef UNIX
#include "ptclgrp.h"
#endif

Gap::Gap(oopicList <LineSegment> *segments, int _EFFlag)
	: Port(segments)
{
	EFFlag = _EFFlag;
//	IEFlag = _IEFlag;
	BoundaryType = NOTHING;
	dt = 0;
	tOld = 0;
	time = 0;
	if(EFFlag)
		{
			EnergyFlux = &EnergyOut;
			EnergyOut =0.0;
			if (normal==1)
				shift = 0;
			else
				shift = -1;
		}
}

//--------------------------------------------------------------------
//	Apply boundary conditions to fields locally.

void Gap::applyFields(Scalar t,Scalar dt)
{
   if(dt==0) return;
	time = t - dt/2;
//	if ((fields->getSub_Cycle_Iter) == 1) EnergyOut = 0.0;
//	else if ((fields->getSub_Cycle_Iter) ==0) EnergyOut = 0.0;
	if (fields->getSub_Cycle_Iter() ==1) EnergyOut = 0.0;
	else if (fields->getSub_Cycle_Iter() ==0) EnergyOut = 0.0;
	grid = fields->get_grid();

	Scalar newE = get_time_value(t);
	Scalar newEdl; 
	if (alongx1())
	  for (int j=MIN(j1,j2); j<MAX(j1,j2); j++)
	  {
	    newEdl = newE*grid->dl1(j, k1);
	    fields->setIntEdl(j, k1, 1, newEdl);
	    if(EFFlag)
	    {
	      if (fields->getiC()[j][k1+normal].e1())
	      {
		HonBoundary = fields->getiL()[j][k1+shift].e3()*
		  fields->getIntBdS()[j][k1+shift].e3()+
		  normal*.5*(fields->getIMesh(j,k1).e1()+
			     get_time_value_deriv(time)/fields->getiC()[j][k1+normal].e1());
		EnergyOut += normal*get_time_value(time)*HonBoundary*grid->dS(j,k1+shift).e2()/(grid->dl(j,k2+shift).e3());
	      }
	    }
	  }
	else
	  for (int k=MIN(k1, k2); k<MAX(k1, k2); k++)
	  {
	    newEdl = newE*grid->dl2(j1, k); 
	    fields->setIntEdl(j1, k, 2, newEdl);
	    if(EFFlag)
	    {
	      if (fields->getiC()[j1+normal][k].e2())
	      {
		HonBoundary = fields->getiL()[j1+shift][k].e3()*
		  fields->getIntBdS()[j1+shift][k].e3() + 
		  normal*.5*(fields->getIMesh(j1,k).e2() + 
			     get_time_value_deriv(time)/fields->getiC()[j1+normal][k].e2());
		Scalar g = grid->dl2(j2+shift,k);
		if (g!=0)
		  EnergyOut += normal*get_time_value(time)*HonBoundary*grid->dS(j1+shift,k).e1()/g;
									}
					}
			}
}

//--------------------------------------------------------------------
//	Set the passive bc for fields at the port.  Currently just a
//	copy of those for conductor.

void Gap::setPassives()
{
	if (alongx1())   							//	horizontal
	{
		for (int j=MIN(j1,j2); j<MAX(j1,j2); j++)
		{
			fields->set_iC1(j, k1, 0);
			fields->set_iC3(j, k1, 0);
		}
		fields->set_iC3(j2, k1, 0);
	}
	else             							//	vertical
	{
		for (int k=MIN(k1,k2); k<MAX(k1,k2); k++)
		{
			fields->set_iC2(j1, k, 0);
			fields->set_iC3(j1, k, 0);
		}
		fields->set_iC3(j1, k2, 0);
	}
}

//--------------------------------------------------------------------
//	Gap::emit() simply deletes Particles in its stack.  May add some
//	diagnostics for particles collected in the future.
#if !defined __linux__ && !defined _WIN32
#pragma argsused
#endif
ParticleList& Gap::emit(Scalar t,Scalar dt)
{
	while(!particleList.isEmpty())
	{
		Particle* p = particleList.pop();
		delete p;
	}
	return particleList;
}


