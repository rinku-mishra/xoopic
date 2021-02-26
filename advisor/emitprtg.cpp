//bemitg.cpp

#include "emitprtg.h"
#include "emitport.h"
#include "spatialg.h"
///\dad{begin}
#include "spbound.h"
///\dad{end}
#ifdef UNIX
#include "ptclgrp.h"
#include "diags.h"
#endif

//=================== BeamEmitterParams Class


Boundary* EmitPortParams::CreateCounterPart()
{
	EmitterParams::CreateCounterPart();//sets up distribution and species
	MaxwellianFlux *beam = distribution;
	Boundary* B =
		new EmitPort(beam, I.getValue(),
						 SP.GetLineSegments(),
						 np2c.getValue(),
						 thetadot.getValue());
	addSecondaries(B);
	return B;}
