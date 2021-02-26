#ifndef	__PLASMASOURCEG_H
#define	__PLASMASOURCEG_H

/*
====================================================================

plasmaSourceg.h

1.0	(JohnV 05-18-96) Original code.
1.01  (PeterM 06-13-96) Analytic function added.
====================================================================
*/

#include "part1g.h"
#include "emitterg.h"

class PlasmaSource;

class PlasmaSourceParams : public EmitterParams
{
protected:
/******stuff for first species******/
/*	StringParameter	speciesName1;      //  ! use speciesName etc. from EmitterParams

	// specify the Maxwellian velocity distribution
	StringParameter units1;
  ScalarParameter temperature1;
	ScalarParameter v1thermal1, v2thermal1, v3thermal1;
  ScalarParameter Ucutoff1, v1Ucutoff1, v2Ucutoff1, v3Ucutoff1;
	ScalarParameter Lcutoff1, v1Lcutoff1, v2Lcutoff1, v3Lcutoff1;
  ScalarParameter v1drift1, v2drift1, v3drift1;
*/
/******stuff for second species******/
	StringParameter	speciesName2;      //	name of species2

	// specify the Maxwellian velocity distribution
	StringParameter units2;
  ScalarParameter temperature2;
	ScalarParameter v1thermal2, v2thermal2, v3thermal2;
	ScalarParameter Ucutoff2, v1Ucutoff2, v2Ucutoff2, v3Ucutoff2;
	ScalarParameter Lcutoff2, v1Lcutoff2, v2Lcutoff2, v3Lcutoff2;
	ScalarParameter v1drift2, v2drift2, v3drift2;
									 
	ScalarParameter sourceRate; 
	StringParameter analyticF;
public:
	PlasmaSourceParams(GridParams* _GP, SpatialRegionGroup* srg);
	virtual Boundary* CreateCounterPart();
};

#endif


