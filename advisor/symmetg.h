//symmetg.h

#ifndef   __SYMMETG_H
#define   __SYMMETG_H \

#include "part1g.h"
#include "boundg.h"

//=================== SymmetryParams Class
// abstract base class for all boundaries imposing symmetry boundary
// conditions

class SymmetryParams : public BoundaryParams
{public:
	SymmetryParams(GridParams* _GP, SpatialRegionGroup* srg)
	 : BoundaryParams(_GP, srg)
	 {name = "Symmetry";};

	Boundary* CreateCounterPart() {return NULL;};
};



#endif  //  __SYMMETG_H
