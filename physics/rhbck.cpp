/*
====================================================================

LOAD.CPP

====================================================================
*/

#include "sptlrgn.h"
#include "ptclgrp.h"
#include "maxwelln.h"
#include "load.h"
#include "oopiclist.h"
#include "misc.h"

#include <txc_streams.h>
using namespace std;

Scalar Load::F(Vector2 x) 
{
return ChargeDensity;
}

BackLoad::BakcLoad(SpatialRegion* _SR,Scalar _ChargeDensity, Scalar _zMinMKS, 
			  Scalar _zMaxMKS, Scalar _rMinMKS, Scalar _rMaxMKS, Scalar _a,
			  Scalar _b, Scalar _c)
{
 	region = _SR;
	grid = region->get_grid();
	fields = region->get_fields();
	zMinMKS = _zMinMKS;
	zMaxMKS = _zMaxMKS;
	rMinMKS = _rMinMKS;
	rMaxMKS = _rMaxMKS;
	ChargeDensity = _ChargeDensity;
	a = _a;
	b = _b;
	c = _c;
	J = grid->getJ();
	K = grid->getK();
	gridMin = grid->getGridCoords(Vector2(zMinMKS,rMinMKS));
	gridMax = grid->getGridCoords(Vector2(zMaxMKS,rMaxMKS));
	RhoBack = new Scalar *[J+1];
	for (int j=0; j<=J; j++)
			RhoBack[j] = new Scalar[K+1];

}

BackLoad::add_to(**rho)
{
	int j,k;
	for(j=gridMin.e1();j<=greidMax.e1();j++) {
		for(k=gridMin.e2();k<=gridMax.e2();k++) {
				rho[j][k]+=F(grid->getGridCoords(Vector2(j,k)));
		}
	}	
}

BackLoad::load_it()
{
	for(j=gridMin.e1();j<=greidMax.e1();j++) {
		for(k=gridMin.e2();k<=gridMax.e2();k++) {
			RhoBack[j][k]=F(grid->getGridCoords(Vector2(j,k)));
			rho[j][k]+=RhoBack[j][k];
		}  
	}
	if(!ElectrostaticFlag)
		EMstart(rho);
}
      
      
  
  



