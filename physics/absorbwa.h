/*
====================================================================

ABSORBWAVE.H

Class representing a boundary object to absorb a wave with phase
velocity vphase.

Revision/Programmer/Date
0.99	(BillP 09-20-94) Create file.
0.91	(JohnV 11-30-94) Compile and enhance.

====================================================================
*/

#ifndef __ABSORBWAVE_H
#define __ABSORBWAVE_H

#include "port.h"

class AbsorbWave : public Port
{
	Scalar vPhase; // phase vel/speed of light
	Scalar tOld;
	Vector2* oldE;
	int jmin, jmax;
	int kmin, kmax;
public:
	AbsorbWave(oopicList <LineSegment>*segments, Scalar wavephase);
	virtual void applyFields(Scalar t,Scalar dt);
	virtual void setPassives() {};
};

#endif  //      ifndef __ABSORBWAVE_H
