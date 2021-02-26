#ifndef	__BOUNDARY_H
#define	__BOUNDARY_H

/*
====================================================================

File:		boundary.h

Purpose:	Define the basics of a boundary

Version:	$Id: boundary.h 2297 2006-09-18 22:17:55Z yew $

The pure virtual base class that represents a generic boundary object.
All boundary objects (e.g., conductor, port) must inherit from this class.

0.99	(NTG, JohnV, 12-28-93) Separated into individual module from
		pic.h.
0.991	(JohnV 05-10-94) Add BEAMEMITTERVANES BCType.
0.992	(JohnV 05-17-94) Add Normal() to indicate positive or negative
		normal.  Normal is determined by a unit vector formed by the
		cyclic (signed) cross product of the boundary edge with phi-hat.
0.993 (JohnV 12-18-94) Modified constructor added normal and get_normal().
0.994 (JohnV 08-21-95) Moved setBoundaryMask() internals from Grid.
0.995	(JohnV 09-02-95) Added virtual setSecondary().
0.996 (PeterM 09-04-95) Added a method to reduce #particles, increase
		the weight of particles.
0.997 (PeterM 06-07-96) Added a points array for oblique boundaries.
1.001 (JohnV 05-03-96) Added toNodes() for more accurate interpolation
		of fields to grid nodes at boundaries (edge and internal).
1.002 (JohnV 11-05-96) Added BOOL oblique() to query boundary alignment.
1.1   (JohnV 01-11-00) Time dependent function -> xt dependent function
CVS1.26 (Cary 23 Jan00) Added the method, recvParticles(), needed by
	SpatialRegionBoundary
1.300 (JohnV 20-Jun-2001) Added capablity to store species histories individually.

====================================================================
 */

// Standard includes
#include <assert.h>
#include <stdio.h>
#include <iostream>

#if defined(_CRAYT3E)
#include <vector.h>
#else
#include <vector>
using std::vector;
#endif   //_CRAYT3E

//XGRead/Write
#include "xgio.h"
// OOPIC includes
#include "ovector.h"
#include "misc.h"
#include "particle.h"
#include "spacetimefunc.h"
#include "ngd.h"

class PDistCol;
class PoytFlux;
class Secondary;
class MapNGDs;

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


//  HDF5 includes
#ifdef HAVE_HDF5
#include "dumpHDF5.h"
#include <string>

using namespace std;
#endif


enum BCTypes { FREESPACE,
	SPATIAL_REGION_BOUNDARY,
	CYLINDRICAL_AXIS,
	DIELECTRIC_BOUNDARY,
	PERIODIC_BOUNDARY,
	CONDUCTING_BOUNDARY
};

class SpatialRegion;
//--------------------------------------------------------------------
//	Boundary:  This class defines the generic boundary object for
//	some region.  All other boundaries inherit from this pure virtual.

class Boundary
{
protected:
	int j1, k1, j2, k2;
	int normal;
	int fill;  // fill in a solid boundary
	int minK, maxK, minJ, maxJ;
	Vector3 _unitNormal; // unit normal for last particle collected
	Fields* fields;
	SpatialRegion *sRegion;
	ParticleList particleList;
	Species **speciesArray;
	int BoundaryType; /* type of boundary for XGrafix and dumping/restore only */
	BCTypes BCType;  /* type of boundary condition used by the Poisson solve*/
	ostring BoundaryName;
	PDistCol *Particle_diagnostic;
	int Ihist_len; /* length of current history arrays */
	int Ihist_avg; /* number of steps over which to average */
	Scalar* EnergyFlux;
	SpaceTimeFunction *xt_depend;  //  the time dependent functions of this boundary,
	//  if any
	int *points;  //an array of 2*abs(j2-j1) points which define this boundary.
	oopicList <LineSegment> *segments;
	// Needed for particle shifting:
	ParticleList emptyPtclList;	// Default return
	vector<Particle*> shiftedParticles;
	//
	// pointer to the class that handles the mapping of the
	// neutral gas density objects is needed for
	// the shifting of the data associated with them
	//
	MapNGDs* ptrMapNGDs;

public:
	Boundary(oopicList <LineSegment> *segments);
	oopicList <LineSegment> *getSegmentList() { return segments; };
	virtual ~Boundary();
	virtual void addSecondary(Secondary* secPtr) {};
	virtual void setSpeciesArray(Species **speciesArr) { speciesArray=speciesArr; };
	virtual void applyFields(Scalar t,Scalar dt) {};
	virtual void collect(Particle& p, Vector3& dxMKS);
	//	virtual Vector2 delta(Particle& p) {return Vector2(0,0);};
	virtual ParticleList&	emit(Scalar t,Scalar dt, Species* species) = 0;
	virtual void setPassives() = 0;	//	set up iC and iL if needed
	virtual void InitializeLaplaceSolution() {};
	virtual void putCharge_and_Current(Scalar t) {};
	virtual void putCharge() {};
#ifdef HAVE_HDF5
	virtual int restoreH5(dumpHDF5 &restoreObj,int bType,string whichBoundary,int nQuads){return(1);}
#endif
	virtual int Restore(FILE *DMPFile, int BoundaryType,
			Scalar A1,Scalar A2, Scalar B1, Scalar B2,int nQuads);
	virtual int Restore_2_00(FILE* DMPFile, int j1test, int k1test,
			int j2test, int k2test);
#ifdef HAVE_HDF5
	virtual int dumpH5(dumpHDF5 &dumpObj,int number);
#endif
	virtual int Dump(FILE *DMPFile);
	virtual Scalar* getEF();
	virtual void setFields(Fields& f);
	virtual void setRegion(SpatialRegion *SR) {
		sRegion = SR;
	}
	virtual void setPtrMapNGDs(MapNGDs* ptrMapNGDsArg) { ptrMapNGDs = ptrMapNGDsArg; }

