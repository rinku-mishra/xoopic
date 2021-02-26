/*
====================================================================

POLAR.CPP

0.99	(JV 09-13-95) Creation.

====================================================================
*/

#include "polar.h"
#include	"fields.h"
#include "ptclgrp.h"
#include "psolve.h"

#ifdef _MSC_VER
using std::cout;
using std::endl;
#endif

extern Evaluator *adv_eval;

Polar::Polar(ostring _Phi,oopicList <LineSegment> *segments, Scalar _transmissivity)
	: Conductor(segments)
{
	transmissivity=_transmissivity;  //initialize the transmissivity
					 //on creation
	BoundaryType = CONDUCTOR;		//new boundary type, not
	Phi = _Phi + ostring('\n');  //evaluator needs a '/n'
}

//--------------------------------------------------------------------
//	Configure Fields::iC and :: iL for passive BC.  Currently assumes
//	conductor, sets iC = 0 along its surface.  Also requires vertical
//	or horizontal boundaries.  Note that we must step thru in the
//	correct order; this once expended a large amount of effort to
//	determine that iC along a conductor with k2 < k1 was not being
//	set, causing a leaky boundary.

void Polar::setPassives()
{
  oopicListIter <LineSegment> lsI(*segments);
	for(lsI.restart();!lsI.Done();lsI++) {
	  int j1,j2,k1,k2,normal,*points;  //local copies of above
	  j1=(int)lsI.current()->A.e1();
	  k1=(int)lsI.current()->A.e2();
	  j2=(int)lsI.current()->B.e1();
	  k2=(int)lsI.current()->B.e2();
	  points=lsI.current()->points; 
	 normal=lsI.current()->normal; 

	  if (j1==j2)								//	vertical
		 {
			for (int k=k1; k<=k2; k++)
			  {
				 Scalar iC2 = fields->getiC()[j1][k].e2();
				 Scalar iC3 = fields->getiC()[j1][k].e3();
				 Scalar PhiX = 0;
				 Scalar X = (Scalar)(k-k1)/((Scalar)(k2-k1));
				 adv_eval->add_variable("X",X);
				 PhiX=adv_eval->Evaluate(Phi);
				 iC2 *= sin(PhiX * 3.141592654 / 180.0);
				 iC3 *= cos(PhiX * 3.141592654 / 180.0);
													 
				 fields->set_iC2(j1, k, iC2);
				 fields->set_iC3(j1, k, iC3);
			  }
		 }
	  else if(k1==k2)											//	horizontal
		 {
			for (int j=j1; j<=j2; j++)
			  {
				 Scalar iC1 = fields->getiC()[j][k1].e1();
				 Scalar iC3 = fields->getiC()[j][k1].e3();
				 Scalar PhiX = 0;
				 Scalar X = (Scalar)(j-j1)/((Scalar)(j2-j1));
				 adv_eval->add_variable("X",X);
				 PhiX=adv_eval->Evaluate(Phi);
				 iC1 *= sin(PhiX * 3.141592654 / 180.0);
				 iC3 *= cos(PhiX * 3.141592654 / 180.0);
				 fields->set_iC1(j, k1, iC1);
				 fields->set_iC3(j, k1, iC3);
			  }
		 }
	  else {
		 cout << "Polarizers don't support oblique segments!  Check input file.\n";
	  }
	}
}
//--------------------------------------------------------------------
// collect() 
#ifndef __linux__
#ifndef _MSC_VER
#pragma argsused
#endif
#endif
void Polar::collect(Particle& p, Vector3& dxMKS)
{
	if((Scalar)frand()<transmissivity) {
		Vector2 x=p.get_x();  // not really a function call, this is inlined
		
		x+=p.get_u().e1()/(fabs(p.get_u().e1())+1.0e-20)
			*Vector2(  j1*1e-6 + 1.0e-20  ,   0);
		Boundary*	bPtr = fields->translateAccumulate(x, dxMKS, 
			0);  // small mistake in physical model
			//get_q()/dt);

		if (bPtr) bPtr->collect(p, dxMKS);
		else particleList.add(&p);
		Boundary::collect(p,dxMKS);
	} else
	  Dielectric::collect(p,dxMKS);
//	delete &p;  deletion already done in Dielectric::collect
}

//--------------------------------------------------------------------
//	Polar::emit() commented out...  Conductor does this for us.
//#pragma argsused
//ParticleList& Polar::emit(Scalar t,Scalar dt)
//{
//	return particleList;
//}
