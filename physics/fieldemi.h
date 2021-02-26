/*
====================================================================

FIELDEMI.H

	Class representing a conductor which emits particles if the
	surface electric field is sufficiently large.

0.99	(NTG 12-30-93) Separated into individual module from pic.h.
0.991	(JohnV, BillP 03-11-94)	Refine algorithm for emission in
		x1-direction only.  Fix random seed for proper particle
		distribution.
0.995 (JohnV 01-29-95) Add Species capability.

====================================================================
*/

#ifndef	__FIELDEMI_H
#define	__FIELDEMI_H

#include "emitter.h"
#include "vmaxwelln.h"
#include <oops.h>


class FieldEmitter : public Emitter
{
	Scalar threshold;
	BOOL emitFlag;
	Scalar const1;								// constants for computing Jemit
   Grid* grid;
public:
	FieldEmitter(oopicList <LineSegment> *segments, Species* s,
		Scalar np2c, Scalar thresholdE);
	ParticleList& emit(Scalar t, Scalar dt, Species* species) /* throw(Oops) */;
};

class FieldEmitter2 : public Emitter
{
	Scalar threshold;
	BOOL emitFlag;
	BOOL init;
	int emitDirection;
	Scalar delta;
   Grid* grid;
	Scalar **rho;
	MaxwellianFlux *maxwellian;
public:
	FieldEmitter2(oopicList <LineSegment> *segments, Species* s,	
					  Scalar np2c, Scalar thresholdE, MaxwellianFlux *m);
	ParticleList& emit(Scalar t, Scalar dt, Species* species);
	void initialize();
};

#endif	//	ifndef __FIELDEMI_H
