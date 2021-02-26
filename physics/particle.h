/*
====================================================================

PARTICLE.H

	This class defines an object which describes a single particle.  It
	is used when the flexibility of a single particle overides efficiency
	concerns.

0.99	(JohnV 12-28-93) Separated into individual module from pic.h.
0.991	(JohnV 08-24-94) Added variable np2c capability.
0.992	(JohnV 01-25-95) Add pointer to species, replace associated methods.
1.001 (JohnV 02-26-96) Add gamma() and energy().
1.002 (JohnV 12-16-96) Add energy_eV().

====================================================================
*/

#ifndef	__PARTICLE_H
#define	__PARTICLE_H

#include	"oopiclist.h"
#include	"misc.h"
#include "species.h"

class Particle
{
	Species*	species;							// species
	Vector2	x;
	Vector3	u;
	Scalar	np2c;                      // num physical/num comp particles
	BOOL	vary_np2c;							// variable np2c
public:
	Particle(const Vector2& _x, const Vector3& _u, Species* _species, Scalar _np2c,
		BOOL _vary_np2c = FALSE)
		{x = _x; u = _u; species = _species; np2c = _np2c; vary_np2c = _vary_np2c;}
	Vector2	get_x() {return x;}
	void     set_x(Vector2 _x) {x = _x;}
	Vector3	get_u() {return u;}
	void		set_u(Vector3 _u) {u = _u;}
	Scalar	get_m() {return np2c*species->get_m();} // m*np2c
	Scalar get_mMKS() {return species->get_m();} 
	Scalar	get_q() {return np2c*species->get_q();}// q*np2c
	Scalar	get_qMKS() {return species->get_q();}
	Scalar	get_q_over_m() {return species->get_q_over_m();}
	Scalar	get_np2c() {return np2c;}
	BOOL	isVariableWeight() {return vary_np2c;}
	int	get_speciesID() {return species->getID();}
	Species*	get_speciesPtr() {return species;}
	Scalar energy() {return np2c*energyMKS();}
	Scalar energyMKS() {
		Scalar u2 = u*u;
		if (u2 < NR_U2_LIMIT) return 0.5*get_mMKS()*u2; 
		else return (gamma()-1)*get_mMKS()*SPEED_OF_LIGHT_SQ;
	}
	Scalar energy_eV_MKS() {return energyMKS()/PROTON_CHARGE;}
	Scalar gamma() {
		Scalar u2 = u*u;
		if (u2 < NR_U2_LIMIT) return 1.;
		else return sqrt(1. + iSPEED_OF_LIGHT_SQ*u2);
	}
};

typedef oopicList<Particle> ParticleList;

#endif	//	ifndef __PARTICLE_H
