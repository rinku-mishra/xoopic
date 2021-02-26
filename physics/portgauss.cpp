/*
====================================================================

PORTGAUSS.CPP

====================================================================
*/

#include "port.h"
#include "portgauss.h"
#include "fields.h"
#include "ptclgrp.h"

#ifdef _MSC_VER
using std::cout;
using std::cerr;
using std::endl;
#endif

PortGauss::PortGauss(oopicList <LineSegment> *segments, int _EFFlag,
           Scalar _spotSize_p0, Scalar _waveLeng_p0, Scalar _focus_p0,
           Scalar _amp_p0, Scalar _chirp_p0, int _pulShp_p0, Scalar _tdelay_p0,
           Scalar _pulLeng_p0, Scalar _spotSize_p1, Scalar _waveLeng_p1,
           Scalar _focus_p1, Scalar _amp_p1, Scalar _chirp_p1, int _pulShp_p1,
           Scalar _tdelay_p1, Scalar _pulLeng_p1, Scalar _offset)
           : Boundary(segments)

{

 if(segments->nItems() > 1)
    {
    cout << "Warning, Port-derived boundaries can only have 1 segment.\n";
    cout << "Check your input file.\n";
    }

 BCType=DIELECTRIC_BOUNDARY;
 EFFlag = _EFFlag;

 offset=_offset;
// Gaussian parameters for wave (0)
 spotSize_p0 = _spotSize_p0;
 waveLeng_p0 = _waveLeng_p0;
 focus_p0 = _focus_p0;
 amp_p0 = _amp_p0;

/*
//debug
 cout << "spotSize_p0 = " << spotSize_p0 << endl;
 cout << "waveLeng_p0 = " << waveLeng_p0 << endl;
 cout << "focus_p0 = " << focus_p0 << endl;
 cout << "amp_p0 = " << amp_p0 << endl;
*/

// Gaussian parameters for wave (1)
 spotSize_p1 = _spotSize_p1;
 waveLeng_p1 = _waveLeng_p1;
 focus_p1 = _focus_p1;
 amp_p1 = _amp_p1;

/*
//debug
 cout << "spotSize_p1 = " << spotSize_p1 << endl;
 cout << "waveLeng_p1 = " << waveLeng_p1 << endl;
 cout << "focus_p1 = " << focus_p1 << endl;
 cout << "amp_p1 = " << amp_p1 << endl;
*/

// Pulse parameters for wave (0)
 tdelay_p0 = _tdelay_p0;
 pulLeng_p0 = _pulLeng_p0;
 chirp_p0 = _chirp_p0;
 pulShp_p0 = _pulShp_p0;

/*
// debug
 cout << "tdelay_p0 = " << tdelay_p0 << endl;
 cout << "pulLeng_p0 = " << pulLeng_p0 << endl;
 cout << "chirp_p0 = " << chirp_p0 << endl;
 cout << "pulShp_p0 = " << pulShp_p0 << endl;
*/

// Pulse parameters for wave (1)
 tdelay_p1 = _tdelay_p1;
 pulLeng_p1 = _pulLeng_p1;
 chirp_p1 = _chirp_p1;
 pulShp_p1 = _pulShp_p1;

/*
// debug
 cout << "tdelay_p1 = " << tdelay_p1 << endl;
 cout << "pulLeng_p1 = " << pulLeng_p1 << endl;
 cout << "chirp_p1 = " << chirp_p1 << endl;
 cout << "pulShp_p1 = " << pulShp_p1<< endl;
*/

 dt = 0;
 tOld = 0;
 
 if(EFFlag)
    {
    EnergyFlux = &EnergyOut;
    EnergyOut =0.0;

    if (normal==1)
       shift = 0;
    else
    shift = -1;
    }

}

//--------------------------------------------------------------------
//	Apply boundary conditions to fields locally.


