/*
//====================================================================

CONDUCTO.H

	This class represents a conducting surface.

0.99	(NTG 12-29-93) Separated into individual module from pic.h.
0.991	(JohnV 02-09-95) Added secondary electron emission coefficient.
0.992	(JohnV 09-02-95) Moved secondary electron params to Dielectric

//====================================================================
*/

#ifndef	__CONDUCTO_H
#define	__CONDUCTO_H

#include	"ovector.h"
#include "misc.h"
#include	"particle.h"
#include "dielectr.h"

class Species;
class Fields;

class Conductor : public Dielectric
{
public:
	Conductor(oopicList <LineSegment> *segments);
	void	applyFields(Scalar t,Scalar dt) {};
//	virtual ParticleList&	emit(Scalar t,Scalar dt, Species species);
	virtual void	setPassives();
	virtual void	setFields(Fields &f) {Boundary::setFields(f);};
	virtual void	collect(Particle& p, Vector3& dxMKS);
#ifdef HAVE_HDF5
	virtual int dumpH5(dumpHDF5 &dumpObj,int number) {
		 return Boundary::dumpH5(dumpObj,number);
	};  
#endif
	virtual int Dump(FILE *DMPFile) {
		 return Boundary::Dump(DMPFile);
	};

};

#endif	//	ifndef __CONDUCTO_H
