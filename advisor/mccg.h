#ifndef __MCCG_H
#define __MCCG_H

/*
====================================================================

MCCG.H

Purpose:  Define the MCCParams class, which contains the
          Monte Carlo collision parameters

Version:  $Id: mccg.h 2210 2004-09-08 15:11:46Z yew $

Revision/Author/Date
2.01  (Bruhwiler, 08-29-2000) added relativisticMCC flag, with get method.
2.02  (Bruhwiler, 10-04-2000) added delayTime and stopTime.

====================================================================
*/

//#include "part1g.h"
//#include "maxwelln.h"
//#include "gridg.h"

#include "mcc.h"
#include "HeMCTI.h"
#include "LiMCTI.h"
#include "CsMCTI.h"
#include "speciesg.h"

class SpatialRegionGroup;

//==================== MCCParams class
// class for loading MCC packages

class MCCParams : public ParameterGroup
{
  SpatialRegion* region;
  SpatialRegionGroup* SRG;

	/** This flag defauts to 0, meaning that the original MCC
	 *  algorithms are used.  If it is set to 1, then the new
	 *  relativistic cross section, energy exchange and
	 *  scattering algorithms will be used.
	 *
	 *  At present, the relativistic algorithms have only been
	 *  implemented for Li and N. -- Bruhwiler  09/03/2000
	 */
  IntParameter relativisticMCC;

 protected:

  StringParameter gas;
  ScalarParameter pressure;
  ScalarParameter temperature;

  StringParameter eSpecies;
  StringParameter iSpecies;
  StringParameter tSpecies;
  StringParameter bSpecies;

  // 
  // Species for the different ions in a multiple electron atoms.
  // Note that the singly ionized ions of an atom are specified 
  // by the iSpecies variable. Then iSpeciesPlusPlus is for the
  // doubly ionized atoms. This is done now only for He and will
  // be generalized for other multi-electron atoms. dad 06/27/01.
  // 
  StringParameter iSpeciesPlusPlus;

  IntParameter ionzFraction_i;

  ScalarParameter icxFactor;
  ScalarParameter ecxFactor;

  ScalarParameter x1MinMKS;
  ScalarParameter x1MaxMKS;
  ScalarParameter x2MinMKS;
  ScalarParameter x2MaxMKS;

  ScalarParameter delayTime;
  ScalarParameter stopTime;

  /**
   * We want to have a function into the MCC structure of the input file
   * which will be used to initialize the "NeutralGasDensity" member
   * data. This data member is to be used for the
   * laser ionization code. The reading of this function, given as
   * a string which will be later parsed follows the example for the
   * variable analyticF in the loadg.h and loadg.cpp files.
   * dad.
   */
  StringParameter analyticF;

  /**
   * The MCCParams will contain the input parameters relevant for the 
   * Tunneling Ionization too. For now it is the addition of
   * two flags "collisionFlag" with default value of 1 (true) and 
   * "tunnelingIonizationFlag" with a default value of 0 (false). The 
   * user can specify in the input file, by changing the values of 
   * these flags, what should be used in the Monte Carlo for each
   * gas type. 
   * dad: 12/20/2000.
   */
  IntParameter collisionFlag;
  IntParameter tunnelingIonizationFlag;

  /**
   * the TI_numMacroParticlesPerCell determines how many macro particles
   * should be created per cell due to tunneling ionization. This 
   * parater effectively determines how many physical particles will 
   * be in a macro particle in each cell given the neutral atom's 
   * density for each cell. The default value is 10. dad 05/04/2000
   */
  IntParameter TI_numMacroParticlesPerCell;

  /**
   * ETIPolarizationFlag is a flag to specify the polarizability of
   * the electric field. This is needed to decide which formula
   * to use for the calculation of the tunneling ionization 
   * probability.
   */
  IntParameter ETIPolarizationFlag;

  /**
   * In the tunneling ionization simulations we need 
   * the frequency of the external time varying 
   * electric field in order to estimate the minimum
   * E field magnitude for which Keldish' condition holds, 
   * i.e. we use gamma = 1, where gamma is the Keldish
   * parameter. 
   * The default value of the frequency is set to -1. to
   * indicate that no value for this paramter was set in 
   * the input file in which case the value used in
   * the simulations is Scalar_EPSILON (as a value close
   * to zero) set in the MCTI_Package class. 
   */
  ScalarParameter EfieldFrequency;  

  /**
   * The discardDumpFileNGDDataFlag is used to decide if 
   * the NGD data in a dump file is to be used or not. 
   * Its default falue is 0, false, in which case the 
   * NGD data in the dump file is read into the corresponding
   * data members of the NGD object instances. If the value
   * of the flag is != 0, then NGD data in a dump file is
   * read but discarded. The simulation uses the input file
   * information to initialize the NGD data structures in this case.
   */
  IntParameter discardDumpFileNGDDataFlag;
 public:

  MCCParams(SpatialRegionGroup* SRG);
  virtual ~MCCParams() {};

  //MCCPackage* CreateCounterPart(SpatialRegion* _SR);
  void CreateCounterPart(SpatialRegion*, MCC*, MCTI*) /* throw(Oops) */;
  void checkRules();

	/** Get the value of the integer parameter: relativisticMCC.
	 *  See comments above for details.
	 */
  int getRelativisticMCC() {
    return (int)relativisticMCC.getValue();
  }
};  

#endif  // ifndef __MCCG_H  

