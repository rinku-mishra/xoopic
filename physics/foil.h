/*
//====================================================================

FOIL.H

	This class represents a conducting surface which can
	collect and re-emit particles.

0.90	(BillP 3-13-95) Creation.

//====================================================================
*/

#ifndef	__FOIL_H
#define	__FOIL_H

#include	"ovector.h"
#include	"misc.h"
#include	"particle.h"
#include "conducto.h"

class Foil : public Conductor
{
  Scalar nemit;    //number of particles to emit per primary

public:
	Foil(Scalar _nemit,
					  oopicList <LineSegment> *segments); 

//   Scalar nemit;
	virtual void	applyFields(Scalar t,Scalar dt) {};
	ParticleList&	emit(Scalar t, Scalar dt);
//	virtual void	setPassives();
   virtual void    collect(Particle& p, Vector3& dxMKS);
};

#endif	//	ifndef __FOIL_H
