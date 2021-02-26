//conductg.h

#ifndef   __CONDUCTG_H
#define   __CONDUCTG_H

#include "part1g.h"
#include "dielecg.h"

//=================== ConductorParams Class
// base class for all boundaries imposing conducting boundary
// conditions

class ConductorParams : public DielectricParams
{
public:
	ConductorParams(GridParams* _GP, SpatialRegionGroup* srg);

	Boundary* CreateCounterPart();
};

#endif  //  __CONDUCTG_H
