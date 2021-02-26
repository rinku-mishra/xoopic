/*
====================================================================

PARTICLEGROUPSR.H

	This class defines a group of particles which are related by a common
	charge to mass ratio and a common computational particle weight.  The
	ParticleGroup class reduces storage of redundant values of charge,
	mass and particle weight, and improves performance in the critical
	particle push loop.  Variable weighting allows q[] to vary, while
	holding np2c fixed at the maximum weighting for the group.

   This version of particle group is fully electromagnetic and includes
   a correction that models synchrotron radiation.

Revision History
0.99	(Bruhwiler 09-29-99)	Original version.

====================================================================
*/

#ifndef	__PTCLGPSR_H
#define	__PTCLGPSR_H

#include "ptclgrp.h"

class ParticleGroupSR : public ParticleGroup
{
public:
	ParticleGroupSR(int maxN, Species* s, Scalar np2c0, BOOL vary_np2c = FALSE)
		: ParticleGroup(maxN, s, np2c0, vary_np2c) {
     synchRadiationOn = TRUE;
   }
	void advance(Fields& fields, Scalar dt);
};

#endif	// ifndef __PTCLGPSR_H
