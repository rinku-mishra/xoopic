/*
 *  secondary.cpp
 *
 * These classes represent a number of secondary models:
 * 
 * Secondary: simple step function secondary emission, with turn-on threshold.
 * 
 * Secondary_Vaughan: a Vaughan-based model, includes energy and angular dependence, 
 * full emission spectrum including reflected and scattered primaries.
 * 
 * Secondary_Furman:  a Furman-based model
 *
 * 1.0 (JohnV, 09-02-98) Created by cutting Secondary from dielectric.h, added
 *     Secondary2.
 * 2.0 (JohnV, 25-Oct-2002) Removed unused variables, cleaned up code, modified
 *     Secondary to enable isotropic emission of neutrals due to ion impact.
 * 2.01 (JohnV, 29-Oct-2002) Corrected coefficient in Emax0 to conform to updated
 *     Vaughan model (Vaughan, IEEE Trans. ED 40 (1993)). Also updated k to
 *     use the empirical values in the same source, and added the high energy
 *     relation therein as well.
 * 3.0 (ScottRice, 03-Jul-2012) Renamed Secondary2 to Secondary_Vaughan, added
 *     Furman's secondary electron model
 */

#include "secondary.h"
#include "boundary.h"
#include "vmaxwelln.h"


Secondary::Secondary(Scalar _secondary, Scalar _threshold, Scalar Eemit, 
		     Species* _secSpecies, Species* _iSpecies)
{
  secondary = _secondary; 
  threshold = _threshold; 
  secSpecies = _secSpecies; 
  iSpecies = _iSpecies;
  v_emit = sqrt(2*PROTON_CHARGE*Eemit/secSpecies->get_m()); 

}


Secondary::~Secondary()
{

}


void Secondary::setBoundaryPtr(Boundary* _bPtr)
{
  bPtr = _bPtr;
  normal = bPtr->unitNormal();
  t = Vector3(0,0,1).cross(normal); // tangent in x1-x2 plane
  // third component is in x3
  //Since XOOPIC is a 2-dimensional code, normal exists in the x1-x2 plane, and the resulting
  //cross product with (0,0,1) defines a normalized vector in the x1-x2 plane.

}


int Secondary::generateSecondaries(Particle& p, ParticleList& pList)
{
  if (p.get_speciesPtr() != iSpecies) return 0;     
  if (p.energy_eV_MKS() < threshold) return 0;
  Scalar yield = number(p);
  int n = (int)yield;
  yield -= n;
  if (frand() <= yield) n++;
  for (int i=0; i<n; i++) pList.add(createSecondary(p));

  return n;
}


// number of secondaries emitted per incident particle
Scalar Secondary::number(Particle& p)
{
  return secondary;
}


Particle* Secondary::createSecondary(Particle& p)
{
  // compute isotropic angles of emission
  // general case should work for oblique boundaries
  Scalar sintheta = frand();				// random value between 0 and 1, defined in misc.h
							//?? where is Scalar defined?  param.h line 163 ?  precision.h 
  Scalar costheta = sqrt(1 - sintheta*sintheta);	
  Scalar phi = 2*M_PI*frand();				// M_PI defined in misc.h 
  Vector3 v = costheta*normal + sintheta*cos(phi)*t;	// Vector3 defined in ovector.h
  v.set_e3(sin(phi)*sintheta);
  // The above few lines define a coordinate system (n, t, z), where the n-axis is along the normal direction, 
  // the t-axis is along the tangent direction, and the z-axis is out of the 2-D plane of this 2-D code, and
  // z = n x t (read "n cross t").  n, t, and z are all assumed to be unit-normal mutually orthogonal vectors.
  // 
  // By defining v = n*cos(theta) + t*sin(theta), we define v such that v dot n = cos(theta) and v dot t = sin(theta) = cos(beta),
  // where beta = (pi/2 - theta) is the angle between t and v.  However, v would then be constrained to lie within the n-t plane.  
  // Let phi to be a rotation angle about the n-axis.  Then by first defining v = n*cos(theta) + t*cos(phi)*sin(theta), and then 
  // defining v(3) = sin(phi)*sin(theta), we have that v dot n = cos(theta), v dot t = sin(theta)*cos(phi), and 
  // v dot z = sin(theta)*sin(phi).  Note that if we generalize the code to 3D, we would need to compute v differently, since n and 
  // t could have z-components. 
 
  v *= frand()*v_emit; // linearly scale magnitude
  // This assigns a velocity which has a magnitude between 0 and v_emit, and in a direction that is uniformily distributed in sin(theta).
  // Uniform distribution of sin(theta) ==> cos(theta) distribution of theta (for 0 <= theta <= pi/2).


  return new Particle(p.get_x(), v, secSpecies, p.get_np2c(), 
		      p.isVariableWeight());
}


