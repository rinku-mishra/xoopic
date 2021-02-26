//particlg.cpp

#include "particlg.h"


//=================== ParticleGroupParams Class

ParticleGroupParams::ParticleGroupParams()
	  : ParameterGroup()
{name = "ParticleGroup";
 maxN.setNameAndDescription(ostring("maxN"),
	ostring("Maximum number of particles in group"));
 mRatio.setNameAndDescription(ostring("mRatio"),
	ostring("Ratio of specie mass to electron mass"));
 np2c.setNameAndDescription(ostring("np2c"),
	ostring("Number of particles per cell"));

 parameterList.add(&maxN);
 parameterList.add(&mRatio);
 parameterList.add(&np2c);

 // rules can be added during construction or by external messages
 addLimitRule(ostring("maxN"), ostring("<="), 0.0,
	ostring("Fatal -- maxN < 0 -- must have positive number of particles"), 1);
}
