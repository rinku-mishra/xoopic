#ifndef	FOWLER_NORDHEIM_EMIT_H
#define	FOWLER_NORDHEIM_EMIT_H

/* ====================================================================
 * Change Log for file  fowler_nordheim_emit.h  
 *
 * 3.01 (DavidB 10-03-02) Initial code.  This development effort was
 *                        funded by a contract from Cornell University
 *                        to Tech-X Corporation.
 * ====================================================================
 */

#include "ovector.h"
#include "maxwelln.h"
#include "vmaxwelln.h"
#include "particle.h"
#include "emitter.h"

/** Class representing a conductor which undergoes field emission of
 *  electrons, according to the Fowler-Nordheim model.
 */
class FowlerNordheimEmitter : public Emitter {

 protected:

  /** Coefficient "A" of the Fowler-Nordheim field emission model.
   *  This is set via the input file, with a default value of 1.5414e-06.
   */
  Scalar A_FN;

  /** Coefficient "beta" of the Fowler-Nordheim field emission model.
   *  This is set via the input file, with a default value of 1.
   */
  Scalar beta_FN;

  /** Coefficient "B" of the Fowler-Nordheim field emission model.
   *  This is set via the input file, with a default value of 6.8308e+09
   */
  Scalar B_FN;

  /** Coefficient "C_v" of the Fowler-Nordheim field emission model.
   *  This is set via the input file, with a default value of 0.
   */
  Scalar C_v_FN;

  /** Coefficient "C_y" of the Fowler-Nordheim field emission model.
   *  This is set via the input file, with a default value of 3.79e-05.
   */
  Scalar C_y_FN;

  /** Function "y(E)" of the Fowler-Nordheim field emission model.
   *  It has the form y = C_y * sqrt(E) / Phi_w, where E is the
   *     normal component of the electric field in V/m and Phi_w
   *     is the work function of the material in eV.
   *  This is calculated dynamically, as required.
   */
  Scalar y_FN;

  /** Function "v(y)" of the Fowler-Nordheim field emission model.
   *  It has the form  v_y = 1. - C_v * y * y.
   *  By default, C_v = 0 and v_y = 1.
   *  This is calculated dynamically, as required.
   */
  Scalar v_y_FN;

  /** The work function "Phi_w" of the Fowler-Nordheim field emission model.
   *  This is set via the input file, with a default value of 4 eV.
   */
  Scalar Phi_w_FN;

  /** The number of intervals to be used for xArray and FArray (below).
   *  This is set via the input file, with a default value of 0.
   *
   *  In the default case, it is set here to be the # of cells along
   *  the emitting boundary, which is the most reasonable thing to do.
   *
   *  If the value is less than 2, then it is set equal to 2.
   */
  int nIntervals;

  MaxwellianFlux *maxwellian;

  Scalar extra;

  /** coordinates of 1st corner (MKS units)
   */
  Vector2 p1;

  /** coordinates of 2nd corner (MKS units)
   */
  Vector2 p2;

  /** 3-D vector pointing in direction normal to the emitter
   */
  Vector3 normal;

  /** initialization flag
   */
  BOOL init;

  /** inner radius (MKS units)
   */
  Scalar rMin;

  /** outer radius (MKS units)
   */
  Scalar rMax;

  /** small displacement to avoid particle being on boundary
   */
  Vector2 delta;

  Vector2 deltaVertical;

  Vector2 deltaHorizontal;

  /** set up optimization variables
   */
  void	initialize();

  /** used to handle spatially varying emission current
   */
  Scalar *xArray;

  /** used to handle spatially varying emission current
   */
  Scalar *FArray;

  /** integral of J(x) over the emitter surface
   */
  Scalar totalCurrent;

  Scalar geomFactor(Scalar x) {
    return (rweight) ? x : 1;
  }

  /**
   * The function computeLocationArray() recomputes an array of equally 
   * spaced points mapping the cumulative distribution to position along
   * the emitter.
   *
   * Note that geomFactor includes the radial nature, when using
   * cylindrical geometry.
   *
   * The integration uses the Trapezoidal Rule.
   */
  void computeLocationArray();

  Vector2 computeLocation();

  Scalar getCurrentDensity(Scalar);

  /**
   *  The virtual method setPassives() is used to define the boundary
   *  conditions for the fields.  By default, one would get a perfect
   *  conductor.  Here, we explicitly use the function as implemented
   *  in the Dielectric class, which essentially imposes no BC's on
   *  the fields.
   *
   *  This is necessary, because we want particles to be emitted according
   *  to the perpendicular electric field at the surface, without doing
   *  anything to alter that field.  (Of course, particle emission will
   *  self-consistently reduce the local field values.)
   *
   *  The FowlerNordheimEmitter boundary can thus be overlaid on other
   *  boundaries, such as an Equipotential, without affecting them.
   */
  virtual void setPassives() {
    Dielectric::setPassives();
  }


 public:

  FowlerNordheimEmitter(oopicList <LineSegment>* _segments,
                        MaxwellianFlux*          _maxwellian,
                        Scalar                   _np2c,
                        Scalar                   _A_FN,
                        Scalar                   _beta_FN,
                        Scalar                   _B_FN,
                        Scalar                   _C_v_FN,
                        Scalar                   _C_y_FN,
                        Scalar                   _Phi_w_FN,
                        int                      _nIntervals);

  virtual ~FowlerNordheimEmitter();

  virtual ParticleList& emit(Scalar t,Scalar dt, Species *species);

  void set_nIntervals(int n) {
    nIntervals = n;
    if (nIntervals < 2) nIntervals = 2;
  }
};

#endif       // FOWLER_NORDHEIM_EMIT_H
