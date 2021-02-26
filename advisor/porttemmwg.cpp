//portg.cpp

#include "portg.h"
#include "port.h"
#include "porttemmw.h"
#include "porttemmwg.h"


//=================== PortParams Class
// port boundaries
Boundary* PortTEM_MW_Params::CreateCounterPart()
{Boundary* B =
    new PortTEM_MW(pwFrequency.getValue(),
                  pwPhase.getValue(),
                  pwWavenumber.getValue(),
                  pwAmplitude.getValue(),
                  pwDelay.getValue(),
                  n0.getValue(),
                  SP.GetLineSegments(),
                   AnalyticN.getValue(),
                   AnalyticEy.getValue(),
                   AnalyticBz.getValue());
 return B;}
