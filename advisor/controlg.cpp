/*
====================================================================

CONTROLG.CPP

Purpose:	Defines ControlParams, which holds overall controlling
		parameters

Version:	$Id: controlg.cpp 2267 2005-09-07 21:53:46Z jhammel $

Revision History
2.01  (Bruhwiler 10/08/99) added SynchRadiationFlag references
2.0?  (Cary 22 Jan 00) Added shiftDelayTime.

====================================================================
*/

#include "controlg.h"

//=================== ControlParams Class

ControlParams::ControlParams()
	  : ParameterGroup()
{
  name = "Control";

#ifdef MPI_VERSION
  MPIpartition.setNameAndDescription("MPIpartition","Partition info for MPI version");
  MPIpartition.setValue("Nx1");
  parameterList.add(&MPIpartition);
#endif


  dt.setNameAndDescription(ostring("dt"), ostring("time step"));
  dt.setValue("1.0e-12");
  
  emdamping.setNameAndDescription(ostring("emdamping"), ostring("EMdamping, a value from 0 to 2"));
  emdamping.setValue("0");

  movingWindow.setNameAndDescription("movingWindow",
	"Moving window:  0 none, 1 system leftward = window right");
  movingWindow.setValue("0");
  parameterList.add(&movingWindow);

  shiftDelayTime.setNameAndDescription("shiftDelayTime",
	"Time before shifting begins");
  shiftDelayTime.setValue("0");
  parameterList.add(&shiftDelayTime);

  gasOffTime.setNameAndDescription("gasOffTime",
	"Time at which to set the neutral gas density to zero.");
  gasOffTime.setValue("-1.0"); // default is a negative value for not switching off the gas
  parameterList.add(&gasOffTime);

  dumpTextFiles.setNameAndDescription("dumpTextFiles",
        "write in files, particles and electric field in text");
  dumpTextFiles.setValue("0");
  parameterList.add(&dumpTextFiles);

  Frandseed.setNameAndDescription("Frandseed","seed for frand");
  Frandseed.setValue("0");

  B01.setNameAndDescription(ostring("B01"), 
			    ostring("Ambient B field in x1 direction"));
  B01.setValue("0.0");

  B01analytic.setNameAndDescription(ostring("B01analytic"), 
												ostring("Ambient B field in x1 direction"));
  B01analytic.setValue("0.0");
  parameterList.add(&B01analytic);

  B02analytic.setNameAndDescription(ostring("B02analytic"), 
				    ostring("Ambient B field in x2 direction"));
  B02analytic.setValue("0.0");
  parameterList.add(&B02analytic);
  
  B03analytic.setNameAndDescription(ostring("B03analytic"), 
				    ostring("Ambient B field in x3 direction"));
  B03analytic.setValue("0.0");
  parameterList.add(&B03analytic);
  
  B02.setNameAndDescription(ostring("B02"), 
			    ostring("Ambient B field in x2 direction"));
  B02.setValue("0.0");

  B03.setNameAndDescription(ostring("B03"), 
			    ostring("Ambient B field in x3 direction"));
  B03.setValue("0.0");

  B1init.setNameAndDescription("B1init",
			       "Initial B1 fields as a function of x1, x2");
  B1init.setValue("0");
  parameterList.add(&B1init);

  B2init.setNameAndDescription("B2init",
			       "Initial B2 fields as a function of x1, x2");
  B2init.setValue("0");
  parameterList.add(&B2init);

  B3init.setNameAndDescription("B3init",
			       "Initial B3 fields as a function of x1, x2");
  B3init.setValue("0");
  parameterList.add(&B3init);

  E1init.setNameAndDescription("E1init",
			       "Initial E1 fields as a function of x1, x2");
  E1init.setValue("0");
  parameterList.add(&E1init);

  E2init.setNameAndDescription("E2init",
			       "Initial E2 fields as a function of x1, x2");
  E2init.setValue("0");
  parameterList.add(&E2init);

  E3init.setNameAndDescription("E3init","Initial E3 fields as a function of x1, x2");
  E3init.setValue("0");
  parameterList.add(&E3init);

  Bf.setNameAndDescription(ostring("Bf"), 
			   ostring("file containing B field"));
  Bf.setValue("NULL");
  
  zoff.setNameAndDescription(ostring("zoff"), 
			     ostring("z offset for wiggler field"));
  zoff.setValue("0.0");
  
  betwig.setNameAndDescription(ostring("betwig"), 
			       ostring("kz for wiggler field"));
  betwig.setValue("0.0");

  j1BeamDump.setNameAndDescription("j1BeamDump", 
				   "grid coord of left side (-1 = off)");
  j1BeamDump.setValue("-1");

  j2BeamDump.setNameAndDescription("j2BeamDump", 
				   "grid coord of right side (-1 = off)");
  j2BeamDump.setValue("-1");
 
  MarderIter.setNameAndDescription(ostring("MarderIter"), "");
  MarderIter.setValue("0");

  MarderParameter.setNameAndDescription(ostring("MarderParameter"),
					ostring("Relaxation parameter, should be less than 0.5"));
  MarderParameter.setValue("0.5");

  ElectrostaticFlag.setNameAndDescription(ostring("ElectrostaticFlag"),
					  ostring("Flag: 1=Electrostatic, 0=Electromagnetic."));
  ElectrostaticFlag.setValue("0");

  SynchRadiationFlag.setNameAndDescription(ostring("SynchRadiationFlag"),
                                           ostring("Flag: 1=SynchRadiation, 0=Electromagnetic."));
  SynchRadiationFlag.setValue("0");

  FreezeFields.setNameAndDescription("FreezeFields", "");
  FreezeFields.setValue("0");

  NonRelativisticFlag.setNameAndDescription(ostring("NonRelativisticFlag"),
					    ostring("Flag:  1=NonRelativistic particle push, 0=Relativisticparticle push - default is 0."));
  NonRelativisticFlag.setValue("0");

  nSmoothing.setNameAndDescription("nSmoothing", "");
  nSmoothing.setValue("0");

  histmax.setNameAndDescription(ostring("histmax"),"");
  histmax.setValue("4096");

  InfiniteBFlag.setNameAndDescription(ostring("InfiniteBFlag"),
				      ostring("Flag:  1=Non Relativistic Infinite B1, 0=Relativisticparticle push - default is 0."));
  InfiniteBFlag.setValue("0");
  
  FieldSubFlag.setNameAndDescription(ostring("FieldSubFlag"),
				     ostring("Flag:  number of times to subcycle the fields - default is 1."));

  FieldSubFlag.setValue("1");

  CurrentWeighting.setNameAndDescription(ostring("CurrentWeighting"),
					 ostring("Flag:  1=bilinear (need divergence clean), 0=charge conserving."));
  CurrentWeighting.setValue("0");

  DivergenceCleanFlag.setNameAndDescription(ostring("DivergenceCleanFlag"),
														  ostring("Flag:  1=use clean, 0=no clean."));
  DivergenceCleanFlag.setValue("0");

  BoltzmannFlag.setNameAndDescription(ostring("BoltzmannFlag"),
				      ostring("Flag: 1=Boltzmann electrons field solve, 0=full particles"));
  BoltzmannFlag.setValue("0");
  
  BoltzmannTemp.setNameAndDescription(ostring("BoltzmannTemp"),
				      ostring("Temperature of Boltzmann electrons"));
  BoltzmannTemp.setValue("1");

  BoltzmannDensity.setNameAndDescription(ostring("BoltzmannDensity"),
					 ostring("Neutral density of Boltzmann electrons"));
  BoltzmannDensity.setValue("0.0");

  BoltzSpecies.setNameAndDescription("BoltzSpecies",	"name of Boltzmann Species");
  BoltzSpecies.setValue("NULL");
  parameterList.add(&BoltzSpecies);

  BoltzmannCharge.setNameAndDescription(ostring("BoltzmannCharge"),
					ostring("Charge of Boltzmann electrons"));
  BoltzmannCharge.setValue("-1");

  BoltzmannEBFlag.setNameAndDescription(ostring("BoltzmannEBFlag"),
					ostring("if EBFlag=1 energy balance is used, EBFlag=0 temperature constant"));
  BoltzmannEBFlag.setValue("0");

  BoltzmannChargeRatio.setNameAndDescription(ostring("BoltzmannChargeRatio"),
				ostring("Set the charge ratio of source to n0"));
  BoltzmannChargeRatio.setValue("1");

  echoFlag.setNameAndDescription("echoFlag", "0 = no echo, 1 = echo input file");
  echoFlag.setValue("0");
  parameterList.add(&echoFlag);

  duplicateParticles.setNameAndDescription("duplicateParticles", "number of times to duplicate particles");
  duplicateParticles.setValue("0");
  parameterList.add(&duplicateParticles);

  np2cFactor.setNameAndDescription("np2cFactor", "Dump file np2c multiplier");
  np2cFactor.setValue("1");
  parameterList.add(&np2cFactor);

  particleLimit.setNameAndDescription("particleLimit", "max. total particles in sim");
  particleLimit.setValue("1E8");
  parameterList.add(&particleLimit);

  presidue.setNameAndDescription("presidue","Residue for poisson solves");
  presidue.setValue("1E-3");
  parameterList.add(&presidue);

  initPoissonSolve.setNameAndDescription("initPoissonSolve","flag turning on/off the intital poisson solve.");
  initPoissonSolve.setValue("1");
  parameterList.add(&initPoissonSolve);

  parameterList.add(&dt);
  parameterList.add(&emdamping); 
  parameterList.add(&B01);
  parameterList.add(&B02);
  parameterList.add(&B03);
  parameterList.add(&Bf);
  parameterList.add(&zoff);
  parameterList.add(&betwig);
  parameterList.add(&j1BeamDump);
  parameterList.add(&j2BeamDump);
  parameterList.add(&MarderIter);
  parameterList.add(&MarderParameter);
  parameterList.add(&ElectrostaticFlag);
  parameterList.add(&SynchRadiationFlag);
  parameterList.add(&FreezeFields);
  parameterList.add(&NonRelativisticFlag);
  parameterList.add(&nSmoothing);
  parameterList.add(&histmax);
  parameterList.add(&InfiniteBFlag);
  parameterList.add(&FieldSubFlag);
  parameterList.add(&CurrentWeighting);
  parameterList.add(&DivergenceCleanFlag);
  parameterList.add(&BoltzmannFlag);
  parameterList.add(&BoltzmannTemp);
  parameterList.add(&BoltzmannDensity);
  parameterList.add(&BoltzmannCharge);
  parameterList.add(&BoltzmannEBFlag);
  parameterList.add(&BoltzmannChargeRatio);
  parameterList.add(&Frandseed);

  addLimitRule("emdamping","<",0.0,
				  "Fatal  -- Positive values only",1);

  addLimitRule("emdamping",">",0.5,
				  "Fatal  -- must be less than 0.5",1);

  addLimitRule("MarderIter","<",0.0,
				  "Fatal  --  Positive values only for MarderIter",1);

  addLimitRule("MarderIter",">",20.0,
				  "Warning  --  Do you really want to use so many iterations of the Marder correction?\nMore is not always better!",1);

  addLimitRule("MarderParameter","<",0.0,
				  "Fatal  --  MarderParameter < 0 causes numerical instability",1);

  addLimitRule("MarderParameter",">",1.0,
				  "Fatal  --  MarderParameter > 1 causes numerical instability",1);

  addLimitRule("ElectrostaticFlag","<",0.0,
				  "Fatal  --  0 (off), 1 (DADI), 2 (CGXY), 3 (MULTIGRID), 4 (DADIXY,peridicX1) are valid.",1);

  addLimitRule("ElectrostaticFlag",">",4.0,
				  "Fatal  --  0 (off), 1 (DADI), 2 (CGXY), 3 (MULTIGRID), 4 (DADIXY,peridicX1) are valid.",1);

  addLimitRule("SynchRadiationFlag","<",0.0,
				  "Fatal  --  0 (off), 1 (on) are valid.",1);

  addLimitRule("SynchRadiationFlag",">",1.0,
				  "Fatal  --  0 (off), 1 (on) are valid.",1);

  addLimitRule("CurrentWeighting","<",0.0,
				  "Fatal  --  0 (charge conserving) and 1 (bilinear) are the onlyvalid values for CurrentWeighting.",1);

  addLimitRule("CurrentWeighting",">",1.0,
				  "Fatal  --  0 (charge conserving) and 1 (bilinear) are the onlyvalid values for CurrentWeighting.",1);

  addLimitRule("DivergenceCleanFlag","<",0.0,
				  "Fatal  --  0 (off) and 1 (on) are the only valid values for DivergenceCleanFlag.",1);

  addLimitRule("DivergenceCleanFlag",">",1.0,
				  "Fatal  --  0 (off) and 1 (on) are the only valid values for DivergenceCleanFlag.",1);
}

void ControlParams::checkRules()
{	
/*	
	ostring name = CP.getBoltzSpecies();
	if (name == (ostring)"NULL")
		ruleMessages.add(new ostring("speciesName is required!"));
	else if (!KnownSpecies(name))
		{
			char buffer[512];
			sprintf(buffer, "Unrecognized speciesName: %s", name.c_str());
			ruleMessages.add(new ostring(buffer));
		}
	*/
}
