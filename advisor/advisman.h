//advisman.h

#ifndef	__advisman_H
#define	__advisman_H

#include <oops.h>


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

// Standard headers
#ifdef _MSC_VER
#include <fstream>
#else

using namespace std;
#endif

// otools headers
#include "ostring.h"
#include "oopiclist.h"
#include "varg.h"

// physics headers
#include <misc.h>

class SpatialRegionGroup;
typedef oopicList<SpatialRegionGroup> SpatialRegionGroupList;
class SpeciesParams;
typedef oopicList<SpeciesParams> SpeciesParamsList;
class Species;
typedef oopicList<Species> SpeciesList;
class SpatialRegion;
typedef oopicList<SpatialRegion> SpatialRegionList;

// typedef int BOOL;

class AdvisorManager
{
  VarGroup VG;
	SpatialRegionGroupList	srgList;
	SpeciesParamsList*	speciesParamsList;
	SpeciesList*	speciesList;
	std::ifstream	inputFile;
	std::ofstream outputFile;
	ostring readSpatialRegionGroup(SpatialRegionGroup& srg);
	ostring InputErrorMessage;
	oopicList<ostring> ListOfAllBoundaryTypes;
	oopicList<ostring> ListOfAllParameterGroups;
	oopicList<ostring> ListOfAllProbeTypes;
	ostring caseTitle;
	oopicList<ostring> caseDescription;
public:
	AdvisorManager(const char* fileName);
	virtual ~AdvisorManager();
	void	describe();
	void	checkRules();
	SpatialRegionList*	createDevice() /* throw(Oops) */;
	void setFileName(const char* fileName);
	oopicList<ostring>* getListOfAllBoundaryTypes();
	oopicList<ostring>* getListOfAllProbeTypes();
	SpatialRegionGroupList* getSpatialRegionGroupList();
	void writeOutputFile(const char* fileName);
	Species* get_speciesPtr(const ostring &speciesName);
	ostring GetErrorMessage();
	BOOL KnownBoundary(ostring &s);
	BOOL KnownSpecies(ostring &speciesName);
	ostring readSpecies(std::ifstream& fin);
	SpeciesParamsList* get_speciesParamsList() {return speciesParamsList;}
	SpeciesList* get_speciesList() {return speciesList;}
    BOOL spatialRegionGroupListIsEmpty() {return srgList.nItems() == 0 ? 1 : 0;}
  void linkSRB() /* throw(Oops) */;
  static void getLine(ifstream& input, char* buffer, int buf_size);
protected:
  
#ifdef TXQ_GUI
  virtual void setProgress(int p, const char* c);
#endif
};       

#endif  //  __advisman_H
