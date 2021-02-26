/*
=====================================================================
mcc.h

Describes the MCC (Monte Carlo collision) class.  This class provides 
collisions by operating on ParticleGroups.

0.9	 (JohnV 02-06-95) Initial draft.
1.1  (JohnV 12-12-96) Added Rapp&Golden Argon ionization cross section
2.5  (Bruhwiler 03-01-00) Added electron impact ionization cross section for Li
2.51 (Bruhwiler 08-29-00) added relativisticMCC flag and modified the Li
        ionization cross section to include relativistic effects
2.52 (Bruhwiler 09-03-00) added relativistic elastic scattering for
        Li -- this required changes in CrossSection and FuncCrossSection
2.53 (Bruhwiler 09-26-00) added capability to limit MCC region (i.e.
        limit the neutral gas) to a subregion of the grid.
2.54 (Bruhwiler 10-04-00) added capability to bound MCC algorithm in time.

=====================================================================
*/

#ifndef	__MCC_H
#define	__MCC_H

#include "ovector.h"
#include "misc.h"
#include "particle.h"
#include "sptlrgn.h"
#include "ngd.h"

#include <oops.h>

 
class Maxwellian;
class	Species;

class CrossSection
{
  private:

  void newVelocity(Scalar energy, Scalar v, Vector3& u, int eFlag);

  public:

    enum Type { ELASTIC, EXCITATION, IONIZATION, ION_ELASTIC,
                CHARGE_EXCHANGE, TEST_TO_BOLTZMANN } type;

	/** Return the collision cross section.
	 *  Pure virtual -- must be implemented in derived classes.
	 */
  virtual Scalar sigma(Scalar energy)=0;

	/** return inelastic threshold energy
	 */
  virtual Scalar get_threshold() const {return 0.;}

	/** return atomic number
	 */
	virtual int getAtomicNumber() const {return 1;}

	/** return collision type
	 */
  Type get_type() {return type;}
  virtual ~CrossSection() {};
};

class eCrossSection : public CrossSection
{
	Scalar	energy0;						//	(eV) threshold
	Scalar	energy1;						//	(eV) start of plateau
	Scalar	energy2;						//	(eV) begin logE/E drop 
	Scalar	sigmaMax;						//	peak cross section m^2
	Scalar	const1;							//	internal constant for energy range 1
	Scalar	const3;							//	internal constant for energy range 3
public:
	eCrossSection(Scalar e0, Scalar e1, Scalar e2, Scalar sMax, Type type);
	virtual Scalar sigma(Scalar energy);
	//Scalar	get_energy0() {return energy0;}
	virtual Scalar get_threshold() const {return energy0;}
};

class FuncCrossSection : public CrossSection
{
protected:
   Scalar threshold;
   int atomicNumber;
   Scalar (*sigmaPtr)(Scalar energy);
public:
	 FuncCrossSection(Scalar (*sigma)(Scalar), int _Z,
                    Scalar _threshold, Type _type) {
	   sigmaPtr     = sigma;
     atomicNumber = _Z;
     threshold    = _threshold;
     type         = _type;
}
	virtual Scalar sigma(Scalar energy) {return (*sigmaPtr)(energy);}
	virtual Scalar get_threshold() const {return threshold;}
	virtual int getAtomicNumber() const  {return atomicNumber;}
};


class argonIoniz : public eCrossSection
{
 public:
   argonIoniz() : eCrossSection(15.76f, 30, 100, 1, IONIZATION) {};
   virtual Scalar sigma(Scalar energy);
};

class iCrossSection : public CrossSection
{
	Scalar	a;									//	cross section coeff.
	Scalar	b;									// ditto
public:
	iCrossSection(Scalar a, Scalar b, Type type);
	virtual Scalar	sigma(Scalar energy);
};

/**
 * The BaseMCPackage (base Monte Carlo package) will contain data and methods 
 * common for the derived classes MCCPackage and LIMCPackage. 
 * dad 01/18/2001
 */
