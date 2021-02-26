//eqpotg.cpp

#include "eqpot.h"
#include "eqpotg.h"
#include "spatialg.h"
///\dad{begin}
#include "spbound.h"
///\dad{end}
#ifdef UNIX
#include "diags.h"
#endif

//=================== EquipotentialParams Class
EquipotentialParams::EquipotentialParams(GridParams* _GP,SpatialRegionGroup * srg) : ConductorParams(_GP,srg) {
	name = "Equipotential";
        VanalyticF.setNameAndDescription("VanalyticF","Voltage on this boundary as a function of spatial location x1, x2");
        VanalyticF.setValue("1");
        parameterList.add(&VanalyticF);

}

Boundary* EquipotentialParams::CreateCounterPart()
{
  Boundary* B = new Equipotential(SP.GetLineSegments(),VanalyticF.getValue());
  set_commonParams(B);
  return B;
}
