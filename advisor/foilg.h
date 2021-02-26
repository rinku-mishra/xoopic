//eportg.h

#ifndef   __FOILG_H
#define   __FOILG_H 

#include "part1g.h"
#include "conductg.h"

//=================== FoilParams Class
// Foil boundary

class FoilParams : public ConductorParams
{protected:
	 ScalarParameter nemit;
  public:
	 FoilParams(GridParams* _GP, SpatialRegionGroup* srg)
		 : ConductorParams(_GP, srg)
			 {name = "Foil";
			  nemit.setNameAndDescription(ostring("nemit"),
											ostring("nemit"));
			  nemit.setValue(ostring("0.0"));
			  parameterList.add(&nemit);
		  };

	 Boundary* CreateCounterPart();
};

#endif  //  __FOILG_H
