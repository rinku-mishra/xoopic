/*====================================================================

GPDIST.CPP

  A class which accepts a particle and maintains a space and
velocity distribution.  Meant to be used in boundaries with collect.

0.99	(PeterM ?)  Inception
0.991 (PeterM 8-29-95)  Add a method for computing Q, the sum of the
      charge which has hit the wall, + misc other changes.
1.01  (JohnV 05-16-97) Add ftheta.
2.0   (JohnV 09-30-97) Revamped to store only f(x); should be fixed later.
2.1   (JohnV 06-28-98) Fixing later to include f(x,energy) and f(x,theta).
2.2   (JohnV 01 JUN 2001) add capability for Qcol by species.
2.3   (JohnV 01Jun2003) Remove Qlast as part of correction for Ihist bug
      (see gpdist.h)

====================================================================
*/

#include "ovector.h"
#include "particle.h"
#include "gpdist.h"
#include "grid.h"
#include "ostring.h"

PDistCol::PDistCol(int _nxbins,	 int _nenergybins, int _nthetabins, int _nsp,
						 Scalar _energy_min, Scalar _energy_max,
						 Scalar _x_min,  Scalar _x_max,
						 Scalar _theta_min, Scalar _theta_max/*, Species *diag_species*/)
{ 
	nxbins = _nxbins;
	nenergybins = _nenergybins;
	nepoints = ntpoints = 0;
	nsp = _nsp;
   int i, j;

	// Qlast = Qcol = 0;
	Qcol = 0;
   Qcol_sp = new Scalar[nsp];
   memset(Qcol_sp, 0, nsp*sizeof(Scalar));
	//   Qlast_sp = new Scalar[nsp];
	//   memset(Qlast_sp, 0, nsp*sizeof(Scalar));

	if(nxbins) { 
      // we don't need to allocate diagnostics if nxbins==0
      if (nenergybins) nepoints = nenergybins + 1;
      energy_min = _energy_min;
      energy_max = _energy_max;
      nthetabins = _nthetabins;
      if (nthetabins) ntpoints = nthetabins + 1;
      theta_min = _theta_min*M_PI/180; // convert to radians internally
      theta_max = _theta_max*M_PI/180;
      nxpoints = nxbins + 1;
      x_min = _x_min;
      x_max = _x_max;
      fxdata = new Scalar *[nsp];
      x = new Scalar[nxbins+1];
      for(i=0; i<nsp; i++)	{  
         fxdata[i] = new Scalar[nxbins+1];
         memset(fxdata[i], 0, (nxbins+1)*sizeof(Scalar));
      }
      xscale = nxbins/(x_max - x_min);
      for (i=0; i <= nxbins; i++) x[i]= i/xscale + x_min;

      if (nenergybins > 0){
         fedata = new Scalar **[nsp];
         for (i=0; i<nsp; i++){
            fedata[i] = new Scalar *[nxbins+1];
            for (j=0; j<=nxbins; j++){
               fedata[i][j] = new Scalar[nenergybins+1];
               memset(fedata[i][j], 0, (nenergybins+1)*sizeof(Scalar));
            }
         }
         escale = nenergybins/(energy_max - energy_min);
         energy = new Scalar[nenergybins+1];
         for (j=0; j<=nenergybins; j++) energy[j] = energy_min + j/escale;
      }

      if (nthetabins > 0){
         ftdata = new Scalar **[nsp];
         for (i=0; i<nsp; i++){
            ftdata[i] = new Scalar *[nxbins+1];
            for (j=0; j<=nxbins; j++){
               ftdata[i][j] = new Scalar[nthetabins+1];
               memset(ftdata[i][j], 0, (nthetabins+1)*sizeof(Scalar));
            }
         }
         tscale = nthetabins/(theta_max - theta_min);
         theta = new Scalar [nthetabins+1];
         for (j=0; j<=nthetabins; j++) theta[j] = theta_min + j/tscale;
      }
   }
}

