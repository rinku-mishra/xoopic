/*
====================================================================

MCCG.CPP

Purpose:  Defines MCCParams, which holds parameters for the
          MCC algorithms.

Version:  $Id: mccg.cpp 2289 2006-07-20 21:21:59Z yew $

Revision History
2.01  (Bruhwiler 03/01/2000) added reference to Li collision model
2.51  (Bruhwiler 08/29/2000) added relativisticMCC flag references
2.52  (Bruhwiler 09/03/2000) added reference to N  collision model
2.53  (Bruhwiler, 10-04-2000) added delayTime and stopTime.

====================================================================
*/

#include "mccg.h"
#include "spatialg.h"
#include "advisman.h"
#include "ptclgrp.h"
#include "diags.h"
#include "spbound.h"
using namespace std;
//================= MCCParams Class

MCCParams::MCCParams(SpatialRegionGroup *_SRG)
: ParameterGroup()
{
  name = "MCC";
  SRG = _SRG;

  gas.setNameAndDescription("gas", "name of the gas");
  gas.setValue("NULL");
  parameterList.add(&gas);

  pressure.setNameAndDescription("pressure", "Neutral gas pressure in Torr");
  pressure.setValue("0");
  parameterList.add(&pressure);

  temperature.setNameAndDescription("temperature", "Neutral gas temperature in eV");
  temperature.setValue("0.025");
  parameterList.add(&temperature);

  analyticF.setNameAndDescription(ostring("analyticF"),
    ostring("Analytic function of x1,x2 describing the neutral gas density"));
  analyticF.setValue(ostring("0.0"));
  parameterList.add(&analyticF);

  eSpecies.setNameAndDescription("eSpecies", "electron species for ioniz.");
  eSpecies.setValue("NULL");
  parameterList.add(&eSpecies);

  iSpecies.setNameAndDescription("iSpecies", "ion species for ioniz.");
  iSpecies.setValue("NULL");
  parameterList.add(&iSpecies);

  iSpeciesPlusPlus.setNameAndDescription("iSpeciesPlusPlus", "ion species for ioniz.");
  iSpeciesPlusPlus.setValue("NULL");
  parameterList.add(&iSpeciesPlusPlus);

  ionzFraction_i.setNameAndDescription("ionzFraction_i", "num ions created in ionz event");
  ionzFraction_i.setValue("1");
  parameterList.add(&ionzFraction_i);

  ecxFactor.setNameAndDescription("ecxFactor","");
  ecxFactor.setValue("1");
  parameterList.add(&ecxFactor);

  icxFactor.setNameAndDescription("icxFactor","");
  icxFactor.setValue("1");
  parameterList.add(&icxFactor);

  relativisticMCC.setNameAndDescription(ostring("relativisticMCC"),
        ostring("Flag: 0=Original MCC models, 1=Relativistic MCC models."));
  relativisticMCC.setValue("0");
	parameterList.add(&relativisticMCC);

	x1MinMKS.setNameAndDescription(ostring("x1MinMKS"),
			  ostring("minimum x1 position defining neutral gas region"));
  x1MinMKS.setValue(ostring("-1."));
  parameterList.add(&x1MinMKS);

  x1MaxMKS.setNameAndDescription(ostring("x1MaxMKS"),
			  ostring("maximum x1 position defining neutral gas region"));
  x1MaxMKS.setValue(ostring("-1."));
  parameterList.add(&x1MaxMKS);

  x2MinMKS.setNameAndDescription(ostring("x2MinMKS"),
			  ostring("minimum x2 position defining neutral gas region"));
  x2MinMKS.setValue(ostring("-1."));
  parameterList.add(&x2MinMKS);

  x2MaxMKS.setNameAndDescription(ostring("x2MaxMKS"),
			  ostring("maximum x2 postion defining neutral gas region"));
  x2MaxMKS.setValue(ostring("-1."));
  parameterList.add(&x2MaxMKS);

  delayTime.setNameAndDescription(ostring("delayTime"),
			  ostring("time delay before MCC algorithm activates"));
  delayTime.setValue(ostring("0."));
  parameterList.add(&delayTime);

  stopTime.setNameAndDescription(ostring("stopTime"),
			  ostring("time after which MCC algorithm ceases"));
  stopTime.setValue(ostring("-1."));
  parameterList.add(&stopTime);

  // setting the default value of the "collisionFlag" and the "tunnelingIonizationFlag" 
  collisionFlag.setNameAndDescription("collisionFlag",
                                      "default = 1(true) for invoking collisions in the Monte Carlo");
  collisionFlag.setValue("1");
  parameterList.add(&collisionFlag);
  
  tunnelingIonizationFlag.setNameAndDescription("tunnelingIonizationFlag",
    "default = 0(false) for not using laser ionization in the Monte Carlo");
  tunnelingIonizationFlag.setValue("0");
  parameterList.add(&tunnelingIonizationFlag);

  ETIPolarizationFlag.setNameAndDescription("ETIPolarizationFlag",
    "default = 0 for linearly polarized E field, = 1 for circularly polarized"); 
  ETIPolarizationFlag.setValue("0");
  parameterList.add(&ETIPolarizationFlag);

  EfieldFrequency.setNameAndDescription(ostring("EfieldFrequency"),
    ostring("Frequency of the external alternating E field"));
  EfieldFrequency.setValue(ostring("-1."));
  parameterList.add(&EfieldFrequency);

  discardDumpFileNGDDataFlag.setNameAndDescription("discardDumpFileNGDDataFlag",
    "default = 0(false) for using the NDG data in dump files");
  discardDumpFileNGDDataFlag.setValue("0");
  parameterList.add(&discardDumpFileNGDDataFlag);    
  
  /**
   * set the value for the number of macro particles per cell due to 
   * tunneling ionization the default value is 10
   */
  TI_numMacroParticlesPerCell.setNameAndDescription("TI_numMacroParticlesPerCell",
    "default = 10 for number of macro particles in a cell due to tunneling ionization");
  TI_numMacroParticlesPerCell.setValue("10");
  parameterList.add(&TI_numMacroParticlesPerCell);

  addLimitRule("relativisticMCC","<",0.0, "Fatal  --  0 (off), 1 (on) are valid.",1);
  addLimitRule("relativisticMCC",">",1.0, "Fatal  --  0 (off), 1 (on) are valid.",1);
};

