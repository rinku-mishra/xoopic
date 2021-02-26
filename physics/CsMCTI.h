/** 
 * Cs Monte Carlo Tunneling Ionization (CsMCTI) class.
 * Specializes the MCTIPackage class for the simulations 
 * of tunneling ionization of Cs under the influence
 * of linearly/circularly polarized alternating electric field.
 * 
 * This class implements the formula for the probability
 * rate for tunneling ionization from ground states, see e.g.: 
 * N. B. Delone and V. P. Krainov, "Multiphonon Processes
 * in Atoms", Springer (2000). 
 * This formula is expected to be applicable only for electric 
 * field magnitudes: E << E_atomic = Z/(16n^4). 
 * The adiabatic approximation condition for tunneling is:
 * omega << E_i and 
 * gamma^2 << 1, where gamma = omega*sqrt(2*E_i)/E,
 * omega is the frequency of the alternating electric 
 * field with magnitude E, and E_i is the magnitude 
 * of the bound state energy from which the electron is
 * to tunnel.
 *
 * First added January, 2003.
 *
 * @author D. A. Dimitrov
 */
#ifndef CSMCTI_H
#define CSMCTI_H

#include "mcc.h"
#include <oops.h>

class CsMCTI : public MCTIPackage {
 public:
  CsMCTI(Scalar p, Scalar temp, Species* eSpecies, Species* iSpecies, 
         Species* _iSpeciesPlusPlus,
         SpeciesList& sList, Scalar dt, int ionzFraction, Scalar ecxFactor,
         Scalar icxFactor, SpatialRegion* _SR,
         Scalar _Min1MKS, Scalar _Max1MKS, Scalar _Min2MKS, Scalar _Max2MKS,
         Scalar _delayTime, Scalar _stopTime, const ostring &analyticF, 
         const int& numMacroParticlesPerCell, 
         const int& ETIPolarizationFlag, Scalar argEfieldFrequency,
         const int& discardDumpFileNGDDataFlag) /* throw(Oops) */;
  ~CsMCTI();

  /**
   * This member function manages the tunneling ionization of the
   * Cs atoms and Cs+ ions. 
   */
  void tunnelIonize(ParticleGroupList** pgList, ParticleList& pList) /* throw(Oops) */;

 private:
  /**
   * A helper function used for the tunneling ionization of the 
   * Cs atoms.
   */
  void tunnelIonizeCs(ParticleList& pList) /* throw(Oops) */;

  /** 
   * A helper function used for the tunneling ionization of the 
   * Cs+ ions.
   */
  void tunnelIonizeCsPlus(ParticleGroupList& pgList, ParticleList& pList) /* throw(Oops) */;

  //
  // pointer to the species that represent Cs++ ions
  // 
  Species* iSpeciesPlusPlus; 

  static int const Ztot;   // charge of the nucleus 
  Scalar* nStar;           // effective principal quantum #
  int* Zti;                // charge of the atomic residue after each ti
  Scalar* Ei;              // electron bound level ground state energies
  Scalar* E0;              // a constant factor
  Scalar* wLinearFactor;   // a constant factor for each bound state's
                           // tunneling ionization probability rate
                           // for linearly polarized E field
  Scalar* wCircularFactor; // a constant factor for each bound state's
                           // tunneling ionization probability rate
                           // for circularly polarized and static E fields
  /**
   * limiting value of the atomic field above for which the 
   * probability for tunneling ionization becomes equal to
   * unity for a given value of the time step.
   */
  Scalar* EmaxTI;

  /**
   * Value of the E field below which the Keldish condition
   * is violated. The probability for tunneling ionization 
   * for fields smaller than this value will be set to zero. 
   */
  Scalar* EminTI;

  // do not allow copy and assignment
  CsMCTI(const CsMCTI&);
  CsMCTI& operator=(const CsMCTI&);
  
  /**
   * a helper function to calculate the probability for tunneling 
   * ionization of Hydrogen ions from the ground state
   */
  Scalar TIProbability(Scalar (CsMCTI::*P)(Scalar&, Scalar&, int) const, 
                       Scalar& E, Scalar& dt, int Zindex) /* throw(Oops) */; 
 
  /**
   * A helper function to calculate the circular E field
   * probability for the magnitude E of the field and time
   * step dt (both in atomic units). 
   */
  Scalar getCircularFieldProbabilityTI(Scalar& E, Scalar& dt, 
                                       int Zindex) const 
    /* throw(Oops) */{
    if ( Zindex == 0 ) {
      return (wCircularFactor[Zindex]*pow(E, (Scalar)(1.0-2.0*nStar[Zindex]))/
              exp(E0[Zindex]/(1.5*E))*dt);
    } else if ( Zindex == 1 ) { 
      return (wCircularFactor[Zindex]*pow(E, (Scalar)(2.0-2.0*nStar[Zindex]))/
              exp(E0[Zindex]/(1.5*E))*dt);
    } else {
      stringstream ss (stringstream::in | stringstream::out);
      ss << "CsMCTI::getCircularFieldProbabilityTI: Error: \n"<<
        "Unrecognized Zindex value of " << Zindex  << std::endl;
         
      std::string msg;
      ss >> msg;
      Oops oops(msg);
      throw oops;    // exit()  CsMCTI::CsMCTI   LiMCTI::LiMCTI   HeMCTI::HeMCTI

    }
  }

  /**
   * A helper function to calculate the linear polarized alternating E field
   * probability for E magnitude of the field and time
   * step dt (both in atomic units).
   */
  Scalar getLinearFieldProbabilityTI(Scalar& E, Scalar& dt, 
                                     int Zindex) const 
    /* throw(Oops) */{
    if ( Zindex == 0 ) {
      return (wLinearFactor[Zindex]*pow(E, (Scalar)(1.5-2.0*nStar[Zindex]))/
              exp(E0[Zindex]/(1.5*E))*dt);
    } else if ( Zindex == 1 ) { 
      return (wLinearFactor[Zindex]*pow(E, (Scalar)(2.0-2.0*nStar[Zindex]))/
              exp(E0[Zindex]/(1.5*E))*dt);
    } else {
      stringstream ss (stringstream::in | stringstream::out);
      ss << "CsMCTI::getLinearFieldProbabilityTI: Error: \n"<<
        "Unrecognized Zindex value of " << Zindex  << std::endl;
           
      std::string msg;
      ss >> msg;
      Oops oops(msg);
      throw oops;    // exit()CsMCTI::CsMCTI   LiMCTI::LiMCTI   HeMCTI::HeMCTI  CsMCTI,LiMCTI,HeMCTI::TIProbability

    }
  }
  /**
   * A pointer to the function that will be used to calculate the
   * tunneling ionization probabilities. For now this pointer 
   * will point to getCircluarFieldProbabilityTI(Scalar& E, Scalar& dt)
   * or to getLinearFieldProbabilityTI(Scalar& E, Scalar& dt) to
   * cover the two cases (circularly or linearly polarized alternating E
   * field) of TI from the ground state of Hydrogen.
   */
  Scalar (CsMCTI::*fp)(Scalar&, Scalar&, int) const;

  /**
   * A helper function to calculate the values if the effective
   * principal quantum number nStar using the Hydrogen-like
   * relation with the bound state energy and the charge Z, 
   * both in atomic units. 
   */
  Scalar get_nStar(Scalar E, Scalar Z) const { return (Z/pow(2.0*E, 0.5)); }
};
#endif /* CSMCTI_H */ 
