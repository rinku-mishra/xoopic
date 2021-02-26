/*
=====================================================================
lnchwag.h

Definitions for the LaunchWave class.

0.9	(JohnV 12-05-94) Initial draft to hook up to advisor.
		
=====================================================================
*/

#ifndef	__LNCHWAG_H
#define	__LNCHWAG_H

#include	"param.h"
//#include	"paramsg.h"
#include "portg.h"
#include "part1g.h"
#include "spatialg.h"

class LaunchWaveParams : public PortParams
{
protected:
	ScalarParameter wavePhase;
	ScalarParameter riseTime;
	ScalarParameter pulseLength;
	ScalarParameter amplitude;
public:
	LaunchWaveParams(GridParams* GP, SpatialRegionGroup* srg);
	Boundary* CreateCounterPart();
};

#endif	//__LNCHWAG_H
