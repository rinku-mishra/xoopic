//obemitg.h

#ifndef   __OBEMITG_H
#define   __OBEMITG_H

#include "part1g.h"
#include "bemitg.h"

//=================== OpenBoundaryBeamEmitterParams Class
// class for beam emitting boundary

class OpenBoundaryBeamEmitterParams : public BeamEmitterParams
{
 public:
	OpenBoundaryBeamEmitterParams(GridParams* _GP, SpatialRegionGroup* srg);

	Boundary* CreateCounterPart();
};

#endif  //  __OBEMITG_H
