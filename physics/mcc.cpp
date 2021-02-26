/*
=====================================================================
mcc.cpp

Methods for the MCC (Monte Carlo collision) class.

0.9	(JohnV 02-06-95) Initial draft.
1.01  (JohnV 01-02-97) Restructure for flexibility.
1.1   (JohnV 08-04-97) Added MCCPackages for Ar, Ne and Xe.
1.11  (JohnV 08-15-97) Repaired disparity between cross sections and
      thresholds for inelastic collisions.
1.12  (JohnV 10-01-97) Fixed mcc to work with supercycled timesteps.
1.13  (KC 10-20-97) Fixed mcc to work with M/m < 2.
1.14  (JohnV 02-11-99) Added He package (from Y. Ikeda)
2.5   (Bruhwiler 03-03-00) Added Lithium package (from S.M. Younger)
2.51  (Bruhwiler 08-29-00) Modified Lithium package to be relativistic
2.52  (Bruhwiler 09-03-00) added relativistic elastic scattering for
       Li -- this required changes in CrossSection and FuncCrossSection
2.53  (Bruhwiler 09-26-00) added capability to limit MCC region (i.e.
       limit the neutral gas) to a subregion of the grid.
2.54  (JohnV 08NOV2000) fixed glitch in e-Xe elastic scattering cross section.
(JohnV 09Sep2007) Fixed rare loss of precision in frand() that resulted in index of
collison particle equal to number of particles

Consider how to make cross sections work for adjusted-mass particles
		
=====================================================================
*/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cstdio>


#ifdef _MSC_VER
#include <fstream>
#else
//
using namespace std;
#endif

#include "mcc.h"
#include "species.h"
#include "maxwelln.h"
#include "particle.h"
#include "ptclgrp.h"
#include "misc.h"

const Scalar	NeVperTORR =	8.3221e20;
const Scalar	MAX_ENERGY = 1000;

#define pow(x,y) pow((Scalar)(x),(Scalar)(y))

//-------------------------------------------------------------------
// CrossSection: provides basic cross section handling capability


//-------------------------------------------------------------------
//	eCrossSection methods

eCrossSection::eCrossSection(Scalar e0, Scalar e1, Scalar e2, Scalar sMax, Type _type)
{
	energy0 = e0;
	energy1 = e1;
	energy2 = e2;
	sigmaMax = sMax;
	const1 = sigmaMax/(energy1 - energy0);
	const3 = sigmaMax*energy2/log(energy2);
	type = _type;
}

Scalar eCrossSection::sigma(Scalar energy)
{
	if (energy <= energy0) return 0;
	if (energy <= energy1) return (energy - energy0)*const1;
	if (energy <= energy2) return sigmaMax;
	return const3*log(energy)/energy;
}


//-------------------------------------------------------------------
// Argon ionization cross section (Rapp & Golden) - energies in eV

Scalar argonIoniz::sigma(Scalar energy)
{										  // 
   if (energy < 15.76) return 0;
	else if (energy < 79) 
	  return 1.7155e-18*(energy-15.76)/(energy*energy)*log(0.0634*energy);
	else return 2.648e-18*(energy-15.76)/(energy*energy)*log(0.0344*energy);
}

//-------------------------------------------------------------------
//	iCrossSection methods

iCrossSection::iCrossSection(Scalar _a, Scalar _b, Type _type)
{
	a = _a;
	b = _b;
	type = _type;
}

Scalar iCrossSection::sigma(Scalar energy)
{
	return a + b/(sqrt(energy) + 1E-30);
}

//--------------------------------------------------------------------
// MCC: flexible collision package for handling multiple gases

MCC::~MCC()
{
  packageList.deleteAll(); // remove all list elements and data for packages
  pList.deleteAll();       // delete the list elements and data for the particles
                               
}

ParticleList& MCC::collide(ParticleGroupList& pgList)
  /* throw(Oops) */{
  oopicListIter<MCCPackage> packageIter(packageList);
  for (packageIter.restart(); !packageIter.Done(); packageIter++){
    try{
      //printf("MCC::collide...");
      packageIter()->collide(pgList, pList);
    }
    catch(Oops& oops3){
      oops3.prepend("MCC::collide: Error: \n");
      throw oops3;
    }
  }
  return pList;
}

/**
 * MCTI: Monte Carlo Tunneling Ionization package methods 
 * for handling multiple gases
 * dad 03/27/01
 */
MCTI::~MCTI() {
  packageList.deleteAll(); // remove all list elements and data for the MCTI packages
  pList.deleteAll();       // delete the list elements and data for the particles
}

void MCTI::tunnelIonize(ParticleGroupList** pgList)
/* throw(Oops) */{
  oopicListIter<MCTIPackage> packageIter(packageList);
  for (packageIter.restart(); !packageIter.Done(); packageIter++){
    try{
	    packageIter()->tunnelIonize(pgList, pList);
    }
    catch(Oops& oops2){
      oops2.prepend("MCTI::tunnelIonize: Error: \n");//done
      throw oops2;
    }
  }
}

// BaseMCPackge
BaseMCPackage::BaseMCPackage(GAS_TYPE _type, const ostring& strGasType, 
                             const Scalar& temperature, 
                             Species* _eSpecies, Species* _iSpecies,
                             const Scalar& pressure, SpatialRegion* _SR,
                             Scalar _Min1MKS, Scalar _Max1MKS, 
                             Scalar _Min2MKS, Scalar _Max2MKS,
                             const ostring& _analyticF, 
                             const int& discardDumpFileNGDDataFlag)
  /* throw(Oops) */{
  type = _type;
  gasTypeString = strGasType;
  temp = temperature;
  gasDensity = NeVperTORR * pressure / ( temp + 1.e-30 );

  //
  // set default values for the following two pointers
  // (they are used as handles)
  //
  pList = 0;
  pg = 0;

  // initialize the Species pointers
  eSpecies = _eSpecies;
  iSpecies = _iSpecies;

  // gas may be confined to a portion of the grid (Bruhwiler 09/25/2000)
  Min1MKS = MIN(_Min1MKS,_Max1MKS);
  Max1MKS = MAX(_Max1MKS,_Min1MKS);
  Min2MKS = MIN(_Min2MKS,_Max2MKS);
  Max2MKS = MAX(_Max2MKS,_Min2MKS);

  region = _SR;
  grid   = region -> get_grid();
  J      = grid -> getJ();
  K      = grid -> getK();
  
  // skip the following logic in default case
  if (_Min1MKS > 0. && _Max1MKS > 0. && _Min2MKS > 0. && _Max2MKS > 0.) {
    // determine the maxima for this grid.  Clip the load if it's too large.
    Scalar x1min = grid->getX()[0][0].e1();
    Scalar x2min = grid->getX()[0][0].e2();
    Scalar x1max = grid->getX()[grid->getJ()][0].e1();
    Scalar x2max = grid->getX()[0][grid->getK()].e2();

    Min1MKS = MAX(x1min,Min1MKS);
    Max1MKS = MIN(x1max,Max1MKS);
    Min2MKS = MAX(x2min,Min2MKS);
    Max2MKS = MIN(x2max,Max2MKS);

    p1 = Vector2(Min1MKS,Min2MKS);
    p2 = Vector2(Max1MKS,Max2MKS);
    deltaP = p2-p1;
    
    p1Grid = grid->getGridCoords(p1);
    p2Grid = grid->getGridCoords(p2)+Vector2(1,1);
    Vector2 xt = grid->getGridCoords(p2);

    if ( J <= (int)p2Grid.e1() )  p2Grid.set_e1( (Scalar)J );
    if ( K <= (int)p2Grid.e2() )  p2Grid.set_e2( (Scalar)K );

    if (grid->query_geometry() == ZXGEOM) rz = FALSE;
    else if (grid->query_geometry() == ZRGEOM) rz = TRUE;
    else {
      stringstream ss (stringstream::in | stringstream::out);
      ss << "BaseMCPackage::BaseMCPackage: Error: \n"<<
        "MCCPackage constructor doesn't recognize the geometery flag" << std::endl;

      std::string msg;
      ss >> msg;
      Oops oops(msg);
      throw oops;    // exit() not called 

    }
  }  // end of logic regarding limited region for gas density
  else {
    p1Grid.set_e1( 0. );
    p1Grid.set_e2( 0. );
    p2Grid.set_e1( (Scalar)J );
    p2Grid.set_e2( (Scalar)K );
  }

  /**
   * at this point I am ready to initialize the NGD.
   * pass the string (ostring) of the function to be used for the evaluation of
   * the NeutralGasDensity data member of the Grid class. 
   * dad: Wed Nov 15 14:39:43 MST 2000}
   */
  try{
    ptrNGD = region->initNGD(gasTypeString, _analyticF, gasDensity, p1Grid, p2Grid, 
                           discardDumpFileNGDDataFlag);
  }
  catch(Oops& oops3){
    oops3.prepend("BaseMCPackage::BaseMCPackage: Error: \n");
    throw oops3;
  }
  maxNGD = ptrNGD->getMaxNGD();
  gasDensity = maxNGD;
  
}
//----------------------------------------------------------------
// MCCPackage: individual gas packages

MCCPackage::MCCPackage(GAS_TYPE _type, const ostring& strGasType, 
                       Scalar pressure, Scalar _temp,
                       Species* _eSpecies, Species* _iSpecies, SpeciesList& sList,
                       Scalar dt, int _ionzFraction, Scalar _ecxFactor, Scalar _icxFactor,
                       SpatialRegion* _SR, Scalar _Min1MKS, Scalar _Max1MKS,
                       Scalar _Min2MKS, Scalar _Max2MKS, Scalar _delayTime, Scalar _stopTime, 
                       const ostring& _analyticF, const int& discardDumpFileNGDDataFlag) 
  : BaseMCPackage( _type, strGasType, _temp, _eSpecies, _iSpecies, pressure, _SR, 
                   _Min1MKS, _Max1MKS, _Min2MKS, _Max2MKS, _analyticF, 
                   discardDumpFileNGDDataFlag)
{
  ionzFraction = _ionzFraction;
  ecxFactor = _ecxFactor;

  icxFactor = _icxFactor;
  neCX = niCX = 0;
  eCX = iCX = NULL;

  delayTime = _delayTime;
  stopTime  = _stopTime;

  addCrossSections();
}

void MCCPackage::init(SpeciesList& sList, Scalar dt)
{
  if (gasDensity > 0) {
    extra = new Scalar[sList.nItems()];
    collProb = new Scalar[sList.nItems()];
    // eEnergyScale = fabs(0.5/eSpecies->get_q_over_m());
    // iEnergyScale = fabs(0.5/iSpecies->get_q_over_m());
    eEnergyScale = fabs(0.5/PROTON_CHARGE*eSpecies->get_m());
    iEnergyScale = fabs(0.5/PROTON_CHARGE*iSpecies->get_m());
    
    // check for electron and ion collisions
    maxSigmaVe = maxSigmaVi = 0;
    oopicListIter<Species> sIter(sList);
    for (sIter.restart(); !sIter.Done(); sIter++){
      int id = sIter.current()->getID();
      extra[id] = 0.5;
      switch (sIter()->get_collisionModel()) {
      case none:
	break;
      case boltzmann:
	//			BoltzmannInit();
	break;
      case electron:
      case test:
	if (maxSigmaVe == 0) maxSigmaVe = sumSigmaVe();
	collProb[id] = 1 - exp(-gasDensity*maxSigmaVe*dt*sIter()->get_subcycleIndex()
			       /sIter()->get_supercycleIndex());
	break;
      case ion:
	if (maxSigmaVi == 0) maxSigmaVi = sumSigmaVi();
	collProb[id] = 1 - exp(-gasDensity*maxSigmaVi*dt*sIter()->get_subcycleIndex()
			       /sIter()->get_supercycleIndex());
	break;
      }
    }
    
    //	create isotropic maxwellian gas distribution with ion characteristics
    gasDistribution = new Maxwellian(iSpecies);
    gasDistribution->setThermal(temp, EV);
  }
  else extra = collProb = NULL;
}

MCCPackage::~MCCPackage()
{
  if (eCX) {
	 for (int i=0; i<neCX; i++) delete eCX[i];
	 delete[] eCX;
  }
  if (iCX) {
	 for (int i=0; i<niCX; i++) delete iCX[i];
	 delete[] iCX;
  }
  delete[] extra;
  delete[] collProb;
  delete gasDistribution;
}

void MCCPackage::addCrossSections()
{
  neCX = niCX = 0;
  eCX = iCX = NULL;
}