class BaseMCPackage {
 public:
  enum GAS_TYPE {Ar, Ne, Xe, H, He, Li, N, Cs} type;
  BaseMCPackage(GAS_TYPE _type, const ostring& strGasType, 
                const Scalar& temperature, 
                Species* _eSpecies, Species* _iSpecies, 
                const Scalar& pressure, SpatialRegion* _SR,
                Scalar _Min1MKS, Scalar _Max1MKS, 
                Scalar _Min2MKS, Scalar _Max2MKS,
                const ostring& _analyticF, 
                const int& discardDumpFileNGDDataFlag) /* throw(Oops) */;
  ~BaseMCPackage() {};
 protected: 
  // 
  // variable which are used by both the MCCPackage and the LIMCPackage
  // 
  // the following two lines of variables 
  // are used for confinement of gas density to subregion of grid
  // 
  Scalar Min1MKS, Max1MKS, Min2MKS, Max2MKS;
  Vector2 deltaP, p1, p2, p1Grid, p2Grid;
  
  SpatialRegion* region;  // spatial region that owns this collision model
  Grid* grid;             // the grid associated with the spatial region
  int rz;                 // =0 by default;  =1 for cylindrical geometry
  int J, K;               // maximum grid values

  /**
   * the following two pointers are used in both the collision Monte Carlo
   * and the tunneling ionization. 
   */
  ParticleList* pList;
  ParticleGroup* pg;

  /**
   * electron and ion species will be created both in the 
   * Monte Carlo collisions and the tunneling ionization
   */
  Species* eSpecies;   
  Species* iSpecies;   
  
  /**
   * data members for gas type and gas temperature 
   */
  ostring gasTypeString;  // character representation of the gas type
  Scalar temp;            // gas temp in eV

  /**
   * When the Neutral Gas Density varies, 
   * the collision probability should depend on the maximum value of
   * this density. We'll calcualte it at initialization. 
   * The maximum Neutral Gas Density "maxNGD" is equal to
   * the "gasDensity" in the uniform case, and in the case of varying
   * neutral gas density, the "gasDensity" will be initialized to the value
   * of "maxNGD". dad 12/04/2000. 
   */
 Scalar gasDensity;   // gas number density m^-3 UNIFORM GAS DENSITY CASE
 Scalar maxNGD; 
 NGD* ptrNGD; // pointer to an NGD class (NeutralGasDensity) 
 
 private:
 BaseMCPackage(const BaseMCPackage&);
 BaseMCPackage& operator=(const BaseMCPackage&);
}; 

class MCCPackage : public BaseMCPackage
{
 private:
  Maxwellian* gasDistribution;
  //	Scalar MinPot;
  Scalar* collProb;
  Scalar ecxFactor;    // factor to multiply by electron cx
  Scalar icxFactor;    // factor to multiply by ion cx
  Scalar maxSigmaVe;   // MAX(sigma*v)
  Scalar maxSigmaVi;
  Scalar* extra;       // fractional collisions from previous steps
  int ionzFraction;    // fraction of particles to create in ionization event
  Scalar	eEnergyScale;		//	m/2e (converts (m/s)^2 -> eV)
  Scalar	iEnergyScale;		//	M/2e
  void newVelocity(Scalar energy, Scalar v, Vector3& u, int eFlag);
  Scalar sumSigmaVe();    // returns MAX(sigma*ve)
  Scalar sumSigmaVi();    // returns MAX(sigma*vi)
  
  // following are used in both collision() and elastic(), excitation(), etc.
  Vector3 u;   // 3-D velocity (gamma factor stripped out)
  Scalar v;    // v is the magnitude of u
  Scalar energy;
  int index;
  virtual void addCrossSections();
  
  // used to restrict use of MCC algorithm to specified time interval
  Scalar delayTime;  // don't start MCC until this much time has elapsed
  Scalar stopTime;   // stop use of MCC after this time (if value is > 0.)
  
 protected:
  
  /** Flag indicating whether to use the original nonrelativistic
   *  MCC algorithms (= 0) or the new relativistic models (= 1).
   *
   *  The default value is 0.  Can be set to 1 via the input file.
   *
   *  At present, only the Li (N coming soon) model is relativistic.
   *  The Li MCC model can be used in nonrelativistic mode, but the
   *  default is fully relativistic.
   *
   *  -- Bruhwiler 08/29/2000
   */
  int relativisticMCC;
  
