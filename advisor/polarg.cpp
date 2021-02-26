//polarg.cpp

#include "polar.h"
#include "polarg.h"
#include "spatialg.h"
///\dad{begin}
#include "spbound.h"
///\dad{end}
#ifdef UNIX
#include "diags.h"
#endif 

class Species;

//=================== PolarParams Class

PolarParams::PolarParams(GridParams* _GP, SpatialRegionGroup* srg)
	: ConductorParams(_GP, srg)
{
	name = "Polarizer";
	transmissivity.setNameAndDescription("transmissivity",
	      "transmissivity of this boundary to particles");
	transmissivity.setValue("0");
	parameterList.add(&transmissivity);
	
	Phi.setNameAndDescription("Phi","angle w.r.t simulation plane of the conductivity in the perpendicular direction (degrees)");
	Phi.setValue("90");
	parameterList.add(&Phi);
	

}

Boundary* PolarParams::CreateCounterPart()
{
	Boundary* B = new Polar(Phi.getValue(),SP.GetLineSegments(),
                           transmissivity.getValue());
	set_commonParams(B);
	return B;
 }
