//polar.h

#ifndef   __POLARG_H
#define   __POLARG_H

#include "part1g.h"
#include "conductg.h"

//=================== PolarParams Class
// base class for all boundaries imposing conducting boundary
// conditions

class PolarParams : public ConductorParams
{
	ScalarParameter transmissivity;

	StringParameter Phi;  //  the angle of the conductivity in the symmetry dir
	                      //  w.r.t the simulation plane.  It is intended to
								 //  be a function of X, the direction along the
								 //  polarizer.
public:
	PolarParams(GridParams* _GP, SpatialRegionGroup* srg);

	Boundary* CreateCounterPart();
};

#endif  //  __POLARG_H
