#ifndef __SECONDARY_H
#define __SECONDARY_H

/*
=======================================================================

secondary.h

These classes represent a number of secondary models:

Secondary: simple step function secondary emission, with turn-on threshold.

Secondary_Vaughan: a Vaughan-based model, includes energy and angular dependence, 
full emission spectrum including reflected and scattered primaries.

Secondary_Furman: a Furman-based model, includes energy and angular dependence, 
full emission spectrum including reflected and scattered primaries.


1.0 (JohnV, 09-02-98) Created by cutting Secondary from dielectric.h, added
    Secondary2.

2.0 (JohnV, 25-Oct-2002) Removed unused variables, cleaned up code, modified
    Secondary to enable isotropic emission of neutrals due to ion impact.

3.0 (ScottRice, 07-Jul-2012) Renamed Secondary2 to Secondary_Vaughan, and 
    implemented Secondary_Furman.

=======================================================================
*/

#include "particle.h"
class Boundary;
class MaxwellianFlux;

class Secondary
{
 protected:
  Boundary* bPtr; // access to Boundary geometry
  Vector3 normal; // normal of the boundary surface
  Vector3 t; // z cross normal, tangent to the surface
  Scalar secondary; // secondary electron emission coefficient (SEY) at normal incidence 
  Scalar threshold; // emission threshold;
  Scalar v_emit;  // max emission velocity
  Species* secSpecies; // secondary emission species
  Species* iSpecies; // ions which generate secondaries
  virtual Scalar number(Particle& p);
  virtual Particle* createSecondary(Particle& p);

 public:
  Secondary(Scalar _secondary, Scalar _threshold, Scalar Eemit, Species* 
	    _secSpecies, Species* _iSpecies);
  virtual ~Secondary();
  BOOL checkSpecies() {return secSpecies && iSpecies;};
  virtual int generateSecondaries(Particle& p, ParticleList& pList);
  Scalar get_q() {return secSpecies->get_q();}
  void setBoundaryPtr(Boundary* _bPtr);
  unsigned int factorial(unsigned int N);  // return the factorial of an integer, used for secondary electron modelling
  unsigned int binomial(unsigned int M, Scalar prob); // return a random integer taken from a binomial distribution
  Scalar gaussian();   // return a random value taken from a Gaussian distribution with mean 0 and standard deviation 1.
//  Scalar exp(Scalar x);  // return exp(x)  // already defined in math.h
};

class Secondary_Vaughan : public Secondary
{
  Scalar fractionReflected; // fraction reflected primaries;
  Scalar fractionScattered; // fraction scattered primaries;
  // Scalar energy_w; // max value of emission energy function; unused?
  Scalar energy_max0; // incident energy at which secondary coefficient is maximized at normal incidence;
  MaxwellianFlux *dist; // distribution for true secondaries
  Scalar ks; // surface smoothness (0 = rough, 2 = smooth)
 public:
  Secondary_Vaughan(Scalar secondary, Scalar threshold, Scalar Eemit, Species* 
	     secSpecies, Species* iSpecies, Scalar f_ref, Scalar f_scat, 
	     /*Scalar energy_w,*/ Scalar energy_max0, Scalar ks);
  virtual ~Secondary_Vaughan();
  virtual Scalar number(Particle& p);
  virtual Particle* createSecondary(Particle& p);
};

class Secondary_Furman : public Secondary
{
//  Scalar fractionReflected; // fraction reflected primaries;
//  Scalar fractionScattered; // fraction scattered primaries;
//  // Scalar energy_w; // max value of emission energy function; unused?
//  Scalar energy_max0; // energy at which sec. coeff = secondary for normal inc.;
//  MaxwellianFlux *dist; // distribution for true secondaries
//  Scalar ks; // surface smoothness (0 = rough, 2 = smooth)


//////////////////////////////////////////////////////////////////
// For a complete understanding of these variables, reference "Probabilistic model for the simulation of secondary electron emision"
// by M. A. Furman and M. T. F. Pivi, 31 December 2002, Physical Review Special Topics- Accelerators and Beams 
// The variable names below are listed in the same order as in Tables 1 and 2 of Furman's paper.  Subscript text in Furman's variable
// names are denoted by a preceding underbar (_) in the variable names below.  P_1e_inf denotes P_1e evaluated at infinity.
//

  // Emitted angular spectrum parameter (Furman section IIC1):
  Scalar alpha;

  // Backscattered electron parameters (Furman section IIIB):
  Scalar P_1e_inf;
  Scalar P_1e;
  Scalar E_e;
  Scalar W;  
  Scalar p;
  Scalar sigma_e;
  Scalar e_1;
  Scalar e_2;

  // Rediffused electron parameters (Furman section IIIC):
  Scalar P_1r_inf;
  Scalar E_r;
  Scalar r;
  Scalar q;
  Scalar r_1;
  Scalar r_2;

  // True secondary electron parameters (Furman section IIID):
  Scalar delta_ts;
  Scalar E_ts;
  Scalar s;
  Scalar t_1;
  Scalar t_2;
  Scalar t_3;
  Scalar t_4;

  // Total SEY parameters (Furman Table 1):
  Scalar E_t;
  Scalar delta_t;

  // Probability model parameters for true secondary electrons (Furman Table II):
  Scalar* p_vec;       // will become a vector of 10 elements
  Scalar* eps_vec;     // will become a vector of 10 elements
  int Furman_max_secondaries;   // Maximum number of secondaries that Furman's model can produce
                                // Thie defines the length of the vectors p_vec and eps_vec to be read-in
 
  Scalar P_function(Scalar a, Scalar x);  // Normalized incomplete Gamma function 
  Scalar Inverse_P_function(Scalar a, Scalar y);  // Inverse of normalized incomplete Gamma function 
  Scalar B_function(Scalar x, Scalar u, Scalar v);  // Normalized incomplete Beta function
  Scalar Inverse_B_function(Scalar y, Scalar u, Scalar v);  // Inverse of normalized incomplete Beta function

//////////////////////////////////////////////////////////////////


 public:
  Secondary_Furman(Scalar alpha, Scalar P_1e_inf, Scalar P_1e, Scalar E_e, Scalar W, Scalar p, Scalar sigma_e, Scalar e_1, Scalar e_2,
                   Scalar P_1r_inf, Scalar E_r, Scalar r, Scalar q, Scalar r_1, Scalar r_2, Scalar delta_ts, Scalar E_ts,
                   Scalar s, Scalar t_1, Scalar t_2, Scalar t_3, Scalar t_4, Scalar E_t, Scalar delta_t, 
                   Scalar* p_vec, Scalar* eps_vec, Scalar secondary, Scalar threshold, Scalar Eemit, Species* secSpecies, Species* iSpecies);
  // Note that secondary, threshold, and Eemit are superceded in Furman's model by more fundamental parameters.  These variables
  // are retained in Secondary_Furman to make it compatible with the base class Secondary. 
  virtual ~Secondary_Furman();
//  virtual Scalar number(Particle& part);
//  virtual Particle* createSecondary(Particle& part);
  int generateSecondaries(Particle& p, ParticleList& pList);
};


#endif