unsigned int Secondary::factorial(unsigned int N)
{
// This function will return the factorial of the input argument N.

  unsigned int result = 1;
  for(int i=N; i>0; i--)
  {
    result = result*i;
  }
  return result;
}


unsigned int Secondary::binomial(unsigned int M, Scalar prob)
{
// This function will return a random integer from a binomial random distribution
// Input arguments:
//   M - number of trials underlying the binomial distribution, ie. the return value will be from 0 to M
//   prob - the probability which parameterizes each trial in the overall binomial event
// Return value: an integer from 0 to M  

// Note that the expected value of the binomial R.V. is M*prob.
// Note that there are M+1 possible outcomes: 0 to M.

  Scalar Probability_vector[M+1];    // array of length-(M+1) of the binomial R.V. probabilities
  for(unsigned int count = 0; count <= M; count++)  // M is unsigned, so count is declared to be unsigned to avoid compiler warnings
  {
    Probability_vector[count] = (factorial(M)/(factorial(count)*factorial(M-count)))*pow(prob,count)*pow(1-prob,M-count);  
  }

  Scalar CumSum[M+1];	// cumulative sum of the probabilities, which will be used to generate the random outcome
  CumSum[0] = Probability_vector[0];
  for(unsigned int count = 1; count <= M; count++)  // compute the cumulative sum of the probabilities in this loop
  {                                                 // M is unsigned, so count is declared to be unsigned to avoid compiler warnings 
      CumSum[count] = Probability_vector[count] + CumSum[count-1];
  }

  Scalar RandVar = frand();   // random number between 0 and 1
  // We want to determine the index of the least upper bound for RandVar in CumSum.  For example, suppose RandVar equals 0.15, and the nearest 
  // bounding CumSum values are 0.1 and 0.2.  Then, RandVar would map to the CumSum bin with value 0.2.
  
  int Outcome = 0;    // variable to store the index value in CumSum where the value is the least upper bound for RandVar in CumSum 
  for(unsigned int count = 0; count < M; count++)  // M is unsigned, so count is declared to be unsigned to avoid compiler warnings
  {
    if(RandVar > CumSum[count])
      Outcome = count+1;  
      // If RandVar exceeds CumSum[count], then the least upper bound is greater than the current index position, so assign the next index position
      // to be the least upper bound, and continue looping.  If we are at the (M-1) index position, the the upper bound must be M'th index position
      // since M is the last position in the zero-indexed array of length (M+1).
  }

  return Outcome; 
  // Outcome represents the outcome of the binomial random variable 

}

Scalar Secondary::gaussian()  // return a Gaussian random variable with mean 0 and standard deviation 1
{
  // The Gaussian random variable is generated using the the Box–Muller method 
  Scalar u = frand();
  Scalar v = frand();
  while (u == 0 || u == 1) u = frand();  // ensure that u does not equal 0 or 1
  Scalar ReturnVar = sqrt(-2*log(u))*cos(2*M_PI*v);
  return ReturnVar;
}




/// Secondary_Vaughan implementation ////////////////////////////////////////////////////////////////////////////////////////////////

Secondary_Vaughan::Secondary_Vaughan(Scalar secondary, Scalar threshold, Scalar Eemit, 
		       Species* secSpecies, Species* iSpecies, Scalar f_ref,
		       Scalar f_scat, Scalar _energy_max0, Scalar _ks)
  : Secondary(secondary, threshold, Eemit, secSpecies, iSpecies)
{
  fractionReflected = f_ref;
  fractionScattered = f_scat;
  // energy_w = Eemit; unused?
  energy_max0 = _energy_max0;
  ks = _ks;
  dist = new MaxwellianFlux(secSpecies);
  dist->setThermal(Eemit, EV);
  // The above line will generate... WE DO NOT KNOW AT PRESENT!!! NEED TO UNDERSTAND MaxwellianFlux CLASS! 
  // 7/19/12 - JohnV is working on understanding/verifying the correctness of the setThermal function and dependencies, there is probably an error 

}


Secondary_Vaughan::~Secondary_Vaughan()
{
  delete dist;
}


