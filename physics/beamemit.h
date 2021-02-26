#ifndef	__BEAMEMIT_H
#define	__BEAMEMIT_H

/*
====================================================================

BEAMEMIT.H

Class representing a conductor which emits a uniform beam of electrons.

0.99	(NTG 12-29-93) Separated into individual module from pic.h.
0.991	(JohnV, 01-03-93) Aesthetics, compile.
0.993 (JohnV, ABL 03-03-94) Fix emission so particle train is evenly
		spaced in absence of fields, improve efficiency.
0.994	(JohnV 03-24-94) Add angular rotation for rigid rotor emission.
0.995	(JohnV 03-29-94) Optimization, fix error in angular component.
0.996	(JohnV 05-25-94) Fix emission of uniform current for off-axis
		emitter works properly.
0.997	(JohnV 09-13-94) Make internals protected for use by 
		VarWeightBeamEmitter class.
0.998 (JohnV 12-18-94) Add normal.
0.999	(JohnV 01-27-95) Add Species.
0.9995(PeterM 9-02-95) Add neuman flag to determine what electrostatic boundary
                       condition to use
1.001 (JohnV 11-06-96) Add obliqueEmit to properly handle oblique emitters.
1.002 (JohnV 01-06-99) handle oblique emission near cell edges

====================================================================
*/

#include	"ovector.h"
#include "maxwelln.h"
#include "vmaxwelln.h"
#include	"particle.h"
#include "emitter.h"

class BeamEmitter : public Emitter
{
protected:
	MaxwellianFlux	*maxwellian;
	Scalar	emissionRate;
	Scalar	extra;
	Vector2	p1;								//	MKS coords of 1st corner
	Vector2	p2;                        //	MKS coords of 2nd corner
	Vector3 normal;
	Vector3	thetaDot;						//	(0, 0, azimuthal rot freq)
	BOOL	init;                     //	initialization flag
	BOOL    Farray_compute;

        Scalar	rMin;								//	MKS units of inner radius
	Scalar	rMax;								//	             outer
	Vector2 delta;			// small displacement to avoid particle being on boundary
	Vector2 deltaVertical;
	Vector2 deltaHorizontal;
	int sign;  // sign for oblique normal
	void	initialize();						//	set up optimization variables
	Scalar *oblExtra;    // array of partial emission
	Scalar *area;        // areas for oblique segments								 
	Scalar totalArea;  // total area of an oblique boundary;
	Scalar t; // internal storage of time t;
	int nIntervals; // number of intervals for xArray and FArray
	Scalar *xArray; // for spatially varying emitters;
	Scalar *FArray; // for spatially varying emitters;
	Scalar FTotal; // integral of J(x,t) over the emitter surface;
	Scalar fAvg; // mean value of f over emitter surface
	Scalar geomFactor(Scalar x) {return (rweight) ? x : 1;};
	void computeLocationArray();
	Vector2 computeLocation();
public:
	BeamEmitter(MaxwellianFlux *max, Scalar current,oopicList <LineSegment> *segments,
					Scalar np2c, Scalar thetaDot);
	virtual ~BeamEmitter();
	virtual ParticleList& emit(Scalar t,Scalar dt, Species *species);
	virtual ParticleList& obliqueEmit(Scalar t,Scalar dt);
	virtual void increaseParticleWeight() {emissionRate*=0.5; extra*=0.5;
		Emitter::increaseParticleWeight();}; // this will not work for obliques
	virtual void increaseParticleWeight(int i) {if (i==(species->getID())) 
		{emissionRate*=0.5; extra*=0.5; Emitter::increaseParticleWeight();}}; 
	void set_nIntervals(int n) {nIntervals = n;};
};

#endif	//	ifndef __BEAMEMIT_H
