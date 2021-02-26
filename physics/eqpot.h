/*
//====================================================================

EQPOT.H

	This class represents a conducting surface at a potential.

0.90	(PeterM 1-9-95) Creation.
0.91	(JohnV 02-09-95) Add secondaries.
1.01	(PeterM 08-12-96) Variables to keep track of endpoints.

//====================================================================
*/

#ifndef	__EQPOT_H
#define	__EQPOT_H

#include	"ovector.h"
#include	"misc.h"
#include	"particle.h"
#include "conducto.h"

class Equipotential : public Conductor
{
 protected:  /// changed from private 8/7/02, Jeff H

	Scalar V;
        ostring VanalyticF;
	Scalar **LaplaceSolution;
	  //left_point and right_point are used in the case of
	  //overlapping dirichlet boundary conditions, which won't
     //work properly because of double-superposition when
     //the same point is in 2 boundaries.
	int omit_left;  // or up point, in the case of a vertical boundary
	int omit_right;  //or down point, in the vertical case
public:

//	Equipotential(int j1, int k1, int j2, int k2, int normal);
	Equipotential(oopicList <LineSegment> *_segments,ostring _VanalyticF=ostring("1"));
	~Equipotential();
	void	applyFields(Scalar t,Scalar dt);
//	virtual void	setPassives();
	virtual void collect(Particle &p, Vector3& dxMKS);
	void InitializeLaplaceSolution();
	void applyBC(Scalar **phi);
};

#endif	//	ifndef __EQPOT_H
