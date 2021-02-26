/*
	 ====================================================================

	 LaserPlasmaSrc.cpp

	 This class describes a laser plasma source, and can create pairs
	 with the specified distribution in a moving laser pulse.

	 1.0  (JohnV 06-13-00) Original code.
	 1.1  (Bruhwiler 10-04-00) Added delay time capability.

	 ====================================================================
	 */

#if defined(_MSC_VER)
#include <stdlib.h>
#endif

#include "laserplasmasrc.h"
#include "maxwelln.h"
#include "fields.h"
#include "eval.h"
#include "misc.h"

#include "sptlrgn.h"
#include <globalvars.h>

#if defined(__KCC) || defined(__xlC__)
using std::min;
using std::max;
#endif


LaserPlasmaSource::LaserPlasmaSource(Maxwellian* max1, Maxwellian* max2, Scalar _sourceRate, 
  oopicList <LineSegment> *segments, Scalar np2c,const ostring &_analyticF, Vector2 _v, Scalar
  _pulseWidth, Scalar _delayTime)
: PlasmaSource(max1, max2, _sourceRate, segments, np2c, _analyticF)
{
	pulseWidth = _pulseWidth;
  delayTime  = _delayTime;
	v = _v;
}

LaserPlasmaSource::~LaserPlasmaSource()
{
}

ParticleList& LaserPlasmaSource::emit(Scalar t, Scalar dt, Species* species_to_push)
{
	if (init) { 
		initialize();
		static Vector2 vI = (v.e1() !=0) ? Vector2(1,0) : Vector2(0,1);
	} 

	// Don't begin until the delay time has elapsed.
  if ( theSpace->getTime() < delayTime ) return particleList;

	if (species==species_to_push){
		Vector2 delta1, delta2;
		if (v.e1() != 0) {

#if defined(_MSC_VER)
		  delta2 = Vector2(__min(v.e1()*(t-delayTime), deltaP.e1()), deltaP.e2());
		  delta1 = Vector2(__min(__max(v.e1()*(t-delayTime) - pulseWidth, (Scalar)0.0), deltaP.e1()), 0);
#else
		  delta2 = Vector2(min(v.e1()*(t-delayTime), deltaP.e1()), deltaP.e2());
		  delta1 = Vector2(min(max(v.e1()*(t-delayTime) - pulseWidth, (Scalar)0.0), deltaP.e1()), 0);
#endif
		} 
		else {
#if defined(_MSC_VER)
		  delta2 = Vector2(deltaP.e1(), __min(v.e2()*(t-delayTime), deltaP.e2()));
		  delta1 = Vector2(0, __min(__max(v.e2()*(t-delayTime) - pulseWidth, (Scalar)0.0), deltaP.e2()));
#else
		  delta2 = Vector2(deltaP.e1(), min(v.e2()*(t-delayTime), deltaP.e2()));
		  delta1 = Vector2(0, min(max(v.e2()*(t-delayTime) - pulseWidth, (Scalar)0.0), deltaP.e2()));
#endif
		}

    extra += sourceRate * dt * get_time_value( (t-delayTime) )
           * species->get_subcycleIndex()
           / species->get_supercycleIndex()
           * ( delta2.e1() - delta1.e1() )
           * ( delta2.e2() - delta1.e2() )
           / ( deltaP.e1() * deltaP.e2() );
		int N = (int) extra;
		Vector3 u = 0;							// pick velocity
		Vector2 xMKS, xc;

		// Some preliminary calculations for case of cylindrical geometry
		// Uniform macro-particle density (variable weighting) is used.
    Scalar rMin=0.;
    Scalar rMax=0.;
    Scalar w0=0.;
    Scalar w1=0.;
    Scalar np2cMax=0.;
    if(!is_xy) {
 			rMin = p1.e2()    +delta1.e2();
 			rMax = delta2.e2()-delta1.e2();
      w0 = rMin/rMax;
      w1 = (rMax - rMin)/rMax;
      np2cMax = 2. * np2c / ( 1. + rMin/rMax );
		}

		for (int i=0; i < N; i++) {
			xc.set_e1( p1.e1() + delta1.e1() + 
                 ( delta2.e1()-delta1.e1() ) * frand() );
      // Cartesian geometry -- e2 direction is handled in same was as e1
			if(is_xy) {
				xc.set_e2( p1.e2() + delta1.e2() +
                   ( delta2.e2()-delta1.e2() ) * frand() );
			}
      // Uniform macro-particle density (variable weighting) is used.
			else {
        rFraction = w0 + w1 * frand();
				xc.set_e2(rMax*rFraction);
			}
			u=maxwellian1->get_U();
			Vector2	x = fields->getGridCoords(xc);
			u=maxwellian2->get_U();
      if (is_xy) {
        particleList.add(new Particle(x, u, species,  np2c) );
        particleList.add(new Particle(x, u, species2, np2c) );
			}
      else {
        particleList.add(new Particle(x, u, species,  np2cMax*rFraction, TRUE) );
        particleList.add(new Particle(x, u, species2, np2cMax*rFraction, TRUE) );
			}
		}
		extra -= N;
	}
	return particleList;
}
