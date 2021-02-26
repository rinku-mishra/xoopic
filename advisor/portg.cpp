//portg.cpp

#include "portg.h"
#include "port.h"


//=================== PortParams Class
// port boundaries
Boundary* PortParams::CreateCounterPart()
{Boundary* B =
  new Port(SP.GetLineSegments());
 return B;}
