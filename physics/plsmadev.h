/*
=====================================================================
plsmadev.h

Describes the PlasmaDevice class.

0.9	(JohnV 10-12-94) Initial draft to hook up to advisor.  This
		version replaces the original version completely.
0.91	(JamesA 10-26-94) Moved body of constructor & destructor to cpp.
0.911	(JohnV 11-23-94) Aesthetics, regroup members.
0.92	(JohnV 11-28-94) Add deleteSpatialRegions().
0.921	(JohnV 11-30-94) Add PlasmaDevice(SpatialRegionList*).
0.922	(JohnV 06-12-95) Moved speciesList from SpatialRegion.
0.923 (PeterM 9-04-95) Put in a particle limit number.
1.001 (JohnV 09-30-95) Added guiPtclCount to pass pointer, made native
		ptclCount long to handle large numbers.
1.002 (PeterM 11-04-98) Moving window.
=====================================================================
*/

#ifndef	__PLSMADEV_H
#define	__PLSMADEV_H

#include "ovector.h"
#include "misc.h"
#include "stdio.h"
#include "ostring.h"
#include <oops.h>
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

//  HDF5 includes
#ifdef HAVE_HDF5
#include "dumpHDF5.h"
#endif

// #ifdef UNIX
// #endif

//	forward declarations
//#ifndef UNIX
//class ScalarIter;
//#endif // UNIX

class SpatialRegion;
class AdvisorManager;
class PlasmaDevice
{
	SpatialRegionList* spatialRegionList; 
	long ptclCount;
	int J, K;
	Scalar time;
//	void deleteSpatialRegions(){} ;
	SpeciesList* speciesList;				//	list of available species
	SpatialRegion* theSpace;				// first spatial region
	long int particle_limit;  				// maximum number of particles allowed
													// simulation will increase particle weights
													// to enforce the limit
	Scalar* guiPtclCount;					// particle count for gui must be Scalar
	char* x1Label;
	char* x2Label;
	char* x3Label;
	AdvisorManager* advisor;

public:
	PlasmaDevice(SpatialRegionList* spatialRegionList);
	virtual ~PlasmaDevice();
	void advance() /* throw(Oops) */; 							// advance each region
	SpatialRegion* get_theSpace() {return theSpace;}
	void setSpatialRegionList(SpatialRegionList* spatialRegionList);
	void createPlotManagers();
	Scalar AdvancePIC() /* throw(Oops) */;						// return current time
	SpeciesList* getSpeciesList() {return speciesList;}
	void setParticleLimit(long int maxparticles) {particle_limit = MAX(0,maxparticles);}
	void setAdvisor(AdvisorManager* theAdvisor);
//	long ParticleCount();
	Scalar SimulationTime();
	void getCavSize(Scalar& m1s, Scalar& m1f, Scalar& m2s, Scalar& m2f);
#ifdef HAVE_HDF5
	int dumpH5(dumpHDF5 &dumpObj);
#endif
	int Dump(FILE *DMPFile);

#ifndef UNIX
	int getGridJ();
	int getGridK();
	void DefinePlots();
	void DefineXYPlots();
	void DefineHistoryPlots();
	void DefineSlicePlots();
	void DefineParticlePlots();
	void DefineFieldPlots();
	void createProbeHistories();
	void DefineAdditionalPlots();
#endif
};

#endif	//	ifndef __PLSMADEV_H
