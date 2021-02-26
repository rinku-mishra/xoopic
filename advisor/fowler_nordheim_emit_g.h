#ifndef	FOWLER_NORDHEIM_EMIT_G_H
#define	FOWLER_NORDHEIM_EMIT_G_H

/* ====================================================================
 * Change Log for file  fowler_nordheim_emit.h  
 *
 * 3.01 (DavidB 10-03-02) Initial code.  This development effort was
 *                        funded by a contract from Cornell University
 *                        to Tech-X Corporation.
 * ====================================================================
 */

#include "part1g.h"
#include "emitterg.h"

/**
 *  Class for handling the input file parameters that describe a
 *  boundary that emits particles according to the Fowler-Nordheim
 *  field emission model.
 */
class FowlerNordheimEmitterParams : public EmitterParams {

 protected:

  /** Coefficient "A" of the Fowler-Nordheim field emission model.
   *  Default value is 1.5414e-06.
   */
  ScalarParameter A_FN;

  /** Coefficient "beta" of the Fowler-Nordheim field emission model.
   *  Default value is 1.
   */
  ScalarParameter beta_FN;

  /** Coefficient "B" of the Fowler-Nordheim field emission model.
   *  Default value is 6.8308e+09
   */
  ScalarParameter B_FN;

  /** Coefficient "C_v" of the Fowler-Nordheim field emission model.
   *  Default value is 0.
   */
  ScalarParameter C_v_FN;

  /** Coefficient "C_y" of the Fowler-Nordheim field emission model.
   *  Default value is 3.79e-05.
   */
  ScalarParameter C_y_FN;

  /** The work function "Phi_w" of the Fowler-Nordheim field emission model.
   *  Default value is 4 eV.
   */
  ScalarParameter Phi_w_FN;

  /** The number of intervals to be used for emitting particles.
   *  Default value of 0.  In the default case, nIntervals will be
   *  reset to the # of cells along the emitting boundary, which 
   *  is the most reasonable thing to do.
   */
  IntParameter nIntervals;

 public:

  FowlerNordheimEmitterParams(GridParams* _GP, SpatialRegionGroup* srg);

  void checkRules() {
    EmitterParams::checkRules();
  }

  Boundary* CreateCounterPart();
};

#endif  //  FOWLER_NORDHEIM_EMIT_G_H
