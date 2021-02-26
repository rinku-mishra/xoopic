#ifndef	__CYLINDRI_H
#define	__CYLINDRI_H

/*
====================================================================

cylindri.h

This class represents the axis in an azimuthally symmetric system.

0.99	(NTG 12-29-93) Separated into individual module from pic.h.
0.991	(JohnV 01-03-94) Aesthetics, compile.
1.001	(JohnV 05-06-96) Added toNodes().

====================================================================
*/

// #include	"ovector.h"
//#include	"particle.h"
#ifndef _MSC_VER
#include "symmetry.h"
#endif

#include <oops.h>

class CylindricalAxis : public Symmetry
{
public:
	CylindricalAxis(oopicList <LineSegment> *segments) :
		Symmetry(segments) { BoundaryType = NOTHING; BoundaryName = "noname";
											 BCType = CYLINDRICAL_AXIS;};
	ParticleList&	emit(Scalar t,Scalar dt, Species *species);
	void	setPassives();
	void	collect(Particle& p, Vector3& dxMKS);
   virtual void toNodes() /* throw(Oops) */;
};

#endif	//	ifndef __CYLINDRI_H
