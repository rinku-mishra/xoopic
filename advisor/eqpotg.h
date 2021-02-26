//eqpotg.h

#ifndef   __EQPOTG_H
#define   __EQPOTG_H \

#include "conductg.h"

//=================== EquipotentialParams Class
// base class for all boundaries imposing conducting boundary
// conditions

class EquipotentialParams : public ConductorParams
{
 protected:
	

public:

	EquipotentialParams(GridParams* _GP,SpatialRegionGroup *srg);
        StringParameter VanalyticF;

	Boundary* CreateCounterPart();
};

#endif  //  __EQPOTG_H
