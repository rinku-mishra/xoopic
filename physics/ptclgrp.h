/*
====================================================================

PARTICLEGROUP.H

	This class defines a group of particles which are related by a common
	charge to mass ratio and a common computational particle weight.  The
	ParticleGroup class reduces storage of redundant values of charge,
	mass and particle weight, and improves performance in the critical
	particle push loop.  Variable weighting allows q[] to vary, while
	holding np2c0 fixed at the maximum weighting for the group.

Revision History
0.99	(JohnV 01-07-94)	Separate from pic.h.
0.993	(JohnV 09-13-94) Add variable np2c support.
0.994 (PeterM 09-04-95) Added a method to reduce #particles, increase
      the weight of particles.
1.001 (JohnV, 02-26-96) Add gamma() and energy().
1.1   (JohnV 01-09-97) Add energy_eV().
1.11  (Bruhwiler 09-29-99) Added synchRadiationOn and isSynchRadiationOn()
====================================================================
*/

#ifndef	__PTCLGRP_H
#define	__PTCLGRP_H

#include "stdio.h"
#include "misc.h"
#include "ovector.h"
#include "species.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#ifdef HAVE_HDF5
#include "dumpHDF5.h"
#endif

class ParticleGroup
{
 protected:
  int	maxN;				//	max allowable particles
  int	n;				//	current number of particles
  Scalar  q;				//	computer particle q = qMKS*np2c0
  Scalar  q_over_m;			//	charge to mass ratio
  Scalar  np2c0;			//	physical particles/super particle
  Vector2*  x;		                //	position array (x1, x2)
  Vector3*	u;			//	(gamma*velocity) array (u1, u2, u3)

// mindgame: calculate gamma using sqaure of gamma*velocity
  Scalar gamma(Scalar u2) {
    if (u2 < NR_U2_LIMIT)
      return 1.;
    else
      return sqrt(1. + iSPEED_OF_LIGHT_SQ*u2);
  }

// mindgame: calculate gamma using sqaure of velocity
  Scalar gamma2(Scalar v2) {
    if (v2 < NR_V2_LIMIT)
      return 1.;
    else
      return 1/sqrt(1. - iSPEED_OF_LIGHT_SQ*v2);
  }

  // qArray is NULL, if this group uses fixed weighting of the macroparticles.
  // If variable weighting is used, then qArray[i] is equal to the total charge associated
  //   with macroparticle (i.e. computational particle) i.
  // For variable weighting, np2c0 is equal to the maximum value of np2c for all particles
  //   in this group.  (Note: np2c is not a local variable -- it is calculated when necessary
  //   by the get_np2c(i) method.)
  // The charge associated with a single physical particle, denoted by qMKS, is given by
  //   qMKS=qArray[i]/get_np2c(i).
  // Once more, for the record:  np2c stands for the number of physical particles per
  //   computational particle.  In other words, a single macroparticle represents np2c
  //   physical particles.
  Scalar* qArray;
  Species* species;

  BOOL synchRadiationOn;

public:
  ParticleGroup(int maxN, Species* s, Scalar np2c0, BOOL vary_np2c = FALSE);
  virtual ~ParticleGroup();
  BOOL add(const Vector2& x, const Vector3& u, Scalar np2c=0); // add a single particle
  BOOL add(Particle* p);					//	add a single particle
  BOOL	add(ParticleGroup& p);			//	add a group of particles
  BOOL	addMKS(Vector2 x, Vector3 u);
  ParticleGroup** duplicateParticles(int N);              // duplicate the particles N times
  //
  // we need to be able to remove particles from the particle group (in the
  // tunneling ionization of multi electron atoms). Note the implementation
  // of this function: a particle with index particleIndex is deleted and
  // the last particle in the group is placed at its index.  
  // Returns TRUE if the particle group becomes empty (at deletion of 
  // the last particle in the group), otherwise it resturns FALSE. dad 06/29/01.
  //
  BOOL remove(int particleIndex);

  void fill(int index);
  virtual void advance(Fields& fields, Scalar dt);	//	interpolate+push+accumulate I

