#ifndef	__OBMEMIT_H
#define	__OBMEMIT_H

/*
====================================================================

obmemit.h

Class representing a open boundary emitting a semi-infinite beam of electrons
The beam current density is location dependent which is specified by user. 
The class will get the right boundary condition by solving the Poisson's Equ in 
the emitting boundary. Electric field is assumed to be 0 in the emitting direction
====================================================================
*/

#include	"ovector.h"
#include "vmaxwelln.h"
#include	"particle.h"
#include "beamemit.h"
#include "boundary.h"

class OpenBoundaryBeamEmitter : public BeamEmitter
{
public:
	OpenBoundaryBeamEmitter(MaxwellianFlux *max, Scalar current,
		oopicList <LineSegment> *segments, Scalar max_np2c, Scalar thetaDot);
	~OpenBoundaryBeamEmitter(){};
	virtual void applyFields(Scalar t, Scalar dt);
	virtual void setPassives();
	//	virtual ParticleList& emit(Sclar t, Scalar dt, Species* species_to_push);
};

#endif	//	ifndef __OBMEMIT_H