  /** Method for calculating energy imparted to a secondary (ejected)
   *    electron, using a parametric model that captures relativistic
   *    physics for large impact energy and agrees with data at low energy.
   *
   *  The calculations underlying this method are contained in the
   *    Tech-X notes, "RNG Calculations for Scattering in XOOPIC," by
   *    David Bruhwiler (August 28, 2000).
   *
   *  The parametric model is a relativistic generalization of the
   *    work by Rudd et al., Phys. Rev. A 44, 1644 (1991) and Phys.
   *    Rev. A 47, 1866 (1993).  The relativistic generalization is
   *    described in the Tech-X notes, "Rudd's Differential Cross
   *    Section for Electron Impact Ionization," by David Bruhwiler,
   *    (July 17, 2000).
   */
  Scalar ejectedEnergy(const CrossSection& cx, const Scalar& impactEnergy);
  
  /** Method for calculating the scattering angles (theta, phi) with
   *    respect to the initial direction of the primary electron, for
   *    both the primary and secondary electrons in an impact-ionizaiton
   *    event.
   *
   *  Unchanged input variables are impactEnergy and ejectedEnergy.
   *  Empty variables passed in by reference to hold the desired results
   *    are:  thetaPrimary, thetaSecondary, phiPrimary and phiSecondary.
   *
   *  The calculations underlying this method are contained in the
   *    Tech-X notes, "RNG Calculations for Scattering in XOOPIC," by
   *    David Bruhwiler (August 28, 2000).
   *
   *  The parametric model is discussed in the Tech-X notes, "Rudd's
   *    Differential Cross Section for Electron Impact Ionization,"
   *    by David Bruhwiler, (July 17, 2000).
   */
  void primarySecondaryAngles(const CrossSection& cx,
                              const Scalar& impactEnergy,
                              const Scalar& ejectedEnergy,
                              Scalar& thetaPrimary,   Scalar& phiPrimary,
                              Scalar& thetaSecondary, Scalar& phiSecondary);
  
  /** Method for calculating the scattering angles (theta, phi) with
   *    respect to the initial direction of the electron, for elastic
   *    electron-neutral scattering.
   *
   *  Unchanged input variables are eImpact and the cross section cx.
   *  Empty variables passed in by reference to hold the desired results
   *    are:  thetaScatter and phiScatter.
   *
   *  The calculations underlying this method are contained in the
   *    Tech-X notes, "RNG Calculations for Scattering in XOOPIC," by
   *    David Bruhwiler (August 28, 2000).
   *
   *  The parametric model is discussed in the Tech-X notes, "Elastic
   *    and Inelastic Scattering at all Energies," by David Bruhwiler,
   *    (July 26, 2000).
   */
  void elasticScatteringAngles(
                               const Scalar& eImpact, const CrossSection& cx,
                               Scalar& thetaScatter, Scalar& phiScatter);

  /** Given an initial normalized momentum u=p/m=gamma*v (m/s), a new
   *    energy, and the scattering angles (wrt to initial direction
   *    defined by u) theta and phi, this method calculates and
   *    returns a new momentum.
   *
   *  Note that the magnitude of u is not important -- only the
   *    direction is used.  Thus, this method can be used for
   *    elastic scattering, for the primary in an ionization event,
   *    and also for the ejected secondary.
   *
   *  All of the input variables are unchanged.
   *  The new momentum is returned.
   *
   *  The calculations underlying this method are contained in the
   *    Tech-X notes, "Scattered Velocity Calculation for XOOPIC,"
   *    by David Bruhwiler (August 31, 2000).
   */
  Vector3 newMomentum(const Vector3 U_initial, const Scalar& energy,
                      const Scalar& theta, const Scalar& phi);
  
 public:
  
