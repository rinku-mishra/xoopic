#ifndef	__LASERPLASMASRC_H
#define	__LASERPLASMASRC_H

/*
====================================================================

LaserPlasmaSource.h

This class describes a laser-generated plasma source, and can create pairs
with the specified distribution. It is specified as a region which propagates in 
a specified direction.

1.0  (JohnV 06-13-00) Original code.
1.1  (Bruhwiler 09-25-00) Modified to work in r-z geometry.
1.2  (Bruhwiler 10-04-00) added ability to delay start of laser pulse

====================================================================
*/

#include "plsmasrc.h"

class LaserPlasmaSource : public PlasmaSource
{
  private:
    Vector2 v;         // propagation velocity of the laser pulse
    Scalar pulseWidth; // spatial length of the laser pulse
    Scalar delayTime ; // time delay before the laser pulse begins
    Scalar rFraction;  // used for variable weight loading in r-z geometry
  public:
    LaserPlasmaSource(Maxwellian* max1, Maxwellian* max2, Scalar sourceRate,
                      oopicList <LineSegment> *segments, Scalar np2c,
                      const ostring &analyticF, Vector2 v, Scalar pulseWidth,
                      Scalar _delayTime);
	~LaserPlasmaSource();
	virtual ParticleList& emit(Scalar t, Scalar dt, Species* species);
};

#endif	//	ifdef __LASERPLASMASRC_H

