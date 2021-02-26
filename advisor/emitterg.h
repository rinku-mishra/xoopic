//emitterg.h

#ifndef   __EMITTERG_H
#define   __EMITTERG_H \

#include "part1g.h"
#include "dielecg.h"
#include "vmaxwelln.h"

//=================== EmitterParams Class
// base class for all emitting boundaries
// abstract

class EmitterParams : public DielectricParams
{
protected:
	StringParameter	speciesName;      //	name of species to emit

	// specify the Maxwellian velocity distribution
	StringParameter units;
  ScalarParameter temperature;
	ScalarParameter v1thermal, v2thermal, v3thermal;
  ScalarParameter Ucutoff;
	ScalarParameter v1Ucutoff, v2Ucutoff, v3Ucutoff;
	ScalarParameter Lcutoff;
	ScalarParameter v1Lcutoff, v2Lcutoff, v3Lcutoff;
  ScalarParameter v1drift, v2drift, v3drift;

	MaxwellianFlux* distribution;      // Flux of distribution that will be emitted 
	Species* species;

	ScalarParameter np2c;
public:
	EmitterParams(GridParams* _GP, SpatialRegionGroup* srg);
	virtual void checkRules();
//	virtual Boundary* CreateCounterPart() { return NULL; };
	virtual Boundary* CreateCounterPart();
};

#endif  //  __EMITTERG_H
