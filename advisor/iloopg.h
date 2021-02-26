//portg.h
//
#ifndef   __ILOOPG_H
#define   __ILOOPG_H \

#include "part1g.h"
#include "portg.h"

//=================== IloopParams Class
// The current loop parameter group
// 

class IloopParams : public PortParams
{public:

	ScalarParameter I;
	IloopParams(GridParams* _GP, SpatialRegionGroup* srg)
	 : PortParams(_GP, srg)
	 {name = "Iloop";
     I.setNameAndDescription("Current","Magnitude in Amps of the current in the loop");
	  I.setValue("1");
	  parameterList.add(&I);
  };

	Boundary* CreateCounterPart();
};

#endif  //  __ILOOPG_H
