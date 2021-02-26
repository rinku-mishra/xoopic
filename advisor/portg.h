//portg.h

#ifndef   __PORTG_H
#define   __PORTG_H \

#include "part1g.h"
#include "boundg.h"

//=================== PortParams Class
// base class for all port and gap boundaries
// abstract

class PortParams : public BoundaryParams
{public:
	PortParams(GridParams* _GP, SpatialRegionGroup* srg)
	 : BoundaryParams(_GP, srg)
	 {name = "Port";};

	Boundary* CreateCounterPart();
};

#endif  //  __PORTG_H
