//cursrcg.h

#ifndef   __CURSRCG_H
#define   __CURSRCG_H

#include "eqpotg.h"

//=================== CurrentSourceParams Class
// provides a floating potential current source

class CurrentSourceParams : public EquipotentialParams
{
 protected:
  ScalarParameter Vinitial;

 public:
	CurrentSourceParams(GridParams* _GP,SpatialRegionGroup *srg);
	Boundary* CreateCounterPart();
};

#endif  //  __CURSRCG_H
