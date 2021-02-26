/*
====================================================================

obmemit.cpp
====================================================================
*/

#if defined(_MSC_VER)
#include <algorithm>
#endif

#include "misc.h"
#include "obemit.h"
#include "fields.h"
#include "ptclgrp.h"

#if defined(__KCC) || defined(__xlC__)
using std::min;
#endif

//--------------------------------------------------------------------
//	Construct BeamEmitter object

OpenBoundaryBeamEmitter::OpenBoundaryBeamEmitter(MaxwellianFlux *max, Scalar current,
	oopicList <LineSegment> *segments, Scalar np2c, Scalar
	thetaDot)
        : BeamEmitter(max, current, segments, np2c, thetaDot){
  BCType = FREESPACE;
}

void OpenBoundaryBeamEmitter::setPassives()
{
  PoissonSolve *psolve=fields->getPoissonSolve();
  Grid *grid = fields->get_grid();
  if (alongx2())                                                          //      vertical
    {
      for (int k=k1; k<=k2; k++)
	{
	  fields->set_iC1(j1, k, 0);
	  fields->set_iC3(j1, k, 0);
	}
    }
  else if (k1==k2)                                                        //      horizontal
    {
      for (int j=j1; j<=j2; j++)
	{
	  fields->set_iC2(j, k1, 0);
	  fields->set_iC3(j, k1, 0);
	}
    }   
}



void OpenBoundaryBeamEmitter::applyFields(Scalar t, Scalar dt)
{
  if(fields->getElectrostatic()==0){
    if(init) BeamEmitter::initialize();
    if (get_xtFlag() ==0) { 
	 if (nIntervals == 0) {
		if (alongx2()) nIntervals = abs(k2 - k1);
		else nIntervals = abs(j2 - j1);
	 }
	 FArray = new Scalar[nIntervals+1];
	 
	 Vector2 component(0,0);
	 if (alongx2()) component.set_e2(1);
	 else component.set_e1(1);
	 Scalar dx = (p2 - p1)*component/nIntervals;
	 FArray[0] = 0;
	 int i;
	 for (i=1; i<=nIntervals; ++i) {
	 FArray[i] = FArray[i-1] + dx; 
	 }
	 FTotal = FArray[nIntervals];
	 for (i=1; i<=nIntervals; i++) FArray[i] /= FTotal;
    }
    else{
    computeLocationArray();
    Farray_compute = TRUE;
    }
    Grid *grid = fields->get_grid();
    Vector3 **intEdl = fields->getIntEdl();
    Scalar Fdl = 0;

   Vector2 component(0,0);
   if (alongx2()) component.set_e2(1);
   else component.set_e1(1);
   Scalar dx = ((p2 - p1)*component)/nIntervals;
    if (alongx1())
      {
	if(Boundary::get_normal() == 1)                                               //lower boundary, update intEdl[][k1]
	  {
	    for(int i=0; i<j2-j1; ++i)
	      {
		Fdl += grid->dl1(i+j1,k1);
		
		intEdl[i+j1][k1].set_e1(intEdl[j1][k1].e1()-emissionRate*dt*get_q()*FArray[static_cast<int> (Fdl/dx)]*FTotal/fields->get_epsi(i+j1,k1)*grid->dl1(i+j1,k1));
	      }
	  }
	else                                                          //upper boundary, update intEdl[][k2]
	  {
	    for(int i=0; i<j2-j1; ++i)
              {
		Fdl += grid->dl1(i+j1,k2);
                intEdl[i+j1][k2].set_e1(intEdl[j1][k2].e1()-emissionRate*dt*get_q()*FArray[static_cast<int> (Fdl/dx)]*FTotal/fields->get_epsi(i+j1,k2-1)*grid->dl1(i+j1,k2));
              }

	  }
      }
    else
      {
	if(Boundary::get_normal() == 1)                                               //left boundary, update intEdl[j1][]
          {
            for(int i=0; i<k2-k1; ++i)
              {
		Fdl += grid->dl2(j1,i+k1);
                intEdl[j1][i+k1].set_e2(intEdl[j1][k1].e2()-emissionRate*dt*get_q()*FArray[static_cast<int> (Fdl/dx)]*FTotal/fields->get_epsi(j1,i+k1)*grid->dl2(j1,i+k1));
              }
          }
        else                                                          //upper boundary, update intEdl[j2][]
          {
            for(int i=0; i<k2-k1; ++i)
              {
		Fdl += grid->dl2(j2,i+k1);
                intEdl[j2][i+k1].set_e2(intEdl[j2][k1].e2()-emissionRate*dt*get_q()*FArray[static_cast<int> (Fdl/dx)]*FTotal/fields->get_epsi(j2-1,i+k1)*grid->dl2(j2,i+k1));
              }

          }

      }
  }
  return;
}
