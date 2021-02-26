/*
====================================================================

LAUNCHWAVE.H

Class representing a boundary object to launch a wave with phase
velocity vphase, amplitude temamp, and trapezoidal pulse.

Revision/Programmer/Date
0.9	(BillP 10-20-94) Create file.
0.91	(JohnV 11-30-94) Compile and add to project.

====================================================================
*/

#ifndef __LAUNCHWAVE_H
#define __LAUNCHWAVE_H

#include "port.h"

class LaunchWave : public Port
{
	Scalar vPhase;
	Scalar riseTime;
	Scalar pulseLength;
	Scalar amplitude;
	Scalar tOld;
	Scalar frequency;
	Vector2* oldE;
	int jmin, jmax;
	int kmin, kmax;
public:
	LaunchWave(oopicList <LineSegment> *segments, Scalar wavePhase,
		Scalar amplitude, Scalar riseTime, Scalar pulseLength, Scalar frequency);
	~LaunchWave();
	virtual void applyFields(Scalar t,Scalar dt);
	virtual void setPassives() {};
};

#endif  //      ifndef __LAUNCHWAVE_H
