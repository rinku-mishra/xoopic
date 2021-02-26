/*
====================================================================

LOOKBACK.H

	Class representing a boundary on which a wave type boundary
	condition is applied.

0.99	(NTG 12-29-93) Separated into individual module from pic.h.
0.991	(JohnV 01-03-93) Aesthetics, compile.

====================================================================
*/

#ifndef	__LOOKBACK_H
#define	__LOOKBACK_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if defined(_MSC_VER)
#include <iomanip>
#include <iostream>
using std::ends;
#else

using namespace std;
#endif

#include "ovector.h"
#include "misc.h"
#include "particle.h"
#include "boundary.h"
#include "port.h"


class Lookback : public Port
{
public:
	Lookback(oopicList <LineSegment> *segments);
   virtual void	applyFields(Scalar t,Scalar dt);
	//virtual ParticleList&	emit(Scalar t,Scalar dt);
	virtual void	setPassives();
	Vector3** oldB; 
	Vector3** oldE;
	Vector3** dirvB;
	Vector3** dirvE;
	Vector2*  edgePos;
	Vector3*  N;
	Scalar i4PI;
	int currentTime, MaxLookback;
	int J;
	int K;
	Scalar x1f;
	Scalar x2f;
	Scalar dt, idt;

	};

#endif	//	ifndef __LOOKBACK_H
