/* ====================================================================
 * Change Log for file  fowler_nordheim_emit.cpp
 *
 * 3.01 (DavidB 10-03-02) Initial code.  This development effort was
 *                        funded by a contract from Cornell University
 *                        to Tech-X Corporation.
 * 3.02 (JohnV 10-May-2003) Fixed emission problem for some orientations & cylindrical
 * 3.03 (JohnV 27-May-2003) Prevent emission when field is reversed.
 * 3.04 (JohnV 28-May-2003) Repaired bug in the location of E for FN calculation
 * ====================================================================
 */

#include "float.h"
#include "misc.h"
#include "fowler_nordheim_emit.h"
#include "fields.h"
#include "ptclgrp.h"

//--------------------------------------------------------------------
// The constructor

FowlerNordheimEmitter::
FowlerNordheimEmitter(oopicList <LineSegment>* _segments,
                      MaxwellianFlux*          _maxwellian,
                      Scalar                   _np2c,
                      Scalar                   _A_FN,
                      Scalar                   _beta_FN,
                      Scalar                   _B_FN,
                      Scalar                   _C_v_FN,
                      Scalar                   _C_y_FN,
                      Scalar                   _Phi_w_FN,
                      int                      _nIntervals)
  : Emitter(_segments,
            _maxwellian -> get_speciesPtr(),
            _np2c)
{
  /** Debugging Code */
  //  cerr << "\n\nEntering constructor of the FowlerNordheimEmitter class:" << endl;
  //  cerr << "    _np2c       = " << _np2c       << endl;
  //  cerr << "    _A_FN       = " << _A_FN       << endl;
  //  cerr << "    _beta_FN    = " << _beta_FN    << endl;
  //  cerr << "    _B_FN       = " << _B_FN       << endl;
  //  cerr << "    _C_v_FN     = " << _C_v_FN     << endl;
  //  cerr << "    _C_y_FN     = " << _C_y_FN     << endl;
  //  cerr << "    _Phi_w_FN   = " << _Phi_w_FN   << endl;
  //  cerr << "    _nIntervals = " << _nIntervals << endl;
  /** End of Debugging Code */

  maxwellian = _maxwellian;
  A_FN       = _A_FN;
  B_FN       = _B_FN;
  C_v_FN     = _C_v_FN;
  C_y_FN     = _C_y_FN;
  Phi_w_FN   = _Phi_w_FN;
  beta_FN    = _beta_FN;  

  extra = 0.5;
  if (alongx2()) normal = Boundary::normal*Vector3(1,0,0);
  else normal = Boundary::normal*Vector3(0,1,0);
  init = TRUE;
  BoundaryType = FOWLER_NORDHEIM_EMITTER;
  BCType = FREESPACE;

  deltaVertical   = Vector2(MAX(j1,j2)*Scalar_EPSILON + Scalar_MIN, 0) * Boundary::get_normal();
  deltaHorizontal = Vector2(0, MAX(k1,k2)*Scalar_EPSILON + Scalar_MIN) * Boundary::get_normal();
  if (alongx2()) delta = deltaVertical;
  else delta = deltaHorizontal;

  nIntervals = _nIntervals;

  xArray = NULL;
  FArray = NULL;
}

FowlerNordheimEmitter::~FowlerNordheimEmitter()
{
  if (maxwellian) {
    delete maxwellian;
    maxwellian = 0;
  }
  if (xArray) {
    delete[] xArray;
    xArray = 0;
  }
  if (FArray) {
    delete[] FArray;
    FArray = 0;
  }
}

//--------------------------------------------------------------------
//	initialize internal variables which must wait for the fields ptr.

void FowlerNordheimEmitter::initialize() {
  Emitter::initialize();
  Grid* g = fields->get_grid();
  p1 = g->getMKS(j1, k1);
  p2 = g->getMKS(j2, k2);
  if (p1.e2() < p2.e2()) {
    rMin = p1.e2();
    rMax = p2.e2();
  }
  else {
    rMin = p2.e2();	
    rMax = p1.e2();
  }	
  init = FALSE;

  /** Debugging Code */
  //  cerr << "\n\nEntering FowlerNordheimEmitter::initialize() --" << endl;
  //  cerr << "    nIntervals = " << nIntervals << endl;
  /** End of Debugging Code */

  if (nIntervals == 0) {
    if (alongx2()) nIntervals = abs(k2 - k1);
    else nIntervals = abs(j2 - j1);
  }

  /** Debugging Code */
  //  cerr << "    nIntervals = " << nIntervals << endl;
  /** End of Debugging Code */

  if (nIntervals < 2) nIntervals = 2;

  /** Debugging Code */
  //  cerr << "    nIntervals = " << nIntervals << endl;
  /** End of Debugging Code */

  xArray = new Scalar[nIntervals+1];
  FArray = new Scalar[nIntervals+1];
  computeLocationArray();
}

