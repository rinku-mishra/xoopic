/*
====================================================================

EMITTER.H

	A pure virtual class representing all conducting surfaces that can
	emit particles.

0.99	(NTG 12-29-93) Separated into individual module from pic.h.
0.992	(JohnV 01-11-94) Restructure inheritance from Dielectric, fix
		collect() to throw out particles, setPassives() for conductor
		like default behavior.
0.993	(JohnV 02-10-95) Add secondary emission.
0.994	(JohnV 09-02-95) Moved secondary parameters up to Dielectric.
0.995 (JohnV 09-06-95) Added get_q(), removed q and m.
1.1   (JohnV 03-12-97) Add generic initialPush() for advancing emitted particles
3.01  (DavidB 10-07-02) Added an empty virtual destructor to fix restart bug.

====================================================================
*/

#ifndef	__EMITTER_H
#define	__EMITTER_H

#include "ovector.h"
#include "misc.h"
#include "particle.h"
#include "dielectr.h"

class Species;

class Emitter : public Dielectric {

 protected:

  Species* species;                  // species info for emitted particles
  Scalar np2c;                       // n_physical/n_computer particles
  BOOL rweight;                      // should we use r^2 weighting?

  Boundary* initialPush(Scalar dt, Scalar Dt, Particle& p);

 public:

  Emitter(oopicList <LineSegment> *segments, Species* s, Scalar np2c);
  virtual ~Emitter() {}
  virtual ParticleList&	emit(Scalar t, Scalar dt, Species *species) = 0;
  virtual void collect(Particle& p, Vector3& dxMKS);
  void initialize();
  virtual void setPassives();
  virtual void setFields(Fields &f) {
    Boundary::setFields(f);
  }
  virtual void increaseParticleWeight() {
    np2c*=2;
  }
  virtual void increaseParticleWeight(int i) {
    if (i==(species->getID())) np2c*=2;
  } 
  Scalar get_q() {
    return species->get_q()*np2c;
  }
  Scalar get_m() {
    return species->get_m()*np2c;
  }
  virtual int Dump(FILE *DMPFile) {
    return Boundary::Dump(DMPFile);
  }
};

#endif   // __EMITTER_H
