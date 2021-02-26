#include <fstream>
#include <cstdio>

#include "HeMCTI.h"
#include "species.h"
#include "particle.h"
#include "ptclgrp.h"
#include "misc.h"
using namespace std;

int const HeMCTI::Ztot = 2; // charge of the nucleus (He)

HeMCTI::HeMCTI(Scalar p, Scalar temp, Species* eSpecies, Species* iSpecies, 
               Species* _iSpeciesPlusPlus, 
               SpeciesList& sList, Scalar dt, int ionzFraction, Scalar ecxFactor,
               Scalar icxFactor, SpatialRegion* _SR,
               Scalar _Min1MKS, Scalar _Max1MKS, Scalar _Min2MKS, Scalar _Max2MKS,
               Scalar _delayTime, Scalar _stopTime, const ostring &analyticF,
               const int& numMacroParticlesPerCell, 
               const int& ETIPolarizationFlag, Scalar argEfieldFrequency,
               const int& discardDumpFileNGDDataFlag) /* throw(Oops) */
  : MCTIPackage(He, (ostring)"He", p, temp, eSpecies, iSpecies, sList, dt, ionzFraction,
                ecxFactor, icxFactor, _SR, _Min1MKS, _Max1MKS, _Min2MKS, _Max2MKS,
                _delayTime, _stopTime, analyticF, numMacroParticlesPerCell, 
                argEfieldFrequency, discardDumpFileNGDDataFlag)
                { 
  cout << endl << endl << "Creating HeMCTI" << endl;
  iSpeciesPlusPlus = _iSpeciesPlusPlus;
  nStar = new Scalar[Ztot]; // effective principal quantum #
  Zti   = new int[Ztot];    // charge of the atomic residue after each ti
  Ei    = new Scalar[Ztot]; // electron bound level ground state energies in au

  // 
  // a constant factor for each bound state's
  // tunneling ionization probability rate
  // for linearly polarized E field  
  // 
  wLinearFactor = new Scalar[Ztot];

  // 
  // a constant factor for each bound state's    
  // tunneling ionization probability rate       
  // for circularly polarized and static E fields                         
  // 
  wCircularFactor = new Scalar[Ztot];                         

  //
  // memory for the max and min values of the external E field with
  // respect to the applicability of the AKD theory for
  // tunneling ionization 
  // 
  EmaxTI = new Scalar[Ztot];
  EminTI = new Scalar[Ztot];
 
  cout << "ATOMIC_ENERGY_UNIT_EV = " << ATOMIC_ENERGY_UNIT_EV << endl;
  Ei[0]  = 24.481/ATOMIC_ENERGY_UNIT_EV;
  Ei[1]  = 54.403/ATOMIC_ENERGY_UNIT_EV;
  Scalar E0;

  //
  // set the function pointer to the proper probability function
  // depending on the polarization of the electric field
  // 
  if ( ETIPolarizationFlag == 0 ) { // linear polarization
    fp = &HeMCTI::getLinearFieldProbabilityTI;
  } else if ( ETIPolarizationFlag == 1 ) { // circular polarization
    fp = &HeMCTI::getCircularFieldProbabilityTI;
  } else { // unrecognized value for the ETIPolarizationFlag
      stringstream ss (stringstream::in | stringstream::out);
      ss << "HeMCTI::HeMCTI: Error: \n"<<
          "Unrecognized value ETIPolarizationFlag = " << 
          ETIPolarizationFlag 
          << endl << 
          "There are currently defined valid values of 0 and 1" 
          << endl << 
          "Check your input paramter for ETIPolarizationFlag.";
           
      
      std::string msg;
      ss >> msg;
      Oops oops(msg);
      throw oops;    // exit()MCCParams::CreateCounterPart

  }
  Scalar tmp_dt = region->get_dt()/ATOMIC_TIME_UNIT;

  for ( int i = 0; i < Ztot; i++ ) {
    Zti[i]   = i + 1;
    nStar[i] = get_nStar(Ei[i], Zti[i]);
    E0 = pow((Scalar)(2.0*Ei[i]), (Scalar)1.5);
    wCircularFactor[i]  = pow((Scalar)4.0, (Scalar)nStar[i])*Ei[i]/nStar[i];
    wCircularFactor[i] /= sqrt(2.*M_PI)*exp(-2.0*nStar[i])*pow((Scalar)(2.0*nStar[i]), (Scalar)(2.0*nStar[i]-0.5));
    wCircularFactor[i] *= pow((Scalar)(2.0*E0), (Scalar)(2.0*nStar[i]-1.0));
    wLinearFactor[i]  = wCircularFactor[i]*sqrt(3.0/(M_PI*E0));

    //
    // set the max field as the value at the inflection point of the 
    // probability rate function. 
    // 
    if ( (2.0*nStar[i]-1.) > 1. )
      EmaxTI[i] = 2.0*E0/(3.0*(2.0*nStar[i]-1.)); 
    else 
      EmaxTI[i] = 2.0*E0/(3.0*(2.0*nStar[i]-1.));

    //
    // Check if the probability at EmaxTI and the time step of the simulation
    // is greater than one. If yes, print a warning. 
    // 
    Scalar temp = 0.0;
    try {
      temp = (this->*fp)(EmaxTI[i], tmp_dt, Zti[i]-1);
    }
    catch(Oops& oops){
      oops.prepend("HeMCTI::HeMCTI: Error:\n"); //OK CreateCounterPart.
      throw oops;
    }

    if ( temp < 1.0 ) 
      cout << endl << endl << "WARNING: The probability at the max E field: " << endl 
           << "P(Emax[" << i << "] = " << EmaxTI[i] << ", dt = " << tmp_dt << ") = " 
           << temp << endl 
           << " is less than 1. The time step of the simulations is probably" << endl
           << "out of the regime of applicability of the tunneling ionization"
           << "theory!?" << endl << endl;

    cout << "Ei[" << i << "] = " << Ei[i] << endl
         << "Zti[" << i << "] = " << Zti[i] << endl
         << "nStar[" << i << "] = " << nStar[i] << endl
         << "E0 = " << E0 << endl << endl;

    cout << "P(Emax[" << i << "] = " << EmaxTI[i] << ", dt = " << tmp_dt << ") = " 
         << temp << endl << endl;

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
  EminTI[i] = EfieldFrequency*ATOMIC_TIME_UNIT*sqrt(2.*Ei[i]);
  cout << "EminTI[" << i << "] = " << EminTI[i] << endl;
  cout << "P(EminTI[" << i << "] = " << EminTI[i] << ", dt = " << tmp_dt 
       << ") = " << temp << endl;    
  }
}

