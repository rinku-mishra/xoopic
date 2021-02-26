//eportg.h

#ifndef   __EPORTG_H
#define   __EPORTG_H 

#include "part1g.h"
#include "portg.h"
#include "controlg.h"

//=================== ExitPortParams Class
// Exit port boundary

class ExitPortParams : public PortParams
{
protected:
	ScalarParameter R;
	ScalarParameter L;
	ScalarParameter Cap;
	ScalarParameter Rin;
	ScalarParameter Lin;
	ScalarParameter Capin;
	ControlParams*  CP;

public:
	ExitPortParams(ControlParams*  _CP, GridParams* _GP, SpatialRegionGroup* srg);

	virtual Boundary* CreateCounterPart();
};

#endif  //  __EPORTG_H
