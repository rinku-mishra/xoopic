//loadg.h

#ifndef __LOADG_H
#define __LOADG_H

#include "part1g.h"
#include "gridg.h"
#include "maxwelln.h"
#include "load.h"
#include "speciesg.h"

#include <oops.h>

#if defined(_MSC_VER)
extern "C" int WasDumpFileGiven;
#else
extern int WasDumpFileGiven;
#endif

class SpatialRegionGroup;

//==================== LoadParams Class
// class for t=0 loading

class LoadParams : public ParameterGroup
{
 protected:
  Grid* G;
  Fields* F;
  SpatialRegion* region;
  SpatialRegionGroup* spatialRegionGroup;

  // specify the Maxwellian velocity distribution
	StringParameter units;
  ScalarParameter temperature;
	ScalarParameter v1thermal, v2thermal, v3thermal;
  ScalarParameter Lcutoff;
	ScalarParameter v1Lcutoff, v2Lcutoff, v3Lcutoff;
	ScalarParameter Ucutoff;
	ScalarParameter v1Ucutoff, v2Ucutoff, v3Ucutoff;
  //  Vector3Parameter V0 one day have a vector define this
  ScalarParameter v1drift, v2drift, v3drift;
 
  // specity the load
  ScalarParameter density;
	IntParameter LoadMethodFlag;
  ScalarParameter np2c;
  ScalarParameter x1MinMKS, x1MaxMKS, x2MinMKS, x2MaxMKS;

	StringParameter speciesName;
  StringParameter analyticF;
	StringParameter densityfile;
	StringParameter Name;

 public:
    LoadParams(SpatialRegionGroup* srg);
  //  since boundaries are all placed on grid, this class needs
  //  to know the dimensions of the grid

    virtual ~LoadParams() {};

    virtual Load* CreateCounterPart(SpatialRegion* region) /* throw(Oops) */;

    void checkRules();

};  

#endif  // ifndef __LOADG_H  
