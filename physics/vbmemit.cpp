/*
====================================================================

vbmemit.cpp

0.99	(JohnV 09-13-94) Generate to polymorph BeamEmitter for variable
		weighted particles.
0.991	(JohnV 02-10-95) Add secondary electron emission.
1.001 (JohnV 07-11-97) Fix problem for emission of subcycled species,
      use Emit::initialPush().
1.1   (JohnV 02-13-00) Add J(x,t) capability.
1.21  (JohnV 03-26-00) Add nEmit to set the number per dt to emit

====================================================================
*/

#if defined(_MSC_VER)
#include <algorithm>
#endif

#include "misc.h"
#include "vbmemit.h"
#include "fields.h"
#include "ptclgrp.h"

#if defined(__KCC) || defined(__xlC__)
using std::min;
#endif

//--------------------------------------------------------------------
//	Construct BeamEmitter object

VarWeightBeamEmitter::VarWeightBeamEmitter(MaxwellianFlux *max, Scalar current,
	oopicList <LineSegment> *segments, Scalar np2c, Scalar
	thetaDot, int _nEmit)
	: BeamEmitter(max, current, segments, np2c, thetaDot)
{
	extra = 0;
	q_over_m = get_q()/get_m();
	obl_rFraction = NULL;
	nEmit = _nEmit;
}

VarWeightBeamEmitter::~VarWeightBeamEmitter()
{
  delete[] obl_rFraction;
}

//--------------------------------------------------------------------
//	Set up local initializations and call BeamEmitter::initalize()

void VarWeightBeamEmitter::initialize()
{
	BeamEmitter::initialize();
	w0 = rMin/rMax;
	w1 = (rMax - rMin)/rMax;
	if (alongx2()) rFraction = getNext_rFraction();
	else if (alongx1()) rFraction = 1;
	if (points!=0) {
	  Grid* g = fields->get_grid();
	  obl_rFraction = new Scalar[2*(j2 - j1) + 2];
	  for (int j=0; j < 4*(j2 - j1) + 2; j += 2){
		 int index = j/2;
		 int jl = points[j];
		 int kl = points[j+1];
		 int jh = points[j+2];
		 int kh = points[j+3];
		 if (jl == jh) {
			if (kl == kh) continue; // skip duplicates
			else {
			  /*			  rMin = g->getMKS(jl, kl).e2();
			  rMax = g->getMKS(jl, kh).e2();*/
			  w0 = g->getMKS(jl, kl).e2()/rMax;
			  w1 = (g->getMKS(jl, kh).e2() - g->getMKS(jl, kl).e2())/rMax;
			  obl_rFraction[index] = getNext_rFraction();
			}
		 }
		 else obl_rFraction[index] = g->getMKS(jl, kl).e2()/rMax;
	  }
	}
}

//--------------------------------------------------------------------
//	Emit a beam of electrons along the surface
// of the emitter.  The weights of the particles are adjusted so that
//	the particles are uniformly distributed in r, with weights from
//	0 to max_np2c for radii 0 to rmBeam = p2.e2().

ParticleList& VarWeightBeamEmitter::emit(Scalar _t, Scalar dt, Species* species_to_push)
{
  if (species==species_to_push){
	 t = _t;
	 dt *= species->get_subcycleIndex()/(Scalar)species->get_supercycleIndex(); // expand dt for subcycled particles
	 if (init) initialize();
	 if (points!=0) return obliqueEmit(t, dt);
	 if (emissionRate <= 0) return particleList;
	 Scalar nPer_dt;
	 switch (get_xtFlag()) {
	 case 0: 
	 case 1: 
		fAvg = get_time_value(t);
		break;
	 case 2:
		break;
	 case 3:
	 case 4:
		computeLocationArray();
		break;
	 }
	 nPer_dt = fAvg*emissionRate*dt; // based on np2cMax
	 extra += nPer_dt;
	 // Don't divide by zero: don't bother if nPer_dt < 1
	 Scalar rel_weight = 1;
	 if (nPer_dt < 1e-9) return particleList;
	 else if (nEmit > 0) rel_weight = nPer_dt/nEmit; // scale for fixed nEmit
#if defined(_MSC_VER)
	 Scalar del_t = __min(dt, dt/nPer_dt*rel_weight);
#else
	 Scalar del_t = min(dt, dt/nPer_dt*rel_weight);
#endif
	 Vector2 xMKS;
	 Scalar wt;
	 while (extra > 0) {
		if (alongx1() || !rweight) xMKS = p1 + frand()*(p2 - p1);
		else xMKS = Vector2(0.5*(p1.e1() + p2.e1()), rMax*rFraction);
		Vector2	x = fields->getGridCoords(xMKS);// x in grid coords
		x += delta;								// perturb just off of surface
		Vector3 u;
		if (thetaDot.e3()){
		  // only correct if r*thetaDot << vz !!
		  Vector3 v = maxwellian->get_V(normal);
		  if (rweight) v+=xMKS.e2()*thetaDot;
		  u = v/sqrt(1-v*v*iSPEED_OF_LIGHT_SQ);
		}
		else 
		  u = maxwellian->get_U(normal);
		Scalar local_x;
		if (alongx2()) local_x = xMKS.e2();
		else local_x = xMKS.e1();
		wt = 2*rFraction*get_xt_value(local_x, t)/fAvg*rel_weight;
		if (wt > extra) break;
		Particle* p = new Particle(x, u, species, np2c*wt, TRUE);
		extra -= wt;
		Boundary* bPtr = initialPush(del_t*extra, dt, *p);
		if (!bPtr) particleList.add(p);
		if (alongx2() && rweight) rFraction = getNext_rFraction();
	 }
  }  
  return particleList;
}