HeMCTI::~HeMCTI() {
  if (nStar)
    delete [] nStar;
  if (Zti)
    delete [] Zti;
  if (Ei)
    delete [] Ei;
  if (wLinearFactor)
    delete [] wLinearFactor;
  if (wCircularFactor)
    delete [] wCircularFactor;
  if (EmaxTI)
    delete [] EmaxTI;
  if (EminTI)
    delete [] EminTI;
}
/**
 * This member function manages the tunneling ionization of the
 * He atoms and He+ ions. 
 */
void HeMCTI::tunnelIonize(ParticleGroupList** pgList, ParticleList& pList) 
  /* throw(Oops) */{
  // 
  // attempt to tunnel ionize first He atoms for the current values of the 
  // E field: 
  // 
    try{
      tunnelIonizeHe(pList);
    }
    catch(Oops& oops2){
      oops2.prepend("HeMCTI::tunnelIonize: Error: \n");//done
      throw oops2;
    }
  
  // 
  // find the ParticleGroupList for the HePlus species (ions) and then
  // attempt to tunnel ionize the particles in this list: 
  // 
  SpeciesList* speciesList = region->getSpeciesList();
  oopicListIter<Species> siter(*speciesList);
  for (siter.restart(); !siter.Done(); siter++) {
    Species* species = siter.current();
    int i = species->getID();
    //
    // we kept a pointer to the iSpecies in the BaseMCPackage class
    // and it points to the singly ionized atoms of this type, i.e.
    // He. 
    //
    if ( i == iSpecies->getID() ) {
      // 
      // now attempt to tunnel ionize the HePlus ions
      // 
      try{
        tunnelIonizeHePlus(*pgList[i], pList);
      }
      catch(Oops& oops3){
        oops3.prepend("HeMCTI::tunnelIonize: Error: \n"); //done
        throw oops3;
      }
      break;
    }
  }
}

/**
 * A helper function used for the tunneling ionization of the 
 * He atoms.
 */
