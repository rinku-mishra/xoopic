/*
====================================================================

ILOOP.H

	This causes a loop of current to be in the simulation.  It
is useful as an external source for input.
	

0.90 (PeterM 04-16-96) Inception.

====================================================================
*/

#ifndef	__ILOOP_H
#define	__ILOOP_H

#include	"ovector.h"
#include "misc.h"
#include "port.h"


class Iloop : public Port
{
	Scalar I;
protected:
public:
	Iloop(oopicList <LineSegment> *segments,Scalar _II) : 
		Port(segments)
			{
				I=_II;
				BoundaryType=ILOOP;
			};
//	virtual ParticleList&	emit(Scalar t,Scalar dt) {return ;};
	virtual void setPassives() {};
	virtual void applyFields(Scalar t,Scalar dt) {};
	virtual void putCharge_and_Current(Scalar t);
	virtual void setBoundaryMask(Grid &grid){};
};

#endif	//	ifndef __ILOOP_H
