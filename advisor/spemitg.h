//bemitg.h

#ifndef   __SPEMITG_H
#define   __SPEMITG_H
#include "bemitg.h"

#include "spemit.h"

//=================== BeamEmitterParams Class
// class for beam emitting boundary

class SingleParticleEmitterParams : public BeamEmitterParams 
{public:
  StringParameter u1;
  StringParameter u2;
  StringParameter x;
  ScalarParameter delay;
  
	SingleParticleEmitterParams(GridParams* _GP, SpatialRegionGroup* srg) : BeamEmitterParams( _GP,srg) {name ="SingleParticleEmitter";
	u1.setNameAndDescription("u1","u1 velocity");
	u1.setValue("0");
	parameterList.add(&u1);
	u2.setNameAndDescription("u2","u2 velocity");
	u2.setValue("0");
	parameterList.add(&u2);
	x.setNameAndDescription("x","x emissions position");
	x.setValue("0");
	parameterList.add(&x);

	delay.setNameAndDescription("delay","delay between particles");
	delay.setValue("0");
	parameterList.add(&delay);
	
	};

	Boundary* CreateCounterPart();
};

#endif  //  __SPEMITG_H
