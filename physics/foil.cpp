//====================================================================
#ifdef	COMMENTS

FOIL.CPP

0.90	(BillP 3-13-95) Creation.

#endif	//COMMENTS
//====================================================================

#include "foil.h"
#include	"fields.h"
#include "ptclgrp.h"
#include "gpdist.h"

Foil::Foil(Scalar _nemit, oopicList <LineSegment> *segments) 
: Conductor(segments) 
{
	 nemit = _nemit;

 //null body since this is just a conductor
  
};




//--------------------------------------------------------------------
//	Collect particles

void Foil::collect(Particle& p, Vector3& dxMKS)
{
	nemit = 2.4 ;                         // example
	Particle *np;
	int intNemit = (int) nemit;
	Scalar fract = nemit - intNemit;
	if ((Scalar)frand() < fract)
		{
			intNemit = intNemit + 1;
		}
	Scalar P1 = -p.get_u().e1();
	Vector2 x(p.get_x().e1(),p.get_x().e2());
	Vector3 u(-p.get_u().e1(),p.get_u().e2(),p.get_u().e3());
	np = new Particle(x,u,p.get_speciesPtr(),p.get_np2c(),
							p.isVariableWeight());

	p.get_u().set_e1(P1);		//	reverse velocity
	//		p.get_u().set_e2(p.get_u().e2());		//	 velocity
	p.get_x().set_e2(fabs(p.get_x().e2()));	//	abs(radius)
	dxMKS.set_e2(-dxMKS.e2());	         // currently unused...

	particleList.push(np);
	Boundary::collect(p,dxMKS);
	delete &p;

/*	if (intNemit>1)
		{
			for (int k = 1; k < intNemit; k++) //sec emit loop
				{
					//					p.get_u().set_e1(-p.get_u().e1());		//	reverse velocity
					//		p.get_u().set_e2(p.get_u().e2());		//	 velocity
					p.get_x().set_e2(fabs(p.get_x().e2()+.1*frand()*p.get_x().e2()));	//	abs(radius)
					//					dxMKS.set_e2(-dxMKS.e2());	         // currently unused...

					Particle* particle = new Particle(p.get_x(),p.get_u(), p.get_speciesPtr(), p.get_np2c());
					particleList.add(particle);
				}
		}*/
}

//--------------------------------------------------------------------
//	Reinject collected particles

ParticleList& Foil::emit(Scalar t,Scalar dt)
{
	return particleList;
}
