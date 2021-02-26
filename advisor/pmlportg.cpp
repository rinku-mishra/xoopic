/*
=====================================================================
pmlportg.cpp


		
=====================================================================
*/

#include "pmlportg.h"
///\dad{begin}
#include "spbound.h"
///\dad{end}
#include "pmlport.h"
#ifdef UNIX
#include "ptclgrp.h"
#include "diags.h"
#endif

PMLportParams::PMLportParams(GridParams* GP, SpatialRegionGroup *srg) : PortParams(GP,srg)
{
	name = "PMLWave";
	sigma.setNameAndDescription("sigma", "sigma");
	sigma.setValue("1");
	parameterList.add(&sigma);

	thickness.setNameAndDescription("thickness", "thichness");
	thickness.setValue("4");
	parameterList.add(&thickness);
}

Boundary* PMLportParams::CreateCounterPart()
{
  return new PMLPort(SP.GetLineSegments(), sigma.getValue(), thickness.getValue());
}
