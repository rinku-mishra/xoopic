//spboundg.h
//  Boundary between spatial regions
//  4/4/97  PeterM  Inception.

#ifndef   __SPBOUNDG_H
#define   __SPBOUNDG_H

#include "boundg.h"
#include "spbound.h"
class SpatialRegionGroup;
class PDistCol;
class Grid;
//=================== BoundaryParams Class
// base class for all boundaries
// abstract

class SpatialRegionBoundaryParams : public BoundaryParams
{protected:
	//SpatialRegionGroup *srg;
public:
	StringParameter PartnerName;
  //  The Name is actually how boundaries are matched across regions
  SpatialRegionBoundaryParams(GridParams *GP, SpatialRegionGroup *_srg) /* throw(Oops) */ :
	 BoundaryParams(GP,_srg){
		//srg = _srg;
		PartnerName.setNameAndDescription("PartnerName","Name of the boundary in the other region to link to.");
		PartnerName.setValue("Noname");
		parameterList.add(&PartnerName);
	 };

   Boundary *CreateCounterPart() {
	 Boundary *B;
	 SpatialRegionBoundary *SRB;
   try{
	   B=SRB = new SpatialRegionBoundary(SP.GetLineSegments());
   }
   catch(Oops& oops){
     oops.prepend("SpatialRegionBoundaryParams::CreateCounterPart: Error:\n"); 
     throw oops;
   }
	 SRB->PartnerName = PartnerName.getValue();
	 spatialRegionGroup->addtoSpatialRegionBoundaryList(SRB);

	return SRB;
  }
};                                       

#endif  //  __SPBOUNDG_H
