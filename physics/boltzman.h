//====================================================================

#ifndef	__BOLTZMAN_H
#define	__BOLTZMAN_H

#include	"ovector.h"
#include	"misc.h"
#include  "particle.h"
#include  "stdio.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_HDF5
#include "dumpHDF5.h"
#endif

//---------------------------------------------------------------------
//This uses boltzmann electrons to neutralize the charge
//See write up.

// forward declarations
class Fields;
class PoissonSolve;

class Boltzmann
{
  Species* BoltzSpecies;
  int ElectrostaticFlag;
  bool dump;

  // energy balance flag [doesn't work when on? JH, Sept. 7, 2005]
  bool EB_flag;   
  
  // what are these?  should be documented, JH, Sept. 7, 2005
  int EBCFlag, PSCFlag; 
  
  Scalar qbytemp;  // units of per energy [1/V]
  Scalar energy;   // total energy of boltzmann species [eV/C]
  Scalar MinPot;   // minimum potential in the system [V]
  ParticleList pList;
  Fields* fields;
  Grid* grid;
  PoissonSolve* psolve;
  Scalar tolerance;
  int itermax;  // # of iterations for nonlinear solve
  Scalar totalCharge; // of boltzmann species in C
  Scalar n0, q, temperature, qMKS, chargeRatio;
  int J, K, sourceFlag;
  Scalar** phi;
  Scalar** rho;
  Scalar** BoltzmannRho;
  Scalar** bRho;
  Scalar** totalRho;
  Scalar** foo;
  Scalar** b1_orig;
  Scalar** b2_orig;
  Scalar** a1_local;
  Scalar** a2_local;
  Scalar** b1_local;
  Scalar** b2_local;
  Scalar** c1_local;
  Scalar** c2_local;
  Scalar*** resCoeff;
  Scalar*** resCoeff_local;
  Boundary*** bcNode;

  
  void init(Scalar **source, Scalar **dest);
  void LinearSolve(Scalar **source);
  int NonlinearSolve(Scalar **source);
  void PSolveCoeff(Scalar **source);
  Scalar Residual();
  Vector2 ParticleFlux(Scalar dt);
  Vector2 Collisions(Scalar dt);
  inline Scalar Collect(Boundary* bPtr, 
			Scalar area, 
			const Vector2&, 
			const Scalar& 
			edge_rho, const Scalar& scale);
  void getMinPot(void);
  void UpdateBRho();
  int EnergyBalance();

 public:
  Boltzmann(Fields *F, Scalar temperature, 
	    Species * BoltzSpecies, bool _EB_flag);

  ~Boltzmann();

  void updatePhi(Scalar **source, Scalar **dest, const Scalar t, Scalar dt);

  void CopyBoltzmannRho() {for(int j=0;j<=J;j++) for(int k=0;k<=K;k++)
    bRho[j][k] = BoltzmannRho[j][k];};

  // accessor functions
  Scalar ** getBoltzmannRho() {return BoltzmannRho;};
  Species* getBSpecies() {return BoltzSpecies;};
  Scalar ** getTotalRho() {return totalRho;};
  ParticleList& testParticleList(ParticleGroupList& pgList);

  void IncBoltzParticles(ParticleGroupList& pgList);

  // dump/restore functions
#ifdef HAVE_HDF5
  int dumpH5(dumpHDF5 &dumpObj);
  int restoreH5(dumpHDF5 &restoreObj);
#endif
	
  int Dump(FILE *DMPFile);
  int Restore(FILE *DMPFile);  
};

#endif // ifndef __BOLTZMAN_H
