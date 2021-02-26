//emitterg.cpp

#include "emitterg.h"
#include "spatialg.h"
///\dad{begin}
#include "spbound.h"
#include "diags.h"
///\dad{end}
//=================== EmitterParams Class

EmitterParams::EmitterParams(GridParams* _GP, SpatialRegionGroup* srg)
	: DielectricParams(_GP, srg)
{  
	name = "Emitter";

	speciesName.setNameAndDescription("speciesName",	"name of species to emit");
	speciesName.setValue("NULL");
	parameterList.add(&speciesName);

	v1drift.setNameAndDescription("v1drift","drift velocity in direction 1");
	v1drift.setValue(ostring("0.0"));
	parameterList.add(&v1drift);
	v2drift.setNameAndDescription("v2drift","drift velocity in direction 2");
	v2drift.setValue(ostring("0.0"));
	parameterList.add(&v2drift);
  v3drift.setNameAndDescription("v3drift","drift velocity in direction 3");
  v3drift.setValue(ostring("0.0"));
	parameterList.add(&v3drift);

	temperature.setNameAndDescription("temperature","isotropic thermal velocity");
  temperature.setValue(ostring("0.0"));
	parameterList.add(&temperature);
	v1thermal.setNameAndDescription("v1thermal","thermal velocity in direction 1");
  v1thermal.setValue(ostring("0.0"));
	parameterList.add(&v1thermal);
  v2thermal.setNameAndDescription("v2thermal","thermal velocity in direction 2");
  v2thermal.setValue(ostring("0.0"));
	parameterList.add(&v2thermal);
  v3thermal.setNameAndDescription("v3thermal","thermal velocity in direction 3");
  v3thermal.setValue(ostring("0.0"));
	parameterList.add(&v3thermal);

  Lcutoff.setNameAndDescription("Lcutoff","Lower cutoff");
  Lcutoff.setValue(ostring("0.0"));
	parameterList.add(&Lcutoff);
	v1Lcutoff.setNameAndDescription("v1Lcutoff","Lower velocity cutoff in direction 1");
	v1Lcutoff.setValue(ostring("0.0"));
	parameterList.add(&v1Lcutoff);
	v2Lcutoff.setNameAndDescription("v2Lcutoff","Lower velocity cutoff in direction 2");
  v2Lcutoff.setValue(ostring("0.0"));
	parameterList.add(&v2Lcutoff);
	v3Lcutoff.setNameAndDescription("v3Lcutoff","Lower velocity cutoff in direction 3");
  v3Lcutoff.setValue(ostring("0.0"));
	parameterList.add(&v3Lcutoff);

  Ucutoff.setNameAndDescription("Ucutoff","Upper cutoff");
  Ucutoff.setValue(ostring("0.0"));
	parameterList.add(&Ucutoff);
	v1Ucutoff.setNameAndDescription("v1Ucutoff","Upper velocity cutoff in direction 1");
	v1Ucutoff.setValue(ostring("0.0"));
	parameterList.add(&v1Ucutoff);
	v2Ucutoff.setNameAndDescription("v2Ucutoff","Upper velocity cutoff in direction 2");
  v2Ucutoff.setValue(ostring("0.0"));
	parameterList.add(&v2Ucutoff);
	v3Ucutoff.setNameAndDescription("v3Ucutoff","Upper velocity cutoff in direction 3");
  v3Ucutoff.setValue(ostring("0.0"));
	parameterList.add(&v3Ucutoff);

	units.setNameAndDescription(ostring("units"),
				ostring("units of ALL velocities for maxwellian (MKS,EV)"));
	units.setValue("MKS");
	parameterList.add(&units);

	np2c.setNameAndDescription("np2c",
			 "Number of physical particles per computer particle");
	np2c.setValue(ostring("1E6"));
	parameterList.add(&np2c);
}

// JohnV 05-11-95 added this to determine whether the emitter is valid.
//	modified 06-28-95 to handle string speciesName
//  KC 01-16-97 modified to put the maxwellian here

void EmitterParams::checkRules()
{
	DielectricParams::checkRules();		//	perform default boundary check
	
	ostring name = speciesName.getValue();
	if (name == (ostring)"NULL")
		ruleMessages.add(new ostring("speciesName is required!"));
	else if (!spatialRegionGroup->KnownSpecies(name))
	{
		char buffer[512];
		sprintf(buffer, "Unrecognized speciesName: %s", name.c_str());
		ruleMessages.add(new ostring(buffer));
	}
}

Boundary* EmitterParams::CreateCounterPart()  //sets up distribution and species
{
	species = spatialRegionGroup->get_speciesPtr(speciesName.getValue());

	UNIT type=MKS;
	if (units.getValue()==(ostring)"MKS")
		type = MKS;
	else if (units.getValue()==(ostring)"EV")
		type = EV;
	distribution = new MaxwellianFlux(species);
	if (temperature.getValue()==0)
		distribution->setThermal(Vector3(v1thermal.getValue(),v2thermal.getValue(),
																	 v3thermal.getValue()),type);
	else 
		distribution->setThermal(temperature.getValue(),type);

	if ((Lcutoff.getValue()==0)&&(Ucutoff.getValue()==0))
		distribution->setCutoff(Vector3(v1Lcutoff.getValue(),v2Lcutoff.getValue(),
																		v3Lcutoff.getValue()),
														Vector3(v1Ucutoff.getValue(),v2Ucutoff.getValue(),
																		v3Ucutoff.getValue()),type);
	else
		distribution->setCutoff(Lcutoff.getValue(),Ucutoff.getValue(),type);
	
	distribution->setDrift(Vector3(v1drift.getValue(), v2drift.getValue(),
																 v3drift.getValue()),type);
	return NULL;
}