void MCCPackage::collide(ParticleGroupList& pgList, ParticleList& _pList)
  /* throw(Oops) */{
  if (pgList.isEmpty() || !collProb) return;
  
  // return if simulation time is not within the specified time interval
  Scalar time = region->getTime();
  //  cout << "_____________________" << endl;
  //  cout << " simulation time is: " << time << endl << endl;
  if ( time<delayTime ) return;
  if ( time>stopTime && stopTime>0.) return;
  
  Species* species = pgList.headData()->get_species();
  
  CollisionModel collisionModel = species->get_collisionModel();
  if (collisionModel == none) return;
  int id = species->getID();
  if (collProb[id] < 1e-20) return;
  
  pList = &_pList;
  int n = 0;
  oopicListIter<ParticleGroup> pgIter(pgList);
  for (pgIter.restart(); !pgIter.Done(); pgIter++) {
    // n is the total # of particles in all relevant particle groups
    n += pgIter()->get_n();
  }
  if (n==0) return; // return if no particles
  
  // number of potential collisions is n times the max. collision prob.

  ///cout << "n = " << n << endl;
  extra[id] += n * collProb[id];
  int nCollisions = (int)extra[id];
  extra[id] -= nCollisions;

  // holds position vector of particle that might undergo collision
  Vector2 xTemp;
  
  // loop over random selection of particles and see if collision occurs:
  while (nCollisions--){
    // index = int(double(frand())*n);   // get extended particle index
    index = int(frand()*n);   // get extended particle index
    // must cast to double or precision loss can result in index = 1
    // find it
    for (pgIter.restart(); !pgIter.Done(); pgIter++) {
      if (index < pgIter()->get_n()) break;
      else index -= (int)pgIter()->get_n();
    }
    /** 
     * the initialization of the class member data:
     * ParticleGroup* pg; (declared in mcc.h) follows.
     * Note that this pointer is not initialized in the 
     * constructor of MCCPackage.
     */
    pg = pgIter(); 
    // This is new code to determine if particle lies within the region
    //   of the grid that contains neutral gas. (Bruhwiler 09/26/2000)
    //   If the particle is outside the gas region, then we simply
    //   "continue" back to the top of the while construct.
    xTemp = pg->get_x(index);
    if ( xTemp.e1() < p1Grid.e1() || xTemp.e1() > p2Grid.e1() ||
         xTemp.e2() < p1Grid.e2() || xTemp.e2() > p2Grid.e2() ) continue;
    /**
     * indices for the cell on the grid, the particle is on:
     */
    int iNGD = static_cast<int>(xTemp.e1());
    int jNGD = static_cast<int>(xTemp.e2());
    
    // check that the NeutralGasDensity in the cell is 
    // greater than zero so ionization events can occur, if not
    // return at this point.
    try{
      if ( ptrNGD->getNGD(iNGD, jNGD) > 0.0 ) {
        // do nothing and continue the rest of the loop's body
      } else { 
        // start over at the beginning of the loop by attempting 
        // a collision with another particle
        continue; 
      }
    }
    catch(Oops& oops2){
      oops2.prepend("MCCPackage::collide: Error: \n"); //MCC::collide:
      throw oops2;
    }

    // Particle lies within gas region, so proceed with previous logic
    // mindgame: start with gasVelocity stuff
    Vector3 gasVelocity;
    Vector3 *uPtr = pg->get_u();
    // play with the velocity, not gamma*velocity
    u = uPtr[index]/pg->gamma(index);

    if (collisionModel == ion) {
       // velocity should be relative velocity to the neutral gas.
		gasVelocity = gasDistribution->get_V();
		u -= gasVelocity;
    }

	  energy = pg->energy2_eV(u, index);
	  v = u.magnitude(); // johnv: removed + 1E-30
    
    // mindgame: make it simpler and self-consistent.
#if 0
    // This is part of the original, nonrelativistic algorithm.
    // For this algorithm, in the "relativistic" case, the gamma
    //  factor is extracted from the normalized momentum, leaving
    //  v to be the velocity in m/s.
    BOOL relativistic = FALSE;
    if ( relativisticMCC == 0 ) {
      if (u > 1.e8) {
        relativistic = TRUE;
        u /= pg->gamma(index);
      }
      energy = pg->energy_eV(index); // in eV
		v = u.magnitude(); // johnv: removed + 1E-30
    }
    // This is for the new, truly relativistic algorithm.
    // This logic is a mess.  It was done this way to
    //   introduce new capabilities with minimal impact
    //   on existing code.  At some point, we need to
    //   clean this up.
    else {
      Scalar gamma = sqrt(1.+v*v*iSPEED_OF_LIGHT_SQ);
      energy = ELECTRON_MASS_EV*(gamma-1.);
      
      //      cout << "^^^^^^^^" << endl;
      //      cout << " Initially, v=gamma*beta*c = " << v      << endl;
      //      cout << " gamma  is calculated to be: " << gamma  << endl;
      //      cout << " energy is calculated to be: " << energy << endl;
      
      v /= gamma;     // v must be the velocity in m/s (no gamma)
      
      //      cout << " Finally, v=beta*c is found: " << v      << endl;
      //      cout << endl << endl;
    }
#endif
    
    Scalar	random = frand()/(v+1E-30);
    Scalar	sumSigma = 0.;
    if ((collisionModel == electron)||(collisionModel == test)) {
      random *= maxSigmaVe;
      // loop over all possible collisions with this gas
      for (int i=0; i<neCX; i++) {
	
	/**
	 * instead of the old line of code:
	 * sumSigma += ecxFactor*eCX[i]->sigma(energy);
	 * which worked for the uniform gas density we use 
	 * the scaled neutral gas density: dad
	 */
        try{
			 sumSigma += ecxFactor*eCX[i]->sigma(energy)*(ptrNGD->getNGD(iNGD,jNGD))/maxNGD;
        }
        catch(Oops& oops2){
          oops2.prepend("MCCPackage::collide: Error: \n");//OK
          throw oops2;
        }
	///  sumSigma += ecxFactor*eCX[i]->sigma(energy);
		  if (random < sumSigma) { // johnv: changed <= to < to avoid roundoff when sigma small
			 // if (energy < eCX[i]->get_threshold()) printf("collision: energy=%G, threshold=%G, sigma=%G\n", energy, eCX[i]->get_threshold(), eCX[i]->sigma(energy));
          dynamic(*eCX[i]);
			 break; // leave for loop if collision done
		  }
      }
    }
    else if (collisionModel == ion) {
      random *= maxSigmaVi;
      // loop over all possible collisions with this gas
      for (int i=0; i<niCX; i++) {
        try{
	        sumSigma += icxFactor*iCX[i]->sigma(energy)*(ptrNGD->getNGD(iNGD,jNGD))/maxNGD;
        }
        catch(Oops& oops2){
          oops2.prepend("MCCPackage::collide: Error: \n");//OK
          throw oops2;
        }
        ///sumSigma += icxFactor*iCX[i]->sigma(energy);	
		  if (random <= sumSigma) {
			 dynamic(*iCX[i]);
			 break; // leave for loop if collision done
		  }
      }
    }
    
    if (collisionModel == ion) {
      u += gasVelocity;
    }

    u *= pg->gamma2(u); // johnv: is this needed when nonrelativistic?
    // mindgame: save gamma*velocity to the array.
    uPtr[index] = u;
  } // end of "while(nCollisions--)" construct
  return;
}

// compute null (max) cross section for e-neutral collisions
Scalar MCCPackage::sumSigmaVe()
{
  maxSigmaVe = 0;
  for (Scalar energy=0; energy<MAX_ENERGY; energy+=0.5) 
    {
      Scalar temp = 0.;
      for (int i=0; i<neCX; i++) 
	{ 
	  temp += eCX[i]->sigma(energy);
	}
      maxSigmaVe = MAX(maxSigmaVe, sqrt(energy)*temp);
    }
  maxSigmaVe *= ecxFactor/sqrt(eEnergyScale);
  return maxSigmaVe;
}

// compute null (max) cross section for ion-neutral collisions
Scalar MCCPackage::sumSigmaVi()
{
  maxSigmaVi = 0;
  for (Scalar energy=0; energy<MAX_ENERGY; energy+=0.5) {
	 Scalar temp = 0;
	 for (int i=0; i<niCX; i++) temp += iCX[i]->sigma(energy);
	 // fprintf(stderr, "\nenergy: %g, sigmav=%g, max=%g", 
	 //	 energy, sqrt(energy)*temp, maxSigmaVi);
	 maxSigmaVi = MAX(maxSigmaVi, sqrt(energy)*temp);
  }
  maxSigmaVi *= icxFactor/sqrt(iEnergyScale);
  return maxSigmaVi;
}

void MCCPackage::dynamic(CrossSection& cx)
/* throw(Oops) */{
  switch(cx.get_type()) 
	 {
	 case CrossSection::ELASTIC:
		elastic(cx);
		break;
	 case CrossSection::EXCITATION:
		excitation(cx);
		break;
	 case CrossSection::IONIZATION:
     try{
		   ionization(cx);
     }
     catch(Oops& oops3){
       oops3.prepend("MCCPackage::dynamic: Error: \n");
       throw oops3;
     }
		 break;
	 case CrossSection::ION_ELASTIC:
		ionElastic(cx);
		break;
	 case CrossSection::CHARGE_EXCHANGE:
		chargeExchange(cx);
		break;
	 default:
		printf("unrecognized collision type!\n");
	 }
}

void MCCPackage::elastic(CrossSection& cx)
{
  	// This is the original, nonrelativistic algorithm
  if (relativisticMCC == 0) {
    u /= (v+1E-30); // normalize
    newVelocity(energy, v, u, TRUE); // scatter electron
	}

  	// This is the new relativistic algorithm
  else {

		// Calculate the scattering angles.
    Scalar thetaS, phiS;
    elasticScatteringAngles(energy, cx, thetaS, phiS);

		// Now, calculate the new normalized momentum vector
    //   for the scattered electron.
    Vector3 uScatter;
    uScatter = newMomentum(u, energy, thetaS, phiS);
    u = uScatter;
	}
}

void MCCPackage::excitation(CrossSection& cx)
{
  u /= v;   // is this in the right place? Should be after energy loss?
  energy -= cx.get_threshold();   // subtract inelastic energy loss
  //  if (energy < 0) printf("excitation: energy=%G, sigma=%G\n", energy, cx.sigma(energy));
  v = sqrt(energy/eEnergyScale);  // recompute magnitude of velocity
  newVelocity(energy, v, u, TRUE);
}

