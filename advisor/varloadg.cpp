// varloadg.cpp

#include "varloadg.h"
#include "spatialg.h"
///\dad{begin}
#include "spbound.h"
///\dad{end}
#ifdef UNIX
#include "ptclgrp.h"
#include "diags.h"
#endif

//================= VarWeightLoadParams Class

Load* VarWeightLoadParams::CreateCounterPart(SpatialRegion * SP)
	/* throw(Oops) */ {
	region = SP;
	Load* loader;
	Species* species = spatialRegionGroup->get_speciesPtr(speciesName.getValue());
	Maxwellian* maxwellian;

	UNIT type=MKS;
	if (units.getValue()==(ostring)"MKS")
		type = MKS;
	else if (units.getValue()==(ostring)"EV")
		type = EV;
	maxwellian = new Maxwellian(species);
	if (temperature.getValue()==0)
		maxwellian->setThermal(Vector3(v1thermal.getValue(),v2thermal.getValue(),
																	 v3thermal.getValue()),type);
	else 
		maxwellian->setThermal(temperature.getValue(),type);

	if ((Ucutoff.getValue()==0)&&(Lcutoff.getValue()==0))
		maxwellian->setCutoff(Vector3(v1Lcutoff.getValue(),v2Lcutoff.getValue(),
																	v3Lcutoff.getValue()),
													Vector3(v1Ucutoff.getValue(),v2Ucutoff.getValue(),
																	v3Ucutoff.getValue()),type);
	else
		maxwellian->setCutoff(Lcutoff.getValue(),Ucutoff.getValue(),type);
	
	maxwellian->setDrift(Vector3(v1drift.getValue(), v2drift.getValue(),
															 v3drift.getValue()),type);

	loader = new VarWeightLoad(region,density.getValue(), maxwellian, 
                             x1MinMKS.getValue(), x1MaxMKS.getValue(), 
										         x2MinMKS.getValue(), x2MaxMKS.getValue(),
										         LoadMethodFlag.getValue(),
										         np2c.getValue(),analyticF.getValue(),
										         densityfile.getValue(),Name.getValue());
  if (!WasDumpFileGiven){
    try{ 
      loader->load_it();
    }
    catch(Oops& oops){
      oops.prepend("VarWeightLoadParams::CreateCounterPart: Error:\n"); //notcalled
      throw oops;
    }
  }
	region->theLoads->add(loader);  // for the movingWindow code.
	//delete maxwellian;  // need this maxwellian later for movingWindow
	return loader;

}
