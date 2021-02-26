//portg.h

#ifndef   __PORTG_TEM_MW_G_H
#define   __PORTG_TEM_MW_G_H \

#include "part1g.h"
#include "boundg.h"

class SpatialRegion;
//=================== PortParams Class
// base class for all port and gap boundaries
// abstract

class PortTEM_MW_Params : public BoundaryParams
{public:
    PortTEM_MW_Params(GridParams* _GP, SpatialRegionGroup* srg)
       : BoundaryParams(_GP, srg)
          {name = "PortTEM_MW";
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



           n0.setNameAndDescription(ostring("n0"),
                                    ostring("plasma density wave is in"));
           n0.setValue("0");
           parameterList.add(&n0);

           AnalyticEy.setNameAndDescription(ostring("AnalyticEy"),
                                    ostring("Normalized analytic expression for Ey(y)"));
           AnalyticEy.setValue("1");
           parameterList.add(&AnalyticEy);

           AnalyticBz.setNameAndDescription(ostring("AnalyticBz"),
                                    ostring("Normalized analytic expression for Bz(y)"));
           AnalyticBz.setValue("1");
           parameterList.add(&AnalyticBz);

           AnalyticN.setNameAndDescription(ostring("AnalyticN"),
                                    ostring("Normalized analytic expression for N(y)"));
           AnalyticN.setValue("1");
           parameterList.add(&AnalyticN);



           

           
        };

    ScalarParameter pwFrequency, pwPhase, pwWavenumber, pwAmplitude, pwDelay;
    ScalarParameter n0;
    StringParameter AnalyticN,AnalyticEy,AnalyticBz;

    virtual	Boundary* CreateCounterPart();
 };

#endif  //  __PORTG_TEM_MW_G_H
