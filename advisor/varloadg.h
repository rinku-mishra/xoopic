//varloadg.h

#ifndef __VARLOADG_H
#define __VARLOADG_H

#include "varload.h"
#include "loadg.h"

class SpatialRegionGroup;

//==================== VarWeightLoadParams Class
// class for t=0 loading, with variable weighting of particles,
// such that macroparticles are uniformly distributed in radius

class VarWeightLoadParams : public LoadParams
{
 public:

  //  since boundaries are all placed on grid, this class needs
  //  to know the dimensions of the grid
  VarWeightLoadParams(SpatialRegionGroup* srg) : LoadParams(srg) {
    name = "VarWeightLoad";
	}

  virtual ~VarWeightLoadParams() {};

  virtual Load* CreateCounterPart(SpatialRegion* region) /* throw(Oops) */;
};  

#endif  // ifndef __VARLOADG_H  