ParticleList& VarWeightBeamEmitter::obliqueEmit(Scalar t, Scalar dt)
{
  Vector2 xMKS;
  for (int j=0; j < 4*(j2 - j1) + 2; j += 2){
	 int index = j/2;
	 int jl = points[j];
	 int kl = points[j+1];
	 int jh = points[j+2];
	 int kh = points[j+3];

	 if (jh == jl && kh == kl) continue; // if this is a duplicate point, get next

	 Vector2 p1 = fields->getMKS(jl, kl); // note these are local to this segment
	 Vector2 p2 = fields->getMKS(jh, kh);
	 w0 = p1.e2()/rMax;
	 w1 = (p2.e2() - p1.e2())/rMax;
	 Scalar localRate = emissionRate*dt*get_time_value(t)*area[index]/totalArea;
	 oblExtra[index] += localRate;
	 Scalar del_t = dt/localRate;
	 printf("index=%d, obl_rFraction=%g, oblExtra=%g\n", index, obl_rFraction[index], oblExtra[index]);
	 while (obl_rFraction[index] <= oblExtra[index]) {
		oblExtra[index] -= obl_rFraction[index];
		if (kl == kh || !rweight) xMKS = p1 + frand()*(p2 - p1);
		else {
		  xMKS = Vector2(0.5*(p1.e1() + p2.e1()), rMax*obl_rFraction[index]);
		}
		Vector2 x = fields->getGridCoords(xMKS);
		if (kl == kh) { // horizontal
		  x += deltaHorizontal; // perturb particles off boundary
		  normal = Vector3(0, 1, 0)*get_normal();
		}
		else if (k2 > k1) { // up and right
		  x -= deltaVertical;
		  normal = Vector3(-1, 0, 0)*get_normal();
		}
		else { // down and right
		  x += deltaVertical;
		  normal = Vector3(1, 0, 0)*get_normal();
		}
		Vector3 u;
		if (thetaDot.e3()){
			// only correct if r*thetaDot << vz
			//  thetaDot is yDot for ZXgeometry
			Vector3 v = maxwellian->get_V(normal);
			if (rweight) v+=xMKS.e2()*thetaDot;
			u = v/sqrt(1-v*v*iSPEED_OF_LIGHT_SQ);
		}
		else 
			u = maxwellian->get_U(normal);
		Particle* p = new Particle(x, u, species, np2c);
		Boundary* bPtr = initialPush(del_t*oblExtra[index], dt, *p);
		if (!bPtr) particleList.add(p);

/*		Vector3	dxMKS = fields->differentialMove(xMKS, v,
			u, del_t*oblExtra[index]);
		Boundary* bPtr = fields->translateAccumulate(x, dxMKS, 
			obl_rFraction[index]*get_q()/dt);

		Particle* particle = new Particle(x, u, species, np2cMax()*obl_rFraction[index], 
													 TRUE);
		if (bPtr) bPtr->collect(*particle, dxMKS);
		else particleList.add(particle);
*/
		if (kh != kl) obl_rFraction[index] = getNext_rFraction();
	 }
  }
  return particleList;
}