  MCCPackage( GAS_TYPE type, const ostring& strGasType, 
              Scalar pressure, Scalar temp, Species* eSpecies,
              Species* iSpecies, SpeciesList& sList, Scalar dt, int ionzFraction,
              Scalar ecxFactor, Scalar icxFactor, SpatialRegion* _SR,
              Scalar _Min1MKS, Scalar _Max1MKS, Scalar _Min2MKS, Scalar _Max2MKS,
              Scalar _delayTime, Scalar _stopTime, const ostring& _analyticF, 
              const int& discardDumpFileNGDDataFlag);
  virtual ~MCCPackage();
  virtual void init(SpeciesList& sList, Scalar dt);
  virtual void BoltzmannInit();
  Scalar *G,*E;
  int neCX; // number of cross sections
  int niCX; // number of cross sections
  int ntCX; 
  int nbCX;
  CrossSection** eCX;
  CrossSection** iCX;
  CrossSection** tCX;
  CrossSection** bCX;
  void collide(ParticleGroupList &pgList, ParticleList& pList)/* throw(Oops) */;
  virtual void dynamic(CrossSection& cx)/* throw(Oops) */;
  virtual void elastic(CrossSection& cx);
  virtual void excitation(CrossSection& cx);
  virtual void ionization(CrossSection& cx) /* throw(Oops) */;
  virtual void ionElastic(CrossSection& cx);
  virtual void chargeExchange(CrossSection& cx);
  
  virtual void setRelativisticMCC(int flag) {relativisticMCC = flag;}
  virtual int  getRelativisticMCC()  {return relativisticMCC;}
};


class ArMCC : public MCCPackage
{
  static Scalar sigmaElastic(Scalar energy);
  static Scalar sigmaExc(Scalar energy);
  static Scalar sigmaIz(Scalar energy);
  static Scalar sigmaCX(Scalar energy);
  static Scalar sigmaIelastic(Scalar energy);
public:
  ArMCC(Scalar p, Scalar temp, Species* eSpecies, Species* iSpecies,  
        SpeciesList& sList, Scalar dt, int ionzFraction,
        Scalar ecxFactor, Scalar icxFactor, SpatialRegion* _SR,
        Scalar _Min1MKS, Scalar _Max1MKS, Scalar _Min2MKS,
        Scalar _Max2MKS, Scalar _delayTime, Scalar _stopTime, const ostring &analyticF,
        const int& discardDumpFileNGDDataFlag);
  virtual void addCrossSections();
};

class NeMCC : public MCCPackage
{
  static Scalar sigmaElastic(Scalar energy);
  static Scalar sigmaExc(Scalar energy);
  static Scalar sigmaIz(Scalar energy);
public:
  NeMCC(Scalar p, Scalar temp, Species* eSpecies, Species* iSpecies,  
        SpeciesList& sList, Scalar dt, int ionzFraction, Scalar ecxFactor,
        Scalar icxFactor, SpatialRegion* _SR,
        Scalar _Min1MKS, Scalar _Max1MKS, Scalar _Min2MKS, Scalar _Max2MKS,
        Scalar _delayTime, Scalar _stopTime, const ostring &analyticF, 
        const int& discardDumpFileNGDDataFlag);
  virtual void addCrossSections(); 
};

class XeMCC : public MCCPackage
{
  static Scalar sigmaElastic(Scalar energy);
  static Scalar sigmaExc(Scalar energy);
  static Scalar sigmaIz(Scalar energy);
public:
  XeMCC(Scalar p, Scalar temp, Species* eSpecies, Species* iSpecies, 
        SpeciesList& sList, Scalar dt, int ionzFraction, Scalar ecxFactor,
        Scalar icxFactor, SpatialRegion* _SR,
        Scalar _Min1MKS, Scalar _Max1MKS, Scalar _Min2MKS, Scalar _Max2MKS,
        Scalar _delayTime, Scalar _stopTime, const ostring &analyticF, 
        const int& discardDumpFileNGDDataFlag);

  virtual void addCrossSections();
};


