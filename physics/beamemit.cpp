/*
====================================================================

BEAMEMIT.CPP

0.99	(NTG 12-29-93) Separated into individual module from pic.h.
0.991	(JohnV, 01-03-94) Aesthetics, compile.
0.992	(JohnV, 02-28-94) Ensure emit() handles zero particle emission
      gracefully, start extra at 0.5 so emission is not lagging by
		1/2 particle on average.
0.993 (JohnV, ABL 03-03-94) Fix emission so particle train is evenly
	   spaced in absence of fields, improve efficiency.
0.994	(JohnV 03-24-94) Add angular rotation for rigid rotor emission.
0.995	(JohnV 03-29-94) Optimization, fix error in angular component.
0.996	(JohnV, KC 05-02-94) Fix Brilloiun beam problem (was neglecting
      centrifugal force).
0.997 (JohnV 05-17-94) Upgrade to handle arbitrary emission direction.		
0.998	(JohnV 05-25-94) Fix emission of uniform current for off-axis
	   emitter works properly.
0.999 (JohnV 12-18-94) Add normal.
1.001	(JohnV 01-27-95) Add Species.
1.002 (JohnV 07-11-97) Fix problem for emission of subcycled species,
      use Emit::initialPush().
1.003 (JohnV 01-06-99) handle oblique emission near cell edges
1.004 (JohnV 03-10-99) Repair oblique emitter bug for negative angle 
      from atan2().
1.1   (JohnV 01-20-00) Add capability for space and time dependence of
      current in emit().

====================================================================
*/

#include "float.h"
#include	"misc.h"
#include "beamemit.h"
#include	"fields.h"
#include "ptclgrp.h"

//--------------------------------------------------------------------
//	Construct BeamEmitter object

BeamEmitter::BeamEmitter(MaxwellianFlux *max, Scalar current, 
								 oopicList <LineSegment> *segments,
								 Scalar np2c, Scalar _thetaDot)
: Emitter(segments, max->get_speciesPtr(), np2c)
{
  
	maxwellian = max;
//	emissionRate = fabs(current/get_q()*max->get_speciesPtr()->get_supercycleIndex()/max->get_speciesPtr()->get_subcycleIndex());
	emissionRate = fabs(current/get_q());
	extra = 0.5;
	if(alongx2()) normal = Boundary::normal*Vector3(1,0,0);
	else normal = Boundary::normal*Vector3(0,1,0);
	thetaDot = Vector3(0, 0, _thetaDot);
	init = TRUE;
	Farray_compute = FALSE;
	BoundaryType = BEAMEMITTER;
	deltaVertical = Vector2(MAX(j1,j2)*1e-6 + 1e-20, 0)*Boundary::get_normal();
	deltaHorizontal = Vector2(0, MAX(k1,k2)*1e-6 + 1e-20)*Boundary::get_normal();
	if (alongx2()) delta = deltaVertical;
	else delta = deltaHorizontal;
	nIntervals = 0;
	t = 0;

	xArray = NULL;
	FArray = NULL;
}

BeamEmitter::~BeamEmitter()
{
	delete maxwellian;
	if (points!=0) {
	  delete[] oblExtra;
	  delete[] area;
	}
	if (xArray) delete[] xArray;
	if (FArray) delete[] FArray;
}

//--------------------------------------------------------------------
//	initialize internal variables which must wait for the fields ptr.

