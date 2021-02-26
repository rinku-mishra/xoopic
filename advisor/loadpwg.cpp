// loadg.cpp

#include "loadpwg.h"
#include "spatialg.h"
///\dad{begin}
#include "spbound.h"
///\dad{end}
#ifdef UNIX
#include "ptclgrp.h"
#include "diags.h"
#endif
#include "loadpw.h"

//================= LoadParams Class

LoadPWParams::LoadPWParams(SpatialRegionGroup* srg)
: LoadParams(srg)
{
   name = "Load_PW";
   spatialRegionGroup = srg;
   /* can't make it backwards compatable,
      new geometry indepenant variables*/


   parameterList.add(&Name);
   Name.setNameAndDescription(ostring("Name"),ostring("name of this load"));
   Name.setValue(ostring("shiftLoad"));  // The whole point of this
   // is to load this in moving window

   pwFrequency.setNameAndDescription(ostring("pwFrequency"),
                                     ostring("frequency of input wave."));
   pwFrequency.setValue("0");
   parameterList.add(&pwFrequency);

   pwPhase.setNameAndDescription(ostring("pwPhase"),
                                 ostring("phase of input wave."));
   pwPhase.setValue("0");
   parameterList.add(&pwPhase);
   
   pwWavenumber.setNameAndDescription(ostring("pwWavenumber"),
                                      ostring("wave number(k) of input wave."));
   pwWavenumber.setValue("0");
   parameterList.add(&pwWavenumber);
   
   
   pwAmplitude.setNameAndDescription(ostring("pwAmplitude"),
                                     ostring("amplitude of input wave."));
   pwAmplitude.setValue("0");
   parameterList.add(&pwAmplitude);


   pwDelay.setNameAndDescription(ostring("pwDelay"),
                                 ostring("delay of input wave."));
   pwDelay.setValue("0");
   parameterList.add(&pwDelay);

   

   
};

Load* LoadPWParams::CreateCounterPart(SpatialRegion* SP) /* throw(Oops) */ {
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

   loader = new LoadPlaneWave(region,density.getValue(), maxwellian, 
                     x1MinMKS.getValue(), x1MaxMKS.getValue(), 
                     x2MinMKS.getValue(), x2MaxMKS.getValue(),
                     LoadMethodFlag.getValue(),
                     np2c.getValue(),analyticF.getValue(),
                              densityfile.getValue(),Name.getValue(),
                              pwFrequency.getValue(),
                              pwPhase.getValue(),
                              pwWavenumber.getValue(),
                              pwAmplitude.getValue(),
                              pwDelay.getValue()
                              );
   if (!WasDumpFileGiven) loader->load_it();
   region->theLoads->add(loader);
   //delete maxwellian;
   return loader;
}