class HMCC : public MCCPackage
{
	static Scalar sigmaIz(Scalar energy);
public:
  HMCC(Scalar p, Scalar temp, Species* eSpecies, Species* iSpecies,  
       SpeciesList& sList, Scalar dt, int ionzFraction, Scalar ecxFactor,
       Scalar icxFactor, SpatialRegion* _SR,
       Scalar _Min1MKS, Scalar _Max1MKS, Scalar _Min2MKS, Scalar _Max2MKS,
       Scalar _delayTime, Scalar _stopTime, const ostring &analyticF,
       const int& discardDumpFileNGDDataFlag);
  virtual void addCrossSections();
};

/** Monte Carlo collision (MCC) for Helium -- Jeff Hammel **/
class HeMCC : public MCCPackage
{
  static Scalar sigmaElastic(Scalar energy);
  static Scalar sigmaExc(Scalar energy);
  static Scalar sigmaIz(Scalar energy);
  static Scalar sigmaIelastic(Scalar energy);
  static Scalar sigmaCX(Scalar energy);

public:
  HeMCC(Scalar p, Scalar temp, Species* eSpecies, Species* iSpecies,  
       SpeciesList& sList, Scalar dt, int ionzFraction, Scalar ecxFactor,
       Scalar icxFactor, SpatialRegion* _SR,
       Scalar _Min1MKS, Scalar _Max1MKS, Scalar _Min2MKS, Scalar _Max2MKS,
       Scalar _delayTime, Scalar _stopTime, const ostring &analyticF,
       const int& discardDumpFileNGDDataFlag);
  virtual void addCrossSections();
};

/** Monte Carlo collision (MCC) class for Lithium.
 *  Only electron impact ionization is implemented at present.
 *  First added March 2, 2000.
 *
 *  Modified to include relativistic effects August 30, 2000.
 *  Modified to include elastic scattering September 3, 2000.
 *
 *  @author David Bruhwiler
 */
class LiMCC : public MCCPackage {

  public:

    /** Constructor
     */
    LiMCC(Scalar p, Scalar temp, Species* eSpecies, Species* iSpecies, 
          SpeciesList& sList, Scalar dt, int ionzFraction, Scalar ecxFactor,
          Scalar icxFactor, int relativisticFlag, SpatialRegion* _SR,
          Scalar _Min1MKS, Scalar _Max1MKS, Scalar _Min2MKS, Scalar _Max2MKS,
          Scalar _delayTime, Scalar _stopTime, const ostring &analyticF,
          const int& discardDumpFileNGDDataFlag);

    /** Method from MCCPackage class for adding cross sections.
     */
    virtual void addCrossSections();

		/** Set the relativisticMCC flag and make sure the local
		 *  static version, staticRelMCC, is consistent.
		 */
    virtual void setRelativisticMCC(int flag) {
      relativisticMCC = flag;
      staticRelMCC = flag;
    }

  protected:

    /** Method that calculates the elastic scattering cross-section
     *  in m^2 as a function of the electron impact energy in eV.
     *
     *  Cross section is a fit to LLNL's EEDL calculations at high
     *     energy and, for low energy, to calculations/data from
     *     Bray, Fursa and McCarthy, Phys. Rev. A 47, 1101 (1993).
     *
     *  Units are eV for energy and m^2 for cross-sections.
     *  NOTE: units are NOT cm^2 and NOT barns!
     */
    static Scalar sigmaElastic(Scalar energy);

    /** Method that calculates the ionization cross-section in m^2
		 *  as a function of the electron impact energy in eV.
		 *
     *  The Younger fitting parameters come directly from
     *  Journal of Quantitative Spectroscopy and Radiative Transfer,
     *  Vol 26, 1981, p. 329,  by S.M. Younger.  The paper by Bray,
     *  "Calculation of electron-impact ionization of lithium-like
     *  targets," J. Phys. B: At. Mol. Opt. Phys., Vol. 28 (1995),
     *  p. L247,  suggests that this fit might yield a cross-section
     *  that's too large for impact energies less than 30 eV.
     *
     *  This is for ionization of neutral Lithium, assuming the electron
     *  is removed from the 2s shell (ie outer electron).  This dominates
     *  the cross-section for removal from the inner shell.
     *
     *  Units are eV for energy and m^2 for cross-sections.
     *  NOTE: units are NOT cm^2 and NOT barns!
     */
    static Scalar sigmaIz(Scalar energy);

