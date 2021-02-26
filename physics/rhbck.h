//====================================================================

#ifndef	__LOAD_H
#define	__LOAD_H

#include	"ovector.h"
#include	"misc.h"
#include        "maxwelln.h"
#include        "ptclgrp.h"
#include        "particle.h"
#include "species.h"

//--------------------------------------------------------------------
//	load:  This class loads the particles in at t=0.

class BackLoad
{
  	Scalar zMinMKS, zMaxMKS, rMinMKS, rMaxMKS;
	Scalar ChargeDensity;
	Scalar a,b,c; //These are parameters that the user can use quickly
	Fields* fields;
	Grid* grid;
	Scalar** RhoBack;
	int J,K;
	Scalar q;
	SpatialRegion* region;
 public:
	BackLoad(SpatialRegion* SP, Scalar ChargeDensity, Scalar zMinMKS, Scalar zMaxMKS, Scalar rMinMKS, Scalar rMaxMKS, Scalar a, Scalar b, Scalar c);
	Scalar F(Vector2 x);
	int load_it();
};
#endif  // ifndef __LOAD_H
