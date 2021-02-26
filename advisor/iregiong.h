#ifndef   __IREGIONG_H
#define   __IREGIONG_H \

#include "part1g.h"
#include "portg.h"
#include "iregion.h"

//=================== CurrentRegionParams Class
// The current Region parameter group
// 

class CurrentRegionParams : public PortParams
{
protected:
	ScalarParameter I;
	IntParameter direction;
	StringParameter currentfile;
	StringParameter analyticF;
public:
	CurrentRegionParams(GridParams* _GP, SpatialRegionGroup* srg);
	Boundary* CreateCounterPart();
};

#endif  //  __IREGIONG_H