void PortGauss::applyFields(Scalar t, Scalar dt)
{

// Define pulse Shape function

 Scalar pulShpF(int pulShp, Scalar tdly, Scalar lengpul, Scalar t);
 Scalar PI = 3.14159265358979323846;
  

 if(dt==0) return;
 time = t- dt/2;
 grid = fields->get_grid();

 if (fields->getSub_Cycle_Iter() ==1) EnergyOut = 0.0;
 else if (fields->getSub_Cycle_Iter() ==0) EnergyOut = 0.0;

// Wave (0)
 zb_p0 = grid->getMKS(j1,k1).e2();
 raylLeng_p0 = PI*(spotSize_p0*spotSize_p0)/waveLeng_p0; 
 zp02_p0 = raylLeng_p0*raylLeng_p0;
 zp2_p0 = (zb_p0 - focus_p0)*(zb_p0 - focus_p0);
 wz_p0 = spotSize_p0*sqrt(1. + zp2_p0/zp02_p0);
 wz2_p0 = wz_p0*wz_p0;
 Rz_p0 = (zb_p0 - focus_p0) + zp02_p0/(zb_p0 - focus_p0);
 phig_p0 = atan((zb_p0-focus_p0)/raylLeng_p0)/2.;
 omeg_p0 = 2*PI*2.9979e8/waveLeng_p0;
 kg_p0 = 2.*PI/waveLeng_p0;

/*
// debug
 cout << "raylLeng_p0 = " << raylLeng_p0 << endl;
 cout << "wz_p0 = " << wz_p0 << endl;
 cout << "Rz_p0 = " << Rz_p0 << endl;
 cout << "phig_p0 = " << phig_p0 << endl;
 cout << " omeg_p0 = " << omeg_p0 << endl;
 cout << "kg_p0 = " << kg_p0 << endl;
*/

// Wave (1)
 zb_p1 = grid->getMKS(j1,k1).e2();
 raylLeng_p1 = PI*(spotSize_p1*spotSize_p1)/waveLeng_p1;
 zp02_p1 = raylLeng_p1*raylLeng_p1;
 zp2_p1 = (zb_p1 - focus_p1)*(zb_p1 - focus_p1);
 wz_p1 = spotSize_p1*sqrt(1. + zp2_p1/zp02_p1);
 wz2_p1 = wz_p1*wz_p1;
 Rz_p1 = (zb_p1 - focus_p1) + zp02_p1/(zb_p1 - focus_p1);
 phig_p1 = atan((zb_p1-focus_p1)/raylLeng_p1)/2.;
 omeg_p1 = 2*PI*2.9979e8/waveLeng_p1;
 kg_p1 = 2.*PI/waveLeng_p1;

/*
// debug
 cout << "raylLeng_p1 = " << raylLeng_p1 << endl;
 cout << "wz_p1 = " << wz_p1 << endl;
 cout << "Rz_p1 = " << Rz_p1 << endl;
 cout << "phig_p1 = " << phig_p1 << endl;
 cout << " omeg_p1 = " << omeg_p1 << endl;
 cout << "kg_p1 = " << kg_p1 << endl;
*/

 if (grid->query_geometry() == ZRGEOM)
    {
    cout << "Gaussian wave is not implemented for cylindrical geometry"
          << endl;
    }

 if (grid->query_geometry() == ZXGEOM)
    {
    if (alongx2())
       {
       a = (grid->getMKS(j2,k2) - grid->getMKS(j1,k1)).e2();


       A0 = pulShpF(pulShp_p0,tdelay_p0,pulLeng_p0,t);
       omgCh_p0 = omeg_p0 + chirp_p0*(t - tdelay_p0);
       kCh_p0 = omgCh_p0/2.9979e08;
/*
// debug
   cout << "omgCh_p0 = " << omgCh_p0 << endl;
   cout << "kCh_p0 = " << kCh_p0 << endl;
*/
       A1 = pulShpF(pulShp_p1,tdelay_p1,pulLeng_p1,t);
       omgCh_p1 = omeg_p1 + chirp_p1*(t - tdelay_p1);
       kCh_p1 = omgCh_p1/2.9979e08;
/*
// debug
   cout << "omgCh_p1 = " << omgCh_p1 << endl;
   cout << "kCh_p1 = " << kCh_p1 << endl;
*/

       for (int k = k1; k < k2; k++)
          {
          // Wave (0) - y polarization
          Scalar yforEy = (grid->getMKS(j1, k + 1).e2() +
          grid->getMKS(j1, k).e2())/2.0 - grid->getMKS(j1,k1).e2() -offset;
          Scalar rt20 = (yforEy - 0.5*a)*(yforEy - 0.5*a);
          Scalar Ey = (A0*amp_p0)*sqrt(spotSize_p0/wz_p0)*exp(-rt20/wz2_p0)*(
                 cos(0.5*kCh_p0*rt20/Rz_p0+kCh_p0*zb_p0-phig_p0)
                 *cos(omgCh_p0*(t - tdelay_p0))+
                  sin(0.5*kCh_p0*rt20/Rz_p0+kCh_p0*zb_p0-phig_p0)
		 *sin(omgCh_p0*(t - tdelay_p0)));
          Scalar intEy = grid->dl2(j1, k)*Ey;
          fields->setIntEdl(j1, k, 2, intEy);

          // Wave (1) - z polarization
          Scalar yforEz = grid->getMKS(j1, k).e2()-grid->getMKS(j1,k1).e2();
          Scalar rt21 = (yforEz - 0.5*a)*(yforEz - 0.5*a);
          Scalar Ez = (A1*amp_p1)*sqrt(spotSize_p1/wz_p1)*exp(-rt21/wz2_p1)*(
                  cos(0.5*kCh_p1*rt21/Rz_p1+kCh_p1*zb_p1-phig_p1)
                  *cos(omgCh_p1*(t - tdelay_p1))+
                  sin(0.5*kCh_p1*rt21/Rz_p1+kCh_p1*zb_p1-phig_p1)
                 *sin(omgCh_p1*(t - tdelay_p1)));
          Scalar intEz = grid->dl3(j1, k)*Ez;
          fields->setIntEdl(j1, k, 3, intEz);

	  if(EFFlag)
	     {
	     if (fields->getiC()[j2+normal][k].e2())
	       {
	       HonBoundary = (fields->getiL()[j2+shift][k].e3())*
	       (fields->getIntBdS()[j2+shift][k].e3()) + 
	       normal*.5*(fields->getIMesh(j2,k).e2() + 
	       get_time_value_deriv(time)/(fields->getiC()[j2+normal][k].e2()));
	       EnergyOut += normal*Ey*HonBoundary*grid->dS(j2+shift,k).e1();
	       }
	     }
          }
       }
    }

 if (fields->getFieldSub() == fields->getSub_Cycle_Iter()) EnergyOut
      /= (fields->getFieldSub());
}

