//--------------------------------------------------------------------
//
// File:        spbound.h
//
// Purpose:     Interface for a SpatialRegionBoundary - a boundary
//              between two spatial regions.  Has methods for passing
//              data between regions.
//
// Version:     $Id: spbound.h 2193 2004-08-27 22:54:57Z yew $
//
//--------------------------------------------------------------------

#ifndef	__SPBOUND_H
#define	__SPBOUND_H

/*
====================================================================

spbound.h

The boundary between spatial regions.

0.99	(PeterM 4-4-97) Inception
2.0?  (Cary 23 Jan 00) Added method recvParticles()
====================================================================
*/

#include <stdio.h>
#include "boundary.h"
#include "spblink.h"
#include <oops.h>


class ostring;
class PDistCol;
class PoytFlux;



//--------------------------------------------------------------------
//	Boundary:  This class defines the generic boundary object for
//	some region.  All other boundaries inherit from this pure virtual.

class SpatialRegionBoundary : public Boundary
{
protected:
public:
   // guard fields
   Vector3 *lJa,*lJb,*lJc,*lEa,*lEb,*lEc,*lBa,*lBb,*lBc;
   Vector3 *liLa,*liLb,*liLc;

  Vector3 *lJ;  // the J's we'll need for this boundary
  Vector3 *lE;  // the E's we'll need for this boundary
  Vector3 *lB;  // the B's we'll need for this boundary
  Vector3 *liL;  // the iL's needed for this boundary	
  
  Vector3 *lJsend;  // the J's we'll send for this boundary
  Vector3 *lEsend;  // the E's we'll send for this boundary
  Vector3 *lBsend;  // the B's we'll send for this boundary
  Vector3 *liLsend;  // the iL's needed for this boundary

  //
  // the following buffers are needed for the communication of
  // the neutral gas density (in NGD objects) data strucures
  // across boundaries; dad 05/16/01
  // 
  int numElementsNGDbuf;
  Scalar* lNGDbuf;
  Scalar* lNGDbufSend;
  //
  // a helper function to allocate memory for the NGD buffers
  //
  void allocMemNGDbuffers();
  //
  // access the number of elements of the NGD buffers
  // 
  int getnumElementsNGDbuf() const { return numElementsNGDbuf; }

  SPBoundLink *theLink;  //  communications with the paired boundary
	 
  particlePassDat *ptclPassArray;
  int ptclPassArraySize;
  int number_Particles_passing;
  ParticleGroupList **localParticles; // local particle group list.

  ostring PartnerName;
  SpatialRegionBoundary(oopicList <LineSegment> *segments) /* throw(Oops) */;
  virtual ~SpatialRegionBoundary();
  virtual void applyFields(Scalar t,Scalar dt);
  virtual void collect(Particle& p, Vector3& dxMKS);
  virtual ParticleList&	emit(Scalar t,Scalar dt, Species *species);
  virtual void setPassives();	//	set up iC and iL if needed
  virtual void putCharge_and_Current(Scalar t);
  //virtual int Restore(FILE *DMPFile, int j1, int k1, int j2, int k2){};
  //virtual int Dump(FILE *DMPFile);
  //virtual void increaseParticleWeight(){};
  void send_liL();  //initialization only, exchange liL between boundary pairs.
  void get_liL();  //initialization only, get the liL which was sent.
  void initialize_passives();
   void SRBadvanceB(Scalar t,Scalar dt);
   void SRBadvanceE(Scalar t,Scalar dt);
   virtual void toNodes();
  virtual void passParticles();
  void LinkBoundary(SPBoundLink *theCounterpart) {
	  theLink = theCounterpart;
	}
  int isLinked() { return ((theLink==0)? 0:1);}
  int getLength() { return MAX(j2-j1,k2-k1);};

  /**
   * Collect the particle into the shiftedParticles array.
   * This is a handoff.  SpatialRegionBoundary is subsequently
   * responsible for deletion
   *
   * @param p the particle to be collected
   */
  virtual void collectShiftedParticle(Particle* p);
  
  /**
   * pass the shifted particles over the boundary
   */
  virtual void passShiftedParticles();
  
  /**
   * Receive the shifted particles from the boundary
   *
   * @param dir the direction integer
   */
  virtual ParticleList& recvShiftedParticles();
  
  /**
   * Ask for a stripe of fields for this boundary
   *
   */
  virtual void askStripe();
  
  /**
   * Send a stripe of fields for this boundary
   */
  virtual void sendStripe(int dir);
  
  /**
   * Receive a stripe of fields from this boundary
   *
   * @param dir the direction integer
   */
  virtual void recvStripe(int dir);

  /**
   * Ask for a stripe of NGDs for this boundary
   */
  virtual void askNGDStripe();
  /**
   * Send a stripe of the data structures related to 
   * the Neutral Gas Density  for this boundary
   *
   * @param dir the direction integer
   */
  virtual void sendNGDStripe(int dir) /* throw(Oops) */;
  /**
   * Receive a stripe of NGDs from this boundary.
   */
  virtual void recvNGDStripe(int dir) /* throw(Oops) */;

 private:  

};

#endif	//	ifndef __SPBOUND_H
