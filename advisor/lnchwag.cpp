/*
=====================================================================
lnchwag.cpp

Provides methods for the LaunchWaveParams class.

0.9	(JohnV 12-05-94) Initial draft to hook up to advisor.
		
=====================================================================
*/

#include "lnchwag.h"
#include "launchwa.h"
///\dad{begin}
#include "spbound.h"
#include "diags.h"
///\dad{end}
#ifdef UNIX
#include "ptclgrp.h"
#include "diags.h"
#endif

LaunchWaveParams::LaunchWaveParams(GridParams* GP,SpatialRegionGroup *srg) : PortParams(GP,srg)
{
	name = "LaunchWave";
	wavePhase.setNameAndDescription("wavePhase", "wavePhase");
	wavePhase.setValue("1.0");
	parameterList.add(&wavePhase);
	riseTime.setNameAndDescription("riseTime", "riseTime");
	riseTime.setValue("0");
	parameterList.add(&riseTime);
	amplitude.setNameAndDescription("amplitude", "amplitude");
	amplitude.setValue("1.0");
	parameterList.add(&amplitude);
	pulseLength.setNameAndDescription("pulseLength", "pulseLength");
	pulseLength.setValue("1.0");
	parameterList.add(&pulseLength);
}                           

Boundary* LaunchWaveParams::CreateCounterPart()
{
	return new LaunchWave(SP.GetLineSegments(),
								 wavePhase.getValue(), amplitude.getValue(), 
								 riseTime.getValue(), pulseLength.getValue(), frequency.getValue());
}

