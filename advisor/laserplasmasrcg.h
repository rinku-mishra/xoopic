#ifndef	__LASERPLASMASRCG_H
#define	__LASERPLASMASRCG_H

/*
====================================================================

LaserPlasmaSourceg.h

1.0	 (JohnV 06-13-96) Original code.
1.01 (Bruhwiler, 10-04-2000) added delayTime.

====================================================================
*/

#include "plsmsrcg.h"

class LaserPlasmaSourceParams : public PlasmaSourceParams
{
protected:
	ScalarParameter velocity;
	IntParameter x1Flag;
	ScalarParameter pulseWidth;
  ScalarParameter delayTime;

public:
	LaserPlasmaSourceParams(GridParams* _GP, SpatialRegionGroup* srg);
	virtual Boundary* CreateCounterPart();
};

#endif


