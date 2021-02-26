#ifndef	__PERIODIC_H
#define	__PERIODIC_H

/*
====================================================================

periodic.h

This class represents a periodic surface

0.1 (kc) first code, getting poisson equation to work
====================================================================
*/

#include "boundary.h"

class Periodic : public Boundary
{
protected:
	int PeriodicFlagX1, PeriodicFlagX2;
	int J,K;
//	Vector2 deltaVertical;
//	Vector2 deltaHorizontal;
	Vector3* iCX1;
	Vector3* iCX2;

public:
	Periodic(oopicList <LineSegment> *segments, int PeriodicFlagX1, int PeriodicFlagX2);
	~Periodic();
	virtual ParticleList&	emit(Scalar t,Scalar dt, Species* species) {return particleList;};
	virtual void setBoundaryMask(Grid &grid);
	virtual Scalar getBoltzmannFlux();
	virtual void setPassives();
	virtual void applyFields(Scalar t,Scalar dt);
	virtual void collect(Particle& p, Vector3& dxMKS);
	virtual void putCharge_and_Current(Scalar t);
//	virtual void setFields(Fields &f);
//	virtual int Restore(FILE* DMPFile, int j1, int k1, int j2, int k2);
//	virtual int Dump(FILE* DMPFile);
	virtual void toNodes(); // {Boundary::toNodes();}
};

#endif	//	ifndef __PERIODIC_H
