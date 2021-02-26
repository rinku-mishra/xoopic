/*
====================================================================

GAP.H

	Class representing a boundary on which a wave type boundary
	condition with sinusoidal field variation is applied.

0.99	(KC 06-94) Initial code.
0.991	(JohnV 11-17-94) Aesthetics, add additional comments.

====================================================================
*/

#ifndef	__GAP_H
#define	__GAP_H

#include "port.h"

class Gap : public Port
{
 protected:
	//	int IEFlag;
   int shift, EFFlag;
	Scalar  EnergyOut, HonBoundary, time, tOld, dt;
	Grid* grid;	
 public:
	Gap(oopicList <LineSegment> *segments, int EFFlag);
	virtual void	applyFields(Scalar t,Scalar dt);
	virtual ParticleList&	emit(Scalar t,Scalar dt);
	virtual void	setPassives();
};

#endif	//	ifndef __GAP_H
