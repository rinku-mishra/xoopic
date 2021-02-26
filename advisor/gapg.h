//gapg.h

#ifndef   __GAPG_H
#define   __GAPG_H \

#include "part1g.h"
#include "portg.h"


//=================== GapParams Class
// Gap boundary

class GapParams : public PortParams
{
 protected:
	 IntParameter IEFlag;

 public:
	GapParams(GridParams* _GP, SpatialRegionGroup* srg);

	Boundary* CreateCounterPart();
};

#endif  //  __GAPG_H