Scalar Secondary_Vaughan::number(Particle& p)
{
  // References: 
  // Vaughn_1 - "A New Formula for Secondary Emission Yield" by Vaughan, September 1989
  // Vaughan_2 - "Secondary Emission Formulas" by Vaughan, April 1993

  if (secondary <= 0) return 0;
  if(p.energy_eV_MKS() < threshold) return 0;

  Scalar theta, delta_ratio;
  Vector3 v = p.get_u()/p.gamma();        // v is the velocity of the incident particle
  theta = acos(-(v*bPtr->unitNormal())/v.magnitude()); // angle of incidence 

  Scalar Emax = energy_max0*(1 + 0.5*ks*theta*theta/M_PI); // Equation (1) of Vaughan_2
  Scalar delta_max = secondary*(1 + 0.5*ks*theta*theta/M_PI); // Equation (2) of Vaughan_2

  Scalar w = (p.energy_eV_MKS() - threshold)/(Emax - threshold);  // Equation (3) of Vaughan_2, note that Vaughan uses "v" but we already use that for velocity
  Scalar k = (w > 1) ? 0.25 : 0.56;  // Equations (5) and (6) of Vaughan_2:  if w<=1, then k=0.56, otherwise k=0.25

  if (w < 3.6)  // Equation (4) in Vaughan_2:  For w < 3.6, delta(theta)/delta_max(theta) = (w*exp(1-w))^k
     delta_ratio = pow(w*exp(1-w), k);
  else          // Equation (7) in Vaughan_2:  For w >= 3.6, delta(theta)/delta_max(theta) = 1.125 / (w^(0.35))
     delta_ratio = 1.125/pow(w, Scalar(0.35));

  return delta_max*delta_ratio;

  // Instead of simply returning delta_max*delta_ratio, we may want to return an random integer drawn from a Poisson or Binomial distribution that has the 
  // expected value equal to the non-integer delta_max*delta_ratio, since right now generateSecondaries() will just cast this value as an integer.  The following block of commented lines will do this. 
  

//  int M = 10;       // ceiling on the number of emitted secondaries, note that M+1 possibilities are possible, since we can have zero emitted secondaries
//  int NumEmitted;     // NumEmitted represents the number of electrons scattered after impact  
//  Scalar prob = delta_max*delta_ratio/M;
//  NumEmitted = binomial(M, prob);  

//  return NumEmitted; 
}


Particle* Secondary_Vaughan::createSecondary(Particle& p)
{
  Scalar R = frand();
  Vector3 u;
  if (R < fractionScattered+fractionReflected)  // primary electron is scattered or reflected
  {    
    u = p.get_u();       // u is gamma*velocity of the incident particle  

    // The following if-else-block applies only to the reflected primaries, since scattered
    // primaries have their u vector set in the next if-block below, and true secondaries are 
    // not handled inside the current if-block.
    if (bPtr->alongx1()) u.set_e2(-u.e2());  
    else u.set_e1(-u.e1());                
    // For reflected primaries, if the boundary is along x1, then invert the e2 (x2) coordinate 
    // of the particle velocity.  Otherwise, invert the e1 (x1) coordinate of the particle velocity. 


    if (R > fractionReflected)       // scattered primary
    {  
       // compute isotropic angles of emission
       // general case should work for oblique boundaries
       Scalar sintheta = frand();
       Scalar costheta = sqrt(1 - sintheta*sintheta);
       Scalar phi = 2*M_PI*frand();
       Vector3 v = costheta*normal + sintheta*cos(phi)*t;
       v.set_e3(sin(phi)*sintheta);
       v *= frand()*u.magnitude(); // linearly scale magnitude
       u = v;    // this equality between u and v assumes that v is not relativistic
    }
  }
  else u = dist->get_U(bPtr->unitNormal()); // true secondary;

  // The above lines of code do the following:
  // If R >= (fractionReflected+fractionScattered):
  //   Return a true secondary electron with a random u=gamma*v drawn from a Maxwellian distribution.
  // Elseif fractionReflected < R < (fractionReflected+fractionScattered):   
  //   Return a scattered primary electron with a random theta (scattering angle from normal) and velocity, 
  //   where theta has a cos(theta) distribution and velocity has a uniform [0, u_incident] distribution, 
  //   where u_incident is the magnitude of the incident (gamma*v). 
  // Elseif R <= fractionReflected:
  //   Return a reflected primary electron with the following criteria:
  //     If the scattering boundary is along the x1 or x2 direction, then simply reflect u at the boundary.


  return new Particle(p.get_x(), u, secSpecies, p.get_np2c(), 
		      p.isVariableWeight());
}


/// Secondary_Furman implementation ////////////////////////////////////////////////////////////////////////////////////////////////



