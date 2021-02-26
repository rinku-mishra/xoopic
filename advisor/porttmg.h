//eportg.h

#ifndef   __PORTTMG_H
#define   __PORTTMG_H \

#include "part1g.h"
#include "portg.h"

//=================== PortTMParams Class
// Exit port boundary

class PortTMParams : public PortParams
{public:
	PortTMParams(GridParams* _GP, SpatialRegionGroup* srg)
	 : PortParams(_GP, srg)
	 {name = "PortTM";};

	Boundary* CreateCounterPart();
};

#endif  //  __PORTTMG_H
