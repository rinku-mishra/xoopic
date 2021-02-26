//conductg.cpp

#include "conducto.h"
#include "conductg.h"
#include "spatialg.h"
///\dad{begin}
#include "spbound.h"
///\dad{end}
#ifdef UNIX
#include "diags.h"
#endif 

class Species;

//=================== ConductorParams Class

ConductorParams::ConductorParams(GridParams* _GP, SpatialRegionGroup* srg)
	: DielectricParams(_GP, srg)
{
	name = "Conductor";
}

Boundary* ConductorParams::CreateCounterPart()
{
  Boundary* B = new Conductor(SP.GetLineSegments());
  set_commonParams(B);
  return B;
}