Secondary_Furman::Secondary_Furman(Scalar _alpha, Scalar _P_1e_inf, Scalar _P_1e, Scalar _E_e, Scalar _W, Scalar _p, Scalar _sigma_e, Scalar _e_1, Scalar _e_2,
                   Scalar _P_1r_inf, Scalar _E_r, Scalar _r, Scalar _q, Scalar _r_1, Scalar _r_2, Scalar _delta_ts, Scalar _E_ts,
                   Scalar _s, Scalar _t_1, Scalar _t_2, Scalar _t_3, Scalar _t_4, Scalar _E_t, Scalar _delta_t, 
                   Scalar* _p_vec, Scalar* _eps_vec,  Scalar secondary, Scalar threshold, Scalar Eemit, Species* secSpecies, Species* iSpecies)
                : Secondary(secondary, threshold, Eemit, secSpecies, iSpecies) 
{

  Furman_max_secondaries = 10;   // change this if we read in more than 10 values of p_n and eps_n
                               // Furman uses 10 values in his paper

  // Emitted angular spectrum parameter (Furman section IIC1):
  alpha = _alpha;

  // Backscattered electron parameters (Furman section IIIB):
  P_1e_inf = _P_1e_inf;
  P_1e = _P_1e;
  E_e = _E_e;
  W = _W;  
  p = _p;
  sigma_e = _sigma_e;
  e_1 = _e_1;
  e_2 = _e_2;

  // Rediffused electron parameters (Furman section IIIC):
  P_1r_inf = _P_1r_inf;
  E_r = _E_r;
  r = _r;
  q = _q;
  r_1 = _r_1;
  r_2 = _r_2;

  // True secondary electron parameters (Furman section IIID):
  delta_ts = _delta_ts;
  E_ts = _E_ts;
  s = _s;
  t_1 = _t_1;
  t_2 = _t_2;
  t_3 = _t_3;
  t_4 = _t_4;

  // Total SEY parameters (Furman Table 1):
  E_t = _E_t;
  delta_t = _delta_t;

  // Probability model parameters for true secondary electrons (Furman Table II):
  p_vec = _p_vec;         // will become a vector of 10 elements
  eps_vec = _eps_vec;     // will become a vector of 10 elements

  p_vec = new Scalar[Furman_max_secondaries];
  eps_vec = new Scalar[Furman_max_secondaries];
 
  for(int count = 0; count < Furman_max_secondaries; count++)
  {
    p_vec[count] = _p_vec[count];
    eps_vec[count] = _eps_vec[count];
  }

}

Secondary_Furman::~Secondary_Furman()    
{
  delete [] p_vec;
  delete [] eps_vec;
}


