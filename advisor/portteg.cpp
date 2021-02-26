//eportg.cpp

#include "portteg.h"
#include "portte.h"

//=================== TE Port Params Class
// TE Port boundary
Boundary* PortTEParams::CreateCounterPart()
{Boundary* B =
  new PortTE(SP.GetLineSegments(), EFFlag.getValue());
 return B;}
