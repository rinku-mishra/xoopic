
#ifndef	__LOAD_H
#define	__LOAD_H

/*
====================================================================

load.h

Purpose:	Loads particles at start-up.

Version:	$Id: load.h 2193 2004-08-27 22:54:57Z yew $

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
#include <oops.h>


enum LoadMethods {RANDOM, QUIET_START, UNIFORM, PLANE_WAVE};

//--------------------------------------------------------------------
//	load:  This class loads the particles in at t=0.

class Load
{
 protected:

	Scalar Min1MKS, Max1MKS, Min2MKS, Max2MKS;
	Vector2 deltaP,p1,p2;
	int rz;
	int J,K;
	int LoadMethodFlag;
	Species* species;
	Fields* fields;
	Grid* grid;
	Scalar q;
	int numberParticles;
	Scalar density, np2c;
	Scalar rMin, rMax, rMinSqr, drSqr;
	Scalar volume;
	Scalar **gridded_density;
	Scalar *rmax_fn_z;
	Maxwellian* maxwellian;
	ParticleList particleList;
	SpatialRegion* region;
	ostring analyticF; 
	ostring DensityFile;

   void set_coefficients();
	void init_evaluator();
	void init_fajans_load() /* throw(Oops) */;
	void init_default();
	void IntegrateGriddedLoad(void);
	int fajans_load(Vector2& x);
	void init_file_load() /* throw(Oops) */;
	Scalar gridded_load(Vector2& x);
//	Scalar file_load(Vector2& x);
//	Scalar evaluator_load(Vector2& x);
virtual	Scalar F(Vector2& x);

 public:

	ostring name;

	Load(SpatialRegion* SP, Scalar Density, Maxwellian* max, Scalar zMinMKS, Scalar zMaxMKS, 
    Scalar rMinMKS, Scalar rMaxMKS, int LoadMethodFlag, Scalar np2c, const ostring &analyticF, 
    const ostring &DensityFile,const ostring &name_in) /* throw(Oops) */;
	virtual ~Load();
	virtual int load_it() /* throw(Oops) */;
};

#endif  // ifndef __LOAD_H