//--------------------------------------------------------------------
// Emit a beam of electrons along the surface of the emitter, following
// the Fowler-Nordheim model.  We ignore the the time variable _t.

ParticleList& FowlerNordheimEmitter::
emit(Scalar _t, Scalar dt, Species* species_to_push)
{
  /** Debugging Code */
  //  cerr << "\n\nEntering FowlerNordheimEmitter::emit(_t, dt, species_to_push) --" << endl;
  //  cerr << "    _t    = " << _t    << endl;
  //  cerr << "    dt    = " << dt    << endl;
  //  cerr << "    extra = " << extra << endl;
  /** End of Debugging Code */

  if (species==species_to_push){
      // expand dt for subcycled particles
    dt *= species->get_subcycleIndex()/(Scalar)species->get_supercycleIndex();

    /** Debugging Code */
    //    cerr << "    About to call initialize()..." << endl;
    /** End of Debugging Code */

    if (init) initialize();
    Scalar nPer_dt;

    /** Debugging Code */
    //    cerr << "\n\nBack in FowlerNordheimEmitter::emit(...):" << endl;
    //    cerr << "    nPer_dt = " << nPer_dt << endl;
    //    cerr << "\n    About to call computeLocationArray()..." << endl;
    /** End of Debugging Code */

    computeLocationArray();      // This sets the value of totalCurrent
    nPer_dt = fabs( totalCurrent * dt / get_q() ); 
    extra += nPer_dt;

    /** Debugging Code */
    //    cerr << "\n\nBack in FowlerNordheimEmitter::emit(...):" << endl;
    //    cerr << "    nPer_dt = " << nPer_dt << endl;
    //    cerr << "    extra   = " << extra   << endl;
    /** End of Debugging Code */

    if (extra < 1) return particleList;         // <1 particle to emit
    Scalar del_t = dt/nPer_dt;
    Vector2 xMKS;
    while (extra >= 1){
      extra -= 1;

      /** Debugging Code */
      //      cerr << "    About to call computeLocation()..." << endl;
      /** End of Debugging Code */

      xMKS = computeLocation();
      Vector2 x = fields->getGridCoords(xMKS);
      x += delta;                              // make sure the particle isn't on the boundary
      Vector3 u;
      u = maxwellian->get_U(normal);
      Particle* p = new Particle(x, u, species, np2c);
      Boundary* bPtr = initialPush(del_t*extra, dt, *p);
      if (!bPtr) particleList.add(p);

      /** Debugging Code */
      //      cerr << "\n\nBack in FowlerNordheimEmitter::emit(...):" << endl;
      //      cerr << "    extra   = " << extra   << endl;
      /** End of Debugging Code */
    }
  }
  return particleList;
}

//--------------------------------------------------------------------
// computeLocationArray()
//
// Precomputes a array of equally spaced points mapping the 
// cumulative distribution to position along the emitter.
//
// Note that geomFactor includes the radial nature, when the
// geometry is cylindrical.
//
// The integration uses the Trapezoidal Rule.

void FowlerNordheimEmitter::computeLocationArray()
{
  Vector2 component(0,0);
  if (alongx2()) component.set_e2(1.);
  else component.set_e1(1.);

  Scalar dx = ( (p2 - p1)*component ) / nIntervals;
  xArray[0] = p1*component;
  FArray[0] = 0;

    // set up for initial pass thru loop
  Scalar geometryFactor = 1.;
  if ( alongx2() ) geometryFactor = geomFactor(xArray[0]);

  Scalar f0, f1;
  f1 = geometryFactor * getCurrentDensity( xArray[0] ); 

  /** Debugging Code */
  //  cerr << "\n\nEntering FowlerNordheimEmitter::computeLocationArray() --" << endl;
  //  cerr << "    geometryFactor = " << geometryFactor  << endl;
  //  cerr << "    xArray[0]      = " << xArray[0]       << endl;
  //  cerr << "    FArray[0]      = " << FArray[0]       << endl;
  /** End of Debugging Code */

  int i;
  for (i=1; i<=nIntervals; i++) {
    f0 = f1;                                           // i-1 value
    xArray[i] = xArray[0] + i * dx;

    geometryFactor = 1.;
    if ( alongx2() ) geometryFactor = geomFactor(xArray[i]);

    f1 = geometryFactor*getCurrentDensity( xArray[i] );
    
    FArray[i] = FArray[i-1] + 0.5 * dx * (f0 + f1);    // trapezoidal integration

    /** Debugging Code */
    //    cerr << "\n\nBack to FowlerNordheimEmitter::computeLocationArray() --" << endl;
    //    cerr << "    geometryFactor =      " << geometryFactor  << endl;
    //    cerr << "    xArray[" << i << "] = " << xArray[i]       << endl;
    //    cerr << "    fArray[" << i << "] = " << FArray[i]       << endl;
    /** End of Debugging Code */

  }
  totalCurrent = FArray[nIntervals];
  for (i=1; i<=nIntervals; i++) FArray[i] /= totalCurrent;

  /** Debugging Code */
  //  cerr << "    totalCurrent = " << totalCurrent << endl;
  /** End of Debugging Code */
}

