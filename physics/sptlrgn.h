#ifndef	__SPTLRGN_H
#define	__SPTLRGN_H

/*************************************************************************************
 * SPTLRGN.H
 *
 * Purpose:	Class definitions for the SpatialRegion class.  
 *		The SpatialRegion is the fundamental building block 
 *		of the PlasmaDevice to be simulated.
 *
 * Version:	$Id: sptlrgn.h 2295 2006-09-18 18:05:10Z yew $
 *
 *

Revision/Author/Date
0.1	(JohnV, 03-15-93)	Proto code.
0.9	(JohnV, 04-30-93)	Initial draft with particles.
0.91	(NTG, JohnV, 05-05-93) Add EmittingBoundary, ActiveBoundary, class
		library use for list management and iterators, KinematicalObject.
0.93	(JohnV, NTG 05-12-93) Fix Windows large model Gen. Prot. Fault due
		to reading outside arrays.
0.95	(JohnV, 06-29-93) Add Boundary::setPassives() to set up passive
		boundary conditions (Fields::iC and ::iL), add Fields::set_iCx()
		to support above.
0.96	(JohnV 08-23-93) Add new Boundary subtypes: BeamEmitter,
		CylindricalAxis, FieldEmitter, Port.  Modifications to
		Maxwellian: stores all parameters in MKS.
0.961	(PeterM, JohnV, 08-24-93) Replace Borland list containers with
		vanilla C++ template lists in oopiclist.h
0.963	(JohnV, 09-24-93)	Add Particle object for moving particle quantities
		around (emission, collection, etc.)
0.964	(JohnV, 11-22-93) Make agreed-upon changes in names: Vector->Vector3,
		Boundaries->BoundaryList, Particles->ParticleList, ParticleGroups
		->ParticleGroupList
0.965	(JohnV, 02-01-94) Move magnetic field advance to Fields::advanceB(),
		move boundary code, particle code to respective files.
0.966	(JohnV, 02-07-94)	Fixed striation in vr vs. z phase space seen
		for beam spreading in a simple cylinder.  Problem was weighting
		error in Grid::interpolateBilinear().
0.967	(JohnV 03-03-94) added Fields::setBz0() and ::epsilonR.
0.968	(JohnV 03-11-94) Fix divide by zero for non-translating particles
		with finite spin in Fields::translateAccumulate().  Also ensure all
		particles that touch boundaries get collected in Grid::translate()
		by changing </> to <=/>= for check conditions.
0.969	(JohnV 03-23-94) Restructure Grid, Fields, and SpatialRegion to
		an association rather than inheritance.
0.970	(JohnV 10-16-94) Add dt to prepare for multi region multi time
		scale use.
0.971	(Hadon 10-28-94) Added getTime(), get_dt().
0.972 (JohnV 01-23-95) Restructure particleGroupList to an array, which
		allows for multiple species with rapid lookup.
0.973	(JohnV 06-12-95) Moved speciesList ownership to PlasmaDevice.
0.974 (PeterM 9-04-95) Added a method to reduce #/increase weight of particles
1.001 (JohnV 03-04-96) Added query_geometry().
1.002 (PeterM 11-04-98) Added ShiftRegion, for moving window.
2.001 (Bruhwiler 10-08-99) implemented get/setSynchRadiationFlag()
2.00? (Cary 22 Jan 00) Added setShiftDelayTime.
CVS1.27.2.4 (Cary 27 Jan 00) ShiftRegion -> shiftRegion.
3.0   (Johnv 08-Oct-2003) Added histmax for temporal array length.

 *
 ***************************************************************************************/

// innocent pragma to keep the VC++ compiler quiet
// about the too long names in STL headers.
#ifdef _WIN32
#pragma warning ( disable: 4786)
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "misc.h"	  //	need forward references
#include "fields.h"
#include "ovector.h"
#include "newdiag.h"
#include "ngd.h"
#include "mapNGDs.h"

#include <oops.h>


#ifdef HAVE_HDF5
#include "dumpHDF5.h"
#endif

#include "h5diagg.h"

#include <string>


#ifndef _WIN32
using namespace std;
#endif

class MCC;
class MCTI;
class Load;
class Coulomb;

class SpatialRegion
{
private:
	Fields* fields;
	Grid* grid;
	Scalar t;
	Scalar dt;
	ostring Name;
	Scalar np2cFactor; 				// multiply np2c from dump file by this
	int duplicateParticles; 			// number of times to duplicate particles
	ParticleGroupList** particleGroupList; // array of ParticleGroupLists
	BoundaryList *boundaryList;            // list of Boundary objects
	DiagList *diagList;                    // list of Diagnostics
	int histmax;					// length of temporal Diagnostic arrays

	int maxN;						// max particles per ParticleGroup
	SpeciesList*	speciesList;	// list of available species
	Species **speciesArray;		// array of the species
	int	nSpecies;				// number of species in this region
	void	emit(Species *species) /* throw(Oops) */;		 // emit particles into groups
	MCC*  mcc;  // pointer to the Monte Carlo Collision Object that accumulates
	// MCCPackages and drives the MCC simulations

