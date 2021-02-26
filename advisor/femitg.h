//femitg.h

#ifndef   __FEMITG_H
#define   __FEMITG_H \

#include "part1g.h"
#include "emitterg.h"

//=================== FieldEmitterParams Class
// class for field emitting boundary

class FieldEmitterParams : public EmitterParams
{
 protected:
  ScalarParameter threshold;

 public:
  FieldEmitterParams(GridParams* _GP, SpatialRegionGroup* srg);

  Boundary* CreateCounterPart();
};

class FieldEmitterParams2 : public EmitterParams
{
 protected:
  ScalarParameter threshold;

public:
  FieldEmitterParams2(GridParams* _GP, SpatialRegionGroup* srg);
  Boundary* CreateCounterPart();
};  

#endif  //  __FEMITG_H
