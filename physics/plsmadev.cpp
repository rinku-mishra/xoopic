/*
=====================================================================
plsmadev.cpp

Provides methods for the PlasmaDevice class.

0.9	(JohnV 10-16-94) Initial draft to hook up to advisor.  This
		version replaces the original version completely.
0.911	(JohnV 11-23-94) Aesthetics.
0.92	(JohnV 11-28-94) Add deleteSpatialRegions().
0.921	(JohnV 11-30-94) Add PlasmaDevice(SpatialRegionList*).

(Hadon 12-17-94) replace AllPGGroupListIter with ScalarIter
(Hadon 12-19-94) change typedef AllVectorFeilds to ScalarIter
(Hadon 12-19-94) Remove getBField (not needed by new ScalarIter's)
(Hadon & James 3-6-95) Removed all OWL-related code, Added GUIConfig

1.001 (JohnV 02-29-96) Added additional plots, cleaned up destructor.

=====================================================================
*/

#include "diags.h"
#include "plsmadev.h"
#include "sptlrgn.h"
#include "advisman.h"
#include "ptclgrp.h"
#include "spatialg.h"
#include "spbound.h"

PlasmaDevice::PlasmaDevice(SpatialRegionList* spatialRegionList)
{
	J = 0;
	K = 0;
	time = 0;
	ptclCount=0;
	setSpatialRegionList(spatialRegionList);
	particle_limit = 100000000;  //High limit by default, 100 million
	guiPtclCount = new Scalar[(int)speciesList->nItems()];
}

PlasmaDevice::~PlasmaDevice()
{
	// will need to delete all physics objects here
//	deleteSpatialRegions();
	spatialRegionList->deleteAll();
	delete spatialRegionList;
	spatialRegionList = 0;
	speciesList->deleteAll();
	delete speciesList;
	speciesList = 0;
	delete[] guiPtclCount;
	guiPtclCount = 0;
}


void PlasmaDevice::setSpatialRegionList(oopicList<SpatialRegion>* _spatialRegionList)
{
	spatialRegionList = _spatialRegionList;

	oopicListIter<SpatialRegion> srIter(*spatialRegionList);
	theSpace = srIter.current();
    
	J = theSpace->getJ();
	K = theSpace->getK();
	speciesList = theSpace->getSpeciesList();

	switch (theSpace->query_geometry())
	{
		case ZRGEOM:
			x1Label = (char *)"z [m]";
			x2Label = (char *)"r [m]";
			x3Label = (char *)"phi [rad]";
			break;
		case ZXGEOM:
			x1Label = (char *)"z [m]";
			x2Label = (char *)"x [m]";
			x3Label = (char *)"y [m]";
			break;
	}
}

void PlasmaDevice::setAdvisor(AdvisorManager* theAdvisor)
{
	advisor = theAdvisor;
}

void PlasmaDevice::advance()
/* throw(Oops) */{
	oopicListIter<SpatialRegion> srIter(*spatialRegionList);
	for (srIter.restart(); !srIter.Done();	srIter++)
	{
    try{
	    srIter()->advance();
    }
    catch(Oops& oops3){
      oops3.prepend("PlasmaDevice::advance: Error: \n");//done
      throw oops3;
    }
	}
}

Scalar PlasmaDevice::AdvancePIC() // return current time
  /* throw(Oops) */{
 
  try{
    advance();
  }
  catch(Oops& oops3){
    oops3.prepend("PlasmaDevice::AdvancePIC: Error: \n");
    throw oops3;
  }
	time = theSpace->getTime();
	return time;
}


Scalar PlasmaDevice::SimulationTime()
{
	return time;
}

#ifdef HAVE_HDF5

int PlasmaDevice::dumpH5(dumpHDF5 &dumpObj)
{
  SpatialRegionList reversedRegionList;
  
  oopicListIter <SpatialRegion> Siter(*spatialRegionList);
  for(Siter.restart();!Siter.Done();Siter++) 
    reversedRegionList.add(Siter());
  
  int i = 0;
  oopicListIter <SpatialRegion> Riter(reversedRegionList);
  for(Riter.restart();!Riter.Done();Riter++) {
    Riter()->dumpH5(dumpObj);
  }
    return(1);
}

#endif


int PlasmaDevice::Dump(FILE* DMPFile)
{
  SpatialRegionList reversedRegionList;
  
  oopicListIter <SpatialRegion> Siter(*spatialRegionList);
  for(Siter.restart();!Siter.Done();Siter++) 
    reversedRegionList.add(Siter());
  
  int i = 0;
  oopicListIter <SpatialRegion> Riter(reversedRegionList);
  for(Riter.restart();!Riter.Done();Riter++) {
    Riter()->Dump(DMPFile);
  }

  return (1);
}