void MCCParams::CreateCounterPart(SpatialRegion* SP, MCC* pMCC, MCTI* pMCTI)
  /* throw(Oops) */{
  region = SP;
  
  Species* eSpeciesPtr = SRG->get_speciesPtr(eSpecies.getValue());
  Species* iSpeciesPtr = SRG->get_speciesPtr(iSpecies.getValue());
  Species* tSpeciesPtr = SRG->get_speciesPtr(tSpecies.getValue());
  Species* bSpeciesPtr = SRG->get_speciesPtr(bSpecies.getValue());

  Species* iSpeciesPlusPlusPtr = SRG->get_speciesPtr(iSpeciesPlusPlus.getValue());
  
  MCCPackage* mccPackage = NULL;
  
  /**
   * The ostring returned from the call "analyticF.getValue()" will be 
   * passed to the constructors of the various MCC classes. It contains
   * the function which will be used for the initialization of the 
   * neutral gas density on the grid needed for the laser ionization
   * simulations. 
   * dad 
   */
  if (collisionFlag.getValue()) {
	if (gas.getValue() ==(ostring) "Ar") 
          pMCC->addPackage( new ArMCC(pressure.getValue(), 
 		temperature.getValue(), eSpeciesPtr, iSpeciesPtr, 
		*SRG->advisorp->get_speciesList(), SRG->get_dtBase(), ionzFraction_i.getValue(),
		ecxFactor.getValue(), icxFactor.getValue(), region, x1MinMKS.getValue(),
		x1MaxMKS.getValue(), x2MinMKS.getValue(), x2MaxMKS.getValue(),
                delayTime.getValue(), stopTime.getValue(), analyticF.getValue(),
                                      discardDumpFileNGDDataFlag.getValue() ) );

	else if (gas.getValue() ==(ostring) "Ne") 
          pMCC->addPackage(new NeMCC(pressure.getValue(), 
		temperature.getValue(), eSpeciesPtr, iSpeciesPtr, 
		*SRG->advisorp->get_speciesList(), SRG->get_dtBase(), ionzFraction_i.getValue(),
		ecxFactor.getValue(), icxFactor.getValue(), region, x1MinMKS.getValue(),
                x1MaxMKS.getValue(), x2MinMKS.getValue(), x2MaxMKS.getValue(),
                delayTime.getValue(), stopTime.getValue(), analyticF.getValue(),
                                     discardDumpFileNGDDataFlag.getValue() ) );

	else if (gas.getValue() == (ostring)"Xe") 
          pMCC->addPackage( new XeMCC(pressure.getValue(), 
		temperature.getValue(), eSpeciesPtr, iSpeciesPtr, 
		*SRG->advisorp->get_speciesList(), SRG->get_dtBase(), ionzFraction_i.getValue(),
		ecxFactor.getValue(), icxFactor.getValue(), region, x1MinMKS.getValue(),
                x1MaxMKS.getValue(), x2MinMKS.getValue(), x2MaxMKS.getValue(),
                delayTime.getValue(), stopTime.getValue(), analyticF.getValue(),
                                      discardDumpFileNGDDataFlag.getValue() ) );

	else if (gas.getValue() == (ostring)"H") 
          pMCC->addPackage(new HMCC(pressure.getValue(), 
		temperature.getValue(), eSpeciesPtr, iSpeciesPtr, 
		*SRG->advisorp->get_speciesList(), SRG->get_dtBase(), ionzFraction_i.getValue(),
		ecxFactor.getValue(), icxFactor.getValue(), region, x1MinMKS.getValue(),
                x1MaxMKS.getValue(), x2MinMKS.getValue(), x2MaxMKS.getValue(),
                delayTime.getValue(), stopTime.getValue(), analyticF.getValue(),
                                    discardDumpFileNGDDataFlag.getValue() ) );

	else if (gas.getValue() == (ostring)"Li") 
          pMCC->addPackage(new LiMCC(pressure.getValue(), 
		temperature.getValue(), eSpeciesPtr, iSpeciesPtr, 
		*SRG->advisorp->get_speciesList(), SRG->get_dtBase(), ionzFraction_i.getValue(),
		ecxFactor.getValue(), icxFactor.getValue(), relativisticMCC.getValue(), region,
                x1MinMKS.getValue(), x1MaxMKS.getValue(), x2MinMKS.getValue(), x2MaxMKS.getValue(),
                delayTime.getValue(), stopTime.getValue(), analyticF.getValue(),
                                     discardDumpFileNGDDataFlag.getValue() ) );

	else if (gas.getValue() == (ostring)"N") 
          pMCC->addPackage(new NMCC(pressure.getValue(), 
		temperature.getValue(), eSpeciesPtr, iSpeciesPtr, 
		*SRG->advisorp->get_speciesList(), SRG->get_dtBase(), ionzFraction_i.getValue(),
		ecxFactor.getValue(), icxFactor.getValue(), relativisticMCC.getValue(), region,
                x1MinMKS.getValue(), x1MaxMKS.getValue(), x2MinMKS.getValue(), x2MaxMKS.getValue(),
                delayTime.getValue(), stopTime.getValue(), analyticF.getValue(),
                                    discardDumpFileNGDDataFlag.getValue() ) );
        else if (gas.getValue() == (ostring)"He") {
	  pMCC->addPackage(new HeMCC(pressure.getValue(), 
		temperature.getValue(), eSpeciesPtr, iSpeciesPtr, 
		*SRG->advisorp->get_speciesList(), SRG->get_dtBase(), ionzFraction_i.getValue(),
		ecxFactor.getValue(), icxFactor.getValue(), region, x1MinMKS.getValue(),
                x1MaxMKS.getValue(), x2MinMKS.getValue(), x2MaxMKS.getValue(),
                delayTime.getValue(), stopTime.getValue(), analyticF.getValue(),
                                    discardDumpFileNGDDataFlag.getValue() ) );
        }
	else cout << "Unrecognized MCC package requested: " << gas.getValue();
  }

  if (tunnelingIonizationFlag.getValue()) {
        if (gas.getValue() ==(ostring) "H") { 
          try{
            pMCTI->addPackage(new HMCTI(pressure.getValue(), 
                                      temperature.getValue(), eSpeciesPtr, iSpeciesPtr, 
                                      *SRG->advisorp->get_speciesList(), SRG->get_dtBase(), 
                                      ionzFraction_i.getValue(), ecxFactor.getValue(), 
                                      icxFactor.getValue(), region, x1MinMKS.getValue(),
                                      x1MaxMKS.getValue(), x2MinMKS.getValue(), 
                                      x2MaxMKS.getValue(), delayTime.getValue(), 
                                      stopTime.getValue(), analyticF.getValue(),
                                      TI_numMacroParticlesPerCell.getValue(), 
                                      ETIPolarizationFlag.getValue(),
                                      EfieldFrequency.getValue(),
                                      discardDumpFileNGDDataFlag.getValue()));
          }
          catch(Oops& oops){
            oops.prepend("MCCParams::CreateCounterPart: Error: \n");//SpatialRegionGroup::CreateCounterPart
            throw oops;
          }

        }
        else if (gas.getValue() == (ostring)"He") {
          try {
            pMCTI->addPackage(new HeMCTI(pressure.getValue(), 
                                       temperature.getValue(), eSpeciesPtr, iSpeciesPtr, 
                                       iSpeciesPlusPlusPtr, 
                                       *SRG->advisorp->get_speciesList(), SRG->get_dtBase(), 
                                       ionzFraction_i.getValue(), ecxFactor.getValue(), 
                                       icxFactor.getValue(), region, x1MinMKS.getValue(),
                                       x1MaxMKS.getValue(), x2MinMKS.getValue(), 
                                       x2MaxMKS.getValue(), delayTime.getValue(), 
                                       stopTime.getValue(), analyticF.getValue(),
                                       TI_numMacroParticlesPerCell.getValue(), 
                                       ETIPolarizationFlag.getValue(),
                                       EfieldFrequency.getValue(),
                                       discardDumpFileNGDDataFlag.getValue()));
          }
          catch(Oops& oops){
            oops.prepend("MCCParams::CreateCounterPart: Error: \n"); //OK
            throw oops;
          }
        }
        else if (gas.getValue() == (ostring)"Li") {
          try {
          cout << "Creating a LiMCTI instance" << endl; 
          pMCTI->addPackage(new LiMCTI(pressure.getValue(), 
                                       temperature.getValue(), eSpeciesPtr, iSpeciesPtr, 
                                       *SRG->advisorp->get_speciesList(), SRG->get_dtBase(), 
                                       ionzFraction_i.getValue(), ecxFactor.getValue(), 
                                       icxFactor.getValue(), region, x1MinMKS.getValue(),
                                       x1MaxMKS.getValue(), x2MinMKS.getValue(), 
                                       x2MaxMKS.getValue(), delayTime.getValue(), 
                                       stopTime.getValue(), analyticF.getValue(),
                                       TI_numMacroParticlesPerCell.getValue(), 
                                       ETIPolarizationFlag.getValue(),
                                       EfieldFrequency.getValue(),
                                       discardDumpFileNGDDataFlag.getValue()));
          }
          catch(Oops& oops){
            oops.prepend("MCCParams::CreateCounterPart: Error: \n"); //OK
            throw oops;
          }
        }
        else if (gas.getValue() == (ostring)"Cs") {
          cout << "Creating a CsMCTI instance" << endl;  
          try{
            pMCTI->addPackage(new CsMCTI(pressure.getValue(), 
                                       temperature.getValue(), eSpeciesPtr, iSpeciesPtr, 
                                       iSpeciesPlusPlusPtr, 
                                       *SRG->advisorp->get_speciesList(), SRG->get_dtBase(), 
                                       ionzFraction_i.getValue(), ecxFactor.getValue(), 
                                       icxFactor.getValue(), region, x1MinMKS.getValue(),
                                       x1MaxMKS.getValue(), x2MinMKS.getValue(), 
                                       x2MaxMKS.getValue(), delayTime.getValue(), 
                                       stopTime.getValue(), analyticF.getValue(),
                                       TI_numMacroParticlesPerCell.getValue(), 
                                       ETIPolarizationFlag.getValue(),
                                       EfieldFrequency.getValue(),
                                       discardDumpFileNGDDataFlag.getValue()));
          }
          catch(Oops& oops){
            oops.prepend("MCCParams::CreateCounterPart: Error:\n"); //OK
            throw oops; 
          }
        }
	else if (gas.getValue() ==(ostring) "Ar") 
          // pMCTI->addPackage( new ArMCTI(...) );
          ;
	else if (gas.getValue() ==(ostring) "Ne") 
          // pMCTI->addPackage( new NeMCTI(...) );
          ;
	else if (gas.getValue() == (ostring)"Xe") 
          // pMCTI->addPackage( new XeMCTI(...) );
          ;
	else if (gas.getValue() == (ostring)"N") 
          // pMCTI->addPackage( new NMCTI(...) );
          ;
	else cout << "Unrecognized MCTI package requested: " << gas.getValue();
  }
  return;
}