int Secondary_Furman::generateSecondaries(Particle& p_ptr, ParticleList& pList)
{
  if (p_ptr.get_speciesPtr() != iSpecies) return 0;                 

  Vector3 Incident_v = p_ptr.get_u()/p_ptr.gamma();        // Incident_v is the velocity of the incident particle
  Scalar Incident_theta_rad = acos(-(Incident_v*bPtr->unitNormal())/Incident_v.magnitude()); // incident particle angle of incidence 

  Scalar E_0 = pow(Incident_v.magnitude(), 2) * ELECTRON_MASS / (2*PROTON_CHARGE);  // Incident electron energy in electron volts
  // NOTE THAT WE MAY WANT TO CHANGE THE MASS AND CHARGE TO ALLOW FOR ARBITRARY INCIDENT PARTICLES, NOT JUST ELECTRONS

  // Step 1:  Compute delta_e, delta_r, and delta_ts from Equations 25-48 in Furman's paper.
  Scalar delta_e_0 = P_1e_inf + (P_1e - P_1e_inf)*exp( -pow( fabs(E_0-E_e)/W, p) / p ); // Eq. (25)
  Scalar delta_e = delta_e_0 * (1 + e_1*(1-pow(cos(Incident_theta_rad),e_2)));  // Eq. (47a)

  Scalar delta_r_0 = P_1r_inf*( 1 - exp(-pow(E_0/E_r,r) ) ); // Eq. (28)
  Scalar delta_r = delta_r_0 * (1 + r_1*(1-pow(cos(Incident_theta_rad),r_2)));  // Eq. (47b)

  Scalar delta_theta = delta_ts * (1 + t_1*(1-pow(cos(Incident_theta_rad),t_2)));  // Eq. (48a)
  Scalar E_theta = E_ts * (1 + t_3*(1-pow(cos(Incident_theta_rad),t_4)));  // Eq. (48b)
  Scalar delta_ts_theta = delta_theta * s*(E_0/E_theta)/(s - 1 + pow(E_0/E_theta,s) ); // Eq. (31) 


  // Step 2:  Compute P_n for n = 0 to M, according to the model chosen.
  // Section IIID2 (Equations 35-46) in Furman's paper
  int M = Furman_max_secondaries;  // maximum allowable number of secondary electrons


  Scalar delta_prime_ts = delta_ts_theta / (1 - delta_e - delta_r);  // Eq. (39)  
  Scalar P_prime_n_ts[M+1];  // array size changed from M to M+1 on 3/13/13

  Scalar prob = delta_prime_ts/M;
  Scalar P_n[M+1];  // This vector will hold the P_n values defined in Furman's Eq. 43 
  for(int count = 0; count <= M; count++)
  {
    P_prime_n_ts[count] = (factorial(M)/(factorial(count)*factorial(M-count)))*pow(prob,count)*pow(1-prob,M-count);  // Eq. (46)
  }
  P_n[0] = P_prime_n_ts[0] * (1 - delta_e - delta_r);   // Eq. (43a) 
  P_n[1] = P_prime_n_ts[1] * (1 - delta_e - delta_r) + delta_e + delta_r;   // Eq. (43b) 
  for(int count = 2; count <= M; count++)
  {
    P_n[count] = P_prime_n_ts[count] * (1 - delta_e - delta_r);   // Eq. (43c) 
  }

  // Step 3:  Generate random integer from 0 to M with probability distribution P_n, 
  //          this random integer represents the number of secondary electrons generated. 
  Scalar Cum_P_n[M+1];  // This vector will hold the cumulative sum of the P_n vector
  Cum_P_n[0] = P_n[0];
  for (int count = 1; count <= M; count++)
  {
    Cum_P_n[count] = P_n[count] + Cum_P_n[count-1]; 
  }
  Scalar RandVar = frand();   // random number between 0 and 1


  int TotalNumEmitted = 0;    // This will be set to the index of CumSum where the value is the least upper bound for RandVar 
  for(int count = 0; count < M; count++)
  {
    if(RandVar > Cum_P_n[count])
      TotalNumEmitted = count+1;  
      // If RandVar exceeds CumSum[count], then the least upper bound is greater than the current index position, so assign the next index position
      // to be the least upper bound, and continue looping.  If we are at the (M-1) index position, the the upper bound must be M'th index position
      // since M is the last position in the zero-indexed array of length (M+1).
  }


//TotalNumEmitted = TotalNumEmitted+1;  // delete when done testing  


  // Step 4:  Generate the scattering energies for the emitted electrons.

  // The following declared variables are used for the TotalNumEmitted==1 block below.  Comments
  // and/or references to a Furman equation are provided where they appear in the code below.
  Scalar P_1_ts;
  Scalar delta_1;
  Scalar a_e;
  Scalar a_r;
  Scalar a_ts;
  Scalar u;
  Scalar u_1;
  Scalar u_2;
  Scalar eps_n;
  Scalar p_n;
  Scalar P_0;
  Scalar x;
  Scalar x_0;


  // The following declared variables are used for the TotalNumEmitted>1 block below.
  Scalar* KE_vec = new Scalar[TotalNumEmitted];    // secondary electron kinetic energies in electron-volts
  Scalar* y_vec = new Scalar[TotalNumEmitted];
  Scalar u_k;
  Scalar* theta_k = new Scalar[TotalNumEmitted];  // theta_k is only of length TotalNumEmitted-1, but this can cause run-time
                                                  // errors for TotalNumEmitted=0, so we just allow for one unused index position
  Scalar mu;
  Scalar nu;
  Scalar B_inv;
  Scalar P_inv;
  Scalar uu;
  Scalar y;


  if(TotalNumEmitted == 1)  // Furman Appendix D1
  {
    P_1_ts = P_prime_n_ts[1] * (1 - delta_e - delta_r);   // Eq. (42)
    delta_1 = delta_e + delta_r + P_1_ts;  // non-numbered equation just after Eq. (D4c)  
    a_e = delta_e / delta_1;               // Eq. (D4a)
    a_r = delta_r / delta_1;               // Eq. (D4b)
    a_ts = P_1_ts / delta_1;               // Eq. (D4c)


    u = frand();   // uniform random number between 0 and 1
    if(u < a_e)  // elastic collision for secondary electron
    {
      KE_vec[0] = -1;   // initialize the KE to a negative number, the loop below ensures it will become positive
      while(KE_vec[0] < 0)  // ensure that KE is non-negative
        KE_vec[0] = E_0 - sigma_e * fabs(gaussian());  // non-numbered equation after Eq. (D4c)
    }
    else if(u < a_e + a_r) // rediffused secondary electron
    {
      u_1 = frand();             // uniform random number between 0 and 1
      KE_vec[0] = E_0 * pow(u_1,1/(1+q));  // non-numbered equation after Eq. (D4c)
    }
    else // true secondary electron
    {
      u_2 = frand();             // uniform random number between 0 and 1
      p_n = p_vec[0];            // p_n here represents p_1 in Furman's non-numbered equations after Eq. (D4c) 
      eps_n = eps_vec[0];        // eps_n here represents eps_1 in Furman's non-numbered equations after Eq. (D4c)
      // For p_n and eps_n values above, remember that c arrays are 0-indexed, but Furman's paper is 1-indexed

      P_0 = P_function(p_n, E_0/eps_n);  // non-numbered equation after Eq. (D4c)

      // Next we need to find x such that u2*P_0 = P(p_1, x).  Since u_2 is between 0 and 1, 
      // and the P_function is monotonically increasing, x must be between 0 and E_0/eps_1.
      x = Inverse_P_function(p_n, u_2*P_0);  

      KE_vec[0] = eps_n * x;
    }

  }
  else if(TotalNumEmitted > 1)  // Furman Appendix D2
  {
    eps_n = eps_vec[TotalNumEmitted-1];
    p_n = p_vec[TotalNumEmitted-1];
    // The -1 in the indexing is because c uses zero-indexing, and Furman uses 1-indexing
    
    // Following the procedure provided in Appendix D2 in Furman:
    // Step D2.1:  Compute x_0 and P_0
    x_0 = E_0/eps_n;
    P_0 = P_function(TotalNumEmitted*p_n, x_0);

    // Step D2.2: Generate u_k and compute theta_k, k = 1:(TotalNumEmitted - 1)  
    for(int k = 0; k <= TotalNumEmitted-2; k++)
    { 
      u_k = frand();
      mu = p_n*(TotalNumEmitted - k);
      nu = p_n;
      B_inv = Inverse_B_function(u_k, mu, nu);
      theta_k[k] = asin(sqrt(B_inv));
    } 
  
    // Step D2.3:  Compute scalar y:  
    uu = frand();
    P_inv = Inverse_P_function(TotalNumEmitted*p_n, uu*P_0); 

    y = sqrt(P_inv); 
  
    // Step D2.4:  Compute vector y_vec:
    for(int count = 0; count < TotalNumEmitted-1; count++)  y_vec[count] = y*cos(theta_k[count]);    
    y_vec[TotalNumEmitted-1] = y;

    for(int count = 1; count < TotalNumEmitted; count++)
    {
      for(int count2 = 0; count2++; count2 < count)  y_vec[count] = y_vec[count]*sin(theta_k[count2]);
    }
  
    // Step D2.5:  Compute kinetic energies for each electron:
    for(int count = 0; count < TotalNumEmitted; count++)
    {
      KE_vec[count] = eps_n * pow(y_vec[count],2);
    }
  }


  // Step 5:  Generated the scattering angles for the emitted electrons.  
  Scalar* phi_scattered = new Scalar[TotalNumEmitted];
  Scalar* theta_scattered = new Scalar[TotalNumEmitted];
  Scalar theta_scattered_temp;
  Scalar theta_test;
  Scalar pdf_test;
  Scalar compare_val;

  for(int count = 0; count < TotalNumEmitted; count++)
    phi_scattered[count] = 2*M_PI*frand();           // TotalNumEmitted independent polar angles from Section V of Furman

  //theta_scattered follows a nontrivial PDF f(theta) = A*cos(theta)^alpha, where theta can range from [0 pi/2].  
  //  To compute theta_scattered, we need to follow a two-step process:
  //    Step A:  generate uniform random variables theta_test (range of 0 to pi/2) and compare_val (range of 0 to 1)
  //    Step B:  if compare_val <= cos(theta_test)^alpha, then theta = theta_test, otherwise repeat from step A    
  for(int count = 0; count < TotalNumEmitted; count++)
  {
    theta_scattered_temp = -1;
    while(theta_scattered_temp == -1)
    {
      theta_test = (M_PI/2)*frand();
      pdf_test = pow(cos(theta_test),alpha);
      compare_val = frand();
      if(compare_val <= pdf_test) theta_scattered_temp = theta_test;
    }
    theta_scattered[count] = theta_scattered_temp;
  }


  // Step 6:  Assign the emitted particles to the particle list.  
  Particle* New_p_ptr;  // pointer to the secondary particles that are generated
  Scalar v_magnitude;  // variable to store the magnitude of the velocity
  Vector3 v_temp;  // variable to store the emission velocity of the secondary electron(s)    

  // Note that we have computed length-TotalNumEmitted vector KE_vec, theta_scattered, and phi_scattered. 
  // For each value corresponding to a secondary electron, we compute the 3-dimensional velocity vector for the secondary electron. 

  for (int count=0; count<TotalNumEmitted; count++) 
  {
    v_magnitude = sqrt(2*(KE_vec[count]*PROTON_CHARGE)/ELECTRON_MASS);
    // remember that KE is expressed in eV, so (KE*PROTON_CHARGE) provides the kinetic energy in Joules

    v_temp = v_magnitude*(cos(theta_scattered[count])*normal + sin(theta_scattered[count])*cos(phi_scattered[count])*t);
    v_temp.set_e3(v_magnitude*sin(phi_scattered[count])*sin(theta_scattered[count]));

    New_p_ptr = new Particle(p_ptr.get_x(), v_temp, secSpecies, p_ptr.get_np2c(), p_ptr.isVariableWeight());
    // The Particle class constructor accepts u instead of v, where u = gamma*v, and gamma is the relativistic gamma.  However, secondary electrons are
    // at low velocities, such that gamma is effectively unity, and we can substitute v for u.

    pList.add(New_p_ptr);

  }

  delete [] KE_vec;
  delete [] y_vec;
  delete [] theta_k;
  delete [] phi_scattered;
  delete [] theta_scattered;

  return TotalNumEmitted;
}