    /** Ionization threshold energy in eV.
     */
    static Scalar threshold;

    /** Younger fitting parameter A, in units of m^2 * eV^2
     */
    static Scalar youngerParamA;

    /** Younger fitting parameter B, in units of m^2 * eV^2
     */
    static Scalar youngerParamB;

    /** Younger fitting parameter C, in units of m^2 * eV^2
     */
    static Scalar youngerParamC;

    /** Younger fitting parameter D, in units of m^2 * eV^2
     */
    static Scalar youngerParamD;

    /** Fitting parameter A1 for relativistic model.
     */
    static Scalar fitParamA1;

    /** Fitting parameter A2 for relativistic model.
     */
    static Scalar fitParamA2;

    /** Fitting parameter A3 for relativistic model.
     */
    static Scalar fitParamA3;

    /** Fitting parameter A4 for relativistic model.
     */
    static Scalar fitParamA4;

    /** Fitting parameter N for relativistic model.
     */
    static Scalar fitParamN;

    /** Local static version of the base class data member
		 *  relativisticMCC, which is a flag specifying whether
		 *  the scattering, etc. should be relativistic.
     */
    static int staticRelMCC;
};


/** Monte Carlo collision (MCC) class for Nitrogen -- fully
 *    relativistic -- including electron impact ionization 
 *    and elastic scattering.
 *
 *  First added September 3, 2000.
 *
 *  @author David Bruhwiler
 */
class NMCC : public MCCPackage {

  public:

    /** Constructor
     */
    NMCC(Scalar p, Scalar temp, Species* eSpecies, Species* iSpecies, 
         SpeciesList& sList, Scalar dt, int ionzFraction, Scalar ecxFactor,
         Scalar icxFactor, int relativisticFlag, SpatialRegion* _SR,
         Scalar _Min1MKS, Scalar _Max1MKS, Scalar _Min2MKS, Scalar _Max2MKS,
         Scalar _delayTime, Scalar _stopTime, const ostring &analyticF,
         const int& discardDumpFileNGDDataFlag);
    ~NMCC() { std::cout << "calling the destructor of NMCC!!!!!" << std::endl; }
    /** Method from MCCPackage class for adding cross sections.
     */
    virtual void addCrossSections();

		/** Set the relativisticMCC flag and make sure the local
		 *  static version, staticRelMCC, is consistent.
		 */
    virtual void setRelativisticMCC(int flag) {
      relativisticMCC = flag;
      staticRelMCC = flag;
    }

  protected:

    /** Method that calculates the elastic scattering cross-section
		 *  in m^2 as a function of the electron impact energy in eV.
		 *
		 *  Cross section is a fit to LLNL's EEDL calculations.
     *
     *  Units are eV for energy and m^2 for cross-sections.
     *  NOTE: units are NOT cm^2 and NOT barns!
     */
    static Scalar sigmaElastic(Scalar energy);

    /** Method that calculates the ionization cross-section in m^2
		 *  as a function of the electron impact energy in eV.
		 *
     *  Units are eV for energy and m^2 for cross-sections.
     *  NOTE: units are NOT cm^2 and NOT barns!
     */
    static Scalar sigmaIz(Scalar energy);

    /** Ionization threshold energy in eV.
     */
    static Scalar threshold;

    /** Fitting parameter A1 for relativistic model.
     */
    static Scalar fitParamA1;

    /** Fitting parameter A2 for relativistic model.
     */
    static Scalar fitParamA2;

    /** Fitting parameter A3 for relativistic model.
     */
    static Scalar fitParamA3;

    /** Fitting parameter A4 for relativistic model.
     */
    static Scalar fitParamA4;

    /** Fitting parameter N for relativistic model.
     */
    static Scalar fitParamN;

    /** Local static version of the base class data member
		 *  relativisticMCC, which is a flag specifying whether
		 *  the scattering, etc. should be relativistic.
     */
    static int staticRelMCC;
};

