// loadg.cpp

#include "loadg.h"
#include "spatialg.h"
#include "spbound.h"
#include "ptclgrp.h"
#include "diags.h"

//================= LoadParams Class

LoadParams::LoadParams(SpatialRegionGroup* srg)
	 : ParameterGroup()
{
  name = "Load";
  spatialRegionGroup = srg;
	/* can't make it backwards compatable,
	 new geometry indepenant variables*/

	v1drift.setNameAndDescription(ostring("v1drift"),
				ostring("drift velocity in direction 1"));
  v1drift.setValue(ostring("0.0"));
  v2drift.setNameAndDescription(ostring("v2drift"),
				ostring("drift velocity in direction 2"));
  v2drift.setValue(ostring("0.0"));
  v3drift.setNameAndDescription(ostring("v3drift"),
				 ostring("drift velocity in direction 3"));
  v3drift.setValue(ostring("0.0"));
	
	temperature.setNameAndDescription(ostring("temperature"),
				ostring("isotropic thermal velocity"));
  temperature.setValue(ostring("0.0"));
	v1thermal.setNameAndDescription(ostring("v1thermal"),
				ostring("thermal velocity in direction 1"));
  v1thermal.setValue(ostring("0.0"));
  v2thermal.setNameAndDescription(ostring("v2thermal"),
				ostring("thermal velocity in direction 2"));
  v2thermal.setValue(ostring("0.0"));
  v3thermal.setNameAndDescription(ostring("v3thermal"),
				ostring("thermal velocity in direction 3"));
  v3thermal.setValue(ostring("0.0"));

  Ucutoff.setNameAndDescription("Ucutoff","Upper isotropic cutoff");
  Ucutoff.setValue(ostring("0.0"));
	v1Ucutoff.setNameAndDescription("v1Ucutoff","Upper velocity cutoff in direction 1");
  v1Ucutoff.setValue(ostring("0.0"));
	v2Ucutoff.setNameAndDescription("v2Ucutoff","Upper velocity cutoff in direction 2");
  v2Ucutoff.setValue(ostring("0.0"));
	v3Ucutoff.setNameAndDescription("v3Ucutoff","Upper velocity cutoff in direction 3");
  v3Ucutoff.setValue(ostring("0.0"));

  Lcutoff.setNameAndDescription("Lcutoff","isotropic cutoff");
  Lcutoff.setValue(ostring("0.0"));
	v1Lcutoff.setNameAndDescription("v1Lcutoff","Lower velocity cutoff in direction 1");
  v1Lcutoff.setValue(ostring("0.0"));
	v2Lcutoff.setNameAndDescription("v2Lcutoff","Lower velocity cutoff in direction 2");
  v2Lcutoff.setValue(ostring("0.0"));
	v3Lcutoff.setNameAndDescription("v3Lcutoff","Lower velocity cutoff in direction 3");
  v3Lcutoff.setValue(ostring("0.0"));

  density.setNameAndDescription(ostring("density"),
			  ostring("uniform density of real particles"));
  density.setValue(ostring("0.0"));

	x1MinMKS.setNameAndDescription(ostring("x1MinMKS"),
			  ostring("minimun x1 postion to start load"));
  x1MinMKS.setValue(ostring("0.0"));
  x1MaxMKS.setNameAndDescription(ostring("x1MaxMKS"),
			  ostring("maximun z postion to start load"));
  x1MaxMKS.setValue(ostring("0.0"));
  x2MinMKS.setNameAndDescription(ostring("x2MinMKS"),
			  ostring("minimun r postion to start load"));
  x2MinMKS.setValue(ostring("0.0"));
  x2MaxMKS.setNameAndDescription(ostring("x2MaxMKS"),
			  ostring("maximun r postion to start load"));
  x2MaxMKS.setValue(ostring("0.0"));

  np2c.setNameAndDescription(ostring("np2c"),
			  ostring("number of computer particles to real particles"));
  np2c.setValue(ostring("1e6")); 
  speciesName.setNameAndDescription(ostring("speciesName"), 
											 ostring("unique species name"));
  speciesName.setValue("NULL");

  analyticF.setNameAndDescription(ostring("analyticF"),
			  ostring("Analytic function of x1,x2 describing the particle density"));
  analyticF.setValue(ostring("0.0"));

  parameterList.add(&Name);
  Name.setNameAndDescription(ostring("Name"),ostring("name of this load"));
  Name.setValue(ostring("noname"));
	 

	densityfile.setNameAndDescription(ostring("densityfile"), 
				ostring("file containing density"));
	densityfile.setValue("NULL");

	LoadMethodFlag.setNameAndDescription(ostring("LoadMethodFlag"), 
				ostring("Flag to control loading method, RANDOM or QUIET_START."));
	LoadMethodFlag.setValue("RANDOM");

	units.setNameAndDescription(ostring("units"),
				ostring("units of ALL velocities for maxwellian (MKS,EV)"));
	units.setValue("MKS");

  parameterList.add(&speciesName);

	parameterList.add(&v1drift);
  parameterList.add(&v2drift);
  parameterList.add(&v3drift);

  parameterList.add(&Ucutoff);
	parameterList.add(&v1Ucutoff);
	parameterList.add(&v2Ucutoff);
	parameterList.add(&v3Ucutoff);

  parameterList.add(&Lcutoff);
	parameterList.add(&v1Lcutoff);
	parameterList.add(&v2Lcutoff);
	parameterList.add(&v3Lcutoff);

  parameterList.add(&temperature);
	parameterList.add(&v1thermal);
  parameterList.add(&v2thermal);
  parameterList.add(&v3thermal);

	parameterList.add(&units);

  parameterList.add(&x1MinMKS);
  parameterList.add(&x1MaxMKS);
  parameterList.add(&x2MinMKS);
  parameterList.add(&x2MaxMKS);

	parameterList.add(&density);
	parameterList.add(&LoadMethodFlag);
  parameterList.add(&np2c);
	parameterList.add(&densityfile);
  parameterList.add(&analyticF);
	
};

