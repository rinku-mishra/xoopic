#ifndef	__PLASMASOURCE_H
#define	__PLASMASOURCE_H

/*
====================================================================

plasmaSource.h

This class describes a volumetric plasma source, and can create pairs
with the specified distribution.

1.0  (JohnV 05-18-96) Original code.
1.1  (JohnV 06-11-00) Add time dependence.

====================================================================
*/

#include "species.h"
#include "emitter.h"

class Maxwellian;
class Evaluator;

class PlasmaSource : public Emitter
{
 protected:
	Species *species2;						// use Emitter::species as species1
	Maxwellian *maxwellian1, *maxwellian2;
	Scalar sourceRate;						// m^-3 s^-1
	void	initialize();						//	set up optimization variables
	int init;
	int is_xy;
	Vector2 p1, p2;							// MKS coords of corners
	Vector2 deltaP;             // p2-p1
	Vector2 x0;                 // MKS corrd of position of last laided particle
	Scalar Fx0, Fxc;                 
	Scalar drSqr,rMinSqr;                // difference of outer radius squared minus inner radius squared.
  Scalar extra;
	ostring analyticF;
public:
	PlasmaSource(Maxwellian* max1, Maxwellian* max2, Scalar sourceRate,
					 oopicList <LineSegment> *segments, Scalar np2c,const ostring &analyticF);
	~PlasmaSource();
	virtual void collect(Particle& p, Vector3& dxMKS) {return;}
	virtual void setBoundaryMask(Grid &grid) {return;}
	virtual void setPassives() {return;}
	Scalar get_q2() {return species2->get_q()*np2c;};
	Scalar get_m2() {return species2->get_m()*np2c;};
	virtual ParticleList& emit(Scalar t, Scalar dt, Species* species);
	Scalar F(Vector2 x, Scalar t);
	// is a particle weight increaser needed?
};

#endif	//	ifdef __PLASMASOURCE_H

