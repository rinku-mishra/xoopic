/*
====================================================================

LINDMAN.H

	Class representing a boundary on which a wave type boundary
	condition is applied.

0.99	(NTG 12-29-93) Separated into individual module from pic.h.
0.991	(JohnV 01-03-93) Aesthetics, compile.

====================================================================
*/

#ifndef	__LINDMAN_H
#define	__LINDMAN_H

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


class Lindman : public Port
{
public:
	Lindman(oopicList <LineSegment> *segments);
	virtual void	applyFields(Scalar t,Scalar dt);
	virtual ParticleList&	emit(Scalar dt);
	virtual void	setPassives();
	Scalar tOld;
	Vector2* oldHonBoundary;
};

#endif	//	ifndef __LINDMAN_H