/** 
 *  Monte Carlo Tunneling Ionization (MCTI) Package class 
 *  from which the specializations for different gas types 
 *  will derive.
 *
 *  First added March, 2001.
 *
 *  @author D. A. Dimitrov
 */
class MCTIPackage : public BaseMCPackage {
 public:
  MCTIPackage( GAS_TYPE type, const ostring& strGasType, 
               Scalar pressure, Scalar temp, Species* eSpecies,
               Species* iSpecies, SpeciesList& sList, Scalar dt, int ionzFraction,
               Scalar ecxFactor, Scalar icxFactor, SpatialRegion* _SR,
               Scalar _Min1MKS, Scalar _Max1MKS, Scalar _Min2MKS, Scalar _Max2MKS,
               Scalar _delayTime, Scalar _stopTime, const ostring& _analyticF,
               const int& numMacroParticlesPerCell, 
               Scalar argEfieldFrequency,
               const int& discardDumpFileNGDDataFlag)/* throw(Oops) */;
  virtual ~MCTIPackage() { }

  //
  // the following pure virtual function must be implemented in the derived classes.
  //
  virtual void tunnelIonize(ParticleGroupList** pgList, ParticleList& pList) = 0; 
 protected: 
  // atomic units:
  static const double H_BAR;                     // [erg*s]
  static const double E_CHARGE;                  // [esu]
  static const double E_MASS;                    // [g]
  static const double ATOMIC_TIME_UNIT;          // [s]
  static const double ATOMIC_LENGTH_UNIT;        // [cm]
  static const double ATOMIC_E_FIELD_UNIT;       // [Statvolt/cm]
  static const double ATOMIC_E_FIELD_UNIT_MKS;   // [V/m]
  static const double ATOMIC_ENERGY_UNIT;        // [erg]
  static const double ATOMIC_ENERGY_UNIT_EV;     // [eV]

  /**
   * In the tunneling ionization simulations we need 
   * the frequency of the external time varying 
   * electric field in order to estimate the minimum
   * E field magnitude for which Keldish' condition holds, 
   * i.e. we use gamma = 1, where gamma is the Keldish
   * parameter. 
   * The default value of the frequency is Scalar_EPSILON.
   */
  Scalar EfieldFrequency; 

  /**
   * A function to calculate the electric field energy
   * stored in a cell with lower left corner indexed
   * by j,k. The energy is calculated integrating over
   * the cell volume the square of the electric field.
   * The field is interpolated from the values at the nodes 
   * using the formula in:
   * Grid::interpolateBilinear(Vector3** A, const Vector2& x)
   */
  Scalar getCellEfieldEnergy(const int& j, const int& k) const;
  
 private:
  MCTIPackage(const MCTIPackage&);
  MCTIPackage& operator=(const MCTIPackage&);

};

/** 
 * Hydrogen Monte Carlo Tunneling Ionization (HMCTI) class.
 * Specializes the MCTIPackage class for the simulations 
 * of tunneling ionization of Hydrogen under the influence
 * of linearly polarized alternating electric field.
 * 
 * This class implements the formula for the probability
 * rate for tunneling ionization from: 
 * N. B. Delone and V. P. Krainov, "Multiphonon Processes
 * in Atoms", Springer (2000), Eq. (4.6) on page 71 with
 * quantum numbers (n, l, m) = (1, 0, 0), i.e. only for
 * tunneling ionization from the ground state. 
 * This formula is expected to be applicable only for electric 
 * field magnitudes: E << E_atomic = 1/(16n^4) for Hydrogen. 
 * The condition for tunneling ionization to occur is:
 * gamma^2 << 1, where gamma = omega*sqrt(2*E_i)/E,
 * omega is the frequency of the alternating electric 
 * field with magnitude E, and E_i is the magnitude 
 * of the bound state energy from which the electron is
 * to tunnel. Both linearly and circularly polarized
 * fields are handled. 
 *
 * First added March, 2001.
 *
 * @author D. A. Dimitrov
 */
