//portgaussg.cpp

#include "portgaussg.h"
#include "portgauss.h"

// GAUSS Port Params Class
// GAUSS Port boundary

Boundary* PortGaussParams::CreateCounterPart()
{
 Boundary* B = new PortGauss(SP.GetLineSegments(), EFFlag.getValue(),
              spotSize_p0.getValue(), waveLeng_p0.getValue(),
              focus_p0.getValue(),amp_p0.getValue(),
              chirp_p0.getValue(), pulShp_p0.getValue(),  
              tdelay_p0.getValue(),pulLeng_p0.getValue(),
              spotSize_p1.getValue(), waveLeng_p1.getValue(),
              focus_p1.getValue(),amp_p1.getValue(),
              chirp_p1.getValue(), pulShp_p1.getValue(),
              tdelay_p1.getValue(),pulLeng_p1.getValue(),
              offset.getValue());
 return B;
}