Scalar Secondary_Furman::P_function(Scalar a, Scalar x)  // Normalized incomplete Gamma function
{
//
// This function will return the approximate normalized incomplete gamma function P(a,x), defined as:
//
//   Integral[exp(-t)*t^(a-1), {t,0,x}]/Integral[exp(-t)*t^(a-1), {t,Infinity,x}]
//     = Integral[exp(-t)*t^(a-1), {t,0,x}]/gamma(a)
//
//   where gamma(a) is the gamma function evaluated at argument a.  The approximatation implemented
//   in this function is accurate for 0.9 <= a <= 45.
// 
// Reference:  U. Blahak, "Efficient approximation of the incomplete gamma
// function for use in cloud model applications", Geoscientific Model Development, 23 July 2010
//
// For Furman's secondary electron model, the parameter a = n*p_n, where n is the number of secondary
//   electrons generated, and p_n ranges from 1.3 to 4.7 in Table II of Furman and Pivi's 2002 paper
//   "Probabilistic model for the simulation of secondary electron emission".  For n <=10, this corresponds
//   to parameter a between around 1 and 50.  
// For Furman's model, the parameter x = E_incident / eps_n, where eps_n ranges from 1 to 17.8 eV in Table
//   II of Furman's paper.  Thus x can range from 0 to Infinity, since there is no upper bound on the
//   incident energy.   

//cout << "\nInput arguments to P_function:  a = " << a << ", x = " << x;

  Scalar p[] = {9.4368392235E-03,
               -1.0782666481E-04,
               -5.8969657295E-06,
                2.8939523781E-07,
                1.0043326298E-01,
                5.5637848465E-01};

  Scalar q[] = {1.1464706419E-01,
                2.6963429121E+00,
               -2.9647038257E+00,
                2.1080724954E+00};
 
  Scalar r[] = {0,
                1.1428716184E+00,
               -6.6981186438E-03,
                1.0480765092E-04};
  
  Scalar s[] = {1.0356711153E+00,
                2.3423452308E+00,
               -3.6174503174E-01,
               -3.1376557650E+00,
                2.9092306039E+00};
  
  Scalar c[4];

  Scalar a_2 = pow(a,2);
  Scalar a_3 = pow(a,3);
  Scalar a_4 = pow(a,4);

  c[0] = 1 + p[0]*a + p[1]*a_2 + p[2]*a_3 + p[3]*a_4 + p[4]*(exp(-p[5]*a)-1);
  c[1] = q[0] + q[1]/a + q[2]/a_2 + q[3]/a_3;
  c[2] = r[0] + r[1]*a + r[2]*a_2 + r[3]*a_3;
  c[3] = s[0] + s[1]/a + s[2]/a_2 + s[3]/a_3 + s[4]/a_4;

  Scalar W = 0.5 + 0.5 * tanh(c[1]*(x-c[2])); 
  Scalar G = tgamma(a);
  // tgamma() is defined in math.h for double precision input and output arguments, alternate functions tgammaf() and tgammal() are also 
  // respectively defined for float and long double arguments, in case the datatype Scalar has problems compiling with the above
  // double precision arguments.
  
  Scalar incomplete_gamma = exp(-x) * pow(x,a) * ( 1/a + c[0]*x/(a*(a+1)) + pow(c[0]*x,2)/(a*(a+1)*(a+2)) ) * (1 - W) + G * W * (1 - pow(c[3],-x)); 

  Scalar P = incomplete_gamma / G;  // normalized incomplete gamma function 

  if(incomplete_gamma >= G)  P = 1;  // if G or (or both G and P) goes to infinity, then the output goes to unity
  // This is useful for when x grows large, and G evaluates to Infinity due to numerical out-of-bounds.
  
  return P;
}

 
Scalar Secondary_Furman::Inverse_P_function(Scalar a, Scalar y)  // Inverse of normalized incomplete Gamma function 
{
  //
  // This function will find the x which yields y = P_function(a,x).  It does this by iteratively updating a trial value 
  // of x, and ending when y_trial is within a pre-defined delta of y, or when the max number of iterations is reached.
  //
  // Note that 0 <= y < 1 for this function to make sense.  No error checking is done to ensure that y is within these
  // bounds!

  Scalar delta = 0.00000001;  // y(x_test) must be within this distance of the true value
  // delta is set very small, since for x >~ 100, P(a,x) changes very little as x varies.
 
  int MaxIterations = 30;

  Scalar x_min = 0;
  Scalar x_max = 10000;
  Scalar x_test = 5000;
  Scalar x;            // this will become the eventual return value

  Scalar test_y;       
  Scalar error;
  
  for(int count = 1; count <= MaxIterations; count++)
  {
    test_y = P_function(a,x_test);     
    error = test_y - y;

    if(fabs(error) < delta)
    {
      x = x_test;
      break;
    }

    if(error > 0)   // if x_test is too big
    {
      x_max = x_test;
      x_test = 0.5*(x_min + x_test);
    }
    else // if x is too small
    {
      x_min = x_test;
      x_test = 0.5*(x_max + x_test); 
    }     
        
    x = x_test;
    // if MaxIterations is reached, then this will be the final value
  }

  return x;
}


