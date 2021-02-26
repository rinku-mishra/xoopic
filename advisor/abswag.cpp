/*
=====================================================================
abswag.cpp

Provides methods for the AbsorbWaveGroup class.

0.9	(JohnV and BillP 10-16-94) Initial draft to hook up to advisor.
		
=====================================================================
*/

#include "abswag.h"
///\dad{begin}
#include "spbound.h"
///\dad{end}
#include "absorbwa.h"
#ifdef UNIX
#include "ptclgrp.h"
#include "diags.h"
#endif

AbsorbWaveParams::AbsorbWaveParams(GridParams* GP, SpatialRegionGroup *srg) : PortParams(GP,srg)
{
	name = "AbsorbWave";
	wavePhase.setNameAndDescription("wavePhase", "wavePhase");
	wavePhase.setValue("1.0");
	parameterList.add(&wavePhase);
}

Boundary* AbsorbWaveParams::CreateCounterPart()
{
	return new AbsorbWave(SP.GetLineSegments(), wavePhase.getValue());
}