	Coulomb* coulomb;

	MCTI* mcti; // pointer to the Monte Carlo Tunneling Ionization. It is
	// analogous to the MCC object.
	void globalParticleLimit();
	long particleLimit;			// max number of particles in this region
	long* nParticles; 			// array to cache particle count by species
	BOOL nParticleFlag;			// TRUE = cache valid, FALSE = reload cache

	// Parameters relevant to shift
	Scalar shiftDelayTime;	// Time for first shift
	Scalar gasOffTime;            // Time to switch of the neutral gas density
	Vector2 shift;		// The direction of system shift
	long unsigned int shiftNum;  //  number of shifts which have occurred.

	int	shiftDir;		// The shift direction:
	// Note that (per grid.h), up is lower k,
	// (which would be towards smaller r)
	//     1 for system left,  window right
	//     2 for system right, window left
	//     3 for system up,    window down
	//     4 for system down,  window up
	// Scalar last_shift_time;	// Last time window was shifted
	Scalar shift_dt;			// Time between shifts
	Scalar next_shift_time;	// Next time for shift

	/**
	 * pointer to an NGD object needed in the initialization of the NGDList
	 */
	NGD* ptrNGD;
	/**
	 * The list of Neutral Gas Densities (NGDs)
	 */
	oopicList<NGD> NGDList;
	/**
	 * a pointer to an object to handle the mapping of NGDs across boundaries
	 * when using MPI...
	 */
	MapNGDs* ptrMapNGDs;

public:

	oopicList<H5DiagParams>* H5DiagDumpList; // list of diagnostics to be dumped in HDF5 format


	long unsigned int getShiftNum() { return shiftNum;};
	oopicList<Load> *theLoads;
	SpatialRegion(Fields* fields, BoundaryList* boundaryList,
			ParticleGroupList** particleGroupList, SpeciesList* speciesList,
			MCC* _mcc, MCTI* _mcti, Scalar dt);
	~SpatialRegion();
	ParticleGroupList& getParticleGroupList() {return *particleGroupList[0];}
	SpeciesList* getSpeciesList() {return speciesList;}
	ParticleGroupList **getParticleGroupListArray() {return particleGroupList;};
	void ShowboundaryList();
	void advance() /* throw(Oops) */;
	void particleAdvance() /* throw(Oops) */;
	void fieldsAdvance();
	ostring getName() { return Name;};
	void setName(const ostring &_Name) { Name=_Name;};
	Vector2**	getX() {return grid->getX();}
	Vector3**	getENode() {return fields->getENode();}
	Vector3**	getBNode() {return fields->getBNode();}
	Vector3**   getIntBdS() {return fields->getIntBdS();}
	Vector3**   getIntEdl() {return fields->getIntEdl();}
	Vector3**   getIntEdlBar() {return fields->getIntEdlBar();}
	void setIntEdl(int j, int k, int component, Scalar value) {fields->setIntEdl(j, k, component, value);}
	void setI(int j, int k, int component, Scalar value) {fields->setI(j, k, component, value);}
	void setIntBdS(int j, int k, int component, Scalar value) {fields->setIntBdS(j, k, component, value);}
	void setIntEdlBar(int j, int k, int component, Scalar value) {fields->setIntEdlBar(j, k, component, value);}
	Vector3**   getBNodeDynamic() {return fields->getBNodeDynamic();}
	Vector3**	getI() {return fields->getI();}
	int	getJ() {return grid->getJ();}
	int	getK() {return grid->getK();}
	Vector2	getMKS(int j, int k){return grid->getMKS(j, k);}
	Vector2	getMKS(Vector2 x){return grid->getMKS(x);}

	oopicList<NGD>* getPtrNGDList() {
		return &NGDList;
	}

	Grid*	get_grid(){return grid;}  //added by kc (5-31-94)
	Fields* get_fields(){return fields;} //added by kc (1-27-95)
	int   getmaxN() {return maxN;}  //added by kc (1-23-95)
	void addParticleList(ParticleList& particleList); //added by kc (1-23-95)
	void packParticleGroups(int i);
	Scalar** get_halfCellVolumes() {return grid->get_halfCellVolumes();}
	Scalar**	getRho() {return fields->getrho();}
	Scalar**  getQ() {return fields->getQ();};
	Scalar** getBoltzmannRho() {return fields->getBoltzmannRho();};
	void CopyBoltzmannRho() {fields->CopyBoltzmannRho();};
	Scalar** getTotalRho() {return fields->getTotalRho();};
	Scalar*** getloaddensity() {return fields->getloaddensity();};
	int getShowInitialDensityFlag() {return fields->getShowInitialDensityFlag();};
	int getBoltzmannFlag() {return fields->getBoltzmannFlag();};
	int getElectrostaticFlag() {return fields->getElectrostatic();};
	int getSynchRadiationFlag() {return fields->getSynchRadiation();};
	Scalar getEMdamping() {return fields->getEMdamping();};
	int getNonRelFlag() {return fields->getNonRelativistic();};
	int getInfiniteBFlag() {return fields->getInfiniteBFlag();};
	int getFieldSubFlag() {return fields->getFieldSub();};
	Scalar getBz0() {return fields->getBz0();};
	Scalar getBx0() {return fields->getBx0();};
	Scalar**	getPhi() {return fields->getphi();}
	Scalar**	getDivDerror() {return fields->getDivDerror();}
	Scalar	getTime() {return t;}
	Scalar	get_dt() {return dt;}
	Scalar get_np2cFactor() {return np2cFactor;}
	int  getduplicateParticles(void) { return duplicateParticles; }
	void setduplicateParticles(int dp) { duplicateParticles = dp; }
	void set_np2cFactor(Scalar factor) {np2cFactor = factor;}
	void  setTime(Scalar time) { t = time; }
	void  setDiagList(DiagList* DL) {diagList = DL;}
	void setH5DiagDumpList( oopicList<H5DiagParams> *H5DiagDumpParams) {H5DiagDumpList = H5DiagDumpParams;}

