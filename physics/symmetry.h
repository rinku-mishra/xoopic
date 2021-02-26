#ifndef	__SYMMETRY_H
#define	__SYMMETRY_H

/*
====================================================================

symmetry.h

A pure virtual base class for all symmetry related boundary conditions.

0.99	(NTG, 12-29-93) Separated into individual module from
		pic.h.
0.991	(JohnV, 01-03-94) Aesthetics, compile.
1.001	(JohnV 05-06-96) Add toNodes() to support cylindrical axis.

====================================================================
*/

//#include	"ovector.h"
//#include "misc.h"
//#include	"particle.h"
#include "boundary.h"

class Particle;

class Symmetry : public Boundary
{
public:
	Symmetry(oopicList <LineSegment> *segments) : Boundary(segments) {
	if(segments->nItems() > 1) {
	  std::cout << "Warning, Symmetry boundaries and derivatives can only have 1 segment.\n";
	  std::cout << "Check your input file.\n";
	}};
	virtual void	collect(Particle& p, Vector3& dxMKS) = 0;
	virtual ParticleList&	emit(Scalar t,Scalar dt, Species *species) = 0;
	virtual void setPassives() = 0;
	virtual void applyFields(Scalar t,Scalar dt) {};
	virtual void toNodes() {Boundary::toNodes();}
};

#endif	//	ifndef __SYMMETRY_H

