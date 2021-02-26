//bemitg.cpp

#include "bemitg.h"
#include "beamemit.h"
#include "spatialg.h"
#include "ptclgrp.h"
#include "diags.h"
///\dad{begin}
#include "spbound.h"
///\dad{end}
//=================== BeamEmitterParams Class

BeamEmitterParams::BeamEmitterParams(GridParams* _GP, SpatialRegionGroup* srg)
	  : EmitterParams(_GP, srg)
{
  name = "BeamEmitter";

  I.setNameAndDescription("I", "");
  I.setValue("1.0");
  parameterList.add(&I);
  
  thetadot.setNameAndDescription("thetadot","");
  thetadot.setValue("0.0");
  parameterList.add(&thetadot);

  nIntervals.setNameAndDescription("nIntervals","");
  nIntervals.setValue("0");
  parameterList.add(&nIntervals);
};

Boundary* BeamEmitterParams::CreateCounterPart()
{
  EmitterParams::CreateCounterPart();//sets up distribution and species

  MaxwellianFlux *beam = distribution;
  Boundary* B = new BeamEmitter(beam, I.getValue(),
										  SP.GetLineSegments(),
										  np2c.getValue(),
										  thetadot.getValue());
  addSecondaries(B);
  return B;
}