void BeamEmitter::initialize()
{
  Emitter::initialize();
  Grid* g = fields->get_grid();
  p1 = g->getMKS(j1, k1);
  p2 = g->getMKS(j2, k2);
  if (p1.e2() < p2.e2())
	 {
		rMin = p1.e2();
		rMax = p2.e2();
	 }
  else
	 {
		rMin = p2.e2();	
		rMax = p1.e2();
	 }	
  init = FALSE;

  // handle oblique emitter:
  if (points!=0) {
	  oblExtra = new Scalar[2*(j2 - j1) + 2];  //initialize removed.
	  for(int i=0;i<2*(j2 - j1)+2;i++) oblExtra[i]=0.5;
	  area = new Scalar[2*(j2 - j1) + 2];
	  totalArea = 0;
	  // 90-angle between real and stepwise surface normals;
	  Scalar angle = atan2(p2.e2()-p1.e2(), p2.e1()-p1.e1());
	  for (int j=0; j < 4*(j2 - j1) + 2; j += 2){
		  int index = j/2;
		  int jl = points[j];
		  int kl = points[j+1];
		  int jh = points[j+2];
		  int kh = points[j+3];
		  if (kl==kh) { 
			  if (jl==jh) continue; // dump repeated points
			  else area[index] = g->dS2Prime(jl, kl)*cos(angle);
		  }
		  else {
			  area[index] = 0;
			  for (int k=MIN(kl, kh); k<MAX(kl, kh); k++)
				  area[index] += g->dS1Prime(jl, k)*fabs(sin(angle));
		  }
		  totalArea += area[index];
	  }
	  if (k2 > k1) sign = -1;
	  else sign = 1;
	  delta = deltaHorizontal + sign*deltaVertical;
  }

  t = 0;
  if (get_xtFlag() > 1) { // x-dependence
	 if (nIntervals == 0) {
		if (alongx2()) nIntervals = abs(k2 - k1);
		else nIntervals = abs(j2 - j1);
	 }
	 xArray = new Scalar[nIntervals+1];
	 FArray = new Scalar[nIntervals+1];
	 if (get_xtFlag() < 4) computeLocationArray(); // x and t are decoupled
  }
}

//--------------------------------------------------------------------
//	Emit a beam of electrons uniformly (in current) along the surface
// of the emitter.

ParticleList& BeamEmitter::emit(Scalar _t, Scalar dt, Species* species_to_push)
{
  t = _t;
  if (species==species_to_push){
	 dt *= species->get_subcycleIndex()/(Scalar)species->get_supercycleIndex(); // expand dt for subcycled particles
	 if (init) initialize();
	 if (points!=0) return obliqueEmit(t, dt);
	 Scalar nPer_dt;
	 switch (get_xtFlag()) {
	 case 0: 
	 case 1: 
		fAvg = get_time_value(t);
//	   FTotal = get_time_value(t);
		break;
	 case 2:
		break;
	 case 3:
	 case 4:
	        if(!Farray_compute) computeLocationArray();
		Farray_compute = FALSE;
		break;
	 }
	 nPer_dt = fAvg*emissionRate*dt; 
//	 nPer_dt = FTotal*emissionRate*dt; 
	 extra += nPer_dt;
	 if (extra < 1) return particleList;		//	< 1 particle to emit
	 Scalar del_t = dt/nPer_dt;
	 Vector2 xMKS;
	 while (extra >= 1){
		extra -= 1;
		xMKS = computeLocation();
/*		if (alongx1()||!rweight)
		  xMKS = p1 + frand()*(p2 - p1);
		else {
		  xMKS = Vector2(0.5*(p1.e1() + p2.e1()),
							  sqrt(rMin*rMin + (rMax*rMax - rMin*rMin)*frand()));
		}
*/
		Vector2	x = fields->getGridCoords(xMKS);
		x+=delta;  //make sure the particle isn't on the boundary
		Vector3 u;
		if (thetaDot.e3()){
		  // only correct if r*thetaDot << vz
		  Vector3 v = maxwellian->get_V(normal);
		  if (rweight) v+=xMKS.e2()*thetaDot;
		  u = v/sqrt(1-v*v*iSPEED_OF_LIGHT_SQ);
		}
		else 
		  u = maxwellian->get_U(normal);
		Particle* p = new Particle(x, u, species, np2c);
		Boundary* bPtr = initialPush(del_t*extra, dt, *p);
		if (!bPtr) particleList.add(p);
	 }
  }
  return particleList;
}