Load* LoadParams::CreateCounterPart(SpatialRegion * SP) 
  /* throw(Oops) */ {
	region = SP;
	Load* loader;
	Species* species = spatialRegionGroup->get_speciesPtr(
		speciesName.getValue());
	Maxwellian* maxwellian;

	UNIT type=MKS;
	if (units.getValue()==(ostring)"MKS") type = MKS;
	else if (units.getValue()==(ostring)"EV") type = EV;
        else {
          std::string msg = "LoadParams::CreateCounterPart: units '";
	  msg += units.getValue().c_str();
	  msg += "' unknown.";
          Oops oops(msg);
	  throw oops;
        }
	maxwellian = new Maxwellian(species);
	if (temperature.getValue()==0) maxwellian->setThermal(
		Vector3(v1thermal.getValue(),v2thermal.getValue(), 
		v3thermal.getValue()),type);
	else maxwellian->setThermal(temperature.getValue(),type);

	if ((Ucutoff.getValue()==0)&&(Lcutoff.getValue()==0))
		maxwellian->setCutoff(Vector3(v1Lcutoff.getValue(),v2Lcutoff.getValue(),
																	v3Lcutoff.getValue()),
													Vector3(v1Ucutoff.getValue(),v2Ucutoff.getValue(),
																	v3Ucutoff.getValue()),type);
	else
		maxwellian->setCutoff(Lcutoff.getValue(),Ucutoff.getValue(),type);
	
	maxwellian->setDrift(Vector3(v1drift.getValue(), v2drift.getValue(),
															 v3drift.getValue()),type);
  try{
	  loader = new Load(region,density.getValue(), maxwellian, 
										x1MinMKS.getValue(), x1MaxMKS.getValue(), 
										x2MinMKS.getValue(), x2MaxMKS.getValue(),
										LoadMethodFlag.getValue(),
										np2c.getValue(),analyticF.getValue(),
                    densityfile.getValue(),Name.getValue());
  }
  catch(Oops& oops){
    oops.prepend("LoadParams::CreateCounterPart: Error: \n"); // not called
    throw oops;
  }


  if (!WasDumpFileGiven) {
    try{
      loader->load_it();
    }
    catch(Oops& oops){
      oops.prepend("LoadParams::CreateCounterPart: Error: \n"); //OK
      throw oops;
    }
  }

  region->theLoads->add(loader);
	//delete maxwellian;
	return loader;
}

void LoadParams::checkRules()
{
	ostring name = speciesName.getValue();
	if (name == (ostring)"")
		ruleMessages.add(new ostring("speciesName is required!"));
	else if (!spatialRegionGroup->KnownSpecies(name))
	{
		char buffer[512];
		sprintf(buffer, "Unrecognized speciesName: %s", name.c_str());
		ruleMessages.add(new ostring(buffer));
	}
}


