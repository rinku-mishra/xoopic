//obemitg.cpp

#include "obemitg.h"
#include "obemit.h"
#include "spatialg.h"
///\dad{begin}
#include "spbound.h"
#include "diags.h"
///\dad{end}

//=================== OpenBoundaryBeamEmitterParams Class

OpenBoundaryBeamEmitterParams::OpenBoundaryBeamEmitterParams(GridParams* _GP, 
SpatialRegionGroup* srg) : BeamEmitterParams(_GP, srg)
{
  name = "OpenBoundaryEmitter";

  //nEmit.setNameAndDescription("nEmit", "");
  //nEmit.setValue("0");
  //parameterList.add(&nEmit);
}

Boundary* OpenBoundaryBeamEmitterParams::CreateCounterPart()
{
	EmitterParams::CreateCounterPart();//sets up distribution and species
	MaxwellianFlux *beam = distribution;
	Boundary* B = new OpenBoundaryBeamEmitter(beam, I.getValue(), 
														SP.GetLineSegments(),
		np2c.getValue(), thetadot.getValue());
	addSecondaries(B);
	return B;
}