ParticleList& BeamEmitter::obliqueEmit(Scalar t, Scalar dt)
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
		Scalar localRate = emissionRate*dt*get_time_value(t)*area[index]/totalArea;
		oblExtra[index] += localRate;

		if (oblExtra[index] < 1) continue; // not enough to emit in this cell
		Scalar del_t = dt/localRate;
		while (oblExtra[index] >= 1) {
			oblExtra[index] -= 1;
			if (kl == kh || !rweight)
				xMKS = p1 + frand()*(p2 - p1);
			else {
				Scalar r_min = p1.e2();
				Scalar r_max = p2.e2();
				xMKS = Vector2(0.5*(p1.e1() + p2.e1()),
									sqrt(r_min*r_min + (r_max*r_max - r_min*r_min)*frand()));
			}
			Vector2 x = fields->getGridCoords(xMKS);
			if (kl == kh) { // horizontal
//				x += deltaHorizontal; // perturb particles off boundary
				normal = Vector3(0, 1, 0)*get_normal();
			}
			else if (k2 > k1) { // up and right
//				x -= deltaVertical;
				normal = Vector3(-1, 0, 0)*get_normal();
			}
			else { // down and right
//				x += deltaVertical;
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

			x += delta;

			Particle* p = new Particle(x, u, species, np2c);
			Boundary* bPtr = initialPush(del_t*oblExtra[index], dt, *p);
			if (!bPtr) particleList.add(p);
		}
  }
  return particleList;
}

//--------------------------------------------------------------------
// computeLocationArray()
// precomputes a array of equally spaced points mapping the 
// cumulative distribution to position along the emitter
// presently only works for orthogonal emitters. Note that
// geomFactor includes the radial nature if cylindrical.
// The integration uses the Trapezoidal Rule.

void BeamEmitter::computeLocationArray()
{
  Vector2 component(0,0);
  if (alongx2()) component.set_e2(1);
  else component.set_e1(1);
  Scalar dx = ((p2 - p1)*component)/nIntervals;
  xArray[0] = p1*component;
  FArray[0] = 0;
  // set up for initial pass thru loop
  // The following line has a bug for the case of cylindrical
  //   geometry, when the emitter is aligned along the X1 (z)
  //   direction (perhaps an unlikely situation).
  // Bruhwiler/Dimitrov 10/12/2000
  //  Scalar f1 = geomFactor(xArray[0])*get_xt_value(xArray[0],t); 

  // The following three lines fix the bug noted above.
  Scalar geometryFactor = 1.;
  if ( alongx2() ) geometryFactor = geomFactor(xArray[0]);
  Scalar f1 = geometryFactor * get_xt_value(xArray[0],t); 

  int i;
  for (i=1; i<=nIntervals; i++) {
	 Scalar f0 = f1; // i-1 value
	 xArray[i] = xArray[0] + i*dx;
	 // Here we fix the bug noted above in the same way.
	 //	 f1 = geomFactor(xArray[i])*get_xt_value(xArray[i], t);
	 geometryFactor = 1.;
	 if ( alongx2() ) geometryFactor = geomFactor(xArray[i]);
	 f1 = geometryFactor*get_xt_value(xArray[i], t);

	 FArray[i] = FArray[i-1] + 0.5*dx*(f0 + f1); // trapezoidalintegration
  }
  FTotal = FArray[nIntervals];
  // avoid a division by zero
  if(FTotal <= FLT_MIN) { fAvg = 0; return;};
  for (i=1; i<=nIntervals; i++) FArray[i] /= FTotal;
  Scalar temp;
  if (alongx2()) 
	 if (rweight) temp = 0.5*(rMax*rMax - rMin*rMin);
	 else temp = p2.e2() - p1.e2();
  else temp = p2.e1() - p1.e1();
  fAvg = FTotal/temp;
}

//--------------------------------------------------------------------
// computeLocation()
// Computes the particle location depending on the distribution function and 
// random numbers.

Vector2 BeamEmitter::computeLocation()
{
  Scalar F = frand();
  if (get_xtFlag() < 2) {
	 if (rweight && alongx2()) return Vector2(0.5*(p1.e1() + p2.e1()),
		 sqrt(rMin*rMin + (rMax*rMax - rMin*rMin)*frand()));
	 else return p1 + F*(p2 - p1);
  }
  else {
	 int i;
	 for (i=0; i<nIntervals; i++) if (F < FArray[i]) break;
	 Scalar x = xArray[i-1] + (xArray[i] - xArray[i-1])*(F - FArray[i-1])/(FArray[i] - FArray[i-1]);
	 if (alongx1()) return Vector2(x, p1.e2());
	 else return Vector2(p1.e1(), x);
  }
}