void MCCPackage::ionization(CrossSection& cx)
  /* throw(Oops) */{
  	// This is the original, nonrelativistic algorithm
  if (relativisticMCC == 0) {
    u /= v;
    energy -= cx.get_threshold(); // subtract ionization energy
	 //	 if (energy < 0) printf("ionization: energy=%G, sigma=%G\n", energy, cx.sigma(energy));
    Scalar rEnergy = 10*tan(frand()*atan(energy/20));
    energy -= rEnergy;			//	partition remaining energy
				
    //	scatter created electron:
    v = sqrt(rEnergy/eEnergyScale);
    Vector3 uNew = u;
    newVelocity(rEnergy, v, uNew, FALSE);
    
    // holds position vector of particle that undergos collision
    Vector2 xTemp;
    xTemp = pg->get_x(index);
    
    /**
     * indices for the cell on the grid, the particle is on:
     */
    int iNGD = static_cast<int>(xTemp.e1());
    int jNGD = static_cast<int>(xTemp.e2());
    Scalar cellVol = grid->cellVolume(iNGD, jNGD);
    
    Scalar numNeutralAtoms;
    try{
      numNeutralAtoms =  (ptrNGD->getNGD(iNGD,jNGD)) * cellVol;
    }
    catch(Oops& oops2){
      oops2.prepend("MCCPackage::ionization: Error: \n");//done
      throw oops2;
    }
    // Calculate number of physical ions created
    Scalar numIons = pg->get_np2c(index);
    // Check that numNeutral > numIons 
    if ( numNeutralAtoms > numIons ) { 
      // create a computational particle with electron species with
      // number of physical electrons equal to the number of physical
      // ions: numIons
      pList->add(new Particle(pg->get_x(index), uNew*pg->gamma(uNew), eSpecies, 
                              numIons, pg->isVariableWeight() ) );
      
      // continue with creation of the ions and reset the 
      // Neutral Gas Density accordingly.
      Scalar stmp = (numNeutralAtoms - numIons)/cellVol; 
      ptrNGD->set(iNGD, jNGD,  stmp);

      // create n=ionzFraction ions from isotropic gas distribution
      for (int i=0; i<ionzFraction; i++) {
	uNew = gasDistribution->get_V();
	pList->add(new Particle(pg->get_x(index), uNew, iSpecies,
				numIons/ionzFraction, pg->isVariableWeight()));
      }
    } else { 
      // this is the case of numIons >= numNeutralAtoms
      // if so, set numIons = numNeutralAtoms
      //        set density to zero
      //        replace pg->getnp2c(index) with numNeutralAtoms and
      //        replace pg->isVariableWeight() with "true" when 
      //        creating the Particle(...).

      // create first the electrons:
      pList->add(new Particle(pg->get_x(index), uNew*pg->gamma(uNew), eSpecies, 
                              numNeutralAtoms, true ) );
      
      ptrNGD->set(iNGD, jNGD, 0.0 );
      
      // create n=ionzFraction ions from isotropic gas distribution
      for (int i=0; i<ionzFraction; i++) {
        uNew = gasDistribution->get_V();
        pList->add(new Particle(pg->get_x(index), uNew, iSpecies,
                                numNeutralAtoms/ionzFraction, true));
      }
    }

    // scatter incident electron
    v = sqrt(energy/eEnergyScale);
    newVelocity(energy, v, u, FALSE);
  }
  
  // This is the new relativistic algorithm
  
  /// All new ionization/gas code implemented above is repeated below.
  /// Eventually, the nonrelativistic code above will go away.
  /// --- Let's put the relevant code into helper functions. ---

  else {

		// First, calculate the energy of the ejected electron.
    Scalar eEjected = ejectedEnergy(cx, energy);
    Scalar newEnergy = energy - cx.get_threshold() - eEjected;
		//    cout << "*******************************" << endl;
		//    cout << " impact     energy = " << energy             << endl;
		//    cout << " ejected    energy = " << eEjected           << endl;
		//    cout << " ionization energy = " << cx.get_threshold() << endl;
		//    cout << " remaining  energy = " << newEnergy          << endl;
		//    cout << " energy difference = " << newEnergy-energy   << endl;
		//    cout << endl;

		// Next, calculate the scattering angles of both the
		//   primary electron and the ejected electron (these
		//   angles are wrt initial direction of primary).
    Scalar thetaP, phiP, thetaS, phiS;
    primarySecondaryAngles(cx, energy, eEjected,
                           thetaP, phiP, thetaS, phiS);

    // Now, calculate the new normalized momentum vector
    // for the ejected secondary electron, and add it
    // to the appropriate particle list when the 
    // electron species particle is created.
    Vector3 uEjected = newMomentum(u, eEjected, thetaS, phiS);

    // create n=ionzFraction ions from isotropic gas distribution

    // repeat the NeutralGasDensity reset and creat the ions
    Vector3 uIon;
    // holds position vector of particle that undergos collision
    Vector2 xTemp;
    xTemp = pg->get_x(index);
    
    /**
     * indices for the cell on the grid, the particle is on:
     */
    int iNGD = static_cast<int>(xTemp.e1());
    int jNGD = static_cast<int>(xTemp.e2());
    Scalar cellVol = grid->cellVolume(iNGD, jNGD);
    
    Scalar numNeutralAtoms;
    try{
      numNeutralAtoms =  (ptrNGD->getNGD(iNGD,jNGD)) * cellVol; 
    }
    catch(Oops& oops2){
      oops2.prepend("MCCPackage::ionization: Error: \n");//OK
      throw oops2;
    }
    
    // Calculate number of physical ions created
    Scalar numIons = pg->get_np2c(index);
    // Check that numNeutral > numIons 
    if ( numNeutralAtoms > numIons ) { 
      // create the electron species computational particle with 
      // number of physical electrons equal to numIons
      pList->add(new Particle(pg->get_x(index), uEjected, eSpecies, 
                              numIons, pg->isVariableWeight() ) );
      // continue with the ionization and reset the 
      // Neutral Gas Density accordingly.
      Scalar stmp = (numNeutralAtoms - numIons)/cellVol; 
      ptrNGD->set(iNGD, jNGD,  stmp);
      
      // create n=ionzFraction ions from isotropic gas distribution
      for (int i=0; i<ionzFraction; i++) {
	uIon = gasDistribution->get_V();
	pList->add(new Particle(pg->get_x(index), uIon, iSpecies,
				pg->get_np2c(index)/ionzFraction, pg->isVariableWeight()));
      }
    } else { 
      // this is the case of numIons >= numNeutralAtoms
      // if so, set numIons = numNeutralAtoms
      //        set density to zero
      //        replace pg->getnp2c(index) with numNeutralAtoms and
      //        replace pg->isVariableWeight() with "true" when 
      //        creating the Particle(...).

      // create the electron species particle
      pList->add(new Particle(pg->get_x(index), uEjected, eSpecies, 
                              numNeutralAtoms, true) );
                              
      ptrNGD->set(iNGD, jNGD, 0.0);
      
      // create n=ionzFraction ions from isotropic gas distribution
      for (int i=0; i<ionzFraction; i++) {
      uIon = gasDistribution->get_V();
      pList->add(new Particle(pg->get_x(index), uIon, iSpecies,
			      numNeutralAtoms/ionzFraction, true));
      }
    }

    // Now generate the new normalized momentum vector for
    //   the scattered primary.
    Vector3 uScatter;
    uScatter = newMomentum(u, newEnergy, thetaP, phiP);

		//    cout << " initial gamma*v = " << u.magnitude()        << endl;
		//    cout << " final   gamma*v = " << uScatter.magnitude() << endl;
		//    cout << " difference is:    " << uScatter.magnitude()-u.magnitude() << endl;
		//    cout << endl << endl;

    u = uScatter;
		//    v = u.magnitude();
  }
}

void MCCPackage::ionElastic(CrossSection& cx)
{

  Scalar up1, up2, up3, mag;
  Scalar r11, r12, r13, r21, r22, r23, r31, r32, r33;
			
  Scalar coschi= sqrt(frand());
  Scalar sinchi = sqrt(1. -coschi*coschi);
	
  Scalar phi1  = TWOPI*frand();
  Scalar cosphi = cos(phi1);
  Scalar sinphi = sin(phi1);
	
  r13 = u.e1()/v;
  r23 = u.e2()/v;
  r33 = u.e3()/v;
	
  if(r33 == 1.0) { up1= 0;  up2= 1;  up3= 0; }
  else           { up1= 0;  up2= 0;  up3= 1; }
  
  r12 = r23*up3 -r33*up2;
  r22 = r33*up1 -r13*up3;
  r32 = r13*up2 -r23*up1;
  mag = sqrt(r12*r12 + r22*r22 + r32*r32) + 1E-30;
	
  r12/= mag;
  r22/= mag;
  r32/= mag;
	
  r11 = r22*r33 -r32*r23;
  r21 = r32*r13 -r12*r33;
  r31 = r12*r23 -r22*r13;
	
  u.set_e1(v*coschi*(r11*sinchi*cosphi +r12*sinchi*sinphi +r13*coschi));
  u.set_e2(v*coschi*(r21*sinchi*cosphi +r22*sinchi*sinphi +r23*coschi)); 
  u.set_e3(v*coschi*(r31*sinchi*cosphi +r32*sinchi*sinphi +r33*coschi));
}

void MCCPackage::chargeExchange(CrossSection& cx)
{
  u.set_e1(0);
  u.set_e2(0);
  u.set_e3(0);
}


/* Method for calculating energy imparted to a secondary (ejected)
 *   electron, using a parametric model that captures relativistic
 *   physics for large impact energy and agrees with data at low energy.
 *
 * The calculations underlying this method are contained in the
 *   Tech-X notes, "RNG Calculations for Scattering in XOOPIC," by
 *   David Bruhwiler (August 28, 2000).
 *
 * The parametric model is a relativistic generalization of the
 *   work by Rudd et al., Phys. Rev. A 44, 1644 (1991) and Phys.
 *   Rev. A 47, 1866 (1993).  The relativistic generalization is
 *   described in the Tech-X notes, "Rudd's Differential Cross
 *   Section for Electron Impact Ionization," by David Bruhwiler,
 *   (July 17, 2000).
 */
Scalar MCCPackage::ejectedEnergy(const CrossSection& cx,
                                 const Scalar& impactEnergy) {

	// First, we need to calculate a random # between 0 and A_>,
  //   where A_> = f1(T) * (T-I) / f2(T) / (T+I), with I the threshold
	//   ionization energy and T the kinetic energy of the impacting
	//   electron "impactEnergy".

  Scalar I = cx.get_threshold();
  if (impactEnergy <= I) return 0.;

  Scalar tPlusMC    = impactEnergy + ELECTRON_MASS_EV;
	Scalar twoTplusMC = impactEnergy + tPlusMC;
  Scalar tPlusI     = impactEnergy + I;
  Scalar tMinusI    = impactEnergy - I;

  Scalar invTplusMCsq = 1. / ( tPlusMC * tPlusMC );
  Scalar tPlusIsq     = tPlusI * tPlusI;
  Scalar iOverT       = I / impactEnergy;


  Scalar funcT1 = 14./3. + .25 * tPlusIsq * invTplusMCsq
                - ELECTRON_MASS_EV * twoTplusMC * iOverT * invTplusMCsq;

  Scalar funcT2 = 5./3. - iOverT - 2.*iOverT*iOverT/3.
                + .5 * I * tMinusI * invTplusMCsq
                + ELECTRON_MASS_EV*twoTplusMC*I*invTplusMCsq*log(iOverT)/tPlusI;

  Scalar aGreaterThan = funcT1 * tMinusI / funcT2 / tPlusI;

	// We may have to try several times before the "rejection method"
	//   lets us keep our ejected energy test value, wTest.
	//   Here we define a bool for the while construct and also
	//   declare some floating point variables needed inside.

  int needToTryAgain = 1;
  Scalar randomFW, wTest, wPlusI, wPlusIsq;
  Scalar invTminusW, invTminusWsq, invTminusW3;
  Scalar probabilityRatio;

  while (needToTryAgain==1) {

  	// The random number, called F(W) in the notes and randomFW here,
  	//   is the antiderivative of a phony_but_simple probability, which
  	//   is always >= the correct_but_messy probability.

		//    do {randomFW=frand();} while (randomFW<=0. || randomFW>=1.);
    randomFW = frand() * aGreaterThan;

  	// wTest is the inverse of F(W)
    wTest = I*funcT2*randomFW/(funcT1-funcT2*randomFW);

  	// Because we are not working directly with the distribution function,
    //   we must use the "rejection method".  This involves generating
    //   another random number and seeing whether it is > the ratio of
  	//   the true probability over the phony_but_simple probability.

    wPlusI       = wTest + I;
    wPlusIsq     = wPlusI * wPlusI;
    invTminusW   = 1./(impactEnergy-wTest);
    invTminusWsq = invTminusW * invTminusW;
    invTminusW3  = invTminusW * invTminusWsq;

    probabilityRatio = ( 1. + 4.*I/wPlusI/3. + wPlusIsq*invTminusWsq
                            + 4.*I*wPlusIsq*invTminusW3/3.
                         - ELECTRON_MASS_EV*twoTplusMC*wPlusI*invTminusW*invTplusMCsq
  											 + wPlusIsq*invTplusMCsq ) / funcT1;

    if ( probabilityRatio >= (Scalar)frand() ) needToTryAgain = 0;
		//    else cout << "** wTest was rejected..." << endl;
	}  // end of while construct
	//  cout << "** wTest was accepted!" << endl << endl;

  // wTest is the energy of the ejected particle;  the scattered primary
	//   has energy T-(W+I)

	//  cout << endl;
	//  cout << " impact  energy = " << impactEnergy << endl;
	//	cout << " ejected energy = " << wTest << endl;
  return wTest;
}


/* Method for calculating the scattering angles (theta, phi) with
 *   respect to the initial direction of the primary electron, for
 *   both the primary and secondary electrons in an impact-ionizaiton
 *   event.
 *
 * Unchanged imput variables are eImpact and eSecondary.
 * Empty variables passed in by reference to hold the desired results
 *   are:  thetaPrimary, thetaSecondary, phiPrimary and phiSecondary.
 *
 * The calculations underlying this method are contained in the
 *   Tech-X notes, "RNG Calculations for Scattering in XOOPIC," by
 *   David Bruhwiler (August 28, 2000).
 *
 * The parametric model is discussed in the Tech-X notes, "Rudd's
 *   Differential Cross Section for Electron Impact Ionization,"
 *   by David Bruhwiler, (July 17, 2000).
 */
