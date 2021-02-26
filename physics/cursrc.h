/*
//====================================================================

cursrc.h

	This class represents a conducting surface at the edge connected
to a current source. Uses the algorithm following Vahedi et. al, J. Comp 
Phys. 131, 149-163 (1997).

0.90	(JohnV 06 Jan 2001) Creation.
0.91  (JohnV 06 Apr 2001) Add Dump and Restore to preserve total charge

//====================================================================
*/

#ifndef	__CURSRC_H
#define	__CURSRC_H

#include "eqpot.h"
#include <oops.h>

class CurrentSource : public Equipotential
{
   Scalar current; // total current supplied by the external source in A;
   Scalar Qtotal; // tracks the cumulative charge on the boundary;
   Scalar initV;  // initial voltage
   Scalar area; // surface area of boundary;
	BOOL init; // initialization flag: TRUE = needs initialization;
	BOOL restore; // restore flag: TRUE = from dumpfile
	void initialize(Scalar t, Scalar dt) /* throw(Oops) */; // performs initializations

public:

	CurrentSource(oopicList <LineSegment> *_segments);
	~CurrentSource();
	void	applyFields(Scalar t,Scalar dt) /* throw(Oops) */;
	virtual void collect(Particle &p, Vector3& dxMKS);
   virtual int Restore(FILE *DMPFile, int BoundaryType, Scalar A1, Scalar A2, Scalar B1, 
               Scalar B2,int nQuads);
	virtual int Dump(FILE* DMPFile);

	void setinitV(Scalar V) { initV = V; }
	void initializevoltage(void) /* throw(Oops) */;
};

#endif	//	ifndef __CURSRC_H


