/*====================================================================

GPDIST.H

  A class which accepts a particle and maintains a space and
velocity distribution.  Meant to be used in boundaries with collect.

0.99	(PeterM ?)  Inception
0.991 (PeterM 8-29-95)  Add a method for computing Q, the sum of the
      charge which has hit the wall.
1.01  (JohnV 05-16-97) Add ftheta.
2.0   (JohnV 09-30-97) Revamped to store only f(x); should be fixed later.
2.1   (JohnV 06-28-98) Fixing later to include f(x,energy) and f(x,theta).
2.2   (JohnV 01JUN2001) add capability for Qcol by species.
2.3   (JohnV 01Jun2003) Fix scaling error in Ihist plots for long sims,
      by saving only Q collected since last call rather than taking the
      difference of two numbers of nearly the same magnitude.

====================================================================
*/

#ifndef __GP_DIST_H
#define __GP_DIST_H

#include "misc.h"
class Particle;
class Vector3;
class Species;

class PDistCol
{
	int nxbins;  // the number of 'bins' in the position;
	int nenergybins; // number of bins in energy;
	int nthetabins; // bins in theta;
	int nxpoints; // number of data points = nxbins+1;
	int nepoints;
	int ntpoints;
	Scalar x_min, x_max;
	Scalar energy_min, energy_max;
	Scalar theta_min, theta_max;
	int nsp;  /* the number of species */
	Scalar **fxdata;  /* [nsp][nxbins] */
	Scalar ***fedata; // [nsp][nxbins][nenergybins]
	Scalar ***ftdata; // [nsp][nxbins][nthetabins]
	Scalar xscale;
	Scalar escale;
	Scalar tscale;
	Scalar *x;  /* an array of the positions of the bins in x */
	Scalar *energy; // array of energy bins;
	Scalar *theta; // array of theta bins
	Scalar Qcol;  /* the sum of all the charge collected, ever */
   Scalar *Qcol_sp; // by species
	//	Scalar Qlast; /* the q from the previous request */
	//   Scalar *Qlast_sp; // by species
 public:
	void record_particle(Particle &p, int component);
	PDistCol(int nxbins, int nenergybins, int nthetabins, int nsp,
				Scalar energy_min, Scalar energy_max,
				Scalar x_min, Scalar x_max,
				Scalar theta_min, Scalar theta_max
				/*, Species *diag_species*/);
	~PDistCol();
   int get_nxbins() { return nxbins;};
	int *get_nxpoints() { return &nxpoints;};
	int *get_nepoints() { return &nepoints;};
	int *get_ntpoints() { return &ntpoints;};
	int get_nsp() { return nsp;};
	Scalar get_x_MIN() { return x_min;};
	Scalar get_x_MAX() { return x_max;};
	Scalar *get_x(){ return x;};
	Scalar *get_fxdata(int speciesID) {return fxdata[speciesID]; };
	Scalar get_energy_MIN() {return energy_min;};
	Scalar get_energy_MAX() {return energy_max;};
	Scalar *get_energy() {return energy;};
	Scalar **get_fedata(int speciesID) {return fedata[speciesID];};
	Scalar get_theta_MIN() {return theta_min;};
	Scalar get_theta_MAX() {return theta_max;};
	Scalar *get_theta() {return theta;};
	Scalar **get_ftdata(int speciesID) {return ftdata[speciesID];}
	//	Scalar getQ() {return Qcol;};
   // Scalar getQ(int species) {return Qcol_sp[species];};
	Scalar getdQ() {
	  // Scalar dQ = Qcol - Qlast;
	  // Qlast = Qcol;
	  Scalar dQ = Qcol; // get Q collected since previous call
	  Qcol = 0; // reset to 0
	  return dQ;
	}
   Scalar getdQ(int species) {
	  //      Scalar dQ = Qcol_sp[species] - Qlast_sp[species];
	  //      Qlast_sp[species] = Qcol_sp[species];
	  Scalar dQ = Qcol_sp[species]; // get Q collected since previous call
	  Qcol_sp[species] = 0; // reset to 0
	  return dQ;
   }
};

#endif  //  __GP_DIST_H