	//	long	nParticles(Scalar* nPtcl=NULL);
	void  InitPhi() {fields->initPhi();}
	void  ComputePhi() {fields->updatePhi(fields->getrho(),fields->getphi(),t,dt);}
	void  SetEfromPhi() {fields->setEfromPhi();}
	void  updateDivDerror() {fields->updateDivDerror();}
	void	DivergenceClean() {fields->DivergenceClean();}
	void  MarderClean() { fields->MarderCorrection(dt);}
	void	setMarderIter(int iter) {fields->setMarderIter(iter);}
	void	setDivergenceCleanFlag(int flag) {fields->setDivergenceCleanFlag(flag);}
	void	setCurrentWeightingFlag(int flag) {fields->setCurrentWeightingFlag(flag);}
	void	setElectrostaticFlag(int flag) {fields->setElectrostaticFlag(flag);}
	void  setSynchRadiationFlag(int flag) {fields->setSynchRadiationFlag(flag);};
	void	setMarderParameter(Scalar value) {fields->setMarderParameter(value);}
	long int CountParticles(int species_ID);
	void increaseParticleWeight(int species_ID);

#ifdef HAVE_HDF5
	int dumpH5(dumpHDF5 &dumpObj);
	int restoreH5(dumpHDF5 restoreObj) /* throw(Oops) */;
#endif
	int Dump(FILE* DMPFile);
	int Restore(FILE* DMPFile);
	int Restore_2_00(FILE* DMPFile) /* throw(Oops) */;  // restore previous version dump files.
	BoundaryList * getBoundaryList() { return boundaryList; };
	DiagList * getDiagList() { return diagList; }
	int get_histmax() {return histmax;}
	void set_histmax(int _histmax) {histmax = _histmax;}
	int get_nSpecies() {return nSpecies;}
	Scalar *** get_rho_species() { return fields->getRhoSpecies(); }
	void increaseParticleWeight();
	int query_geometry() {return grid->query_geometry();}
	void setParticleLimit(long int limit) {particleLimit = limit;}
	long int getParticleLimit() {return particleLimit;}
	void shiftRegion() /* throw(Oops) */;
	void setShiftDelayTime(Scalar sdt) {
		shiftDelayTime = sdt;
		next_shift_time = -1.;
		//	  next_shift_time = shiftDelayTime;
	}
	Scalar getShiftDelayTime() const { return shiftDelayTime; }
	void setGasOffTime(Scalar gasOffTimeValue) {
		gasOffTime = gasOffTimeValue;
	}
	bool isGasSwitchOffSet() {
		if ( gasOffTime < 0.0 )
			return false;
		if ( t > gasOffTime )
			return true;
		else
			return false;
	}
	void setShift(Vector2 _shift) {
		shift = _shift;
		if      ( shift.e1() == -1. ) shiftDir = 1;
		else if ( shift.e1() ==  1. ) shiftDir = 2;
		else if ( shift.e2() == -1. ) shiftDir = 3;
		else if ( shift.e2() ==  1. ) shiftDir = 4;
	}
	void setMCC(MCC* _mcc) {mcc = _mcc;}
	void setCoulomb(Coulomb* _coulomb) {coulomb=_coulomb;}
	void setMCTI(MCTI* _mcti) { mcti = _mcti;}
	/**
	 * Create the MapNGDs object and allow each Boundary object to
	 * point to the MapNGDs object.
	 */
	void setNGDListPtrs();
	/**
	 * Initialization of an NGD object and returning a pointer to it.
	 * dad, 01/19/2001
	 */
	NGD* initNGD(const ostring& _gasType, const ostring& _analyticF,
			const Scalar& _gasDensity,
			const Vector2& _p1Grid, const Vector2& _p2Grid,
			const int& discardDumpFileNGDDataFlag) /* throw(Oops) */;

};

#endif  //	#ifndef __SPTLRGN_H
