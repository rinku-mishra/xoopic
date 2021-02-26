//eportg.h

#ifndef   __PORTGAUSSG_H
#define   __PORTGAUSSG_H \

#include "part1g.h"
#include "portg.h"

//=================== PortGaussParams Class
// Gauss Port boundary

class PortGaussParams : public PortParams {
  public:
  ScalarParameter offset; /* offset from the center */

    PortGaussParams(GridParams* _GP, SpatialRegionGroup* srg)
	 : PortParams(_GP, srg) {
       name = "PortGauss";
       offset.setNameAndDescription(ostring("offset"),ostring("Offset from the center of the simulation, + or -, in MKS, for the peak"));
       offset.setValue("0");
       parameterList.add(&offset);

    }

    Boundary* CreateCounterPart();
};

#endif  //  __PORTGAUSSG_H
