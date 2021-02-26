/*
====================================================================

MCMCLD.H  
MARKOV CHAIN MONTE CARLO LOAD 

====================================================================
*/

#ifndef	__MCMCLD_H
#define	__MCMCLD_H

#include "load.h"

class MCMCLoad : public Load
{


public:
	MCMCLoad(SpatialRegion* _SR,Scalar _density, Maxwellian& max, Scalar _zMinMKS, 
			  Scalar _zMaxMKS, Scalar _rMinMKS, Scalar _rMaxMKS, Species* _species,
			  Scalar _np2c));
};

#endif	//	ifndef __MCMCLD_H
