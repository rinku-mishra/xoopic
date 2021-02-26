/*
====================================================================

LASER_PLASMA_SRC_G.CPP

Purpose:  Defines LaserPlasmaSourceParams, which holds parameters for
          the LaserPlasmaSource algorithms.

Version:  $Id: laserplasmasrcg.cpp 1377 2000-11-08 19:10:11Z dad $

Revision History
1.0	 (JohnV 06-13-96) Original code.
1.01 (Bruhwiler, 10-04-2000) added delayTime.

====================================================================
*/

#include "laserplasmasrcg.h"
#include "laserplasmasrc.h"
#include "spatialg.h"
///\dad{begin}
#include "spbound.h"
#include "diags.h"
///\dad{end}

LaserPlasmaSourceParams::LaserPlasmaSourceParams(GridParams* _GP, SpatialRegionGroup* srg)
: PlasmaSourceParams(_GP, srg)
{
	name = "LaserPlasmaSource";

	velocity.setNameAndDescription("velocity","");
	velocity.setValue("2.99792458e+08");
	parameterList.add(&velocity);

	x1Flag.setNameAndDescription("x1Flag", "");
	x1Flag.setValue("1");
	parameterList.add(&x1Flag);

	pulseWidth.setNameAndDescription("pulseWidth","");
	pulseWidth.setValue("0");
	parameterList.add(&pulseWidth);

  delayTime.setNameAndDescription(ostring("delayTime"),
			  ostring("time delay before laser pulse begins"));
  delayTime.setValue(ostring("0."));
  parameterList.add(&delayTime);
}                                

Boundary* LaserPlasmaSourceParams::CreateCounterPart()
{
	UNIT type=MKS;
	/*****first species*****/
	Species* species1 = spatialRegionGroup->get_speciesPtr(speciesName.getValue());

	if (units.getValue()==(ostring)"MKS")
		type = MKS;
	else if (units.getValue()==(ostring)"EV")
		type = EV;
	Maxwellian* max1 = new Maxwellian(species1);
	if (temperature.getValue()==0)
		max1->setThermal(Vector3(v1thermal.getValue(),v2thermal.getValue(),
														 v3thermal.getValue()),type);
	else 
		max1->setThermal(temperature.getValue(),type);

	if ((Lcutoff.getValue()==0)&&(Ucutoff.getValue()==0))
		max1->setCutoff(Vector3(v1Lcutoff.getValue(),v2Lcutoff.getValue(),
														v3Lcutoff.getValue()),
										Vector3(v1Ucutoff.getValue(),v2Ucutoff.getValue(),
														v3Ucutoff.getValue()),type);
	else
		max1->setCutoff(Lcutoff.getValue(),Ucutoff.getValue(),type);
	
	max1->setDrift(Vector3(v1drift.getValue(), v2drift.getValue(),
												 v3drift.getValue()),type);

	/*****second species*****/
	Species* species2 = spatialRegionGroup->get_speciesPtr(speciesName2.getValue());

	if (units2.getValue()==(ostring)"MKS")
		type = MKS;
	else if (units2.getValue()==(ostring)"EV")
		type = EV;
	Maxwellian* max2 = new Maxwellian(species2);
	if (temperature2.getValue()==0)
		max2->setThermal(Vector3(v1thermal2.getValue(),v2thermal2.getValue(),
														 v3thermal2.getValue()),type);
	else 
		max2->setThermal(temperature2.getValue(),type);

	if ((Lcutoff2.getValue()==0)&&(Ucutoff2.getValue()==0))
		max2->setCutoff(Vector3(v1Lcutoff2.getValue(),v2Lcutoff2.getValue(),
														v3Lcutoff2.getValue()),
										Vector3(v1Ucutoff2.getValue(),v2Ucutoff2.getValue(),
														v3Ucutoff2.getValue()),type);
	else
		max2->setCutoff(Lcutoff2.getValue(),Ucutoff2.getValue(),type);

	max2->setDrift(Vector3(v1drift2.getValue(), v2drift2.getValue(),
												 v3drift2.getValue()),type);
	Vector2 v;
	if (x1Flag.getValue() > 0) v.set_e1(velocity.getValue());
	else v.set_e2(velocity.getValue());

  Boundary* B = new LaserPlasmaSource(max1, max2,
                                      sourceRate.getValue(),
                                      SP.GetLineSegments(), 
                                      np2c.getValue(),
                                      analyticF.getValue(), v,
                                      pulseWidth.getValue(),
                                      delayTime.getValue() );
	return B;
}                   