//--------------------------------------------------------------------
// computeLocation()
// Computes the particle location depending on the distribution function and 
// random numbers.

Vector2 FowlerNordheimEmitter::computeLocation()
{
  Scalar F = frand();

  /** Debugging Code */
  //  cerr << "\n\nEntering FowlerNordheimEmitter::computeLocation() --" << endl;
  //  cerr << "    F = " << F << endl;
  /** End of Debugging Code */

  int i;
  for (i=0; i<nIntervals; i++) {

    /** Debugging Code */
    //    cerr << "    FArray[" << i << "] = " << FArray[i] << endl;
    /** End of Debugging Code */

    if (F < FArray[i]) break;
  }
  
  Scalar x = xArray[i-1] + (xArray[i] - xArray[i-1])*(F - FArray[i-1])/(FArray[i] - FArray[i-1]);

  /** Debugging Code */
  //  cerr << "    x = " << x << endl;
  /** End of Debugging Code */

  if (alongx1()) {
    return Vector2(x, p1.e2());
  }
  else {
    return Vector2(p1.e1(), x);
  }
}


Scalar FowlerNordheimEmitter::getCurrentDensity(Scalar _x)
{
  // location at which we wish to know the current density
  Vector2 positionMKS = p1; 
  if (alongx2()) {
    positionMKS.set_e2(_x);
  }
  else {
    positionMKS.set_e1(_x);
  }

  // normal component of the E field (in V/m) at point of interest
  // note that fields->E() requires position in grid units!
  Scalar Efield;
  Vector3 E = fields->E(fields->getGridCoords(positionMKS));
  if (alongx2()) Efield = E.e1();
  else           Efield = E.e2();

  // check the direction of the field to avoid emitting when reversed
  if (Efield*Boundary::normal >= 0) return 0;

  Scalar beta_times_E = fabs(beta_FN * Efield);

  /** Function "y(E)" of the Fowler-Nordheim model.
   *  By default, C_y_FN = 3.79e-05.
   */
  y_FN = C_y_FN * sqrt(beta_times_E) / Phi_w_FN;

  /** Function "v(y)" of the Fowler-Nordheim model
   *  By default, C_v_FN = 0 and v_y_FN = 1.
   */
  v_y_FN = 1. - C_v_FN * y_FN*y_FN;

    // current density J_FN (in A/m^2) at point of interest
  Scalar factor_FN;
  Scalar exp_factor_tmp;
  Scalar exp_factor_FN;
  Scalar currentDensity_FN;
  factor_FN         = A_FN * beta_times_E * beta_times_E / Phi_w_FN;

  exp_factor_tmp    = B_FN * v_y_FN * pow(Phi_w_FN, Scalar(1.5));
  if ( exp_factor_tmp < beta_times_E * Scalar_MAX ) {
    exp_factor_FN = exp_factor_tmp / beta_times_E;
  }
  else {
    exp_factor_FN = Scalar_MAX;
  }
  if ( exp_factor_FN < log(1./Scalar_MIN) ) {
    currentDensity_FN = factor_FN * exp(-exp_factor_FN);
  }
  else {
    currentDensity_FN = 0.;
  }

  /** Debugging Code */
  //  cerr << "\n\nEntering FowlerNordheimEmitter::getCurrentDensity(x) --" << endl;
  //  cerr << "    _x                = " << _x                << endl;
  //  cerr << "    _y                = " << positionMKS.e2()  << endl;
  //  cerr << "    Efield            = " << Efield            << endl;
  //  cerr << "    beta_times_E      = " << beta_times_E      << endl;
  //  cerr << "    y_FN              = " << y_FN              << endl;
  //  cerr << "    v_y_F  N          = " << v_y_FN            << endl;
  //  cerr << "    factor_FN         = " << factor_FN         << endl;
  //  cerr << "    exp_factor_FN     = " << exp_factor_FN     << endl;
  //  cerr << "    currentDensity_FN = " << currentDensity_FN << endl;
  /** End of Debugging Code */

  return currentDensity_FN;
}
