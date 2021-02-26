//gapg.cpp

#include "gapg.h"
#include "gap.h"

//=================== GapParams Class
// Gap boundary

GapParams::GapParams(GridParams* _GP, SpatialRegionGroup* srg)
	  : PortParams(_GP, srg)
{
	name = "Gap";
/*	IEFlag.setNameAndDescription("IEFlag","IEFlag = 0 electric field , IEFlag = 1 current in directin IEFlag");
	IEFlag.setValue(ostring("0"));
	parameterList.add(&IEFlag);*/
	// current Doesn't work see gap.cpp.
};

Boundary* GapParams::CreateCounterPart()
{Boundary* B =
  new Gap(SP.GetLineSegments(), EFFlag.getValue());
 return B;}
