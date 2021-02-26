/*
   ====================================================================

   loadpw.cpp

PeterM 3/28/2001

   ====================================================================
   */

#include "sptlrgn.h"
#include "ptclgrp.h"
#include "maxwelln.h"
#include "load.h"
#include "loadpw.h"
#include "oopiclist.h"
#include "misc.h"
#include "ostring.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cstdio>

using namespace std;

Scalar LoadPlaneWave::F(Vector2& x) 
{
   if(analyticF==(ostring)"0.0\n" || LoadMethodFlag== PLANE_WAVE) 
      return(1);
   else if (analyticF==(ostring)"fajans\n")
      return(fajans_load(x));

   else if (analyticF==(ostring)"file\n")
      return(gridded_load(x));
   
   else 
      return (gridded_load(x)); //evaluator load

}

LoadPlaneWave::LoadPlaneWave(SpatialRegion* _SR,Scalar _density, Maxwellian* max, Scalar _Min1MKS, 
           Scalar _Max1MKS, Scalar _Min2MKS, Scalar _Max2MKS, int _LoadMethodFlag, 
           Scalar _np2c, const ostring &_analyticF, const ostring &_DensityFile,const ostring &_name,Scalar _pwFrequency,Scalar _pwPhase, Scalar _pwWavenumber,
                             Scalar _pwAmplitude, Scalar _pwDelay     ):
  Load(_SR, _density,  max,  _Min1MKS, _Max1MKS, _Min2MKS,  _Max2MKS, _LoadMethodFlag,     _np2c, _analyticF, _DensityFile,_name)
{

  pwFrequency = _pwFrequency;
  pwPhase = _pwPhase;
  pwWavenumber = _pwWavenumber;
  pwAmplitude = _pwAmplitude;
  pwDelay = _pwDelay;

}


int LoadPlaneWave::load_it()
  /* throw(Oops) */{
   Vector3 u;
   Vector2 x;
   Boundary*** CellMask = grid->GetCellMask();
   Scalar dx = grid->dl1(0,0);  //UNIFORM mesh

   // LEAP-FROG:  must take off 1/2 a timeset on
   Scalar pwT = region->getTime() - region->get_dt()*0.5 ;
   int seed = 0;

   if(numberParticles > 0) {
      Vector2 xc;
      Scalar Fxc;
      
      int count = 0;
      while (count < numberParticles)
         {
            switch (LoadMethodFlag){
             case RANDOM:
                xc.set_e1(p1.e1()+deltaP.e1()*frand());
                if(rz)
                   xc.set_e2(sqrt(drSqr*frand()+rMinSqr));
                else
                   xc.set_e2(p1.e2()+deltaP.e2()*frand());
                break;
              case QUIET_START:
            default:
                 xc.set_e1(p1.e1()+deltaP.e1()*revers2(seed));
                if(rz)
                   xc.set_e2(sqrt(drSqr*revers(seed,3)+rMinSqr));
                else
                   xc.set_e2(p1.e2()+deltaP.e2()*revers(seed,3));
                break;
             }
            Fxc = F(xc);
            seed++;
            if ((Fxc!=0) && (Fxc>=(Scalar)frand())){
               x = grid->getGridCoords(xc);

               if (CellMask[(int)x.e1()][(int)x.e2()]==0){  //FREESPACE
                  count++;	
                  u = maxwellian->get_U();
                  // Here is where we set the velocity we actually want.
                  {
                    if(pwT >= pwDelay) {  // wave is on now
                      Scalar w = pwFrequency;
                      // px is the position in the plane wave
                      // it's dx * (number of shifts + the fractional part of x)
                      //  note: x is in grid coordinates.
                      Scalar px = dx * (region->getShiftNum() + x.e1() - ((int) x.e1()));
                      // the velocity to add
                      Scalar A1 = ELECTRON_CHARGE * pwAmplitude / (ELECTRON_MASS * w);
                      
                      Scalar vya = -A1 * cos(pwWavenumber * px + w * pwT + pwPhase);
                      Scalar kcw = SPEED_OF_LIGHT * pwWavenumber / w;
                      Scalar vxa = 0.25 * kcw * kcw *  cos( 2*( pwWavenumber *px + w * pwT + pwPhase))/ SPEED_OF_LIGHT;

                      u = Vector3(vxa, vya, u.e3());
                    }
                  }

                  Particle*       particle = new Particle(x, u, species, np2c);
                  particleList.push(particle);
               }
            }
            if (count)
               if (count%50==0)
                  region->addParticleList(particleList);
         }
      region->addParticleList(particleList);
   }
   
   return(1);
}