class HMCTI : public MCTIPackage {
 public:
  HMCTI(Scalar p, Scalar temp, Species* eSpecies, Species* iSpecies,  
        SpeciesList& sList, Scalar dt, int ionzFraction, Scalar ecxFactor,
        Scalar icxFactor, SpatialRegion* _SR,
        Scalar _Min1MKS, Scalar _Max1MKS, Scalar _Min2MKS, Scalar _Max2MKS,
        Scalar _delayTime, Scalar _stopTime, const ostring &analyticF, 
        const int& numMacroParticlesPerCell, 
        const int& ETIPolarizationFlag, Scalar argEfieldFrequency,
        const int& discardDumpFileNGDDataFlag) /* throw(Oops) */;
  ~HMCTI() {}
  void tunnelIonize(ParticleGroupList** pgList, ParticleList& pList)/* throw(Oops) */;
 private:
  HMCTI(const HMCTI&);
  HMCTI& operator=(const HMCTI&);
  
  /**
   * a helper function to calculate the probability for tunneling 
   * ionization of Hydrogen ions from the ground state
   */
  Scalar TIProbability(Scalar (HMCTI::*P)(Scalar&, Scalar&) const, 
                       Scalar& E, Scalar& dt); 
  
  /**
   * value of the atomic field for which the 
   * probability for tunneling ionization has an extremum. 
   * Above this value the tunneling ionization formalism
   * should not be applied. 
   */
  Scalar EmaxTI;

  /**
   * Value of the E field below which the Keldish condition
   * is violated. The probability for tunneling ionization 
   * for fields smaller than this value will be set to zero. 
   */
  Scalar EminTI;

  /**
   * A helper function to calculate the circular E field
   * probability for the magnitude E of the field and time
   * step dt (both in atomic units). Note that this formula 
   * is specifically for TI from the ground state of Hydrogen
   * and is the same as the TI probability for this state in
   * static E field.
   */
  Scalar getCircularFieldProbabilityTI(Scalar& E, Scalar& dt) const {
    return ((4.0/E)*exp(-2.0/(3.0*E))*dt);
  }
  /**
   * A helper function to calculate the linear polarized alternating E field
   * probability for E magnitude of the field and time
   * step dt (both in atomic units).
   */
  Scalar getLinearFieldProbabilityTI(Scalar& E, Scalar& dt) const {
    return (sqrt(48.0/(M_PI*E))*exp(-2.0/(3.0*E))*dt);
  }
  /**
   * A pointer to the function that will be used to calculate the
   * tunneling ionization probabilities. For now this pointer 
   * will point to getCircluarFieldProbabilityTI(Scalar& E, Scalar& dt)
   * or to getLinearFieldProbabilityTI(Scalar& E, Scalar& dt) to
   * cover the two cases (circularly or linearly polarized alternating E
   * field) of TI from the ground state of Hydrogen.
   */
  Scalar (HMCTI::*fp)(Scalar&, Scalar&) const;
};

class MCC 
{				//	list of newly created particles
 public:
  MCC() {}
  ~MCC();
  void addPackage(MCCPackage *p) {packageList.add(p);}
  ParticleList& collide(ParticleGroupList& pgList) /* throw(Oops) */;
 private:
  MCC(const MCC&);
  MCC& operator=(const MCC&);

  oopicList<MCCPackage> packageList;
  ParticleList	pList;	
};

/** 
 *  Monte Carlo Tunneling Ionization (MCTI) class 
 *  used to store the list of MCTIPackage types
 *  and to drive the tunneling ionization.
 *  It is written by analogy with the MCC class. 
 *
 *  First added March, 2001.
 *
 *  @author D. A. Dimitrov
 */
class MCTI
{
 public:
  MCTI() {}
  ~MCTI();
  void addPackage(MCTIPackage *p) {packageList.add(p);}
  void tunnelIonize(ParticleGroupList** pgList) /* throw(Oops) */;  
 private:
  MCTI(const MCTI&);
  MCTI& operator=(const MCTI&);

  oopicList<MCTIPackage> packageList;
  ParticleList pList;
};
#endif	//	ifndef __MCC_H

