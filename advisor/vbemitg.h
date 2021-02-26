//vbemitg.h

#ifndef   __VBEMITG_H
#define   __VBEMITG_H

#include "part1g.h"
#include "bemitg.h"

//=================== VarWeightBeamEmitterParams Class
// class for beam emitting boundary

class VarWeightBeamEmitterParams : public BeamEmitterParams
{
 protected:
  IntParameter nEmit; // emit nEmit per step if nonzero

 public:
	VarWeightBeamEmitterParams(GridParams* _GP, SpatialRegionGroup* srg);

	Boundary* CreateCounterPart();
};

#endif  //  __VBEMITG_H