/* component is which of the two positions to use for collection */
void PDistCol::record_particle(Particle &p, int component)
{
   int spec = p.get_speciesID();
   //collect the charge of this particle
	Qcol+=p.get_q();
   Qcol_sp[spec] += p.get_q(); // also save by species

   if(!nxbins) return;
	Scalar xp = (component)? p.get_x().e2(): p.get_x().e1();
	xp = (xp-x_min)*xscale;
	if (xp > nxbins+1 || xp < 0) return;
	
	/* actually do the accumulation! */
	/* this assumes that the particle has the position on the boundary given by xp*/
	int j = (int) xp; // the floor index
	Scalar w = xp - j; // the fraction within the bin
	fxdata[spec][j] += p.get_np2c()*(1-w);
	fxdata[spec][j+1] += p.get_np2c()*w;
	
	if (nenergybins){
	  Scalar e = (p.energy_eV_MKS() - energy_min)*escale;
	  if (e > 0 && e < nenergybins) {
		 int k = (int) e;
		 Scalar we = e - k; // fraction in bin
		 if (w){
			 fedata[spec][j][k] += p.get_np2c()*(1-w)*(1-we);
			 fedata[spec][j][k+1] += p.get_np2c()*(1-w)*we;
			 fedata[spec][j+1][k] += p.get_np2c()*w*(1-we);
			 fedata[spec][j+1][k+1] += p.get_np2c()*w*we;
		 }
		 else{
			 fedata[spec][j][k] += p.get_np2c()*(1-we);
			 fedata[spec][j][k+1] += p.get_np2c()*we;
		 }
	 }
	}
	
	if (nthetabins){
	  Scalar t;
	  Vector3 u = p.get_u();
	  if (component) t = atan(sqrt(u.e2()*u.e2() + u.e3()*u.e3())/u.e1());
	  else t = atan(sqrt(u.e1()*u.e1() + u.e3()*u.e3())/u.e2());
	  t = (t - theta_min)*tscale;
	  if (t > 0 && t < nthetabins){
		 int k = (int) t;
		 Scalar wt = t - k;
		 if (w){
			 ftdata[spec][j][k] += p.get_np2c()*(1-w)*(1-wt);
			 ftdata[spec][j][k+1] += p.get_np2c()*(1-w)*wt;
			 ftdata[spec][j+1][k] += p.get_np2c()*w*(1-wt);
			 ftdata[spec][j+1][k+1] += p.get_np2c()*w*wt;
		 }
		 else{
			 ftdata[spec][j][k] += p.get_np2c()*(1-wt);
			 ftdata[spec][j][k+1] += p.get_np2c()*wt;
		 }
	 }
	}
}

PDistCol::~PDistCol() {
  if (nxbins) {
     if (x) {
       delete[] x;
       x = 0;
     }
     for (int i=0; i<nsp; i++){
        if (fxdata[i]) {
          delete[] fxdata[i];
          fxdata[i] = 0;
        }
        for (int j=0; j<=nxbins; j++){
           if (nenergybins) {
             if (fedata[i][j]) {
               delete[] fedata[i][j];
               fedata[i][j] = 0;
             }
           }
           if (nthetabins) {
             if (ftdata[i][j]) {
               delete[] ftdata[i][j];
               ftdata[i][j] = 0;
             }
           }
        }
        if (nenergybins) {
          if (fedata[i]) {
            delete[] fedata[i];
            fedata[i] = 0;
          }
        }
        if (nthetabins) {
          if (ftdata[i]) {
            delete[] ftdata[i];
            ftdata[i] = 0;
          }
        }
     }
     if (fxdata) {
       delete [] fxdata;
       fxdata = 0;
     }
     if (nenergybins) {
       if (fedata) {
         delete [] fedata;
         fedata = 0;
       }
     }
     if (nthetabins) {
       if (ftdata) {
         delete [] ftdata;
         ftdata = 0;
       }
     }
  }
  if (Qcol_sp) {
    delete[] Qcol_sp;
    Qcol_sp = 0;
  }
  //  if (Qlast_sp) {
  //    delete[] Qlast_sp;
  //    Qlast_sp = 0;
  //  }
}

