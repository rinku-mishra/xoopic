//cylaxisg.h

#ifndef   __CYLAXISG_H
#define   __CYLAXISG_H \

#include "part1g.h"
#include "symmetg.h"

//=================== CylindricalAxisParams Class
// class for axial boundary condition

class CylindricalAxisParams : public SymmetryParams
{public:
	CylindricalAxisParams(GridParams* _GP, SpatialRegionGroup* srg)
	 : SymmetryParams(_GP, srg)
	 {name = "CylindricalAxis";};

	ostring OnAxisConstraint();
	// ensures that boundary is on the axis

	void checkRules();
	//  checks all rules for this class and underlying classes

	Boundary* CreateCounterPart();
};


#endif  //  __CYLAXISG_H