void MCCPackage::primarySecondaryAngles(const CrossSection& cx,
            const Scalar& eImpact,  const Scalar& eSecondary,
                  Scalar& thetaPrimary,   Scalar& phiPrimary,
                  Scalar& thetaSecondary, Scalar& phiSecondary) {

    // Bail out, if impact energy < ionization threshold energy.

  Scalar I = cx.get_threshold();
  if (eImpact <= I) {
    thetaPrimary   = 0.;
    thetaSecondary = 0.;

    phiPrimary   = 0.;
    phiSecondary = 0.;

    return;
	}

    // First, we generate a Monte Carlo value for thetaPrimary --
    // We begin with the definition of some temporaries:

  Scalar ePrimary = eImpact - eSecondary - I;

  Scalar w         = ePrimary;
  Scalar wPlusI    = w       + I;
  Scalar wPlusI2MC = wPlusI  + 2.*ELECTRON_MASS_EV;
  Scalar tPlus2MC  = eImpact + 2.*ELECTRON_MASS_EV;

  Scalar alpha;
  alpha  = ELECTRON_MASS_EV / ( eImpact + ELECTRON_MASS_EV );
  alpha *= 0.6 * alpha;

  Scalar g2_T_W = sqrt( wPlusI * tPlus2MC / wPlusI2MC / eImpact );
  Scalar g3_T_W = alpha * sqrt( I * (1.-wPlusI/eImpact) / w );

	//  cout << "  g2-1 = " << g2_T_W - 1. << endl;
	//  cout << "  g3   = " << g3_T_W << endl;

  Scalar fTheta = frand();

  thetaPrimary = acos( g2_T_W + g3_T_W * tan(
                  (1.-fTheta) * atan( (1.-g2_T_W) / g3_T_W )
                     - fTheta * atan( (1.+g2_T_W) / g3_T_W ) ) );

	//  cout << " thetaPrimary = " << thetaPrimary << endl;

  	// Second, we generate a Monte Carlo value for phiPrimary --
  phiPrimary = TWOPI * frand();


    // Third, we generate a Monte Carlo value for thetaSecondary --
  w         = eSecondary;
  wPlusI    = w + I;
  wPlusI2MC = wPlusI + 2.*ELECTRON_MASS_EV;

  g2_T_W = sqrt( wPlusI * tPlus2MC / wPlusI2MC / eImpact );
  g3_T_W = alpha * sqrt( I * (1.-wPlusI/eImpact) / w );

  fTheta = frand();

  thetaSecondary = acos( g2_T_W + g3_T_W * tan(
                    (1.-fTheta) * atan2( (Scalar)(1.-g2_T_W) , (Scalar)g3_T_W )
                       - fTheta * atan2( (Scalar)(1.+g2_T_W) , (Scalar)g3_T_W ) ) );

  	// Fourth, we generate a Monte Carlo value for phiSecondary --
  	// In the low-energy limit, phiPrimary and phiSecondary are
  	//   uncorrelated.  In the high-energy limit, they are 100%
  	//   correlated.  Our approach is to implement a sort of sliding
  	//   scale, in which the correlation increases as ePrimary and
    //   eSecondary both increase.

  Scalar eCritical = 10. * I;
  Scalar a_T_W = 0.;
  if ( eSecondary>eCritical && ePrimary>eCritical ) {
    a_T_W = sqrt((eSecondary-eCritical)*(ePrimary-eCritical))/eCritical;
		//    cout << "  a_T_W = " << a_T_W << endl;
	}
  Scalar delta = TWOPI/(1.+a_T_W);
	//  cout << "  delta/(2*pi) = " << delta/TWOPI << endl;

  phiSecondary = phiPrimary + M_PI + (frand()-.5)*delta;

	//  cout << "  phiSecondary-(phiPrimary+PI) = "
	//       << phiSecondary-phiPrimary-M_PI << endl;

	return;
}


/* Method for calculating the scattering angles (theta, phi) with
 *   respect to the initial direction of the electron, for elastic
 *   electron-neutral scattering.
 *
 * Unchanged input variables are eImpact and the cross section cx.
 * Empty variables passed in by reference to hold the desired results
 *   are:  thetaScatter and phiScatter.
 *
 * The calculations underlying this method are contained in the
 *   Tech-X notes, "RNG Calculations for Scattering in XOOPIC," by
 *   David Bruhwiler (August 28, 2000).
 *
 * The parametric model is discussed in the Tech-X notes, "Elastic
 *   and Inelastic Scattering at all Energies," by David Bruhwiler,
 *   (July 26, 2000).
 */
void MCCPackage::elasticScatteringAngles(
                    const Scalar& eImpact, const CrossSection& cx,
                          Scalar& thetaScatter, Scalar& phiScatter) {

    // First, we generate a Monte Carlo value for thetaScatter --
    // We begin with the definition of some temporaries:

  Scalar atomicNum = cx.getAtomicNumber();

  Scalar gMinus1  = eImpact / ELECTRON_MASS_EV;
  Scalar gamma    = gMinus1 + 1.;
  Scalar beta     = sqrt((gamma+1.)*gMinus1)/gamma;
  Scalar betaMin  = 1.8e-3;

  Scalar thetaMin = pow( atomicNum, 1./3. ) * (1.+betaMin)
                  / ( 192. * gamma * (beta+betaMin) );

  Scalar thetaMinSq = thetaMin * thetaMin;
  Scalar fTheta = frand();
  thetaScatter = acos( (1.+thetaMinSq)
                         - thetaMinSq * (2.+thetaMinSq)
                         / (2.*(1-fTheta) + thetaMinSq) );

	//  if (eImpact>1e10) {
		//    cout << endl;
		//    cout << "impact energy = " << eImpact << endl;
		//    cout << "thetaMin      = " << thetaMin << endl;
		//    cout << "thetaElastic  = " << thetaScatter << endl;
	//	}

  	// Next, we generate a Monte Carlo value for phiScatter --
  phiScatter = TWOPI * frand();

	return;
}


/* Given an initial normalized momentum vel=p/(m*gamma) (m/s), a new
 *   energy, and the scattering angles (wrt to initial direction
 *   defined by u) theta and phi, this method calculates and
 *   returns a new momentum.
 *
 * Note that the magnitude of u is not important -- only the
 *   direction is used.  Thus, this method can be used for
 *   elastic scattering, for the primary in an ionization event,
 *   and also for the ejected secondary.
 *
 * All of the input variables are unchanged.
 * The new momentum is returned.
 *
 * The calculations underlying this method are contained in the
 *   Tech-X notes, "Scattered Velocity Calculation for XOOPIC,"
 *   by David Bruhwiler (August 31, 2000).
 */
Vector3 MCCPackage::newMomentum(const Vector3 U_initial,
                                const Scalar& newEnergy,
             										const Scalar& theta,
                                const Scalar& phi) {

  Scalar u10 = U_initial.e1();
  Scalar u20 = U_initial.e2();
  Scalar u30 = U_initial.e3();
  Scalar u0  = sqrt( u10*u10 + u20*u20 + u30*u30 );

  Scalar cosT0 = u30 / u0;
  Scalar sinT0 = sqrt( 1. - cosT0*cosT0 );

  Scalar cosPhi0;
  Scalar root = sqrt(u0*u0-u30*u30);
  if ( root <= fabs(u10) ) cosPhi0 = 1;
  else cosPhi0 = u10 / root;

  Scalar sinPhi0 = sin( acos( cosPhi0 ) );

  Scalar cosT = cos(theta);
  Scalar sinT = sin(theta);

  Scalar cosPhi = cos(phi);
  Scalar sinPhi = sin(phi);

  Scalar gMin1 = newEnergy / ELECTRON_MASS_EV;
  Scalar u0new = SPEED_OF_LIGHT * sqrt( (gMin1+2) * gMin1 );  // =beta*gamma*c

  Scalar u10new = u0new * ( cosPhi*sinT*cosPhi0*cosT0
                          - sinPhi*sinT*sinPhi0
													+        cosT*cosPhi0*sinT0 );

  Scalar u20new = u0new * ( cosPhi*sinT*sinPhi0*cosT0
                          + sinPhi*sinT*cosPhi0
													+        cosT*sinPhi0*sinT0 );

  Scalar u30new = u0new * (-cosPhi*sinT*sinT0 + cosT*cosT0);

  Vector3 uNew(u10new,u20new,u30new);
  return uNew;
}



void MCCPackage::newVelocity(Scalar energy, Scalar v, Vector3& u, int eFlag)
{
	Scalar	up1, up2, up3;
	Scalar	coschi;
	Scalar	sinchi;
	if (energy < 1e-30) coschi = 1.0;
	else coschi = 1 + 2.0*(1.0 - pow(energy + 1.0, frand()))/energy;
	sinchi = sqrt(1 - coschi*coschi);
	Scalar	phi = TWOPI*frand();
	Scalar	cosphi = cos(phi);
	Scalar	sinphi = sin(phi);

	// transfer momentum:

//	if (eFlag) v *= sqrt(1 - 2*eSpecies->get_m()/iSpecies->get_m()*(1 - coschi));
	// doesn't work when m/M>.5

	// johnv: does this get called for ion collisions?
	Scalar me = eSpecies->get_m();
	Scalar mi = iSpecies->get_m();
	v*= sqrt(1 - 2*me*mi/(me+mi)/(me+mi)*(1-coschi));

	Scalar	r13 = u.e1();
	Scalar	r23 = u.e2();
	Scalar	r33 = u.e3();

	if (r33 == 1)	{up1 = 0; up2 = 1; up3 = 0;}
	else				{up1 = 0; up2 = 0; up3 = 1;}

	Scalar	r12 = r23*up3 - r33*up2;
	Scalar	r22 = r33*up1 - r13*up3;
	Scalar	r32 = r13*up2 -r23*up1;
	Scalar	mag = sqrt(r12*r12 + r22*r22 + r32*r32);
  
	r12 /= mag;
	r22 /= mag;
	r32 /= mag;
  
	Scalar	r11 = r22*r33 - r32*r23;
	Scalar	r21 = r32*r13 - r12*r33;
	Scalar	r31 = r12*r23 - r22*r13;

	u.set_e1(v*(r11*sinchi*cosphi + r12*sinchi*sinphi + r13*coschi));
	u.set_e2(v*(r21*sinchi*cosphi + r22*sinchi*sinphi + r23*coschi)); 
	u.set_e3(v*(r31*sinchi*cosphi + r32*sinchi*sinphi + r33*coschi));
}


//-------------------------------------------------------------
// ArMCC: argon MCC package

ArMCC::ArMCC(Scalar p, Scalar temp, Species* eSpecies, Species* iSpecies,
      SpeciesList& sList, Scalar dt, int ionzFraction, Scalar ecxFactor,
      Scalar icxFactor, SpatialRegion* _SR,
      Scalar _Min1MKS, Scalar _Max1MKS, Scalar _Min2MKS, Scalar _Max2MKS,
      Scalar _delayTime, Scalar _stopTime, const ostring &analyticF, 
      const int& discardDumpFileNGDDataFlag)
 : MCCPackage(Ar, (ostring) "Ar", p, temp, eSpecies, iSpecies, sList, dt, ionzFraction,
      ecxFactor, icxFactor, _SR, _Min1MKS, _Max1MKS, _Min2MKS, _Max2MKS,
      _delayTime, _stopTime, analyticF, discardDumpFileNGDDataFlag)
{
  addCrossSections();
  init(sList, dt);
}

void ArMCC::addCrossSections()
{
  neCX = 3; // electron model
  eCX = new CrossSection*[neCX];
  eCX[0] = new FuncCrossSection(sigmaElastic, 18, 0.0, CrossSection::ELASTIC);
  eCX[1] = new FuncCrossSection(sigmaExc, 18, 12.0, CrossSection::EXCITATION);
  eCX[2] = new FuncCrossSection(sigmaIz, 18, 15.76, CrossSection::IONIZATION);
  // eCX[0] = new eCrossSection(0.3, 15, 20, 1.2e-19, CrossSection::ELASTIC);
  //  eCX[1] = new eCrossSection(11.55, 30, 100, 7e-21, CrossSection::EXCITATION);
  //	ionizationCX = new eCrossSection(15.76, 30, 100, 3e-20, IONIZATION);


  niCX = 2; // ion model
  iCX = new CrossSection*[niCX];
  
  // The original cross-sections.  Cause problems at lower energies. 
  // Commented out by: Jeff Hammel, 11/4/02
  //  iCX[0] = new iCrossSection(1.8e-19, 4e-19, CrossSection::ION_ELASTIC);
  //  iCX[1] = new iCrossSection(2e-19, 5.5e-19, CrossSection::CHARGE_EXCHANGE);
  
  iCX[0] = new FuncCrossSection(sigmaIelastic, 18, 0.0, CrossSection::ION_ELASTIC);
  iCX[1] = new FuncCrossSection(sigmaCX, 18, 0.0, CrossSection::CHARGE_EXCHANGE);
}

