
#include "iregiong.h"
#include "iregion.h"

//=================== CurrentRegionParams Class
CurrentRegionParams::CurrentRegionParams(GridParams* _GP,SpatialRegionGroup *srg)
: PortParams(_GP,srg)
{name = "CurrentRegion";
 I.setNameAndDescription("Current","Magnitude in Amps of the current in the loop");
 I.setValue("1");
 parameterList.add(&I);

 direction.setNameAndDescription("direction","direction of current 1,2,3");
 direction.setValue("3");
 parameterList.add(&direction);

 currentfile.setNameAndDescription("currentFile","file containing current disribution");
 currentfile.setValue("NULL");
 parameterList.add(&currentfile);

 analyticF.setNameAndDescription("analyticF","Analytic function of x1,x2 describing the current");
 analyticF.setValue(ostring("0.0")); 
 parameterList.add(&analyticF);
};

Boundary* CurrentRegionParams::CreateCounterPart()
{Boundary* B =
  new CurrentRegion(SP.GetLineSegments(),
						  I.getValue(),direction.getValue(),
						  analyticF.getValue(), currentfile.getValue());
 return B;}
