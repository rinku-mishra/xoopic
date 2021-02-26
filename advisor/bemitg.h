//bemitg.h

#ifndef   __BEMITG_H
#define   __BEMITG_H

#include "part1g.h"
#include "emitterg.h"

//=================== BeamEmitterParams Class
// class for beam emitting boundary

class BeamEmitterParams : public EmitterParams
{
protected:

  ScalarParameter I;  // Emission current
  ScalarParameter thetadot;
  IntParameter nIntervals;

  public:
	BeamEmitterParams(GridParams* _GP, SpatialRegionGroup* srg);
	void checkRules() {EmitterParams::checkRules();}
	Boundary* CreateCounterPart();
};

#endif  //  __BEMITG_H