Scalar ArMCC::sigmaElastic(Scalar energy)
{
    
  if(energy < 1.0) 
    {
      if(energy < 0.2) { return 1./pow(10.0, 19.0 +energy/.11); }
      else 
	{ 
	  return 9.07e-19*pow(energy, 1.55)*pow(energy+70.0, 1.10)/pow(14.+energy, 3.25); 
	}
    }
  else 
    {
      return 9.07e-19*pow(energy, 1.55)*pow(energy+70.0, 1.10)/pow(14.+energy, 3.25);
    }

}

Scalar ArMCC::sigmaExc(Scalar energy)
{
 
  if(energy < 12.0) { return 0.; }  
  return (3.85116e-19*log(energy/3.4015) -4.85227e-19)/energy;
}

Scalar ArMCC::sigmaCX(Scalar energy)
{
  if(energy > 4.0) return(2.0e-19 +5.5e-19/sqrt(energy));
  return(-2.95e-19*sqrt(energy) +10.65e-19);
}
Scalar ArMCC::sigmaIelastic(Scalar energy)
{
  if(energy > 4.0) return(1.8e-19 +4.0e-19/sqrt(energy));
  return(-2.0e-19*sqrt(energy) +7.8e-19);
}


//-------------------------------------------------------------------
// Argon ionization cross section (Rapp & Golden) - energies in eV

Scalar ArMCC::sigmaIz(Scalar energy)
{
   if (energy < 15.76) return 0;
	else if (energy < 79) 
	  return 1.7155e-18*(energy-15.76)/(energy*energy)*log(0.0634*energy);
	else return 2.648e-18*(energy-15.76)/(energy*energy)*log(0.0344*energy);
}

//-------------------------------------------------------------
// NeMCC: neon MCC package

NeMCC::NeMCC(Scalar p, Scalar temp, Species* eSpecies, Species* iSpecies, 
     SpeciesList& sList, Scalar dt, int ionzFraction, Scalar ecxFactor,
     Scalar icxFactor, SpatialRegion* _SR,
     Scalar _Min1MKS, Scalar _Max1MKS, Scalar _Min2MKS, Scalar _Max2MKS,
     Scalar _delayTime, Scalar _stopTime, const ostring &analyticF, 
     const int& discardDumpFileNGDDataFlag)
 : MCCPackage(Ne, (ostring)"Ne", p, temp, eSpecies, iSpecies, sList, dt, ionzFraction,
     ecxFactor, icxFactor, _SR, _Min1MKS, _Max1MKS, _Min2MKS, _Max2MKS,
     _delayTime, _stopTime, analyticF, discardDumpFileNGDDataFlag)
{
  addCrossSections();
  init(sList, dt);
}

void NeMCC::addCrossSections()
{
  neCX = 3; // electron model
  eCX = new CrossSection*[neCX];
  eCX[0] = new FuncCrossSection(sigmaElastic, 10, 0.,  CrossSection::ELASTIC);
  eCX[1] = new FuncCrossSection(sigmaExc, 10, 15.8489, CrossSection::EXCITATION);
  eCX[2] = new FuncCrossSection(sigmaIz, 10,  20.893,  CrossSection::IONIZATION);

  niCX = 1; // ion model
  iCX = new CrossSection*[niCX];
  iCX[0] = new iCrossSection(0, 5.98e-20, CrossSection::CHARGE_EXCHANGE);
}

Scalar NeMCC::sigmaElastic(Scalar energy)
{
  if (energy < 2.63027) return 1.27808e-20 + energy*(4.22329e-21 - 7.31854e-22*energy);
  else if (energy < 19.4984) return 1.66333e-20 + energy*(8.66972e-22 - 1.14153e-23*energy);
  else if (energy < 33.8844) return 2.2297e-20 + energy*(5.8066e-22 - 1.18023e-23*energy);
  else if (energy < 190.546) return 3.862e-20 + energy*(-3.19062e-22 + 8.00046e-25*energy);
  else if (energy < 501.187) return 1.37254e-20 + energy*(-4.66403e-23 + 4.6479e-26*energy);
  else if (energy < 1000) return 4.38798e-21 + energy*(-6.19168e-24 + 2.5747e-27*energy);
  else return 0;
}

Scalar NeMCC::sigmaExc(Scalar energy) // summed excitations
{
  if (energy <= 15.8489) return 0;
  else if (energy < 23.9883) return -2.37959e-22 + energy*(-5.87101e-23 + 4.6417e-24*energy);
  else if (energy < 36.3078) return -3.49005e-21 + energy*(2.76934e-22 - 3.69893e-24*energy);
  else if (energy < 54.9541) return 2.75471e-22 + energy*(6.0912e-23 - 5.99808e-25*energy);
  else if (energy < 251.189) return 2.271e-21 + energy*(-8.78552e-24 + 1.41951e-26*energy);
  else if (energy < 616.595) return 1.53078e-21 + energy*(-2.7892e-24 + 1.90737e-27*energy);
  else if (energy < 1000) return 1.05134e-21 + energy*(-1.07925e-24 + 3.83108e-28*energy);
  else return 0;
}

Scalar NeMCC::sigmaIz(Scalar energy)
{
  if (energy <= 20.893) return 0;
  else if (energy < 54.9541) return -2.34018e-21 + energy*1.10755e-22;
  else if (energy < 125.893) return -2.4556e-21 + energy*(1.42218e-22 - 5.37266e-25*energy);
  else if (energy < 190.546) return 2.62391e-21 + energy*(5.39463e-23 - 1.54319e-25*energy);
  else if (energy < 436.516) return 8.04001e-21 + energy*(-2.68853e-24 - 6.11372e-27*energy);
  else if (energy < 1000) return 9.16045e-21 + energy*(-9.59021e-24 + 3.85096e-27*energy);
  else return 0;
}

//-------------------------------------------------------------
// XeMCC: xenon MCC package

XeMCC::XeMCC(Scalar p, Scalar temp, Species* eSpecies, Species* iSpecies, 
     SpeciesList& sList, Scalar dt, int ionzFraction, Scalar ecxFactor,
     Scalar icxFactor, SpatialRegion* _SR,
     Scalar _Min1MKS, Scalar _Max1MKS, Scalar _Min2MKS, Scalar _Max2MKS,
     Scalar _delayTime, Scalar _stopTime, const ostring &analyticF,
     const int& discardDumpFileNGDDataFlag)
 : MCCPackage(Xe, (ostring)"Xe", p, temp, eSpecies, iSpecies, sList, dt, ionzFraction,
     ecxFactor, icxFactor, _SR, _Min1MKS, _Max1MKS, _Min2MKS, _Max2MKS,
     _delayTime, _stopTime, analyticF, discardDumpFileNGDDataFlag)
{
  addCrossSections();
  init(sList, dt);
}

void XeMCC::addCrossSections()
{
  neCX = 3; // electron model
  eCX = new CrossSection*[neCX];
  eCX[0] = new FuncCrossSection(sigmaElastic, 54, 0.,  CrossSection::ELASTIC);
  eCX[1] = new FuncCrossSection(sigmaExc, 54, 7.94328, CrossSection::EXCITATION);
  eCX[2] = new FuncCrossSection(sigmaIz,  54, 11.2202, CrossSection::IONIZATION);

  niCX = 1; // ion model
  iCX = new CrossSection*[niCX];
  iCX[0] = new iCrossSection(0, 4.541e-20, CrossSection::CHARGE_EXCHANGE);
}

Scalar XeMCC::sigmaElastic(Scalar energy)
{
  if (energy <= 1) return 0;
  else if (energy < 3.46737) return -2.56426e-20 + energy*(3.05131e-20 + 1.01401e-20*energy);
  else if (energy < 4.2658) return -7.02412e-19 + energy*(4.15012e-19 - 4.46826e-20*energy);
  else if (energy < 5.62341) return -3.14744e-19 + energy*(2.19764e-19 - 2.027176e-20*energy);
  else if (energy < 12.0226) return 2.23972e-19 + energy*(2.61272e-20 - 2.81077e-21*energy);
  else if (energy < 25.704) return 3.39014e-19 + energy*(-2.28038e-20 + 4.64864e-22*energy);
  else if (energy < 72.4436) return 9.50404e-20 + energy*(-1.80585e-21 + 1.24365e-23*energy);
  else if (energy < 190.546) return 4.51567e-20 + energy*(-2.78976e-22 + 6.93189e-25*energy);
  else if (energy < 537.032) return 2.59891e-20 + energy*(-5.72321e-23 + 4.79915e-26*energy);
  else if (energy < 1000) return 1.46698e-20 + energy*(-1.28135e-23 + 3.98769e-27*energy);
  else return 0;
}

Scalar XeMCC::sigmaExc(Scalar energy) // summed excitations
{
  if (energy <= 7.94328) return 0;
  else if (energy < 11.2202) return 7.71935e-20 + energy*(-1.8566e-20 + 1.117e-21*energy);
  else if (energy < 20.893) return -5.72625e-20 + energy*(8.29355e-21 - 2.02235e-22*energy);
  else if (energy < 33.8844) return 9.4784e-21 + energy*(1.36322e-21 - 2.20758e-23*energy);
  else if (energy < 190.546) return 3.66375e-20 + energy*(-1.87228e-22 + 3.94988e-25*energy);
  else if (energy < 467.735) return 2.51242e-20 + energy*(-6.45713e-23 + 6.14246e-26*energy);
  else if (energy < 1000) return 1.4849e-20 + energy*(-1.78916e-23 + 7.74732e-27*energy);
  else return 0;
}

Scalar XeMCC::sigmaIz(Scalar energy)
{
  if (energy <= 11.2202) return 0;
  else if (energy < 44.6684) return -4.78153e-20 + energy*(4.75275e-21 - 5.83101e-23*energy);
  else if (energy < 67.6083) return 4.90931e-20 + energy*(-6.99912e-23 + 1.11671e-24*energy);
  else if (energy < 102.329) return 3.73779e-20 + energy*(3.25673e-22 - 2.15897e-24*energy);
  else if (energy < 204.174) return 6.20189e-20 + energy*(-1.62295e-22 + 2.70937e-25*energy);
  else if (energy < 575.44) return 4.1e-20 - 3.49995e-24*energy;
  else if (energy < 812.831) return 2.19858e-20 + energy*(9.27581e-23 - 1.09139e-25*energy);
  else if (energy < 1000) return 8.37604e-20 + energy*(-1.01201e-22 + 3.62937e-26*energy);
  else return 0;
}

//-------------------------------------------------------------
// HMCC:  Hydrogen MCC package

HMCC::HMCC(Scalar p, Scalar temp, Species* eSpecies, Species* iSpecies, 
     SpeciesList& sList, Scalar dt, int ionzFraction, Scalar ecxFactor,
     Scalar icxFactor, SpatialRegion* _SR,
     Scalar _Min1MKS, Scalar _Max1MKS, Scalar _Min2MKS, Scalar _Max2MKS,
     Scalar _delayTime, Scalar _stopTime, const ostring &analyticF, 
     const int& discardDumpFileNGDDataFlag)
 : MCCPackage(H, (ostring)"H", p, temp, eSpecies, iSpecies, sList, dt, ionzFraction,
     ecxFactor, icxFactor, _SR, _Min1MKS, _Max1MKS, _Min2MKS, _Max2MKS,
     _delayTime, _stopTime, analyticF, discardDumpFileNGDDataFlag)
{
  addCrossSections();
  init(sList, dt);
}

void HMCC::addCrossSections()
{
  neCX = 1; // electron model
  eCX = new CrossSection*[neCX];
  eCX[0] =  new FuncCrossSection(sigmaIz, 1, 13.6, CrossSection::IONIZATION);

}

Scalar HMCC::sigmaIz(Scalar energy)
{
  if (energy <= 13.6) return 0;
  else if (energy <= 17.789) return 8.208063e-21*log(energy)-2.071614e-20;
  else if (energy <= 53.6999) return 3.751578e-21*log(energy)-7.887766e-21;
  else return (2.996231e-19*log(energy)-8.145988e-19)/energy;
}
//-------------------------------------------------------------------

