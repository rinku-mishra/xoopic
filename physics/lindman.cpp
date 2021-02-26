/*
====================================================================

LINDMAN.

Started spring 94 for a 290q project.
Lindman Boundary Conditions
This is based on the paper by E.L. Lindman "'Free-Space'
Boundary Conditions for the Time Dependent Wave Equation",
Journal of computational physics 18, 66-78 (1975)

Know problem in this BC as stated in Quad EM page 9 "...
boundary conditions such as Lindman's that however sometimes
fail. "

From Lindman's paper the method in section 5 of the paper is
being used.  Some changes from the paper is that the paper is in
XY and the BC is need in RZ.

====================================================================
*/

#include "port.h"
#include	"fields.h"
#include "ptclgrp.h"
#include "lindman.h"


Lindman::Lindman(oopicList <LineSegment> *segments) : Port(segments)
{
  // getting memory for old intEdl and old intBdS to get dD/dt

  Scalar tOld =0.0;
  Scalar alpha[3] = {.3264, .1272, .0309};
  Scalar Beta[3]= {.7375, .98384, .9996472};

  if (alongx2())	// vertical
	 {
		int kmax = MAX(k1,k2) + 1;
		oldHonBoundary = new Vector2[kmax];
		E = new Scalar[3][kmax];     //following three wave equations at the wall
		edgeE = new Scalar[kmax];
	 }
  if (alongx1()) // horizontal
	 {
		int jmax = MAX(j1,j2) + 1;
		oldHonBoundary = new Vector2[jmax];
		E = new Scalar[3][jmax];     //following three wave equations at the wall
		edgeE = new Scalar[jmax];
	 }


}

//--------------------------------------------------------------------
//	Apply boundary conditions to fields locally.

void Lindman::applyFields(Scalar t,Scalar dt)
{
   if(dt==0) return;
  Grid*   grid = fields->get_grid();

  Scalar dt = t-tOld;
  Scalar dtDist = dt * SPEED_OF_LIGHT;
  Scalar Lprime = L/dt;
  Scalar Rprime = R+Lprime;
  Scalar A, RidtC, idtC, plus, minus, Hphi, dErdl, Erdl, HonBoundary, Stuff, Hr,
			  dEphidl, Ephidl, Hz, dEzdl, Ezdl;

  if (alongx2())			//	vertical
	 {
		if (Normal()==1)               //      going to the right
	{
	  for (int k = MIN(k1,k2); k < MAX(k1,k2); k++)
		 {
			// TM mode

			// TE mode
		 }
	}
		else   // going left
	{
	  for (int k = MIN(k1,k2); k < MAX(k1,k2); k++)
		 {
	      // TM mode
	      
	      // TE mode
	    }
	}
    }
  else						// 	horizontal
    {
      if (Normal()==1)                       // going out (greater r)
	{
	  for (int j=MIN(j1,j2); j<MAX(j1,j2); j++)
	    {
	      // TM mode
	      
	      // TE mode
	    }
	}
      else  // going in
	{
	  for (int j=MIN(j1,j2); j<MAX(j1,j2); j++)
	    {
	    // TM mode

	    // TE mode 

	    }
	}
    }
  tOld=t;
}


//--------------------------------------------------------------------
//	Set the passive bc for fields at the port.  Currently just a
//	copy of those for conductor.

void Lindman::setPassives()
{
  int	inc;
  if (alongx2())			//	vertical
    {
      for (int k=MIN(k1,k2); k<MAX(k1,k2); k++)
	{
	  fields->set_iC2(j1, k, 0);
	  if (Normal()==1)
	    {
	      if (j1<(fields->getJ())) fields->set_iC1(j1+1, k, 0);
	      if (j1<(fields->getJ())) fields->set_iC2(j1+1, k, 0);
	      if (j1<(fields->getJ())) fields->set_iC3(j1+1, k, 0);
	    }
	  else
	    {
	      if (j1>0) fields->set_iC1(j1-1, k, 0);
	      if (j1>0) fields->set_iC2(j1-1, k, 0);
	      if (j1>0) fields->set_iC3(j1-1, k, 0);
	    }
	  fields->set_iC3(j1, k, 0);
	}
      fields->set_iC3(j1, k2, 0);
    }
  else						  //	horizontal
    {
      for (int j=MIN(j1,j2); j<MAX(j1,j2); j++)
	{
	  fields->set_iC1(j, k1, 0);
	  if (Normal()==1)
	    {
	      if (k1<(fields->getK())) fields->set_iC1(j, k1+1, 0);
	      if (k1<(fields->getK())) fields->set_iC2(j, k1+1, 0);
	      if (k1<(fields->getK())) fields->set_iC3(j, k1+1, 0);
	    }
	  else
	    {
	      if (k1>0) fields->set_iC1(j, k1-1, 0);
	      if (k1>0) fields->set_iC3(j, k1-1, 0);
	      if (k1>0) fields->set_iC2(j, k1-1, 0);
	    }
	  fields->set_iC3(j, k1, 0);
	}
      fields->set_iC3(j2, k1, 0);
    }
}

//--------------------------------------------------------------------
//	Lindman::emit() simply deletes Particles in its stack.  May add some
//	diagnostics for particles collected in the future.
#ifndef __linux__
#pragma argsused
#endif
ParticleList& Lindman::emit(Scalar t, Scalar dt)
{
  while(!particleList.isEmpty())
    {
      Particle* p = particleList.pop();
      delete p;
    }
  return particleList;
}


void Lindman::PassivesSource(int j, int k)
{
  if (alongx2())			//	vertical
    {				
      if (Normal()==1)               //      going to the right
	{
	  if (MIN(k1,k2)=0) 
	    {
	      constant = sqr(dtDist/(grid->dl2(j,k)));
 
	  for (int k = MIN(k1,k2)+1; k < MAX(k1,k2)-1; k++)
	    {
	      // TM mode
	        for (int n=0; n<3; n++)
		  {
		    const = sqr(dtDist/(grid->dl2(j,k)));
		    Esum += 2*E[n][k]-E[n-1][k] + const*(Alpha[n]*(edgeE[k+1]-2*edgeE[k]+edgeE[k-1]) + Beta[n]*(E[n][k+1]-2*E[n][k]+E[n][k-1]));
	      // TE mode
	    }
	}
      else   // going left
	{
	  for (int k = MIN(k1,k2); k < MAX(k1,k2); k++)
	    {
	      // TM mode
	      
	      // TE mode
	    }
	}
    }
  else						// 	horizontal
    {
      if (Normal()==1)                       // going out (greater r)
	{
	  for (int j=MIN(j1,j2); j<MAX(j1,j2); j++)
	    {
	      // TM mode
	      
	      // TE mode
	    }
	}
      else  // going in
	{
	  for (int j=MIN(j1,j2); j<MAX(j1,j2); j++)
	    {
	    // TM mode

	    // TE mode 

	    }
	}
    } 
}     
      
      

