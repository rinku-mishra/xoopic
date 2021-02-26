#ifndef   __CONTROLG_H
#define   __CONTROLG_H

/*
====================================================================

CONTROLG.H

Purpose:	Define the ControlParams class, which contains the
		control parameters

Version:	$Id: controlg.h 2082 2003-12-10 12:44:19Z rmtrines $

ControlParams Class
Class for specifying the miscellaneous parameters in a spatial group.

Revision/Author/Date
2.01  (Bruhwiler, 10-08-99) added SynchRadiationFlag and get method.
2.04  (Cary, 22 Jan 00) added shiftDelayTime and get method.
3.0   (Johnv 08-Oct-2003) added histmax
====================================================================
*/

#include "part1g.h"

class ControlParams :public ParameterGroup
{
  ScalarParameter dt;
  ScalarParameter emdamping;
  // IntParameter nfft;
  IntParameter Frandseed;
  IntParameter duplicateParticles;
  ScalarParameter np2cFactor;
  ScalarParameter B01;
  ScalarParameter B02;
  ScalarParameter B03;
  StringParameter Bf;
  ScalarParameter zoff;
  ScalarParameter betwig;
  StringParameter B01analytic;
  StringParameter B02analytic;
  StringParameter B03analytic;
  StringParameter B1init;
  StringParameter B2init;
  StringParameter B3init;
  StringParameter E1init;
  StringParameter E2init;
  StringParameter E3init;
  IntParameter j1BeamDump;
  IntParameter j2BeamDump;
  IntParameter MarderIter;
  ScalarParameter MarderParameter;
  IntParameter ElectrostaticFlag;
  IntParameter SynchRadiationFlag;
  IntParameter FreezeFields;
  IntParameter NonRelativisticFlag;
  IntParameter nSmoothing;
  IntParameter histmax;
  IntParameter InfiniteBFlag;
  IntParameter FieldSubFlag;
  IntParameter CurrentWeighting;
  IntParameter DivergenceCleanFlag;
  IntParameter BoltzmannFlag, BoltzmannEBFlag; 
  StringParameter	BoltzSpecies; 
  ScalarParameter BoltzmannTemp, BoltzmannDensity, BoltzmannCharge;
  ScalarParameter BoltzmannChargeRatio;
  ScalarParameter particleLimit;
  ScalarParameter presidue;
  IntParameter echoFlag;
  IntParameter movingWindow;
  IntParameter initPoissonSolve;
  IntParameter dumpTextFiles;
  ScalarParameter shiftDelayTime;
  ScalarParameter gasOffTime; 
  StringParameter	flag;
#ifdef MPI_VERSION
  StringParameter MPIpartition;
#endif

public:
  ControlParams();
  virtual ~ControlParams() {};
  void checkRules();
  Scalar getDt() {return dt.getValue();}
  Scalar getEMdamping() {return emdamping.getValue();}
  //	  int getnfft() {return nfft.getValue();}
  int getFrandseed() {return Frandseed.getValue();}
  Scalar getB01() {return B01.getValue();}
  Scalar getB02() {return B02.getValue();}
  Scalar getB03() {return B03.getValue();}
  ostring getB01analytic() { return B01analytic.getValue();}
  ostring getB02analytic() { return B02analytic.getValue();}
  ostring getB03analytic() { return B03analytic.getValue();}
  ostring getB1init() { return B1init.getValue();}
  ostring getB2init() { return B2init.getValue();}
  ostring getB3init() { return B3init.getValue();}
  ostring getE1init() { return E1init.getValue();}
  ostring getE2init() { return E2init.getValue();}
  ostring getE3init() { return E3init.getValue();}
  ostring getBf() {return Bf.getValue();};
  Scalar getzoff() {return zoff.getValue();}
  Scalar getbetwig() {return betwig.getValue();}
  int get_j1BeamDump() {return j1BeamDump.getValue();}
  int get_j2BeamDump() {return j2BeamDump.getValue();}
  int getMarderIter() { return (int) MarderIter.getValue();}
  Scalar getMarderParameter() { return MarderParameter.getValue();}
  int getElectrostaticFlag() { return (int)ElectrostaticFlag.getValue();}
  int getInitPoissonSolveFlag() { return (int) initPoissonSolve.getValue();}
  int getSynchRadiationFlag() { return (int)SynchRadiationFlag.getValue();}
  int get_FreezeFields() {return (int)FreezeFields.getValue();}
  int getNonRelativisticFlag() { return (int)NonRelativisticFlag.getValue();}
  int get_nSmoothing() {return (int)nSmoothing.getValue();}
  int get_histmax() {return (int)histmax.getValue();}
  int getInfiniteBFlag() { return (int)InfiniteBFlag.getValue();}
  int getFieldSubFlag() { return (int)FieldSubFlag.getValue();}
  int getCurrentWeighting() { return (int)CurrentWeighting.getValue();}
  int getDivergenceCleanFlag() { return (int)DivergenceCleanFlag.getValue();}
  int getBoltzmannFlag() { return (int)BoltzmannFlag.getValue();}
  ostring getBoltzSpecies() {return BoltzSpecies.getValue();}
  Scalar getBoltzmannDensity() { return BoltzmannDensity.getValue();}
  Scalar getBoltzmannTemp() { return BoltzmannTemp.getValue();}
  Scalar getBoltzmannCharge() { return BoltzmannCharge.getValue();}
  Scalar getBoltzmannChargeRatio() { return BoltzmannChargeRatio.getValue();}
  int getBoltzmannEBFlag() { return BoltzmannEBFlag.getValue();}
  Scalar get_presidue() {return presidue.getValue();}
  long int get_particleLimit() {return (long int)particleLimit.getValue();}
  int get_echoFlag() {return echoFlag.getValue();}
  Scalar getduplicateParticles() {return (int) duplicateParticles.getValue();}
  Scalar get_np2cFactor() {return np2cFactor.getValue();}
  int getMovingWindow() { return movingWindow.getValue();}
  Scalar getShiftDelayTime() { return shiftDelayTime.getValue();}
  Scalar getGasOffTime() { return gasOffTime.getValue(); } 
#ifdef MPI_VERSION
  ostring getMPIpartition() { return MPIpartition.getValue();}
#endif
};



#endif  //  __CONTROLG_H
