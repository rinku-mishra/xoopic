/*
====================================================================

porttemmw.h

Plane wave emitting boundary condition for 
rectangular geometry.

Presently assumes that it is on the RHS.


====================================================================
*/

#ifndef	__PORT_TEM_MW_H
#define	__PORT_TEM_MW_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if defined(_MSC_VER)
#include <iomanip>
#include <iostream>
using std::ends;
#else

using namespace std;
#endif

#include "ovector.h"
#include "misc.h"
#include "particle.h"
#include "boundary.h"
#include "port.h"
class SpatialRegion;


class PortTEM_MW : public Port
{
 protected:
  Scalar pwFrequency, pwPhase, pwWavenumber, pwAmplitude, pwDelay;
  Scalar n0;
  ostring AnalyticN;  // analytic *transverse* shape functions
  ostring AnalyticEy;  // the user should normalize them to 1
  ostring AnalyticBz;
  int NF,EF,BF;  // Flags for analytic functions.

public:
  PortTEM_MW(Scalar _pwFrequency,Scalar _pwPhase, Scalar _pwWavenumber,
             Scalar _pwAmplitude, Scalar _pwDelay,Scalar _n0,
             oopicList <LineSegment> *segments,
             ostring _AnalyticN, ostring _AnalyticEy, ostring _AnalyticBz ) : Port(segments) {
    pwFrequency = _pwFrequency;
    pwPhase = _pwPhase;
    pwWavenumber = _pwWavenumber;
    pwAmplitude = _pwAmplitude;
    pwDelay = _pwDelay;
    n0 = _n0;
    NF=EF=BF=0;
    AnalyticN = _AnalyticN;
    AnalyticEy = _AnalyticEy;
    AnalyticBz = _AnalyticBz;
    if(AnalyticN != ostring("1.0")) { 
      NF=1;
      AnalyticN = AnalyticN + ostring("\n");
    }
    
    if(AnalyticEy != ostring("1.0")) {
      EF=1;
      AnalyticEy = AnalyticEy + ostring("\n");
    }
    if(AnalyticBz != ostring("1.0")) {
      BF=1;
      AnalyticBz = AnalyticBz + ostring("\n");
    }
  };
  virtual void	applyFields(Scalar t,Scalar dt)
     { 
        // At the time applyFields is called, B lags E by .5 dt
        updateEdgeFields(t,t-0.5*dt,dt);
        
     };

  // this function is meant to be applied when a shift occurs
  // which has a different position in the leaprfrog from
  // applyFields, hence the different function.
  virtual void applyShiftBC(Scalar t, Scalar dt) {
     updateEdgeFields(t,t,dt);
     toNodes();
  }

  // update E and B at times tE and tB for E and B.
  virtual void updateEdgeFields(Scalar tE,Scalar tB,Scalar dt) ;
  virtual void toNodes();

};

#endif	//	ifndef __PORT_TEM_MW_H