Scalar pulShpF(int pulshp, Scalar tdel, Scalar lengP, Scalar t)
{

 Scalar Amp, tp;
 Scalar speedOfLight = 2.9979e08;
 Scalar PI = 3.14159265358979323846;

 tp = lengP/speedOfLight;

 switch(pulshp)
    {
      // Trapezoidal pulse
      case 0:

        if(t<tdel)
           Amp = 0.0;
        else if(t<tdel+tp*0.1)
           Amp = (10.0/tp)*(t - tdel);
        else if(t<tdel+0.9*tp)
           Amp = 1.0;
        else if(t<tdel+tp)
           Amp = (10.0/tp)*(tdel+tp-t); 
        else
           Amp = 0.0;

      break;
 
      // Gaussian pulse
      case 1:

	if(t<tdel)
           Amp = 0.0;
        else if(t<tdel+tp)
           Amp = exp(-4.0*(t-tdel-0.5*tp)*(t-tdel-0.5*tp)/(tp*tp));
        else
           Amp = 0.0;

      break;

      // Half-Sinusoidal pulse
      case 2:
         
         if(t<tdel)
            Amp = 0.0;
         else if(t< tdel+tp)
            Amp = sin(PI*(t-tdel)/tp);
         else
            Amp = 0.0;
      break;

      // Wide Gaussian pulse
      case 3:

	if(t<tdel-3*tp)
           Amp = 0.0;
        else if(t<tdel+3*tp)
          // DLB (Oct. 3, 2003)
          //           Amp = exp(-4.0*(t-tdel-0.5*tp)*(t-tdel-0.5*tp)/(tp*tp));
           Amp = exp( -(t-tdel)*(t-tdel) / (tp*tp) );
        else
           Amp = 0.0;

      break;

      // constant amplitude pulse
      case 4:

        if(t<tdel)
           Amp = 0.0;
        else if(t<tdel+tp)
           Amp = 1.0;
        else
           Amp = 0.0;

      break;


      default:
         Amp = 0.0;
      break;

    }

  return Amp;
}      
 

//--------------------------------------------------------------------
//	Set the passive bc for fields at the port.  Currently just a
//	copy of those for conductor.


void PortGauss::setPassives()
{

 if (alongx2())					//	vertical
    {

    for (int k=MIN(k1,k2); k<MAX(k1,k2); k++)
       {
       fields->set_iC2(j1, k, 0);       	
       fields->set_iC3(j1, k, 0);
       }

    fields->set_iC3(j1, k2, 0);
    }
}

//--------------------------------------------------------------------
//	Port::emit() simply deletes Particles in its stack.  May add some
//	diagnostics for particles collected in the future.

#if !defined __linux__ && !defined _WIN32
#pragma argsused
#endif

ParticleList& PortGauss::emit(Scalar t, Scalar dt, Species *species)
{
 while(!particleList.isEmpty())
    {
    Particle* p = particleList.pop();
    delete p;
    }

 return particleList;
}

