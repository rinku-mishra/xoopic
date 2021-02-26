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

====================================================================
*/

#ifndef	__EMITPORT_H
#define	__EMITPORT_H

#include	"ovector.h"
#include "maxwelln.h"
#include	"particle.h"
#include "beamemit.h"

class EmitPort : public BeamEmitter 
{
protected:
	public:
	EmitPort(MaxwellianFlux *max, Scalar current, 
		oopicList <LineSegment> *segments, Scalar np2c, Scalar thetaDot) :
			BeamEmitter(max, current,segments, np2c, thetaDot) {
	BCType = FREESPACE;};
	virtual void setPassives() {
	 Dielectric::setPassives();
	}

};

#endif	//	ifndef __EMITPORT_H
