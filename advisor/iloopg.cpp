//portg.cpp

#include "iloopg.h"
#include "iloop.h"


//=================== IloopParams Class
// port boundaries


Boundary* IloopParams::CreateCounterPart()
{Boundary* B =
  new Iloop(SP.GetLineSegments(),I.getValue());
 return B;}
