//femitg.cpp

#include "gridg.h"
#include "femitg.h"
#include "fieldemi.h"
#include "spatialg.h"
///\dad{begin}
#include "spbound.h"
///\dad{end}
#ifdef UNIX
#include "diags.h"
#endif

//=================== FieldEmitterParams Class

FieldEmitterParams::FieldEmitterParams(GridParams* _GP, SpatialRegionGroup* srg)
	  : EmitterParams(_GP, srg)
{name = "FieldEmitter";
 threshold.setNameAndDescription(ostring("threshold"),
  ostring("Threshold E field"));
 threshold.setValue(ostring("0.0"));

 parameterList.add(&threshold);
};

Boundary* FieldEmitterParams::CreateCounterPart()
{
 EmitterParams::CreateCounterPart();//sets up distribution and species
	Boundary* B =
  new FieldEmitter(SP.GetLineSegments(),
						 species,
						 np2c.getValue(),
						 threshold.getValue());
 return B;}

//=================== FieldEmitterParams2 Class

FieldEmitterParams2::FieldEmitterParams2(GridParams* _GP, SpatialRegionGroup* srg)
	  : EmitterParams(_GP, srg)
{
  name = "FieldEmitter2";
  threshold.setNameAndDescription(ostring("threshold"),
				  ostring("Threshold E field"));
  threshold.setValue(ostring("0.0"));
  parameterList.add(&threshold);
  
  
};

Boundary* FieldEmitterParams2::CreateCounterPart()
{
  EmitterParams::CreateCounterPart();//sets up distribution and species
  Boundary* B = new FieldEmitter2(SP.GetLineSegments(),
				  species,
				  np2c.getValue(),
				  threshold.getValue(),
				  distribution);
  return B;
}