void MCCParams::checkRules()
{
	char buffer[512];
   BOOL gasTest = FALSE; 
   if (gas.getValue() ==(ostring) "Ar") gasTest = TRUE;
	else if (gas.getValue() ==(ostring) "Ne") gasTest = TRUE;
	else if (gas.getValue() == (ostring)"Xe") gasTest = TRUE;
	else if (gas.getValue() == (ostring)"H") gasTest = TRUE;
	else if (gas.getValue() == (ostring)"He") gasTest = TRUE;
	else if (gas.getValue() == (ostring)"Li") gasTest = TRUE;
	else if (gas.getValue() == (ostring)"N") gasTest = TRUE;
        else if (gas.getValue() == (ostring)"Cs") gasTest = TRUE;
	if (!gasTest) {
	  sprintf(buffer, "unrecognized gas package: %s", gas.getValue().c_str());
	  ruleMessages.add(new ostring(buffer)); 
	}
	ostring name = eSpecies.getValue();
	if (name == (ostring)"NULL") ruleMessages.add(new ostring("eSpecies is required!"));
	else if (!SRG->KnownSpecies(name))
	{
		sprintf(buffer, "Unrecognized speciesName: %s", name());
		ruleMessages.add(new ostring(buffer));
	}
	name = iSpecies.getValue();
	if (name == (ostring)"NULL") ruleMessages.add(new ostring("iSpecies is required!"));
	else if (!SRG->KnownSpecies(name))
	{
		sprintf(buffer, "Unrecognized speciesName: %s", name());
		ruleMessages.add(new ostring(buffer));
	}
}


