/*
====================================================================

SPEMIT

Emits a single particle as specified.

0.999	(PeterM 08-13-97) Inception.

====================================================================
*/

#ifndef	__SPEMIT_H
#define	__SPEMIT_H
#include "ostring.h"
#include	"ovector.h"
#include "maxwelln.h"
#include	"particle.h"
#include "beamemit.h"

class SingleParticleEmitter : public BeamEmitter
{
  ostring u1;  // velocity of particle to emit
  ostring u2;   // velocity of particle to emit
  ostring x;   //  x position						
  Scalar  delay;  // time interval between particles
  Evaluator *evaluator;
  Scalar local_time;    // This can now be Scalar -- RT, 2003/12/09
  Vector3 u;
  Vector2 X;
  Scalar last_emitted;  // time at which last particle was emitted

protected:
	public:
  
	SingleParticleEmitter(const ostring& u1_in,const ostring& u2_in,const ostring& x_in,
								 Scalar delay_in,
								 MaxwellianFlux *max, Scalar current, 
		oopicList <LineSegment> *segments, Scalar np2c, Scalar thetaDot) :
			BeamEmitter(max, current,segments, np2c, thetaDot) {
#ifndef UNIX
    ostring endLine="\n";
	  u1 = u1_in + endLine;
	  u2 = u2_in + endLine;
	  x = x_in + endLine;
#else
	  u1 = u1_in + '\n';
	  u2 = u2_in + '\n';
	  x = x_in + '\n';
#endif
	 evaluator = new Evaluator(adv_eval);
	  delay  = delay_in;
	  last_emitted = 0;
	  evaluator->add_indirect_variable("t",&local_time);
	};
	virtual void setPassives() {
	 Dielectric::setPassives();
	}

	ParticleList& emit(Scalar t, Scalar dt, Species *species_to_push) {
		if(species==species_to_push){
			if(t - last_emitted > delay)  //time to meit a particle now
				{
					last_emitted = t;
					local_time = t;
					u = Vector3(adv_eval->Evaluate(u1.c_str()),
											adv_eval->Evaluate(u2.c_str()),
											0);
					if(j1==j2)  X = Vector2(j1,k1 + adv_eval->Evaluate(x.c_str()));
					else X = Vector2(j1 + adv_eval->Evaluate(x.c_str()),k1);
					Particle *p = new Particle(X,u,species,np2c);
					particleList.add(p);
				}
		}
		return particleList;
	}
};


#endif	//	ifndef __SPEMIT_H
