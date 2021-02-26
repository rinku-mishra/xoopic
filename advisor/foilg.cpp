//eportg.cpp

#include "foilg.h"
#include "foil.h"

//=================== FoilParams Class
// Foil boundary
Boundary* FoilParams::CreateCounterPart()
{
	Boundary* B = new Foil(nemit.getValue(),SP.GetLineSegments());
   set_commonParams(B);
   return B;
}
