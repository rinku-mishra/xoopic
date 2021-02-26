//particlg.h

#ifndef   __PARTICLG_H
#define   __PARTICLG_H

#include "part1g.h"

//=================== ParticleGroupParams Class
// class for specifying a particle group

class ParticleGroupParams :public ParameterGroup
{IntParameter maxN;
 ScalarParameter mRatio;
 IntParameter np2c;

public:
	  ParticleGroupParams();

	  virtual ~ParticleGroupParams() {};

	  //ParticleGroup* CreateCounterPart();
};


#endif  //  __PARTICLG_H
