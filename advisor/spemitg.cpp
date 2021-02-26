#include "spatialg.h"
#include "spemitg.h"
#include "spemit.h"
///\dad{begin}
#include "spbound.h"
///\dad{end}
#ifdef UNIX
#include "ptclgrp.h"
#include "diags.h"
#endif

//=================== BeamEmitterParams Class


Boundary* SingleParticleEmitterParams::CreateCounterPart()
{
	EmitterParams::CreateCounterPart();//sets up distribution and species
	MaxwellianFlux *beam = distribution;
	Boundary* B =
	  new SingleParticleEmitter(u1.getValue(),u2.getValue(),x.getValue(),
										 delay.getValue(),beam, I.getValue(),
										 SP.GetLineSegments(),
										 np2c.getValue(),
										 thetadot.getValue());
	addSecondaries(B);
	return B;}
