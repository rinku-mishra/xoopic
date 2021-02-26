/*
 * port.h
 *
 * 	Class representing a boundary on which a wave type boundary
 *	condition is applied.
 *
 * 0.99 	(NTG 12-29-93) Separated into individual module from pic.h.
 * 0.991	(JohnV 01-03-93) Aesthetics, compile.
 */

#ifndef	__PORT_H
#define	__PORT_H

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


class Port : public Boundary
{
public:
	Port(oopicList <LineSegment> *segments);
   virtual void	applyFields(Scalar t,Scalar dt);
	virtual ParticleList&	emit(Scalar t,Scalar dt, Species* species);
	virtual void collect(Particle& p, Vector3& dxMKS);
	virtual void	setPassives();
};

#endif	//	ifndef __PORT_H