//-------------------------------------------------------------
// HeMCC: Helium MCC package    -- added 10/24/02 by Hammel;
// electron cross-sections from pdp1, 
// ion cross sections from McDaniel et al, 1993  (curve fit)

HeMCC::HeMCC(Scalar p, Scalar temp, Species* eSpecies, Species* iSpecies, 
     SpeciesList& sList, Scalar dt, int ionzFraction, Scalar ecxFactor,
     Scalar icxFactor, SpatialRegion* _SR,
     Scalar _Min1MKS, Scalar _Max1MKS, Scalar _Min2MKS, Scalar _Max2MKS,
     Scalar _delayTime, Scalar _stopTime, const ostring &analyticF, 
     const int& discardDumpFileNGDDataFlag)
 : MCCPackage(H, (ostring)"He", p, temp, eSpecies, iSpecies, sList, dt, ionzFraction,
     ecxFactor, icxFactor, _SR, _Min1MKS, _Max1MKS, _Min2MKS, _Max2MKS,
     _delayTime, _stopTime, analyticF, discardDumpFileNGDDataFlag)
{
  addCrossSections();
  init(sList, dt);
}

void HeMCC::addCrossSections()
{
  neCX = 3; // electron model
  eCX = new CrossSection*[neCX];
  eCX[0] =  new FuncCrossSection(sigmaElastic, 2, 0.0, CrossSection::ELASTIC);
  eCX[1] =  new FuncCrossSection(sigmaExc, 2, 19.8, CrossSection::EXCITATION);
  eCX[2] =  new FuncCrossSection(sigmaIz, 2, 24.5, CrossSection::IONIZATION);

  niCX = 2; // ion model
  iCX = new CrossSection*[niCX];
  iCX[0] = new FuncCrossSection(sigmaIelastic, 2, 0.0, CrossSection::ION_ELASTIC);
  iCX[1] = new FuncCrossSection(sigmaCX, 2, 0.0, CrossSection::CHARGE_EXCHANGE);
}

Scalar HeMCC::sigmaElastic(Scalar energy)
{
  return (8.5e-19/(pow(energy+10.0, 1.1)));
}

Scalar HeMCC::sigmaExc(Scalar energy)
{
  const Scalar extengy0 = 19.8;
  if(energy < extengy0) return (0.0);
  else if(extengy0 <= energy && energy <27.0)
    return (2.08e-22*(energy -extengy0));
  return (3.4e-19/(energy +200));
}

Scalar HeMCC::sigmaIz(Scalar energy)
{
  const Scalar ionengy0 = 24.5;
  if(energy < ionengy0) return (0.0);
	return(1e-17*(energy -ionengy0)/((energy +50)*pow(energy+300.0, 1.2)));
}

Scalar HeMCC::sigmaIelastic(Scalar energy)
{
  Scalar crossx;
  const Scalar emin = 0.01;
  if(energy < emin) { energy = emin; }

  crossx = 3.6463e-19/sqrt(energy) - 7.9897e-21;
  //  fprintf(stderr, "\nenergy=%g, crossx=%g", energy, crossx);
  if(crossx < 0) { return 0.0; }
  return crossx;
}

Scalar HeMCC::sigmaCX(Scalar energy)
{
  Scalar crossx;
  const Scalar emin = 0.01;
  const Scalar cutoff = 377.8;

  if(energy < emin) { energy = emin; }
  if(energy < cutoff)
    {
      crossx = 1.2996e-19  - 7.8872e-23*energy + 1.9873e-19/sqrt(energy);
    }
  else
    {
      crossx = (1.5554e-18*log(energy)/energy) + 8.6407e-20;
    }
  return crossx;
}

//-------------------------------------------------------------------

//-------------------------------------------------------------
// LiMCC: Lithium MCC package    -- added 03/02/00 by Bruhwiler

LiMCC::LiMCC(Scalar p, Scalar temp, Species* eSpecies, Species* iSpecies, 
             SpeciesList& sList, Scalar dt, int ionzFraction,
             Scalar ecxFactor, Scalar icxFactor, int relativisticFlag,
             SpatialRegion* _SR, Scalar _Min1MKS, Scalar _Max1MKS,
             Scalar _Min2MKS, Scalar _Max2MKS,
             Scalar _delayTime, Scalar _stopTime, const ostring &analyticF,
             const int& discardDumpFileNGDDataFlag)
           : MCCPackage(Li, (ostring)"Li", p, temp, eSpecies, iSpecies, sList, dt,
                        ionzFraction, ecxFactor, icxFactor, _SR,
                        _Min1MKS, _Max1MKS, _Min2MKS, _Max2MKS,
                        _delayTime, _stopTime, analyticF, discardDumpFileNGDDataFlag) {

  relativisticMCC = relativisticFlag;
  staticRelMCC    = relativisticFlag;
  addCrossSections();
  init(sList, dt);
}


// Define static data members for the LiMCC class.
  Scalar LiMCC::threshold     = 5.392;
  Scalar LiMCC::youngerParamA =  1.35e-17;
  Scalar LiMCC::youngerParamB = -6.51e-18;
  Scalar LiMCC::youngerParamC =  5.99e-19;
  Scalar LiMCC::youngerParamD = -8.49e-18;
  Scalar LiMCC::fitParamA1    = 4.9785e-20;
  Scalar LiMCC::fitParamA2    = 2.0862e-20;
  Scalar LiMCC::fitParamA3    = 6.0173e-20;
  Scalar LiMCC::fitParamA4    = 1.462;
  Scalar LiMCC::fitParamN     = 0.439;
  int    LiMCC::staticRelMCC  = 1;

//
void LiMCC::addCrossSections() {
    // Specify number of electron collision models that are implemented --
  	//   at present, we have impact ionization and elastic scattering.
  neCX = 2;
    // create an array of CrossSection pointers
  eCX = new CrossSection*[neCX];
    // load up the array of CrossSection pointers
  eCX[1] = new FuncCrossSection(sigmaElastic,  3,  0., CrossSection::ELASTIC);
  eCX[0] = new FuncCrossSection(sigmaIz, 3, threshold, CrossSection::IONIZATION);
}

/* Method that calculates the elastic scattering cross-section
 * in m^2 as a function of the electron impact energy in eV.
 *
 * Cross section is a fit to LLNL's EEDL calculations at high
 *   energy and, for low energy, to calculations/data from
 *   Bray, Fursa and McCarthy, Phys. Rev. A 47, 1101 (1993).
 *
 * Units are eV for energy and m^2 for cross-sections.
 * NOTE: units are NOT cm^2 and NOT barns!
 */
Scalar LiMCC::sigmaElastic(Scalar energy)
{
  Scalar sigmaMin = 6.8e-24;
  Scalar a1 = 1.9e-6;
  Scalar a2 = 8.1e-3;
  Scalar a3 = 2.4e-4;

  Scalar gMin1 = energy / ELECTRON_MASS_EV;
  Scalar betaSq = (gMin1+2.)*gMin1/(gMin1+1.)/(gMin1+1.);

  Scalar crossSection;
  crossSection  = sigmaMin * (a1 + a2*betaSq + betaSq*betaSq);
  crossSection /= pow(a3+betaSq, 3.);

	//  cout << "energy sigmaElastic = " << energy << "; " << crossSection << endl;
  return crossSection;
}


/*  Method for calculating the e-impact ionization cross section
 *  for lithium.
 *
 *  The nonrelativistic model uses the fitting function from Eq. 4
 *  of the 1981 J.Q.S.R.T. paper by S.M. Younger.
 *
 *  The relativistic model uses a parametric function adapted from
 *  Younger, but modified to capture the relativistic rise.  The
 *  parameters were determined by least squares fit to a) Younger's
 *  low-energy values and b) the computational results of the LLNL
 *  report on the EEDL (electron evauated data library), by Perkins,
 *  Cullen and Seltzer (1991).
 *
 *  Units are eV for energy and m^2 for cross sections.
 *  NOTE:  cross section is NOT in cm^2 and NOT in barns!
 *
 *  First added March 2, 2000, by David Bruhwiler
 */
Scalar LiMCC::sigmaIz(Scalar energy) {

  	// This is the value that gets returned.
  Scalar crossSection;

    // normalized impact energy
    // -- assumed that both energy and threshold are in eV
  Scalar U = energy / threshold;
  if ( U <= 1. ) return 0.;

  	// This is needed for both relativistic and NR models
  Scalar oneMinusUinv = 1. - 1. / U;

  // nonrelativistic model from Younger's paper
  if (staticRelMCC == 0) {
    Scalar logU = log(U);
    crossSection = youngerParamA * oneMinusUinv
                 - youngerParamB * oneMinusUinv * oneMinusUinv
                 + youngerParamC * logU
                 - youngerParamD * logU / U;
    crossSection /= (energy * threshold);
	}

    // relativistic model -- fit to Younger and also EEDL calc.'s
  else {
    Scalar gMin1 = energy / ELECTRON_MASS_EV;
    Scalar gamma = gMin1 + 1.;
    Scalar beta  = sqrt((gamma+1.)*gMin1)/gamma;

    Scalar oneMinusUinvSq = 1.-1./(U*U);

    // "nonrelativistic" energy, scaled to threshold
    Scalar Unr = 0.5 * ELECTRON_MASS_EV * beta * beta / threshold;
    Scalar logUnr = log(Unr);

    // Now we apply the empirical function to actually calculate
    //   the cross section:
    crossSection = fitParamA1*oneMinusUinv/U
                 + fitParamA2*logUnr/(U*U)
                 + fitParamA3*logUnr/Unr
                 + fitParamA3*oneMinusUinvSq
                   * ( log( fitParamA4*pow(gamma,fitParamN) ) / Unr
 									 - 2.*threshold/ELECTRON_MASS_EV );

		//    cout << "energy sigmaIz = " << energy << "; " << crossSection << endl;
  }

	// Return the calculated cross section (units = m^2)
  return crossSection;
}



//-------------------------------------------------------------
// NMCC: Nitrogen MCC package  -- added 09/03/2000 by Bruhwiler

NMCC::NMCC(Scalar p, Scalar temp, Species* eSpecies, Species* iSpecies, 
           SpeciesList& sList, Scalar dt, int ionzFraction,
           Scalar ecxFactor, Scalar icxFactor, int relativisticFlag,
           SpatialRegion* _SR, Scalar _Min1MKS, Scalar _Max1MKS,
           Scalar _Min2MKS, Scalar _Max2MKS,
           Scalar _delayTime, Scalar _stopTime, const ostring &analyticF,
           const int& discardDumpFileNGDDataFlag)
          : MCCPackage(N, (ostring)"N", p, temp, eSpecies, iSpecies, sList, dt,
                       ionzFraction, ecxFactor, icxFactor, _SR,
                       _Min1MKS, _Max1MKS, _Min2MKS, _Max2MKS,
                       _delayTime, _stopTime, analyticF, discardDumpFileNGDDataFlag) {

  relativisticMCC = relativisticFlag;
  staticRelMCC    = relativisticFlag;
  addCrossSections();
  init(sList, dt);
}


// Define static data members for the LiMCC class.
  Scalar NMCC::threshold     = 11.48;
  Scalar NMCC::fitParamA1    = -1.6112e-19;
  Scalar NMCC::fitParamA2    = -3.6835e-19;
  Scalar NMCC::fitParamA3    =  8.7133e-20;
  Scalar NMCC::fitParamA4    = 15.;
  Scalar NMCC::fitParamN     = 0.28;
  int    NMCC::staticRelMCC  = 1;

//
void NMCC::addCrossSections() {
    // Specify number of electron collision models that are implemented --
  	//   at present, we have impact ionization and elastic scattering.
  neCX = 2;
    // create an array of CrossSection pointers
  eCX = new CrossSection*[neCX];
    // load up the array of CrossSection pointers
  eCX[1] = new FuncCrossSection(sigmaElastic,  7,  0., CrossSection::ELASTIC);
  eCX[0] = new FuncCrossSection(sigmaIz, 7, threshold, CrossSection::IONIZATION);
}

/* Method that calculates the elastic scattering cross-section
 * in m^2 as a function of the electron impact energy in eV.
 *
 * Cross section is a fit to LLNL's EEDL calculations at high
 *   energy and, for low energy, to calculations/data from
 *   Bray, Fursa and McCarthy, Phys. Rev. A 47, 1101 (1993).
 *
 * Units are eV for energy and m^2 for cross-sections.
 * NOTE: units are NOT cm^2 and NOT barns!
 */
