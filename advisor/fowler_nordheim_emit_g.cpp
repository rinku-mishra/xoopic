/* ====================================================================
 * Change Log for file  fowler_nordheim_emit_g.cpp
 *
 * 3.01 (DavidB 10-04-02) Initial code.  This development effort was
 *                        funded by a contract from Cornell University
 *                        to Tech-X Corporation.
 * ====================================================================
 */

#include "fowler_nordheim_emit_g.h"
#include "fowler_nordheim_emit.h"
#include "spatialg.h"
#include "ptclgrp.h"
#include "diags.h"
#include "spbound.h"

//=================== FowlerNordheimEmitterParams Class

FowlerNordheimEmitterParams::
FowlerNordheimEmitterParams(GridParams* _GP,
                            SpatialRegionGroup* srg)
  : EmitterParams(_GP, srg)
{
  name = "FowlerNordheimEmitter";

  A_FN.setNameAndDescription("A_FN",
                             "Coefficient 'A' of the Fowler-Nordheim field emission model.");
  A_FN.setValue("1.5414e-06");
  parameterList.add(&A_FN);
  
  beta_FN.setNameAndDescription("beta_FN",
                                "Coefficient 'beta' of the Fowler-Nordheim field emission model.");
  beta_FN.setValue("1.0");
  parameterList.add(&beta_FN);
  
  B_FN.setNameAndDescription("B_FN",
                             "Coefficient 'B' of the Fowler-Nordheim field emission model.");
  B_FN.setValue("6.8308e+09");
  parameterList.add(&B_FN);
  
  C_v_FN.setNameAndDescription("C_v_FN",
                               "Coefficient 'C_v' of the Fowler-Nordheim field emission model.");
  C_v_FN.setValue("0.0");
  parameterList.add(&C_v_FN);
  
  C_y_FN.setNameAndDescription("C_y_FN",
                               "Coefficient 'C_y' of the Fowler-Nordheim field emission model.");
  C_y_FN.setValue("3.79e-05");
  parameterList.add(&C_y_FN);
  
  Phi_w_FN.setNameAndDescription("Phi_w_FN",
                                 "The material work function in eV.");
  Phi_w_FN.setValue("4.0");
  parameterList.add(&Phi_w_FN);
  
  nIntervals.setNameAndDescription("nIntervals",
                                   "Number of intervals along the emitting surface.");
  nIntervals.setValue("0");
  parameterList.add(&nIntervals);
};

Boundary* FowlerNordheimEmitterParams::CreateCounterPart()
{
  EmitterParams::CreateCounterPart();//sets up distribution and species

  MaxwellianFlux *beam = distribution;
  Boundary* B = new FowlerNordheimEmitter(SP.GetLineSegments(),
                                          beam,
                                          np2c.getValue(),
                                          A_FN.getValue(),
                                          beta_FN.getValue(),
                                          B_FN.getValue(),
                                          C_v_FN.getValue(),
                                          C_y_FN.getValue(),
                                          Phi_w_FN.getValue(),
                                          nIntervals.getValue() );
  addSecondaries(B);
  return B;
}