Scalar Secondary_Furman::B_function(Scalar x, Scalar u, Scalar v)  // Normalized incomplete Beta function
{
//
// This function will return the approximate normalized incomplete beta function P(x,u,v) defined as:
//
//   Integral[t^(u-1) * (1-t)^(v-1), {t,0,x}] * gamma(u+v)/(gamma(u)*gamma(v)) 
//
// The integral will be evaluated using Simpson's method with 11 points.
// u and v are restricted to be greater than or equal to unity, which in
// practice is not a problem for Furman's SEY model.
//

  Scalar B;  // This becomes the return value
  int N = 11;  // number of points in Simpson's Method
  Scalar delta_t;
  Scalar t_val;
  Scalar* y = new Scalar[N];

  if(x == 0) B = 0;
  else
  {
    N = 11;
    delta_t = x/(N-1);

    for(int count = 0; count < N; count++)
    {
      t_val = delta_t * count;
      y[count] = pow(t_val,u-1) * pow(1-t_val, v-1);
    }

    B = (delta_t/3) * ( y[0]+y[N-1] + 4*(y[1]+y[3]+y[5]+y[7]+y[9]) + 2*(y[2]+y[4]+y[6]+y[8]) );

    B = B * tgamma(u+v)/(tgamma(u)*tgamma(v)); 
    // tgamma() is defined in math.h for double precision input and output arguments, alternate functions tgammaf() and tgammal() are also 
    // respectively defined for float and long double arguments, in case the datatype Scalar has problems compiling with the above
    // double precision arguments.

  }

  return B;
}


Scalar Secondary_Furman::Inverse_B_function(Scalar y, Scalar u, Scalar v)  // Inverse of normalized incomplete Beta function
{
//
// This function will find the x which yields y = B_function(u,v,x).  
// Note that 0 <= y <= 1, u >= 1, and v >= 1 for this function to make sense.  No error checking is provided in this function!
//


  Scalar delta = 0.001;  // y(x_hat) must be within this distance of the true value 
  int MaxIterations = 100;
  Scalar x_min = 0;
  Scalar x_max = 1;
  Scalar x_test = 0.5;
  Scalar x;             // this will become the eventual return value  

  Scalar test_y;
  Scalar error;

  for(int count = 1; count <= MaxIterations; count++)
  {
    test_y = B_function(x_test, u, v);     
    error = test_y - y;

    if(fabs(error) < delta)
    {
      x = x_test;
      break;
    }

    if(error > 0)   // if x_test is too big
    {
      x_max = x_test;
      x_test = 0.5*(x_min + x_test);
    }
    else // if x is too small
    {
      x_min = x_test;
      x_test = 0.5*(x_max + x_test);
    }      

    x = x_test;
    // if MaxIterations is reached, then this will be the final value

  }

  return x;
}