Scalar NMCC::sigmaElastic(Scalar energy)
{
  Scalar sigmaMin = 2.035e-23;
  Scalar u1 = 1.759e-8;
  Scalar u2 = 6.460e-5;
  Scalar l1 = 8.021e-5;

  Scalar gMin1 = energy / ELECTRON_MASS_EV;
  Scalar betaSq = (gMin1+2.)*gMin1/(gMin1+1.)/(gMin1+1.);

  Scalar crossSection;
  crossSection  = sigmaMin * (u1 + u2*betaSq + betaSq*betaSq);
  crossSection /= pow(l1+betaSq, 3.);

	//  cout << "energy sigmaElastic = " << energy << "; " << crossSection << endl;
  return crossSection;
}


/*  Method for calculating the e-impact ionization cross section
 *  for nitrogen.
 *
 *  This relativistic model uses a parametric function adapted from
 *  Younger, but modified to capture the relativistic rise.  The
 *  parameters were determined by least squares fit to the computational
 *  results of the LLNL report on the EEDL (electron evauated data
 *  library), by Perkins, Cullen and Seltzer (1991).
 *
 *  Units are eV for energy and m^2 for cross sections.
 *  NOTE:  cross section is NOT in cm^2 and NOT in barns!
 *
 *  First added September 3, 2000, by David Bruhwiler
 */
Scalar NMCC::sigmaIz(Scalar energy) {

  	// This is the value that gets returned.
  Scalar crossSection;

    // normalized impact energy
    // -- assumed that both energy and threshold are in eV
  Scalar U = energy / threshold;
  if ( U <= 1. ) return 0.;

  Scalar gMin1 = energy / ELECTRON_MASS_EV;
  Scalar gamma = gMin1 + 1.;
  Scalar beta  = sqrt((gamma+1.)*gMin1)/gamma;

  Scalar oneMinusUinv = 1. - 1. / U;
  Scalar oneMinusUinvSq = 1.-1./(U*U);

  // "nonrelativistic" energy, scaled to threshold
  Scalar Unr = 0.5 * ELECTRON_MASS_EV * beta * beta / threshold;
  Scalar logUnr = log(Unr);

  // Now we apply the empirical function to actually calculate
  //   the cross section:
  crossSection = fitParamA1*oneMinusUinv/U
               + fitParamA2*logUnr/(U*U)
               + fitParamA3*logUnr/Unr
               + fitParamA3*oneMinusUinvSq
                 * ( log( fitParamA4*pow(gamma,fitParamN) ) / Unr
								 - 2.*threshold/ELECTRON_MASS_EV );

	//  cout << "energy sigmaIz = " << energy << "; " << crossSection << endl;

	// Return the calculated cross section (units = m^2)
  return crossSection;
}


//
void MCCPackage::BoltzmannInit()
{
  
  const int NumberBins=100;
  //	const int IS; //IntegrationSteps
  const Scalar Tmin = .05;  // this needs to match min temp in boltzman.cpp
  const Scalar Tmax = 10;  // This is hopefull the hightest temperture.
  const Scalar EnergyStep=.5;
  
  Scalar T;
  G = new Scalar[NumberBins];
  E = new Scalar[NumberBins];
  for (int n=0; n<NumberBins; n++){
    G[n] = 0;
    E[n] = 0;
  }
  
  Scalar temp = 0;
  
  for (int i=0; i<neCX; i++)
    switch (eCX[i]->get_type())
      {
      case CrossSection::IONIZATION: {
        for (int n=0; n<NumberBins; n++){
          T = n*(Tmax-Tmin)/(NumberBins-1)+Tmin;
          for (Scalar energy=EnergyStep/2; energy<MAX_ENERGY; energy+=EnergyStep){
            if (energy/T<50)
              temp += energy*exp(-energy/T)*eCX[i]->sigma(energy);
          }
          temp *= gasDensity*sqrt(2/(T*M_PI*eSpecies->get_m()))*2/T;
          G[n] += temp;
          E[n] -= temp*eCX[i]->get_threshold();
        }
        break;
      }
      case CrossSection::ELASTIC:
        {
          for (int n=0; n<NumberBins; n++){
            T = n*(Tmax-Tmin)/(NumberBins-1)+Tmin;
            for (Scalar energy=EnergyStep/2; energy<MAX_ENERGY; energy+=EnergyStep){
              if (energy/T<50)
                temp += energy*exp(-energy/T)*eCX[i]->sigma(energy);
            }
            temp *= gasDensity*sqrt(2/(T*M_PI*eSpecies->get_m()))*2/T;
            E[n] -= temp*eCX[i]->get_threshold();
          }
          break;
        }
      case CrossSection::EXCITATION:
        {
          for (int n=0; n<NumberBins; n++){
            T = n*(Tmax-Tmin)/(NumberBins-1)+Tmin;
            for (Scalar energy=EnergyStep/2; energy<MAX_ENERGY; energy+=EnergyStep){
              if (energy/T<50)
                temp += energy*exp(-energy/T)*eCX[i]->sigma(energy);
            }
            temp *= gasDensity*sqrt(2/(T*M_PI*eSpecies->get_m()))*2/T;
            E[n] -= temp*eCX[i]->get_threshold();
          }
          break;
        }
      default:
        printf("unrecognized collision type!\n");
      }
  
}     

MCTIPackage::MCTIPackage(GAS_TYPE _type, const ostring& strGasType, 
                         Scalar pressure, Scalar _temp,
                         Species* _eSpecies, Species* _iSpecies, SpeciesList& sList,
                         Scalar dt, int _ionzFraction, Scalar _ecxFactor, Scalar _icxFactor,
                         SpatialRegion* _SR, Scalar _Min1MKS, Scalar _Max1MKS,
                         Scalar _Min2MKS, Scalar _Max2MKS, Scalar _delayTime, Scalar _stopTime, 
                         const ostring& _analyticF, const int& numMacroParticlesPerCell, 
                         Scalar argEfieldFrequency, const int& discardDumpFileNGDDataFlag) /* throw(Oops) */
  : BaseMCPackage( _type, strGasType, _temp, _eSpecies, _iSpecies, pressure, _SR, 
                   _Min1MKS, _Max1MKS, _Min2MKS, _Max2MKS, _analyticF,
                   discardDumpFileNGDDataFlag)
{
  /**
   * initiate the call to allocate memory and initiallize 
   * the TI_np2c matrix in the NGD class.
   */
  try{
    ptrNGD->initNGD_TI_np2c(numMacroParticlesPerCell, grid);
  }
  catch(Oops& oops3){
    oops3.prepend("MCTIPackage::MCTIPackage: Error: \n");
    throw oops3;
  }
  /**
   * make the call to allocate memory for the excessNumIons
   * in NGD and set the values to the cell components for
   * the excessNumIons to the default value of zero.
   */
  ptrNGD->initNGD_excessNumIons();
  ptrNGD->setisTIOn();
  /**
   * Set the value of the external alternating E field frequency.
   * If the value was not specified in the input file, print a
   * warning and set the value to Scalar_EPSILON
   */
  if ( argEfieldFrequency < 0. ) {
    //
    // no value was given in the input file... set to w_{CO_2}
    // 
    std::cout << std::endl << std::endl << std::endl 
         << "WARNING: The value of the E field frequency was not " << std::endl
         << "specified in the input file. Setting the value to " << std::endl
         << "Scalar_EPSILON = " << Scalar_EPSILON << std::endl
         << "Make sure that this is what you want...!?" << std::endl
         << std::endl << std::endl << std::endl;
    EfieldFrequency = Scalar_EPSILON;
  } else {
    EfieldFrequency = argEfieldFrequency;
  }
  std::cout << "MCTIPackage::EfieldFrequency = " << EfieldFrequency << std::endl;
}
 
// define const data members for MCTIPackage
/************************ begin less precise values *************************
const double MCTIPackage::H_BAR                   = 1.0544e-27; // [ergs*s]
const double MCTIPackage::E_CHARGE                = 4.8e-10;     // [esu]
const double MCTIPackage::E_MASS                  = 9.10956e-28; // [g]
const double MCTIPackage::ATOMIC_TIME_UNIT        = pow(H_BAR, 3)/(E_MASS * pow(E_CHARGE, 4));          // [s]
const double MCTIPackage::ATOMIC_LENGTH_UNIT      = pow(H_BAR, 2)/(E_MASS * pow(E_CHARGE, 2));       // [cm]
const double MCTIPackage::ATOMIC_E_FIELD_UNIT     = pow(E_MASS, 2) * pow(E_CHARGE, 5)/pow(H_BAR, 4); // [Statvolt/cm]
const double MCTIPackage::ATOMIC_E_FIELD_UNIT_MKS = ATOMIC_E_FIELD_UNIT * 3.0e4;                // [V/m]
const double MCTIPackage::ATOMIC_ENERGY_UNIT      = E_MASS * pow(E_CHARGE, 4) / pow(H_BAR, 2);       // [erg]
const double MCTIPackage::ATOMIC_ENERGY_UNIT_EV   = ATOMIC_ENERGY_UNIT * 6.24196e11;              // [eVs]
*************************** end less precise values *************************/

/************ begin values used in IONPACK taken from the NIST site **********/
const double SPEED_OF_LIGHT_CGS                   = 2.99792458e10; // [cm / s] 
const double ELECTRON_CHARGE_MKSA                 = 1.602176462e-19; // [C]
const double MCTIPackage::H_BAR                   = 1.054751596e-27; // [ergs*s]
const double MCTIPackage::E_CHARGE                = (ELECTRON_CHARGE_MKSA*SPEED_OF_LIGHT_CGS/10.);     // [statcoulombs]
const double MCTIPackage::E_MASS                  = 9.10938188e-28;   // [g]
const double MCTIPackage::ATOMIC_TIME_UNIT        = 2.4188843265e-17; // [s]
const double MCTIPackage::ATOMIC_E_FIELD_UNIT_MKS = 5.14220624e+11; // [V/m]
const double MCTIPackage::ATOMIC_ENERGY_UNIT      = 4.35974381e-11; // [erg]
const double MCTIPackage::ATOMIC_ENERGY_UNIT_EV   = ATOMIC_ENERGY_UNIT*6.24150974e11;              // [eVs]
/************ end values used in IONPACK taken from the NIST site **********/
/**
 * A function to calculate the electric field energy
 * stored in a cell with lower left corner indexed
 * by j,k. The energy is calculated integrating over
 * the cell volume the square of the electric field.
 * The field is interpolated from the at the nodes  
 * using the formula in:
 * Grid::interpolateBilinear(Vector3** A, const Vector2& x)
 */
Scalar MCTIPackage::getCellEfieldEnergy(const int& j, const int& k) const {
  Scalar cellEnergy; // for the return value
  Scalar c9  = 0.5/9.0; 
  Scalar c18 = 0.5/18.0;
  Vector3** ptrENode;// pointer to the E field matrix on the grid 
  // variables for the E field at grid nodes:
  Vector3 Ejk;       // j  , k
  Vector3 Ej1k;      // j+1, k
  Vector3 Ejk1;      // j  , k+1
  Vector3 Ej1k1;     // j+1, k+1

  ptrENode = region->get_fields()->getENode();
  Ejk   = ptrENode[j][k];
  Ej1k  = ptrENode[j+1][k];
  Ejk1  = ptrENode[j][k+1];
  Ej1k1 = ptrENode[j+1][k+1];
  
  cellEnergy = c9*(Ejk*Ejk+Ej1k*Ej1k+Ejk1*Ejk1+Ej1k1*Ej1k1+
                   Ejk*Ejk1+Ejk*Ej1k+Ejk1*Ej1k+Ej1k*Ej1k1)+
               c18*(Ejk*Ej1k1+Ejk1*Ej1k);
  cellEnergy *= region->get_fields()->get_epsi(j,k);
  cellEnergy *= grid->cellVolume(j, k);
  // convert to EV
  cellEnergy *= 6.24196e18;
  
  return cellEnergy;
}
   
