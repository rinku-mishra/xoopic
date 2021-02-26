//eportg.h

#ifndef   __PORTTEG_H
#define   __PORTTEG_H \

#include "part1g.h"
#include "portg.h"

//=================== PortTEParams Class
// TE Port boundary

class PortTEParams : public PortParams
{public:
	PortTEParams(GridParams* _GP, SpatialRegionGroup* srg)
	 : PortParams(_GP, srg)
	 {name = "PortTE";};

	Boundary* CreateCounterPart();
};

#endif  //  __PORTTEG_H
