//cursrcg.cpp

#include "cursrc.h"
#include "cursrcg.h"
#include "spatialg.h"
///\dad{begin}
#include "spbound.h"
///\dad{end}
#ifdef UNIX
#include "diags.h"
#endif

//=================== CurrentSourceParams Class
CurrentSourceParams::CurrentSourceParams(GridParams* _GP,SpatialRegionGroup * srg) : EquipotentialParams(_GP, srg) {
	name = "CurrentSource";

	Vinitial.setNameAndDescription("Vinitial", "initial voltage of driven electrode");
	Vinitial.setValue(ostring("0.0"));
	parameterList.add(&Vinitial);
}

Boundary* CurrentSourceParams::CreateCounterPart()
{
	Boundary* B = new CurrentSource(SP.GetLineSegments());
	((CurrentSource *)B)->setinitV(Vinitial.getValue());
	set_commonParams(B);
	return B;
}
