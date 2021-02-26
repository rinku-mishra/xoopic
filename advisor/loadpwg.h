//loadg.h

#ifndef __LOADPWG_H
#define __LOADPWG_H

#include "part1g.h"
#include "gridg.h"
#include "maxwelln.h"
#include "load.h"
#include "loadg.h"
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

class LoadPWParams : public LoadParams
{
 protected:
   ScalarParameter pwFrequency, pwPhase, pwWavenumber, pwAmplitude, pwDelay;


 public:
  LoadPWParams(SpatialRegionGroup* srg) ;
  //  since boundaries are all placed on grid, this class needs
  //  to know the dimensions of the grid

  virtual ~LoadPWParams() {};

  virtual Load* CreateCounterPart(SpatialRegion* region) /* throw(Oops) */;

};  

#endif  // ifndef __LOADPWG_H  
