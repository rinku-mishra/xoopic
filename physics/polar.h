/*
//====================================================================

POLAR.H
	This class represents a polarizer.

0.99	(JV 9/13/95) Creation.


//====================================================================
*/

#ifndef	__POLAR_H
#define	__POLAR_H

#include	"ovector.h"
#include "misc.h"
#include	"particle.h"
//#include "dielectr.h"
#include "conducto.h"
#include "eval.h"
class Species;
class Fields;

class Polar : public Conductor
{
private:
	Scalar transmissivity;
	ostring Phi;
public:
	Polar(ostring _Phi,oopicList <LineSegment> *segments,Scalar transmissivity);
//	ParticleList&	emit(Scalar t,Scalar dt);
	virtual void	setPassives();
//	virtual void	setFields(Fields &f) {Boundary::setFields(f);};
	virtual void	collect(Particle& p, Vector3& dxMKS);
};

#endif	//	ifndef __POLAR_H
