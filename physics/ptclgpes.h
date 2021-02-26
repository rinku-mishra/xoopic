/*
====================================================================

PARTICLEGROUPES.H

	This class defines a group of particles which are related by a common
	charge to mass ratio and a common computational particle weight.  The
	ParticleGroup class reduces storage of redundant values of charge,
	mass and particle weight, and improves performance in the critical
	particle push loop.  Variable weighting allows q[] to vary, while
	holding np2c fixed at the maximum weighting for the group.  This is
	the electrostatic version of particle group.

Revision History
0.99	(JohnV 01-23-95)	Original version.
0.991	(JohnV 05-28-95)	Complete and integrate.

====================================================================
*/

#ifndef	__PTCLGPES_H
#define	__PTCLGPES_H

#include "ptclgrp.h"

class ParticleGroupES : public ParticleGroup
{
public:
	ParticleGroupES(int maxN, Species* s, Scalar np2c0, BOOL vary_np2c = FALSE)
		: ParticleGroup(maxN, s, np2c0, vary_np2c) {}
	void advance(Fields& fields, Scalar dt);
};

#endif	// ifndef __PTCLGPES_H