	virtual Scalar getBoltzmannFlux();
	virtual void passParticles() {};  // template needed by SRB's.
	BCTypes getBCType() { return BCType; };
	ostring getBoundaryName() { return BoundaryName;}
	void setBoundaryName(ostring Bid) { BoundaryName=Bid;}
	void setFill(int _fill){fill = _fill;}
	int getBoundaryType() { return BoundaryType; }
	int get_j1() {return j1;}
	int get_k1() {return k1;}
	int get_j2() {return j2;}
	int get_k2() {return k2;}
	int getMinJ() {return minJ;}
	int getMinK() {return minK;}
	int getMaxJ() {return maxJ;}
	int getMaxK() {return maxK;}
	int onMe(Scalar tA1,Scalar tA2,Scalar tB1,Scalar tB2); // do both points fall on
	// this boundary?
	Fields* getFields() {return fields;}
	//int *get_points() { return points; }
	PDistCol *get_particle_diag()		{ return Particle_diagnostic;}
	void set_particle_diag(PDistCol *PD) { Particle_diagnostic=PD;}
	void set_Ihist_len(int len) {Ihist_len = len;}
	int get_Ihist_len() {return Ihist_len;}
	void set_Ihist_avg(int avg) {Ihist_avg = avg;}
	int get_Ihist_avg() {return Ihist_avg;}
	BOOL alongx1() {return (k1 == k2) ? TRUE : FALSE;} // boundary lies along x1
	BOOL alongx2() {return (j1 == j2) ? TRUE : FALSE;}
	//BOOL oblique() {return (int)points;} // oblique boundary
	int get_normal() {return normal;}
	int queryFill() {return fill;}
	Vector3 unitNormal() {return _unitNormal;}
	Scalar get_xt_value(Scalar x, Scalar t) {return xt_depend->SpaceTimeValue(x,t);};
	int get_xtFlag() {return xt_depend->get_xtFlag();};
	Scalar get_time_value(Scalar t) { return xt_depend->TimeValue(t);};
	Scalar get_time_value_deriv(Scalar t) { return xt_depend->TimeValueDeriv(t);};
	Scalar get_time_value_int(Scalar t) {return xt_depend->TimeValueInt(t);};
	Scalar get_time_value2(Scalar t) { return xt_depend->TimeValue2(t);};
	Scalar get_time_value3(Scalar t) { return xt_depend->TimeValue3(t);};
	void set_xt_function(SpaceTimeFunction *_xt_depend) {xt_depend = _xt_depend;}
	virtual void setBoundaryMask(Grid &grid);
	virtual void setSecondary(Scalar secondary, Species* secSpecies, Species *
			iSpecies) {};
	virtual void set_transparency(Scalar tranparency) {};
	Scalar* getPF() {return EnergyFlux;};
	virtual void increaseParticleWeight() {};
	virtual void increaseParticleWeight(int) {};
	virtual void toNodes();
	// this function interfaces with a moving window shift boundary
	// condition on the leading edge.
	virtual void applyShiftBC(Scalar t, Scalar dt) {};
	// The following methods are needed by SpatialRegionBoundaries
	// For all other methods they do nothing

	/**
	 * Collect the particles shifted across this boundary.
	 * Does nothing except in SpatialRegionBoundary, where
	 * it collects particles that will be passed across the
	 * boundary by passShiftedParticles.
	 *
	 * This is a handoff, the boundary is subsequently
	 * responsible for deletion.
	 *
	 * @param p the particle to be collected
	 */
	virtual void collectShiftedParticle(Particle* p){
		delete p;
	}

	/**
	 * This method needed by SpatialBoundaryRegion to send particles
	 * to processor across the boundary.  In the base class it does
	 * nothing.
	 */
	virtual void passShiftedParticles(){}

	/**
	 * This method needed by SpatialBoundaryRegion to receive particles
	 * send across the boundary.  Differs from emit in that it does no
	 * current weighting.
	 *
	 * @return a list of the shifted particles to place after shifting
	 */
	virtual ParticleList& recvShiftedParticles(){
		// cerr << "Attempt to get shifted particles when does not make sense.\n";
		// assert(0);
		return emptyPtclList;
	}

	/**
	 * Ask for a stripe of fields for this boundary
	 *
	 * @param dir the direction integer
	 */
	virtual void askStripe(){}

	/**
	 * Send a stripe of fields for this boundary
	 *
	 * @param dir the direction integer
	 */
	virtual void sendStripe(int dir){}

	/**
	 * Receive a stripe of fields from this boundary.
	 * In the base class, this sets the incoming fields to zero
	 *
	 * @param dir the direction integer
	 */
	virtual void recvStripe(int dir);

	/**
	 * Ask for a stripe of NGDs for this boundary
	 */
	virtual void askNGDStripe(){}
	/**
	 * Send a stripe of the data structures related to
	 * the Neutral Gas Density  for this boundary
	 *
	 * @param dir the direction integer
	 */
	virtual void sendNGDStripe(int dir){}
	/**
	 * Receive a stripe of NGDs from this boundary.
	 * This implementation does not follow the fields implementation of
	 * recvStripe above that has a base class Boundary
	 * implementation to set to 0 the elements of the data
	 * structures... (dad)
	 */
	virtual void recvNGDStripe(int dir){}
	virtual void allocMemNGDbuffers() {}
	//
	// the following function must be overwritten in the
	// derived from Boundary classes.
	//
	virtual int getnumElementsNGDbuf() const { return 0; }
};

#endif	//	ifndef __BOUNDARY_H