HMCTI::HMCTI(Scalar p, Scalar temp, Species* eSpecies, Species* iSpecies,  
             SpeciesList& sList, Scalar dt, int ionzFraction, Scalar ecxFactor,
             Scalar icxFactor, SpatialRegion* _SR,
             Scalar _Min1MKS, Scalar _Max1MKS, Scalar _Min2MKS, Scalar _Max2MKS,
             Scalar _delayTime, Scalar _stopTime, const ostring &analyticF,
             const int& numMacroParticlesPerCell, 
             const int& ETIPolarizationFlag, Scalar argEfieldFrequency,
             const int& discardDumpFileNGDDataFlag) /* throw(Oops) */
  : MCTIPackage(H, (ostring)"H", p, temp, eSpecies, iSpecies, sList, dt, ionzFraction,
                ecxFactor, icxFactor, _SR, _Min1MKS, _Max1MKS, _Min2MKS, _Max2MKS,
                _delayTime, _stopTime, analyticF, numMacroParticlesPerCell, 
                argEfieldFrequency, discardDumpFileNGDDataFlag) 
 {

  /***********************************************************************************
  cout << "Atomic Units" << endl << endl << endl;
  cout << "ATOMIC_TIME_UNIT        = " << ATOMIC_TIME_UNIT << " [s]" << endl;
  cout << "ATOMIC_LENGTH_UNIT      = " << ATOMIC_LENGTH_UNIT << " [cm]" << endl;
  cout << "ATOMIC_E_FIELD_UNIT_MKS = " << ATOMIC_E_FIELD_UNIT_MKS << " [V/m]" << endl;
  cout << "ATOMIC_ENERGY_UNIT      = " << ATOMIC_ENERGY_UNIT << " [ergs]" << endl;
  cout << "ATOMIC_ENERGY_UNIT_EV   = " << ATOMIC_ENERGY_UNIT_EV << " [eV]" << endl;
  ***********************************************************************************/

  Scalar tmp_dt = region->get_dt()/ATOMIC_TIME_UNIT;

  if ( ETIPolarizationFlag == 0 ) { // linear polarization
    fp = &HMCTI::getLinearFieldProbabilityTI;
  } else if ( ETIPolarizationFlag == 1 ) { // circular polarization
    fp = &HMCTI::getCircularFieldProbabilityTI;
  } else { // unrecognized value for the ETIPolarizationFlag
    stringstream ss (stringstream::in | stringstream::out);
    ss << "HMCTI::HMCTI: Error: \n"<<
      "Unrecognized value ETIPolarizationFlag = " << ETIPolarizationFlag 
         << endl << "There are currently defined valid values of 0 and 1" 
         << endl << "Check your input paramter for ETIPolarizationFlag.";
         
    
    std::string msg;
    ss >> msg;
    Oops oops(msg);
    throw oops;    // exit()MCCParams::CreateCounterPart

  }
  //
  // set the max field as the value at the inflection point of the 
  // probability rate function. The field is in atomic units.
  // 
  EmaxTI = 2.0/3.0; 

  //
  // Check if the probability at EmaxTI and the time step of the simulation
  // is greater than one. If yes, print a warning. 
  // 
  if ( ((this->*fp)(EmaxTI, tmp_dt)) < 1.0 ) 
    std::cout << std::endl << std::endl << "WARNING: The probability at the max E field: " << std::endl 
         << "P(Emax = " << EmaxTI << ", dt = " << tmp_dt << ") = " 
         << (this->*fp)(EmaxTI, tmp_dt) << std::endl 
         << " is less than 1. The time step of the simulations is probably" << std::endl
         << "out of the regime of applicability of the tunneling ionization"
         << "theory!?" << std::endl << std::endl;
  
  /**
   * Set the value of the E field (in atomic units) below which 
   * the Keldish condition is violated. The probability for 
   * tunneling ionization for fields smaller than this value
   * will be set to zero. 
   * This value is given by w*sqrt(2*Ei) where w is the frequency
   * of the field, Ei is the energy of the bound state, both
   * quantities are in atomic units. Note that for Hydgrogen in
   * the ground state 2*Ei = 1. Therefore: 
   */
  EminTI = EfieldFrequency*ATOMIC_TIME_UNIT;
  std::cout << "EminTI = " << EminTI << std::endl;
  std::cout << "P(EminTI, dt) = " << (this->*fp)(EminTI, tmp_dt) << std::endl;

}

void HMCTI::tunnelIonize(ParticleGroupList** pgList, ParticleList& pList) 
  /* throw(Oops) */{
  /**
   * loop over all cells of the grid and check for nonzero gas density
   * on each cell. If the tunneling ionization eoes not run fast enough, 
   * one way to speed it up could be to keep a list of the cells that have 
   * nonzero gas density and loop only over them. 
   * 
   * The J,K that we loop over are extracted from the Grid and should be
   * the same J,K that are data members of both the BaseMCPackage and NGD 
   * classes. 
   */
  
  int j,k;
  Vector2 x;      // temporary vector variable for positions on the grid
  Vector3 u;      // temporary vector variables for velocities of 
                  // created particles. The default value is 0. The effect of 
                  // temperature on the gas, that may lead to nonzero initial 
                  // velocities of created ions is not considered yet. 
  Vector3 Ecc;    // vector for the electric field at a grid cell center
  Scalar E;       // holds the magnitude of the electric field in atomic units
  Scalar dt;      // time interval during which to calculate the probability 
                  // for tunneling ionization
  Scalar cellVol; // volume of a cell on a grid
  Scalar numCellNeutralAtoms; // number of neutral atoms in a cell 
  int numMacroParticles;      // temporary varible for the number of macro
                              // particles to be created
  Scalar numIons; // temporary variable for the number of ions to create
  Scalar TI_np2c; // temporary variable to hold the number of physical ions
                  // in a macro parcticle for a given cell 
  Scalar expectedNumIons;  // temporary variable for the expected number of 
                           // atoms to become ions
  Scalar excessNumIons;    // temporary variable for the excess number of
                           // ions of a given grid cell
  static bool IonizationEnergyWarningFlag = false;

  /** 
   * set the default value for the velocity of created ion 
   * and electron particles
   */
  u.set_e1( 0.0 );
  u.set_e2( 0.0 );
  u.set_e3( 0.0 );
   
  for ( j = 0; j < J; j++ ) {
    for ( k = 0; k < K; k++ ) {


      cellVol =  grid->cellVolume(j, k);
      try{
        numCellNeutralAtoms =  (ptrNGD->getNGD(j, k)) * cellVol;  
      }
      catch(Oops& oops2){
        oops2.prepend("MCCPackage::tunnelIonize: Error: \n");//done
        throw oops2;
      }
      /**
       * proceed with the creation of macro particles of ions only
       * if the remaining number of neutral atoms is greater
       * than zero
       */
      if ( numCellNeutralAtoms < 1.0 )
        continue;
        
      /**
       * calculate the magnitude of the electric field at the 
       * cell's center in atomic units of field
       */
      x.set_e1( static_cast<Scalar>(j) + 0.5 );
      x.set_e2( static_cast<Scalar>(k) + 0.5 );
      Ecc = (region->get_fields())->E(x); 
      E   = Ecc.magnitude()/ATOMIC_E_FIELD_UNIT_MKS; 
      
      /**
       * calculate the time step in atomic units of time
       */
      dt  = region->get_dt() / ATOMIC_TIME_UNIT;
      
      /**
       * get the exptected number of ions to be created via 
       * the tunneling ionization. 
       *
       * Note that the probability function for ionization 
       * is coded specifically for tunneling from the ground state 
       * of Hydrogen. 
       */
      try{
        expectedNumIons = numCellNeutralAtoms * TIProbability(fp, E, dt); 
      }
      catch(Oops& oops2){
        oops2.prepend("HMCTI::tunnelIonize: Error: \n");//done
        throw oops2;
      }

      
      //
      // is the energy of ionization bigger than 0.1% of the field energy
      // if yes print a warning but just once
      // 
      if ( !IonizationEnergyWarningFlag ) {
        if ( expectedNumIons > 1.0 ) 
          if ( expectedNumIons*0.5*ATOMIC_ENERGY_UNIT_EV/
               getCellEfieldEnergy(j, k) > 0.001 ) { 
            std::cerr << "WARNING: Detected Ionization Energy larger " << std::endl
                 << "than 0.1 % of the cell electric field energy!!!"
                 << std::endl;
            IonizationEnergyWarningFlag = true;
          }
      }
      /**
       * reduce the numCellNeutralAtoms by the expected number
       * of atoms to become ions and update the neutral gas
       * density for the cell
       */
      Scalar stmp = (numCellNeutralAtoms - expectedNumIons)/cellVol; 
      if ( stmp < 0.0 ) {
        std::cerr << "numCellNeutralAtoms - expectedNumIons = " 
             <<  numCellNeutralAtoms - expectedNumIons << std::endl
             << "is negative. This should never happen." << std::endl;
        stmp = 0.;
        expectedNumIons = numCellNeutralAtoms;
      }
      ptrNGD->set(j, k,  stmp);
      
      /** 
       * add to the excess number of ions for the (j, k)th cell
       */
      try{
        ptrNGD->add_to_NGD_excessNumIons(j, k, expectedNumIons);
        
        /**
        * create macro particles with ions only if 
        * the current's cell excessNumIons > TI_np2c
        */
        TI_np2c = ptrNGD->getNGD_TI_np2c(j,k);
        excessNumIons = ptrNGD->getNGD_excessNumIons(j, k);
      }
      catch(Oops& oops){
        oops.prepend("HMCTI::tunnelIonize: Error: \n"); //MCTI::tunnelIonize
        throw oops;
      }

      /**
       * create the macro particles with ions only if 
       * the current excessNumIons is greater than
       * the number of physical ions per macro particle
       */
      if ( excessNumIons > TI_np2c ) {
        /** 
         * calculate the number of macro particles to create
         */
        numMacroParticles = static_cast<int>(excessNumIons/TI_np2c);
        /**
         * reset the excessNumIons[j][k] data member of class NGD
         */
        numIons = TI_np2c * (static_cast<Scalar>(numMacroParticles));
        excessNumIons -= numIons;
        try{
          ptrNGD->setNGD_excessNumIons(j, k, excessNumIons);
        }
        catch(Oops& oops){
          oops.prepend("HMCTI::tunnelIonize: Error: \n"); //OK
          throw oops;
        }
        /**
         * create the macro particles with 0 velocities and positions
         * picked randomly on the cell (in grid coordinates)
         */
        int MPcounter;
        for ( MPcounter = 0; MPcounter < numMacroParticles; MPcounter++ ) {
          /** 
           * create the particles for both ions and electrons
           * with random positions in the cell 
           * and 0 velocities. The number of electrons is equal to
           * the number of ions for hydrogen. 
           */ 
          x.set_e1( static_cast<Scalar>(j) + frand() );
          x.set_e2( static_cast<Scalar>(k) + frand() );
          
          // create electron macro particles
          pList.add(new Particle( x, u, eSpecies, TI_np2c, true ) );
          
          // create ion macro particles
          pList.add(new Particle( x, u, iSpecies, TI_np2c, true ) );
        }
        continue;
      }
      // If all neutral atoms have been ionized and we reach this point,
      // then we must create the final macro-particle for this cell.
      // Otherwise, we should continue with the for-loop.
      if ( (stmp*cellVol < 1.0) && (excessNumIons > 1.0) ) {
        // ionize the rest of the neutral atoms and place them in 
        // a single macro particle.
        x.set_e1( static_cast<Scalar>(j) + frand() );
        x.set_e2( static_cast<Scalar>(k) + frand() );
        
        // create electron macro particles
        pList.add(new Particle( x, u, eSpecies, excessNumIons, true ) );
        
        // create ion macro particles
        pList.add(new Particle( x, u, iSpecies, excessNumIons, true ) );

        stmp = 0.0;
        ptrNGD->set(j, k,  stmp);
        excessNumIons = 0;
        try{
          ptrNGD->setNGD_excessNumIons(j, k, excessNumIons);
        }
        catch(Oops& oops){
          oops.prepend("HMCTI::tunnelIonize: Error: \n");//OK
          throw oops;
        }
      } 
    }
  }
  //
  // distribute the created particles during the tunneling ionization
  // in the appropriate particle group lists according to species
  // 
  region->addParticleList(pList);
}

/**
 * a helper function to calculate the probability for tunneling 
 * ionization of Hydrogen ions from the ground state
 */
Scalar HMCTI::TIProbability(Scalar (HMCTI::*P)(Scalar&, Scalar&) const,
                            Scalar& E, Scalar& dt) {
  Scalar Probability = 0.0;
  
  if ( E > EmaxTI ) { 
    //
    // if the field is greater than the max E field determined
    // from the inflection formula for the probability rate
    // then set the probability to 1. 
    // 
    Probability = 1.0;
  } else if ( E > EminTI ) {
    //
    // if E > EminTI (as determined from Keldish' condition)
    // and is less than EmaxTI then use the TI formula to
    // calculate the probability for ionization
    // 
    Probability = (this->*P)(E, dt);
    if ( Probability > 1.0 )
      Probability = 1.0;
  } else {
    // 
    // this is the default case: for E < EminTI
    // the probability for TI is negligible, so set it to zero
    // 
    Probability = 0.0;
  }
  return Probability; 
}

