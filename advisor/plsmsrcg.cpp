#include "plsmsrcg.h"
#include "plsmasrc.h"
#include "spatialg.h"
#include "maxwelln.h"
///\dad{begin}
#include "spbound.h"
#include "diags.h"
///\dad{end}
PlasmaSourceParams::PlasmaSourceParams(GridParams* _GP, SpatialRegionGroup* srg)
: EmitterParams(_GP, srg)
{
	name = "PlasmaSource";

	/******* all of this is for the first species ******/
	speciesName.setNameAndDescription("speciesName1",	"name of first species");
	speciesName.setValue("NULL");
	parameterList.add(&speciesName);

	v1drift.setNameAndDescription("v1drift1","drift velocity in direction 1");
	v1drift.setValue(ostring("0.0"));
	parameterList.add(&v1drift);
	v2drift.setNameAndDescription("v2drift1","drift velocity in direction 2");
	v2drift.setValue(ostring("0.0"));
	parameterList.add(&v2drift);
  v3drift.setNameAndDescription("v3drift1","drift velocity in direction 3");
  v3drift.setValue(ostring("0.0"));
	parameterList.add(&v3drift);

	temperature.setNameAndDescription("temperature1","isotropic thermal velocity");
  temperature.setValue(ostring("0.0"));
	parameterList.add(&temperature);
	v1thermal.setNameAndDescription("v1thermal1","thermal velocity in direction 1");
  v1thermal.setValue(ostring("0.0"));
	parameterList.add(&v1thermal);
  v2thermal.setNameAndDescription("v2thermal1","thermal velocity in direction 2");
  v2thermal.setValue(ostring("0.0"));
	parameterList.add(&v2thermal);
  v3thermal.setNameAndDescription("v3thermal1","thermal velocity in direction 3");
  v3thermal.setValue(ostring("0.0"));
	parameterList.add(&v3thermal);

  Ucutoff.setNameAndDescription("Ucutoff1","Ucutoff");
  Ucutoff.setValue(ostring("0.0"));
	parameterList.add(&Ucutoff);
	v1Ucutoff.setNameAndDescription("v1Ucutoff1","Upper velocity cutoff in direction 1");
	v1Ucutoff.setValue(ostring("0.0"));
	parameterList.add(&v1Ucutoff);
	v2Ucutoff.setNameAndDescription("v2Ucutoff1","Upper velocity cutoff in direction 2");
  v2Ucutoff.setValue(ostring("0.0"));
	parameterList.add(&v2Ucutoff);
	v3Ucutoff.setNameAndDescription("v3Ucutoff1","Upper velocity cutoff in direction 3");
  v3Ucutoff.setValue(ostring("0.0"));
	parameterList.add(&v3Lcutoff);

	Lcutoff.setNameAndDescription("Lcutoff1","Lcutoff");
  Lcutoff.setValue(ostring("0.0"));
	parameterList.add(&Lcutoff);
	v1Lcutoff.setNameAndDescription("v1Lcutoff1","Lower velocity cutoff in direction 1");
	v1Lcutoff.setValue(ostring("0.0"));
	parameterList.add(&v1Lcutoff);
	v2Lcutoff.setNameAndDescription("v2Lcutoff1","Lower velocity cutoff in direction 2");
  v2Lcutoff.setValue(ostring("0.0"));
	parameterList.add(&v2Lcutoff);
	v3Lcutoff.setNameAndDescription("v3Lcutoff1","Lower velocity cutoff in direction 3");
  v3Lcutoff.setValue(ostring("0.0"));
	parameterList.add(&v3Lcutoff);

	units.setNameAndDescription(ostring("units1"),
															 ostring("units of ALL velocities for maxwellian (MKS,EV)"));
	units.setValue("MKS");
	parameterList.add(&units);

	/********* this stuff is for the second species *********/
	speciesName2.setNameAndDescription("speciesName2",	"name of species to emit");
	speciesName2.setValue("NULL");
	parameterList.add(&speciesName2);

	v1drift2.setNameAndDescription("v1drift2","drift velocity in direction 1");
	v1drift2.setValue(ostring("0.0"));
	parameterList.add(&v1drift2);
	v2drift2.setNameAndDescription("v2drift2","drift velocity in direction 2");
	v2drift2.setValue(ostring("0.0"));
	parameterList.add(&v2drift2);
  v3drift2.setNameAndDescription("v3drift2","drift velocity in direction 3");
  v3drift2.setValue(ostring("0.0"));
	parameterList.add(&v3drift2);

	temperature2.setNameAndDescription("temperature2","isotropic thermal velocity");
  temperature2.setValue(ostring("0.0"));
	parameterList.add(&temperature2);
	v1thermal2.setNameAndDescription("v1thermal2","thermal velocity in direction 1");
  v1thermal2.setValue(ostring("0.0"));
	parameterList.add(&v1thermal2);
  v2thermal2.setNameAndDescription("v2thermal2","thermal velocity in direction 2");
  v2thermal2.setValue(ostring("0.0"));
	parameterList.add(&v2thermal2);
  v3thermal2.setNameAndDescription("v3thermal2","thermal velocity in direction 3");
  v3thermal2.setValue(ostring("0.0"));
	parameterList.add(&v3thermal2);

  Ucutoff2.setNameAndDescription("Ucutoff2","Ucutoff");
  Ucutoff2.setValue(ostring("0.0"));
	parameterList.add(&Ucutoff2);
	v1Ucutoff2.setNameAndDescription("v1Ucutoff2","Upper velocity cutoff in direction 1");
	v1Ucutoff2.setValue(ostring("0.0"));
	parameterList.add(&v1Ucutoff2);
	v2Ucutoff2.setNameAndDescription("v2Ucutoff2","Upper velocity cutoff in direction 2");
  v2Ucutoff2.setValue(ostring("0.0"));
	parameterList.add(&v2Ucutoff2);
	v3Ucutoff2.setNameAndDescription("v3Ucutoff2","Upper velocity cutoff in direction 3");
  v3Ucutoff2.setValue(ostring("0.0"));
	parameterList.add(&v3Lcutoff2);

	Lcutoff2.setNameAndDescription("Lcutoff2","Lcutoff");
  Lcutoff2.setValue(ostring("0.0"));
	parameterList.add(&Lcutoff2);
	v1Lcutoff2.setNameAndDescription("v1Lcutoff2","Lower velocity cutoff in direction 1");
	v1Lcutoff2.setValue(ostring("0.0"));
	parameterList.add(&v1Lcutoff2);
	v2Lcutoff2.setNameAndDescription("v2Lcutoff2","Lower velocity cutoff in direction 2");
  v2Lcutoff2.setValue(ostring("0.0"));
	parameterList.add(&v2Lcutoff2);
	v3Lcutoff2.setNameAndDescription("v3Lcutoff2","Lower velocity cutoff in direction 3");
  v3Lcutoff2.setValue(ostring("0.0"));
	parameterList.add(&v3Lcutoff2);

	units2.setNameAndDescription(ostring("units2"),
															 ostring("units of ALL velocities for second species maxwellian (MKS,EV)"));
	units.setValue("MKS");
	parameterList.add(&units2);

	/*********stuff for both species**********/
	analyticF.setNameAndDescription("analyticF", "analytic distribution functions");
	analyticF.setValue("0.0");
	parameterList.add(&analyticF);
	sourceRate.setNameAndDescription("sourceRate", "rate of plasma generation m^-3 s^-1");
	sourceRate.setValue("0.0");
	parameterList.add(&sourceRate);
	np2c.setNameAndDescription("np2c", "ratio of physical to computer particles");
	np2c.setValue("1E6");
	parameterList.add(&np2c);
}                                

Boundary* PlasmaSourceParams::CreateCounterPart()
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

	Boundary* B = new PlasmaSource(max1, max2, sourceRate.getValue(),
											 SP.GetLineSegments(), 
											 np2c.getValue(),analyticF.getValue());
	return B;
}                   

