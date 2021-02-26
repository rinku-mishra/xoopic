/*
=====================================================================
pmlportg.h

		
=====================================================================
*/

#ifndef	__PMLPORTG_H
#define	__PMLPORTG_H

#include	"param.h"
#include "portg.h"
//#include	"paramsg.h"
//#include "spatialg.h"

class SpatialRegionGroup;

// Should reflect inheritance structure of physics counterpart

class PMLportParams : public PortParams
{
protected:
	ScalarParameter sigma;
	IntParameter thickness;

public:
// Tom: why is GP required for all boundaries? Needed for error checking?
	PMLportParams(GridParams* GP, SpatialRegionGroup* srg);
	Boundary* CreateCounterPart();
};

#endif	