  Vector2*	get_xPtr() {return x;}		// required to take address in PlasmaDev
  Vector2* get_x() {return x;}
  Vector2	get_x(int i) {return x[i];}
  Vector3*	get_uPtr() {return u;}		// required to take address in PlasmaDev
  Vector3* get_u() {return u;}
  Vector3	get_u(int i) {return u[i];}
  int	get_n() {return n;};
  void set_n(int i) {n=i;};
 
  void set_v(Vector3 utmp, int i){
    u[i].set_e1(utmp.e1());
    u[i].set_e2(utmp.e2());
    u[i].set_e3(utmp.e3());
  }

  Scalar	get_q() {return q;};
  Scalar	get_q(int i) {return ((qArray) ? qArray[i] : q);};
  Scalar	get_q_over_m() {return q_over_m;};
  Scalar	get_m() {return q/q_over_m;};
  Scalar	get_m(int i) {return get_q(i)/q_over_m;};
  Scalar	get_np2c(int i) {return (qArray) ? np2c0*qArray[i]/q : np2c0;}
  BOOL	vary_np2c(void) { return (BOOL)(qArray!=0); }
  int	get_maxN() {return maxN;};
  int availableSlots() {return maxN - n;}
  Scalar	gamma(int i) {return gamma(u[i]);}
  Scalar	get_qMKS() {return q/np2c0;};
  Scalar	get_qMKS(int i) {return get_q(i)/np2c0;};
  Scalar	get_mMKS() {return get_m()/np2c0;};
  Scalar	get_mMKS(int i) {return get_m(i)/np2c0;};
  int	get_speciesID() {return species->getID();}
  Species*	get_species() {return species;}
  BOOL	isVariableWeight() {return (BOOL)(qArray!=0);}
  void increaseParticleWeight();

#ifdef HAVE_HDF5
  virtual int dumpH5(dumpHDF5 &dumpObj, Grid *grid,string groupName);
  virtual int restoreH5(dumpHDF5 &restoreObj, Grid *grid, Scalar np2cFactor,string whichGroup);
#endif 
  virtual int Dump(FILE *DMPFile,Grid *grid);
  virtual int Restore(FILE *DMPFile,Grid *grid, Scalar np2cFactor);
  virtual int Restore_2_00(FILE *DMPFile);

// relocates all the particles, discards ones out of bounds.
	void shiftParticles(Fields &f, Vector2 shift);

// determine whether synchrotron radiation effects are being applied
	BOOL isSynchRadiationOn() {return synchRadiationOn;}

// mindgame: calculate energy and gamma which are valid
//         for both rel and non-rel regimes.
//  Scalar gamma(Vector3& u) {return sqrt(1 + iSPEED_OF_LIGHT*iSPEED_OF_LIGHT*(u*u));}
//  Scalar energy(int i) {return 0.5*get_m(i)*(u[i]*u[i])/gamma(i);}
//  Scalar energy_eV(int i) {return 6.242e18*0.5*get_mMKS(i)*(u[i]*u[i])/gamma(i);}
  Scalar energy_eV(int i) {
    Scalar u2 = u[i]*u[i];
    if (u2 < NR_U2_LIMIT){
      return 0.5*get_mMKS(i)/PROTON_CHARGE*u2;
    }
    else
      return (gamma(u2)-1.)*get_mMKS(i)*SPEED_OF_LIGHT_SQ/PROTON_CHARGE;
  }

  Scalar energy2_eV(Vector3 v, int i) {
    Scalar v2 = v*v;
    if (v2 < NR_V2_LIMIT)
      return 0.5*get_mMKS(i)/PROTON_CHARGE*v2;
    else
    return (gamma2(v2)-1.)*get_mMKS(i)*SPEED_OF_LIGHT_SQ/PROTON_CHARGE;
 }

  Scalar gamma(Vector3 u) {
    Scalar u2 = u*u;
    if (u2 < NR_U2_LIMIT)
      return 1.;
    else
      return sqrt(1. + iSPEED_OF_LIGHT_SQ*u2);
  }

  Scalar gamma2(Vector3 v) {
    Scalar v2 = v*v;
    if (v2 < NR_V2_LIMIT)
      return 1.;
    else
      return 1/sqrt(1. - iSPEED_OF_LIGHT_SQ*v2);
  }

};

#endif	//	ifndef __PTCLGRP_H
