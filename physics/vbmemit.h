#ifndef	__VBMEMIT_H
#define	__VBMEMIT_H

/*
====================================================================

vbmemit.h

Class representing a conductor which emits a uniform beam of electrons
which are weighted linearly in radius.  This class only makes sense
for an emitter with normal in z in z-r corrdinates; this should be
enforced by the expert advisor.

0.99	(JohnV 09-13-94) Generate to polymorph BeamEmitter for variable
		weighted particles.
0.991	(JohnV 02-10-95) Add secondary electron emission.
1.101 (JohnV 11-08-96) Add obliqueEmit().
1.2   (JohnV 02-13-00) Add J(x,t) capability.
1.21  (JohnV 03-26-00) Add nEmit to set the number per dt to emit

====================================================================
*/

#include	"ovector.h"
#include "vmaxwelln.h"
#include	"particle.h"
#include "beamemit.h"

class VarWeightBeamEmitter : public BeamEmitter
{
	Scalar	rFraction;						//	stores next r/rMax to be emitted
	Scalar	w0;                        //	r/rMax = w0 + w1*random
	Scalar	w1;

	Scalar	getNext_rFraction() {return w0 + w1*frand();}
	void	initialize();
	Scalar	qMax() {return get_q();}			//	q at outer radius
	Scalar	np2cMax() {return np2c;}	// np2c at outer radius;
	int nEmit; // sets the number of particles to emit per dt if nonzero
	Scalar	q_over_m;
//	Vector2	delta;							//	small perturbation away from boundary
   Scalar* obl_rFraction;              // stores array of rFraction for obliques
public:
	VarWeightBeamEmitter(MaxwellianFlux *max, Scalar current,
		oopicList <LineSegment> *segments, Scalar max_np2c, Scalar thetaDot, int nEmit);
	~VarWeightBeamEmitter();
	ParticleList& emit(Scalar t,Scalar dt, Species* species);
	ParticleList& obliqueEmit(Scalar t,Scalar dt);
};

#endif	//	ifndef __VBMEMIT_H


