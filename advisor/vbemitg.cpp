//vbemitg.cpp

#include "vbemitg.h"
#include "vbmemit.h"
#include "spatialg.h"
///\dad{begin}
#include "spbound.h"
#include "diags.h"
///\dad{end}

//=================== VarWeightBeamEmitterParams Class

VarWeightBeamEmitterParams::VarWeightBeamEmitterParams(GridParams* _GP, 
SpatialRegionGroup* srg) : BeamEmitterParams(_GP, srg)
{
  name = "VarWeightEmitter";

  nEmit.setNameAndDescription("nEmit", "");
  nEmit.setValue("0");
  parameterList.add(&nEmit);
}

Boundary* VarWeightBeamEmitterParams::CreateCounterPart()
{
	EmitterParams::CreateCounterPart();//sets up distribution and species
	MaxwellianFlux *beam = distribution;
	Boundary* B = new VarWeightBeamEmitter(beam, I.getValue(), 
														SP.GetLineSegments(),
		np2c.getValue(), thetadot.getValue(), nEmit.getValue());
	addSecondaries(B);
	return B;
}
