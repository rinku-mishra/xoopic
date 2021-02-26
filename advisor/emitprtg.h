//bemitg.h

#ifndef   __EMITPRTG_H
#define   __EMITPRTG_H

#include "bemitg.h"

//=================== BeamEmitterParams Class
// class for beam emitting boundary

class EmitPortParams : public BeamEmitterParams 
{public:

	EmitPortParams(GridParams* _GP, SpatialRegionGroup* srg) : BeamEmitterParams( _GP,srg) {name ="EmitPort";};

	Boundary* CreateCounterPart();
};

#endif  //  __EMITPRTG_H
