
#ifndef	__LOAD_PLANE_WAVE_H
#define	__LOAD_PLANE_WAVE_H

/*
====================================================================

load.h

Purpose:	Loads particles at start-up.

Version:	$Id: loadpw.h 2196 2004-08-28 00:03:50Z yew $

2.01 (Bruhwiler 10-28-99) Modified for new derived class VarWeightLoad

====================================================================
*/

#include "ovector.h"
#include "misc.h"
#include "maxwelln.h"
#include "ptclgrp.h"
#include "particle.h"
#include "species.h"
#include "eval.h"
#include "load.h"

//  PLANE_WAVE load is specific to the moving window.  It uses
//  the quiet start but modifies the particle velocities.


//--------------------------------------------------------------------
//	load:  This class loads the particles in at t=0.

class LoadPlaneWave: public Load
{
 protected:
  
  Scalar pwFrequency, pwPhase, pwWavenumber, pwAmplitude, pwDelay;


 public:

	ostring name;

	LoadPlaneWave(SpatialRegion* SP, Scalar Density, Maxwellian* max, Scalar zMinMKS, Scalar zMaxMKS, Scalar rMinMKS, Scalar rMaxMKS, int LoadMethodFlag, Scalar np2c, const ostring &analyticF, const ostring &DensityFile,const ostring &name_in,
                    Scalar _pwFrequency,Scalar _pwPhase, Scalar _pwWavenumber,
                             Scalar pwAmplitude, Scalar pwDelay              );
	virtual ~LoadPlaneWave() {};
        virtual Scalar F(Vector2& x);
	virtual int load_it() /* throw(Oops) */;
};

#endif  // ifndef __LOAD_PLANE_WAVE_H