void HeMCTI::tunnelIonizeHe(ParticleList& pList) /* throw(Oops) */ {
  /**
   * loop over all cells of the grid and check for nonzero gas density
   * on each cell. If the tunneling ionization eoes not run fast enough, 
   * one way to speed it up could be to keep a list of the cells that have 
   * nonzero gas density and loop only over them. 
   * 
   * The J, K that we loop over are extracted from the Grid and should be
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
        oops2.prepend("HeMCTI::tunnelIonizeHe: Error: \n");//done
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
        expectedNumIons = numCellNeutralAtoms * TIProbability(fp, E, dt, Zti[0]-1); 
      }
      catch(Oops& oops2){
        oops2.prepend("HeMCTI::tunnelIonizeHe: Error: \n");//OK
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
            cerr << "WARNING: Detected Ionization Energy larger " << endl
                 << "than 0.1 % of the cell electric field energy!!!"
                 << endl;
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
        cerr << "numCellNeutralAtoms - expectedNumIons = " 
             <<  numCellNeutralAtoms - expectedNumIons << endl
             << "is negative. This should never happen." << endl;
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
        oops.prepend("HeMCTI::tunnelIonizeHe: Error: \n"); //HeMCTI::tunnelIonize
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
          oops.prepend("HeMCTI::tunnelIonizeHe: Error: \n"); //OK
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
          oops.prepend("HeMCTI::tunnelIonizeHe: Error: \n"); //OK
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
 * A helper function used for the tunneling ionization of the 
 * He+ ions.
 */
void HeMCTI::tunnelIonizeHePlus(ParticleGroupList& pgList, ParticleList& pList) 
  /* throw(Oops) */{
  //
  // check first if the ParticleGroupList is empty, if yes => return
  // 
  if (pgList.isEmpty()) 
    return;

  int n; // counter
  Scalar EfieldMagnitude;
  Scalar Probability;
  Scalar thisParticle_np2c;
  Vector2 thisParticle_x;
  Vector3 thisParticle_u;  
  /**
   * calculate the time step in atomic units of time
   */
  Scalar timeStep  = region->get_dt() / ATOMIC_TIME_UNIT;
  
  // 
  // loop over the particle group list
  // 
  oopicListIter<ParticleGroup> pgIter(pgList);
  for (pgIter.restart(); !pgIter.Done(); pgIter++) {
    //
    // loop over the particles in each particle group
    //
    int numParticles = pgIter()->get_n(); // number of particles in the group
    n = 0; 
    // 
    // need to keep track if the group becomes empty to
    // schedule its deletion
    // 
    BOOL IsParticleGroupEmpty; 
    while ( n < numParticles ) {
      thisParticle_x    = pgIter()->get_x(n);
      EfieldMagnitude = ((region->get_fields())->E(thisParticle_x)).magnitude()
        /ATOMIC_E_FIELD_UNIT_MKS;
      try{
        Probability = TIProbability(fp, EfieldMagnitude, timeStep, Zti[1]-1); 
      }
      catch(Oops& oops2){
        oops2.prepend("HeMCTI::tunnelIonizeHePlus: Error: \n"); //done
        throw oops2;
      }

      //
      // generate a "quasi" random number QRN and ionize the macroparticle if
      // QRN < Probability:
      // 
      if ( frand() < Probability ) {
        //
        // ionize the particle; note that in this case n is not incremented
        // since the last particle in the group is placed at the index n 
        // of the particle that we are going to ionize and delete from this
        // group
        // 
        thisParticle_np2c = pgIter()->get_np2c(n);
        thisParticle_u    = pgIter()->get_u(n);
        //
        // create electron macro particle
        //
        pList.add(new Particle( thisParticle_x, thisParticle_u, 
                                eSpecies, thisParticle_np2c, true ) );
        //
        // create HePlusPlus ion macro particle
        // 
        pList.add(new Particle( thisParticle_x, thisParticle_u, 
                                iSpeciesPlusPlus, thisParticle_np2c, true ) );
        //
        // delete the old macro particle and keep track if the group is empty
        //
        IsParticleGroupEmpty = pgIter()->remove(n);
        numParticles--;
      } else {
        // 
        // this particle was not ionized => increment the "n" counter to
        // move to the next macro particle in the group
        // 
        n++;
      }
    }
  }
}

/**
 * a helper function to calculate the probability for tunneling 
 * ionization of Hydrogen ions from the ground state
 */
Scalar HeMCTI::TIProbability(Scalar (HeMCTI::*P)(Scalar&, Scalar&, int) const,
                            Scalar& E, Scalar& dt, int Zindex)
  /* throw(Oops) */{
  Scalar Probability = 0.0;
  
  if ( E > EmaxTI[Zindex] ) { 
    //
    // if the field is greater than the max E field determined
    // from the inflection formula for the probability rate
    // then set the probability to 1. 
    // 
    Probability = 1.0;
  } else if ( E > EminTI[Zindex] ) {
    //
    // if E > EminTI (as determined from Keldish' condition)
    // and is less than EmaxTI then use the TI formula to
    // calculate the probability for ionization
    //
    try{
      Probability = (this->*P)(E, dt, Zindex);
    }
    catch(Oops& oops){
      oops.prepend("HeMCTI::TIProbability: Error:\n"); //HeMCTI::tunnelIonizeHePlus  HMCTI::tunnelIonize
      throw oops;
    }
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

