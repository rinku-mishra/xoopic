//eportg.cpp

#include "porttmg.h"
#include "porttm.h"

//=================== PortTMParams Class
// TM port boundary
Boundary* PortTMParams::CreateCounterPart()
{Boundary* B =
  new PortTM(SP.GetLineSegments(), EFFlag.getValue());
 return B;}
