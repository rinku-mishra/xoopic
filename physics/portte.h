/*
====================================================================

PORTTE.H

	Class representing a boundary on which a wave type boundary
	condition is applied.

0.99	(NTG 12-29-93) Separated into individual module from pic.h.
0.991	(JohnV 01-03-93) Aesthetics, compile.

====================================================================
*/

#ifndef	__PORTTE_H
#define	__PORTTE_H

#include	"ovector.h"
#include "misc.h"
#include	"particle.h"
#include "boundary.h"


class PortTE : public Boundary
{
 protected:
	Grid* grid;
	Scalar gamma1;
	Scalar a, A1, rforEphi, Ephi, intEphi;
	Scalar EnergyOut, HonBoundary;
	int K, EFFlag, shift;
	Scalar time, tOld, dt;
public:
	PortTE(oopicList <LineSegment> *segments, int _EEFlag);
   virtual void	applyFields(Scalar t,Scalar dt);
	virtual ParticleList&	emit(Scalar t,Scalar dt, Species *species);
	virtual void	setPassives();
};

#endif	//	ifndef __PORTTE_H
