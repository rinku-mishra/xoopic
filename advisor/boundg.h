//boundg.h

#ifndef   __BOUNDG_H
#define   __BOUNDG_H

#include <oops.h>

#include "part1g.h"
#include "gridg.h"
#include "segmentg.h"
//#ifndef UNIX
//#include "segment.h"
//#endif

class SpatialRegionGroup;
class PDistCol;
class Grid;

//=================== BoundaryParams Class
// base class for all boundaries
// abstract

class BoundaryParams : public ParameterGroup
{
protected:
	GridParams* GP;
	SpatialRegionGroup* spatialRegionGroup;
	Boundary* bPtr;
	SegmentParams SP;

public:
	IntParameter j1;
	IntParameter k1;
	IntParameter j2;
	IntParameter k2;
	IntParameter normal;
	IntParameter fill;
	IntParameter EFFlag;
	IntParameter IdiagFlag;
	IntParameter Ihist_len; // length of the history arrays
	IntParameter Ihist_avg; // n steps for taking an average
	ScalarParameter A1,A2,B1,B2;
	StringParameter Name; /*  the id name of this boundary */
	//These parameters have to do with gathering statistics on
	// collected particles, and are all optional.
	IntParameter nxbins; //number of bins in space
	IntParameter nenergybins; //number of bins in velocity
	IntParameter nthetabins;
	ScalarParameter theta_min; // lower value to collect f(theta)
	ScalarParameter theta_max;
	ScalarParameter energy_min;
	ScalarParameter energy_max;
	// ScalarParameter v1start,v2start,v3start;
	// ScalarParameter v1end,v2end,v3end;
	ScalarParameter spotSize_p0;
	ScalarParameter spotSize_p1;
	ScalarParameter waveLeng_p0;
	ScalarParameter waveLeng_p1;
	ScalarParameter focus_p0;
	ScalarParameter focus_p1;
	ScalarParameter amp_p0;
	ScalarParameter amp_p1;
	ScalarParameter tdelay_p0 ;
	ScalarParameter tdelay_p1 ;
	ScalarParameter pulLeng_p0;
	ScalarParameter pulLeng_p1;
	ScalarParameter chirp_p0;
	ScalarParameter chirp_p1;
	IntParameter pulShp_p0;
	IntParameter pulShp_p1;
	StringParameter F;
	BOOL HasLength();

	//These parameters have to do with any time functions on the boundaries,
	//such as varying potential, or current, or whatever;

	ScalarParameter C,A,frequency,phase,trise,tfall,tpulse,tdelay,a0,a1;
	IntParameter xtFlag;

	BoundaryParams(GridParams* _GP, SpatialRegionGroup* srg);
	//  since boundaries are all placed on grid, this class needs
	// to know the dimensions of the grid

	virtual ~BoundaryParams();

	ostring AlignmentConstraint();
	// ensures that boundary is aligned with grid

	ostring OnGridConstraint();
	// ensures that boundary is on grid

	virtual void checkRules();
	// checks all rules associated with the class

	virtual Boundary* CreateCounterPart(){return 0;};
	virtual void toGrid(Grid *g) /* throw(Oops) */;
	virtual void makeSegmentFitGrid(LineSegment *segment,Grid *g);
	virtual void setBoundaryParams(Boundary* bPtr);
	PDistCol* createDistributionCollector();
};                                       

#endif  //  __BOUNDG_H
