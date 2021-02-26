//spatialg.h

#ifndef   __spatialg_h
#define   __spatialg_h

#include "param.h"
#include "gridg.h"
#include "controlg.h"
#include "particlg.h"

class DiagnosticParams;
class PlasmaDevice;
class PDistCol;
class PoytFlux;
class Grid;
class Boundary;
class SpatialRegionBoundary;

class AdvisorManager;

#include "loadg.h"
#include "varloadg.h"
#include "boundg.h"
#include "diagg.h"
#include "varg.h"
#include "part1g.h"
#include "mccg.h"
#include "h5diagg.h"


//=================== SpatialRegionGroup Class
// Container class for all ParameterGroups associated with a SpatialRegion

class SpatialRegionGroup: public ParameterGroup
{public:
  GridParams GP;
  ControlParams CP;
  ParticleGroupParams PGP;
  VarGroup VG;
  oopicList<BoundaryParams> Boundaries;
  oopicList<MCCParams> MCCParamsList;
  oopicList<LoadParams> Loads;
  oopicList<DiagParams> Diags;
  oopicList<H5DiagParams> H5DiagDumpParams;

  oopicList<DiagnosticParams> Probes;
  oopicList<SpatialRegionBoundary> SpatialRegionBoundaryList;
  ostring readSpecies(std::ifstream& fin); 
  AdvisorManager* advisorp;
  StringParameter Name;
 public:

	SpatialRegionGroup(AdvisorManager* advisor)
	  {  
		 Name.setNameAndDescription("name","A label for the region");
		 Name.setValue("Noname");
		 parameterList.add(&Name);
		 advisorp = advisor;
	  }
	~SpatialRegionGroup();

	ostring CreateParamGroup(std::ifstream &fin, ostring &groupname) /* throw(Oops) */;
	// Parameterizes a ParameterGroup object

	ostring CreateBoundaryParams(std::ifstream &fin, ostring groupname) /* throw(Oops) */;
	// Parameterizes a BoundaryParams object

	ostring AddMCCParamsList(std::ifstream& fin);

	ostring AddLoadParamsList(std::ifstream &fin);
	ostring AddLoadPWParamsList(std::ifstream &fin);
	// Parameterizes a LoadParam object

	ostring AddVarWeightLoadParamsList(std::ifstream &fin);
	// Parameterizes a VarWeightLoadParam object

	ostring AddDiagParamsList(std::ifstream &fin);
	// Parameterizes a DiagParam object

	ostring AddH5DiagDumpParamsList(std::ifstream &fin);
	// Parameterizes a H5DiagParam object

	void describe();
	//  Diagnostic for describing object

	void writeOutputFile(std::ofstream &fout);
	//  Writes output file

	void CreateSpatialRegion();

	void checkRules();

	SpatialRegion*	CreateCounterPart() /* throw(Oops) */;

	// (JohnV 10-16) Temp mechanisms for handling rule messages
	void showRuleMessages();

	// (JohnV 10-16) Need dt for PlasmaDevice::PlasmaDevice()
	Scalar get_dt() {return CP.getDt();}
        Scalar get_dtBase() {return CP.getDt()*CP.getFieldSubFlag();}

	BoundaryParams* constructBoundaryParams(ostring name) /* throw(Oops) */;
	DiagnosticParams* constructProbeParams(ostring name);
	GridParams* getGridParams();
	ControlParams* getControlParams();
	PDistCol *createDistributionCollector(BoundaryParams *boundary,Grid *grid,SpeciesList *slist);
	PoytFlux *createDistributionCollector(BoundaryParams *boundary,Grid *grid);
	void SetTimeFunctions(BoundaryParams *boundary,Boundary *B);
	oopicList<BoundaryParams>* getBoundaryParamsList();
	oopicList<DiagnosticParams>* getProbeParamsList();
	Species* get_speciesPtr(const ostring &speciesName);
	BOOL KnownSpecies(ostring &speciesName);
	SpeciesList* get_speciesList();

  void addtoSpatialRegionBoundaryList(SpatialRegionBoundary *srb) {
	 SpatialRegionBoundaryList.add(srb);}

  oopicList<SpatialRegionBoundary> &getSpatialRegionBoundaryList() {
	 return SpatialRegionBoundaryList;}
#ifdef MPI_VERSION
  void addNeededSRBstoRegion(oopicList<Boundary> *BList,Grid *G) /* throw(Oops) */;
  Boundary *makeSRB(Grid *G,int js,int ks,int je,int ke,int normal,
						  int *neighbor,int Nindex,int bcount) /* throw(Oops) */;
  void linkSRB();

#endif /* MPI_VERSION */
	 
};                    


#endif  /*  __spatialg_h */
