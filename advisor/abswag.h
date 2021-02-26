/*
=====================================================================
abswag.h

Definitions for the AbsorbWaveGroup class.

0.9	(JohnV and BillP 10-16-94) Initial draft to hook up to advisor.
		
=====================================================================
*/

#ifndef	__ABSWAG_H
#define	__ABSWAG_H

#include	"param.h"
#include "portg.h"
//#include	"paramsg.h"
//#include "spatialg.h"

class SpatialRegionGroup;

// Should reflect inheritance structure of physics counterpart

class AbsorbWaveParams : public PortParams
{
protected:
	ScalarParameter wavePhase;
public:
// Tom: why is GP required for all boundaries? Needed for error checking?
	AbsorbWaveParams(GridParams* GP, SpatialRegionGroup* srg);
	Boundary* CreateCounterPart();
};

#endif	//__ABSWAG_H
