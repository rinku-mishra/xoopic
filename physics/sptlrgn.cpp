/*
====================================================================

SPTLRGN.CPP

Purpose:	functions of the SpatialRegion class.  The SpatialRegion
		is the fundamental building block of the PlasmaDevice to be
		simulated.

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
0.970	(GMU 11-16-94) Added TaskYield() call.
0.971 (JohnV 01-26-95) Add code to handle ParticleGroup array and Species.
0.98  (KeithC 08-25-95) Lack of precision caused Bz0 to 'leak` into Etheta 
      and Br.  Added BNodeStatic to separate the Static part from the B
		fields that that Fields  updates.  Changed Fields::setBz0() to 
		Fields::setBNodeStatic.
0.981 (PeterM 09-04-95) Added a method to reduce #particles, increase
      the weight of particles.
1.0	(JohnV 01-18-96) Added subcycling stuff from CooperD; rewrote advance().
1.1   (JohnV 01-21-97) Added supercycling.
1.11  (JohnV 10-01-97) Improved supercycling to work with mcc package.
2.01  (JohnV 11-10-97) CountParticles() deletes empty ParticleGroup's.
2.02  (JohnV 12-12-97) Modified increaseParticleWeight().
2.03  (JohnV 03-31-98) Implement caching scheme for nParticles
2.04  (Bruhwiler 10-08-99) Added getSynchRadiationFlag() check to addParticleList()
2.0?  (Cary 22 Jan 00) Added methods for setting the shift delay time and
	streamlined the shifting code.
CVS-1.53.2.7 (Cary, 22 Jan 00) reorganized calls for shifting in
	anticipation of making MPI work.
CVS-1.53.2.8 (Cary, 22 Jan 00) Added code to determine which boundary is
	involved.  The if is a SPATIAL_REGION_BOUNDARY, call passParticles().
	Similar coding needed for receiving the shifted particles and fields.
	I am not sure that this works except for right and down shifting
	windows.
CVS-1.53.2.12 (Cary 27 Jan 00) ShiftRegion changed to shiftRegion.
CVS-1.53.2.13 (kgl  28 Jan 00) xgmini.h is not needed, only theDumpFile decl.)
CVS-1.53.2.14 (JohnV 15 Mar 00) Fixed bug in np2cFactor (getting ignored)
CVS-1.57      (kgl  4  Aug 00) dropfile size (50) is too small, increased
                               to 80.  Caused coredump for long dir.
                               Changed to 180 by kgl and dad to accomodate
                               long file names. 04/08/02.

====================================================================
*/

//I/O stuff

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


// Standard includes
#include <cassert>

// MPI stuff
#ifdef MPI_VERSION
#include <unistd.h>
#include <mpi.h>

extern int MPI_RANK;

#ifdef MPI_DEBUG
extern std::ofstream dfout;
#endif  // MPI_DEBUG

#endif  // MPI_VERSION


#include "sptlrgn.h"
#include "ptclgrp.h"
#include "ptclgpes.h"
#include "ptclgpsr.h"
#include "ptclgpnr.h"
#include "ptclgpib.h"
#include "mcc.h"
#include "periodic.h"
#include "load.h"
#include "coulomb.h"
// #include "xgmini.h"
#ifdef OOPICPRO

#if defined(_MSC_VER)
extern "C" char theDumpFile[180];
#else
extern char theDumpFile[180];
#endif

#else

/*#if defined(_MSC_VER)
extern "C" char* const volatile theDumpFile;
#else
extern char* const volatile theDumpFile;   // Can't modify or call strlen, strcpy on these .
#endif
*/
#endif
// for dumping diagnostics

#include "diagn.h"
extern oopicList<Diagnostics> *theDiagnostics;



//--------------------------------------------------------------------
//	Build SpatialRegion, along with inherited Fields object

extern void taskYield();    //Yield execution to concurrent processes

SpatialRegion::SpatialRegion(Fields* f, BoundaryList* b, 
			     ParticleGroupList** p, SpeciesList* s, MCC* _mcc, 
                             MCTI* _mcti, Scalar _dt) {

  fields = f;
  grid = fields->get_grid();
  boundaryList = b;
  particleGroupList = p;

 

  speciesList = s;
  nSpecies = speciesList->nItems();
  speciesArray = new Species*[nSpecies];
  nParticles = new long int[nSpecies];
  nParticleFlag = FALSE;

  theLoads = new oopicList<Load>;
  
  mcc  = _mcc;
  mcti = _mcti;
  dt   = _dt;

  // Set defaults for moving window
  shift = 0;
  shiftDir = 0;
  shiftDelayTime = ( get_grid()->getX()[(95*getJ())/100][0].e1() 
                     - get_grid()->getX()[0][0].e1() ) * iSPEED_OF_LIGHT;
  next_shift_time = -1.;
  shiftNum = 0;
  shift_dt = (  get_grid()->getX()[2][0].e1()
                - get_grid()->getX()[1][0].e1() ) * iSPEED_OF_LIGHT;

#if defined(MPI_DEBUG) && defined(MPI_VERSION)
  dfout << "MPI_RANK = " << MPI_RANK << "; shift_dt = " << shift_dt
        << "\niSPEED_OF_LIGHT = " << iSPEED_OF_LIGHT
        << "\nx2 = " << get_grid()->getX()[2][0].e1()
        << "\nx1 = " << get_grid()->getX()[1][0].e1()
        << "\ndeltaX = " << (  get_grid()->getX()[2][0].e1()
                             - get_grid()->getX()[1][0].e1() )
	<< "\n";
#endif /* defined(MPI_DEBUG) && defined(MPI_VERSION) */

  // cout << "Default shift delay time = " << shiftDelayTime << endl;
  // cout << "Shift delta-t = " << shift_dt << endl;


  oopicListIter<ParticleGroup>	pg(*particleGroupList[0]);
  pg.restart();
  if (!pg.Done()) maxN = pg()->get_maxN();
  else maxN = 500; //	choose a default size

  t = 0.0;  // otherwise simulation time won't be initialized

  // distribute particle group information to boundaries.
  ApplyToList(setSpeciesArray(speciesArray),*boundaryList,Boundary);
  oopicListIter<Species> spiter(*speciesList);
  for(spiter.restart();!spiter.Done();spiter++)
    speciesArray[spiter()->getID()]=spiter();

  // set the ptrNGD and the ptrMapNGDs to zero as initial values
  ptrNGD = 0;
  ptrMapNGDs = 0;

}

SpatialRegion::~SpatialRegion()
{
  oopicListIter<Boundary> bIter(*boundaryList);
  for (bIter.restart(); !bIter.Done();	bIter++)
    bIter.deleteCurrent();
  delete boundaryList;
  boundaryList = NULL;
  for (int i=0; i<nSpecies; i++)
    {
      oopicListIter<ParticleGroup> pgIter(*particleGroupList[i]);
      for (pgIter.restart(); !pgIter.Done();	pgIter++)
        pgIter.deleteCurrent();
    }
  delete[] nParticles;
  delete[] particleGroupList;
  particleGroupList = NULL;
  delete fields;
  delete grid;
  fields = NULL;
  grid = NULL;
  // delete mcc;
  // delete the list of NGD's
  NGDList.deleteAll(); // delete the list elements and data for the NGDs
}

NGD* SpatialRegion::initNGD(const ostring& _gasType, const ostring& _analyticF, 
                            const Scalar& _gasDensity, 
                            const Vector2& _p1Grid, const Vector2& _p2Grid, 
                            const int& discardDumpFileNGDDataFlag)
  /* throw(Oops) */{
  // check first if the list is empty:
  if( NGDList.isEmpty() ) {
    try{
      ptrNGD = new NGD(grid, _gasType, _analyticF, _gasDensity,  
                     _p1Grid, _p2Grid, discardDumpFileNGDDataFlag);
    }
    catch(Oops& oops2){
      oops2.prepend("SpatialRegion::initNGD: Error: \n");//ok
      throw oops2;
    }

    NGDList.add(ptrNGD);
    return ptrNGD; 
  }
  
  oopicListIter<NGD> NGDIter(NGDList);
  for ( NGDIter.restart(); !NGDIter.Done(); NGDIter++ ){
    if ( (NGDIter.current())->getGasType() == _gasType ) {
      // this gas density already exists => return a pointer to it.
      return NGDIter.current(); 
    }
  }
  // the GasType was not found in the list, proceed to create it
  try{
    ptrNGD = new NGD(grid, _gasType, _analyticF, _gasDensity,  
                   _p1Grid, _p2Grid, discardDumpFileNGDDataFlag);
  }
  catch(Oops& oops2){
    oops2.prepend("SpatialRegion::initNGD: Error: \n");//OK
    throw oops2;
  }

  NGDList.add(ptrNGD);
  return ptrNGD; 
}

/**
 * A function to give a pointer to the NGDList to the Boundary
 * objects and set the number of NGDs initialized in the 
 * initialized MapNGDs object. 
 */
void SpatialRegion::setNGDListPtrs() {
  // 
  // instantiate an object to handle the NGD mappings 
  // across boundaries.
  // 
  ptrMapNGDs = new MapNGDs(&NGDList, this);
  ptrMapNGDs->setNumNGDs();
  // 
  // allow each Boundary object to point to the MapNGDs object
  // 
  oopicListIter<Boundary>	nextb(*boundaryList);
  for(nextb.restart(); !nextb.Done(); nextb++)
    nextb.current()->setPtrMapNGDs(ptrMapNGDs);
}

#ifdef undef
//--------------------------------------------------------------------
//	Compute the number of particles in this SpatialRegion, return the
// total number, save the number for each species

long SpatialRegion::nParticles(Scalar* nPtcl)
{
  long nTotal = 0;
  for (int i=0; i<nSpecies; i++)
    {
      long n = 0;
      n = CountParticles(i);
      if (nPtcl != NULL) nPtcl[i] = n;
      nTotal += n;
    }
  return nTotal;
}
#endif

long SpatialRegion::CountParticles(int i)
{
  if (nParticleFlag) return nParticles[i]; // caching scheme
  long nTotal = 0;
  long n = 0;
  oopicListIter<ParticleGroup> pgIter(*particleGroupList[i]);
  for (pgIter.restart(); !pgIter.Done(); pgIter++){
    if (pgIter()->get_n()) n += pgIter()->get_n();
    else pgIter.deleteCurrent();
  }
  nTotal += n;
  nParticles[i] = nTotal;
  return nTotal;
}

void SpatialRegion::globalParticleLimit()
{
  long nTotal = 0;
  for (int i=0; i<nSpecies; i++) nTotal += CountParticles(i);
  if (nTotal > particleLimit) increaseParticleWeight();
}

void SpatialRegion::advance() 
  /* throw(Oops) */{

#if defined(MPI_DEBUG) && defined(MPI_VERSION)
  dfout << "MPI_RANK = " << MPI_RANK << "; shift_dt = " << shift_dt
	<< endl;
  dfout << "Begin SpatialRegion::advance(): MPI_RANK = " << MPI_RANK 
        << "t = " << t << endl; 
#endif /* defined(MPI_DEBUG) && defined(MPI_VERSION) */

  // Remove the particles that drop below threshold
  if (getBoltzmannFlag()){  
    oopicListIter<Species> siter(*speciesList);
    for (siter.restart(); !siter.Done(); siter++)
      if (siter()->get_collisionModel()==test){
        int i = siter()->getID();
        addParticleList(fields->testParticleList(*particleGroupList[i]));
      }
  }

  // Advance the particles
  try{
    particleAdvance();
  }
  catch(Oops& oops3){
    oops3.prepend("SpatialRegion::advance: Error: \n"); //done
    throw oops3;
  }

  // Advance the fields
  if (fields->get_freezeFields()) t += dt*getFieldSubFlag();
  else fieldsAdvance();
#ifdef MPI_VERSION
  extern MPI_Comm XOOPIC_COMM;
#endif

  // Shift the region
  if ( next_shift_time < 0.0 ) {
    if ( shiftDelayTime <= t ) next_shift_time = t + shift_dt;
    else                       next_shift_time = shiftDelayTime;
  }
  /************
  cout.precision(15);
  cout << "shift_dt        = " << shift_dt << endl
       << "shiftDelayTime  = " << shiftDelayTime << endl
       << "next_shift_time = " << next_shift_time << endl
       << "dt              = " << dt << endl;
  *************/

#if defined(MPI_DEBUG) && defined(MPI_VERSION)
  cout << "MPI_RANK = " << MPI_RANK << "; shift_dt = " << shift_dt << endl;
  cout << "Begin SpatialRegion::advance(): MPI_RANK = " << MPI_RANK 
       << "; t = " << t << endl; 
#endif /* defined(MPI_DEBUG) && defined(MPI_VERSION) */

  if( shiftDir != 0 && (t >= next_shift_time) ) {


#if defined(MPI_DEBUG) && defined(MPI_VERSION)
    cout << "MPI_RANK = " << MPI_RANK << "  t = " << t << "." << endl
         << "next_shift_time = " << next_shift_time << endl
         << "MPI_RANK = " << MPI_RANK << ": shifting region." << endl;
#endif
    try{
      shiftRegion();
    }
    catch(Oops& oops2){
      oops2.prepend("SpatialRegion::advance: Error: \n");//done
      throw oops2;
    }
  }

}

//--------------------------------------------------------------------
//	Advance particles in spatial region in time by dt, including all its
//

void SpatialRegion::particleAdvance()
  /* throw(Oops) */{
  nParticleFlag = FALSE;
  fields->clearI(); 
  
  oopicListIter<Species> siter(*speciesList);
  for (siter.restart(); !siter.Done(); siter++){
    Species* species = siter.current();
  
    int i = species->getID();
    if (species->cycleNow()){			// this species gets pushed now
      if (!fields->get_freezeFields()) {
        fields->clearI(i);
        fields->setAccumulator(i);
      }

      oopicListIter<ParticleGroup> nextpg(*particleGroupList[i]);

      // if(species->is_coulomb()){
      //	species->initial_cell_list(getJ(),getK());
      //  }

      Scalar species_dt = species->get_subcycleIndex()*getFieldSubFlag()*dt;
      species_dt /= species->get_supercycleIndex();
      for (int super=0; super < species->get_supercycleIndex(); super++){
        if (CountParticles(i)>(species->get_particleLimit()))
          increaseParticleWeight(i);
	

        species->zeroKineticEnergy();
        for (nextpg.restart(); !nextpg.Done(); nextpg++)
          nextpg()->advance(*fields, species_dt);


      // need to pass particles across SRB's
        ApplyToList(passParticles(),*boundaryList,Boundary);
      
        try{
          emit(species); // don't move this without moving the ApplyToList above.
        }
        catch(Oops& oops2){
          oops2.prepend("SpatialRegion::particleAdvance: Error: \n");//done
          throw oops2;
        }



	if (mcc) {
          try{
            addParticleList(mcc->collide(*particleGroupList[i]));
          }
          catch(Oops& oops3){
            oops3.prepend("SpatialRegion::particleAdvance: Error: \n");//done
            throw oops3;
          }
        }

	if (species->is_coulomb()){
	  
	  coulomb = new Coulomb(species, grid, fields, dt, speciesList);
	  
	  coulomb->coulombcollide(particleGroupList);
    
	  delete coulomb;
	}
      }
      species->resetSubcycleCount();
    }
    else {
      species->incSubcycleCount();
      if (!fields->get_freezeFields()) fields->addtoI(i);
    }
  }

 

 /**
   * Do the tunneling ionization if the mcti was instantiated. 
   * Note that this call has different logic than the corresponding
   * call for the mcc class given in the species loop above. 
   * The reason is that we need to control the order in which
   * we ionize the different ion species. For this reason we
   * pass directly the pointer to the array of particle group 
   * lists and do the work in derived classes for the different
   * elements for which the tunneling ionization was implemented. 
   * 
   * D. A. Dimitrov 06/27/01. 
   */
  if (mcti) {
    try{
      mcti->tunnelIonize(particleGroupList);
    }
    catch(Oops& oops3){
      oops3.prepend("SpatialRegion::particleAdvance: Error: \n");
      throw oops3;
    }
  }

  // Need to pass particles across SpatialRegionBoundary's
  // This should go inside the loop when emit goes inside.
  //  ApplyToList(passParticles(),*boundaryList,Boundary);
		
  globalParticleLimit();
  nParticleFlag = FALSE;
  if (!fields->get_freezeFields()) {
    fields->compute_rho(particleGroupList,nSpecies);  //Get the charge density
    try{
      ApplyToList(putCharge_and_Current(t),*boundaryList,Boundary);
    }
    catch(Oops& oops2){
      oops2.prepend("SpatialRegion::particleAdvance: Error: \n");//OK
      throw oops2;
    }

    fields->ZeroCharge();  //not rho but charge from dielectrics, for diag only
    ApplyToList(putCharge(),*boundaryList,Boundary);   //for diag only
  }
}

void SpatialRegion::fieldsAdvance() {
  fields->advance(t, dt);//	advance fields
  t += dt*getFieldSubFlag();
  if (getBoltzmannFlag()) fields->IncBoltzParticles(
                                                    *particleGroupList[fields->get_bSpecies()->getID()]);
  fields->toNodes(t);
}


void SpatialRegion::emit(Species *species) 
  /* throw(Oops) */{
  oopicListIter<Boundary> nextb(*boundaryList);
  for (nextb.restart(); !nextb.Done(); nextb++){ // loop through boundaries
    try{
      addParticleList(nextb.current()->emit(t,getFieldSubFlag()*dt, species));
    }
    catch(Oops& oops){
      oops.prepend("SpatialRegion::emit: Error: \n"); //particleAdvance()
      throw oops;
    }
  }
}

void SpatialRegion::addParticleList(ParticleList& particleList)
{
  // empty Particle list into available ParticleGroups
  oopicListIter<Particle> pIter(particleList);
  for (pIter.restart(); !pIter.Done(); pIter++)
    {
      Particle* p = pIter();
      int s = p->get_speciesID();
      if (s < nSpecies)
        {
          oopicListIter<ParticleGroup> pgIter(*particleGroupList[s]);
          for (pgIter.restart(); !pgIter.Done(); pgIter++)
            if (pgIter()->add(p)) break;
          if (pgIter.Done())			//	no space; add new group
            {
              ParticleGroup* pg;
              if(getInfiniteBFlag() && getElectrostaticFlag())
                pg = new ParticleGroupIBES(maxN, p->get_speciesPtr(), p->get_np2c(), 
                                           p->isVariableWeight());
              else if (getInfiniteBFlag()) // relativistic Infinite B mover
                pg = new ParticleGroupIBEM(maxN, p->get_speciesPtr(), p->get_np2c(), 
                                           p->isVariableWeight());  
              else if (getElectrostaticFlag()) // add ES group
                pg = new ParticleGroupES(maxN, p->get_speciesPtr(), p->get_np2c(),
                                         p->isVariableWeight());
              else if (getSynchRadiationFlag()) // include synchrotron radiation
                pg = new ParticleGroupSR(maxN, p->get_speciesPtr(), p->get_np2c(), 
                                         p->isVariableWeight());
              else if(getNonRelFlag())
                pg = new ParticleGroupNR(maxN, p->get_speciesPtr(), p->get_np2c(), 
                                         p->isVariableWeight());
              else        //	add EM group
                pg = new ParticleGroup(maxN, p->get_speciesPtr(), p->get_np2c(), 
                                       p->isVariableWeight());
              pg->add(p);
              particleGroupList[s]->add(pg);
            }
        }
      pIter.deleteCurrent(); // remove list element and data
    }





}

//---------------------------------------------------------------------
// packParticleGroups() repacks the groups to minimize the number of groups.
// It will result in n full groups plus a partially full remainder group,
// which gets placed at the head of the list. CURRENTLY NON-FUNCTIONAL

void SpatialRegion::packParticleGroups(int i)
{
//  ParticleGroup *current;
  oopicListIter<ParticleGroup> source(*particleGroupList[i]);
  oopicListIter<ParticleGroup> dest(*particleGroupList[i]);
  for (source.restart(); !source.Done(); source++){
    if (!source()->vary_np2c() && source()->get_n() < source()->get_maxN())
      for (dest.restart(); !dest.Done(); dest++){
        if (source() != dest()) 
          if (dest()->add(*source())) break; // TRUE = finished adding
      }
    if (!source()->get_n()) source.deleteCurrent();
  }
}

void SpatialRegion::increaseParticleWeight(void)
{
  for (int i=0; i<nSpecies; i++)
    {
      oopicListIter<ParticleGroup> nextpg(*particleGroupList[i]);
      for (nextpg.restart(); !nextpg.Done(); nextpg++)
        nextpg()->increaseParticleWeight();
    }
   oopicListIter<Boundary>	nextb(*boundaryList);
  for(nextb.restart(); !nextb.Done(); nextb++)
    nextb()->increaseParticleWeight();
}

void SpatialRegion::increaseParticleWeight(int i)
{

  oopicListIter<ParticleGroup> nextpg(*particleGroupList[i]);
  for (nextpg.restart(); !nextpg.Done(); nextpg++)
    nextpg()->increaseParticleWeight();

  //	packParticleGroups(i);

  oopicListIter<Boundary>	nextb(*boundaryList);
  for(nextb.restart(); !nextb.Done(); nextb++)
    nextb()->increaseParticleWeight(i);
}



#ifdef HAVE_HDF5
int SpatialRegion::dumpH5(dumpHDF5 &dumpObj)
{
  std::cout << "Entering spatialRegion::DumpH5"<<endl;

  static int end=-1;
//  int nn;
//  FILE *DropFile;
//  char dropfile[80];
  int seqNumber = 0;
  int mpiRank = 1;
  hid_t fileId, groupId;// datasetId,dataspaceId;
  herr_t status;
  hsize_t rank; 
  hsize_t     dimsf[1];   
  hsize_t dims;
  hid_t scalarType = dumpObj.getHDF5ScalarType();
  hid_t  attrdataspaceId,attributeId; //attrDS;
  int nnnn;

 

  fileId = H5Fopen(dumpObj.getDumpFileName().c_str(), H5F_ACC_RDWR, H5P_DEFAULT);  
  hid_t bGroupId = H5Gcreate(fileId,"/Boundaries",0);




  groupId = H5Gcreate(fileId,"/SpatialRegion",0);
//     oopicListIter<Diag>	nextd(*diagList);
//    nnnn = 0;
//    for (nextd.restart(); !nextd.Done(); nextd++)
//      {
//        if (!(nextd.current()->dumpH5(dumpObj,nnnn)))
//  	cout << "Dump: dump failed to save diagnostics." << endl;
//        nnnn++;
//      }
  rank = 1;
  dimsf[0] = 4;
  string datasetName = "dims";  



// dump dimensions of global grid
 //   dims = 4;
//  #ifdef MPI_VERSION
//    int globalGridSize[4] = {grid->MaxMPIJ,grid->MaxMPIK,grid->M,grid->N};
//  #else
//    int globalGridSize[4] = {grid->getJ(),grid->getK(),1,1};
//  #endif 
//    attrdataspaceId = H5Screate_simple(1, &dims, NULL);
//    attributeId = H5Acreate(groupId, "globalGridSize",H5T_NATIVE_INT, attrdataspaceId, H5P_DEFAULT);
//    status = H5Awrite(attributeId, H5T_NATIVE_INT, globalGridSize);
//    status = H5Aclose(attributeId);
//    status = H5Sclose(attrdataspaceId);
//    cerr << "dumped global grid dimensions: "<< globalGridSize[0]<<"  "<< globalGridSize[1]<<endl;
   
//  // dump positions of grid
//    int J = grid->getJ();
//    int K = grid->getK();
//    Vector2 **X = grid->getX();
//    Scalar *x1List = new Scalar[J+1];
//    Scalar *x2List = new Scalar[K+1];
  
//    for(int i=0;i<=J;i++)
//      x1List[i] = X[i][0].e1();
  
//    for(int i=0;i<=K;i++)
//      x2List[i] = X[0][i].e2();

//    dimsf[0] = J+1;
//    dataspaceId = H5Screate_simple(rank, dimsf, NULL); 
//    datasetId = H5Dcreate(groupId, "x1Nodes", scalarType, dataspaceId, H5P_DEFAULT);
//    status = H5Dwrite(datasetId, scalarType, H5S_ALL,H5S_ALL , H5P_DEFAULT, x1List);
//    status = H5Sclose(dataspaceId); 
//    status = H5Dclose(datasetId);
  
//    dimsf[0] = K+1;
//    dataspaceId = H5Screate_simple(rank, dimsf, NULL); 
//    datasetId = H5Dcreate(groupId, "x2Nodes", scalarType, dataspaceId, H5P_DEFAULT);
//    status = H5Dwrite(datasetId, scalarType, H5S_ALL,H5S_ALL , H5P_DEFAULT, x2List);
//    status = H5Sclose(dataspaceId); 
//    status = H5Dclose(datasetId);

//    delete[] x1List;
//    delete[] x2List;

// dump random seed  as attribute
  dims = 1;
  attrdataspaceId = H5Screate(H5S_SCALAR);
  attributeId = H5Acreate(groupId, "frandseed",H5T_NATIVE_INT, attrdataspaceId, H5P_DEFAULT);
  status = H5Awrite(attributeId, H5T_NATIVE_INT, &frandseed);
  status = H5Aclose(attributeId);
  status = H5Sclose(attrdataspaceId);
  // cerr << "dumped random seed: "<<frandseed<<endl;
// dump time as attribute
  dims = 1;
  attrdataspaceId = H5Screate(H5S_SCALAR);
  attributeId = H5Acreate(groupId, "simulationTime",scalarType, attrdataspaceId, H5P_DEFAULT);
  status = H5Awrite(attributeId, scalarType, &t);
  status = H5Aclose(attributeId);
  status = H5Sclose(attrdataspaceId);
  
  int temp = boundaryList->nItems();

// dump number of boundaries as attribute
  dims = 1;
//attrdataspaceId = H5Screate_simple(1, &dims, NULL);
  
  attrdataspaceId = H5Screate(H5S_SCALAR);
  attributeId = H5Acreate(groupId, "numBoundaries",H5T_NATIVE_INT, attrdataspaceId, H5P_DEFAULT);
  status = H5Awrite(attributeId, H5T_NATIVE_INT, &temp );
  status = H5Aclose(attributeId);
  status = H5Sclose(attrdataspaceId);

  temp = diagList->nItems();

// dump number of diagnostics as attribute
  dims = 1;
  attrdataspaceId = H5Screate(H5S_SCALAR);
  attributeId = H5Acreate(groupId, "numDiagnostics",H5T_NATIVE_INT, attrdataspaceId, H5P_DEFAULT);
  status = H5Awrite(attributeId, H5T_NATIVE_INT, &temp  );
  status = H5Aclose(attributeId);
  status = H5Sclose(attrdataspaceId);

// dump number of species  as attribute
  dims = 1;
  attrdataspaceId = H5Screate(H5S_SCALAR);
  attributeId = H5Acreate(groupId, "nSpecies",H5T_NATIVE_INT, attrdataspaceId, H5P_DEFAULT);
  status = H5Awrite(attributeId, H5T_NATIVE_INT, &nSpecies);
  status = H5Aclose(attributeId);
  status = H5Sclose(attrdataspaceId);

// dump dimensions of region as attribute
  dims = 4;
  attrdataspaceId = H5Screate_simple(1, &dims, NULL);

  attributeId = H5Acreate(groupId, "dims",scalarType, attrdataspaceId, H5P_DEFAULT);
   
  Scalar xw[4];  // in this format:  xs, ys, xf, yf
  Vector2 **X=grid->getX();
  xw[0] = X[0][0].e1();
  xw[1] = X[0][0].e2();
  xw[2] = X[getJ()][getK()].e1();
  xw[3] = X[getJ()][getK()].e2();
  // status = H5Dwrite(datasetId, scalarType, H5S_ALL, H5S_ALL,
  //	      H5P_DEFAULT, xw);
  
  status = H5Awrite(attributeId, scalarType, &xw);
  status = H5Aclose(attributeId);
  status = H5Sclose(attrdataspaceId);



  H5Gset_comment(fileId,"/Boundaries","test comment");







//  // dump dimensions of region as attribute
//    dims = 3;
//    attrdataspaceId = H5Screate_simple(1, &dims, NULL);

//    attributeId = H5Acreate(groupId, "dims",H5T_NATIVE_STRING, attrdataspaceId, H5P_DEFAULT);
   
//    Scalar xw[4];  // in this format:  xs, ys, xf, yf
//    Vector2 **X=grid->getX();
//    xw[0] = X[0][0].e1();
//    xw[1] = X[0][0].e2();
//    xw[2] = X[getJ()][getK()].e1();
//    xw[3] = X[getJ()][getK()].e2();
//    // status = H5Dwrite(datasetId, scalarType, H5S_ALL, H5S_ALL,
//    //	      H5P_DEFAULT, xw);
  
//    status = H5Awrite(attributeId, scalarType, &xw);
//    status = H5Aclose(attributeId);
//    status = H5Sclose(attrdataspaceId);


  hid_t       file_id; //group1_id, group2_id, group3_id;  /* identifiers */
  hid_t       aid, atype, attr;
   
  
  /* Create a new file using default properties. */
  file_id = fileId;
  
  dims = 4;
  aid = H5Screate(H5S_SCALAR);
  atype = H5Tcopy (H5T_C_S1);
  
  status = H5Tset_size (atype, 80*nSpecies);
  
  attr = H5Acreate (bGroupId, "data_contents", atype, aid, H5P_DEFAULT);
  
  char *what = new char[80*nSpecies];
  if (nSpecies!=0){
    strcpy(what, "");
  }
    
  // write a comma delimited list of the species names into an attribute
   oopicListIter<Species> spIter(*speciesList);
   spIter.restart();
   if(!spIter.Done())
     {
       strcat(what,spIter.current()->get_name().c_str() );
       spIter++;
     }

   while(!spIter.Done())
     {
       strcat(what,",");
       strcat(what,spIter.current()->get_name().c_str() );
       spIter++;
       cout<<"count"<<what<<endl;
     }
   cout<<what<<endl;
  

   
//     strstream spec;
//      spec << "/" << speciesIter.current()->get_name().c_str() <<ends;    
//      string spGroupName = spec.str();



   status = H5Awrite (attr, atype,what );

   //status = H5Gclose(bGroupId);

   status = H5Sclose (aid);

   status = H5Aclose (attr);

   delete [] what;
  //    // make a drop file entry
  //    nn = strlen(theDumpFile);               
  //    strcpy(dropfile, theDumpFile);
  //    dropfile[nn-2] = 'r';
  //    if ((DropFile = fopen(dropfile, "a")) == NULL)
  //      {
  //        printf("filename = %s\n", dropfile);
  //        puts("Drop: open failed error#1");
  //        return (0);
  //      }
  //    fprintf(DropFile, "%g ", t);
  
  //    for (int i=0; i<nSpecies; i++){
  //      int number = CountParticles(i);
  //      fprintf(DropFile, " %d ", number);
  //    }
  
  //    // Drop the number of shifts the spatial region has done.
  //    fprintf(DropFile," %lu ", shiftNum);
  
  //    fprintf(DropFile, "\n");
  //    fclose(DropFile);
 
//  Dump Boundaries
  oopicListIter<Boundary>	nextb(*boundaryList);
  nnnn = 0;
  for (nextb.restart(); !nextb.Done(); nextb++)
    {
      // string groupName = nn;
      // hid_t groupId = H5Gcreate(fileId, groupName.c_str(),0);
      if (!(nextb.current()->dumpH5(dumpObj,nnnn)))
        printf("Dump: dump failed to save boundaries");
      nnnn++;
    }
	
 H5Gclose(bGroupId);

//  Dump diagnostics
  //   {
  //      oopicListIter <Diagnostics> Diter(*theDiagnostics);
  //      for(Diter.restart();!Diter.Done();Diter++)
  //        Diter.current()->dumpH5(dumpObj);
  //    }  

//
// dump fields
// 

 //  int J = grid->getJ();
//    int K = grid->getK();

//   for(int j=0;j<J;j++){
//      for(int k=0;k<K;k++){
//        cout << fields->getIntEdl()[j][k].e1()<<" ";
//        //  cout << fields->getENode()[j][k].e1()<<"\n";
//      }
//    }
 
 fields->dumpH5(dumpObj);
//
// dump NGD
// 
  oopicListIter<NGD> NGDIter(NGDList);
  nnnn = 0;
  for ( NGDIter.restart(); !NGDIter.Done(); NGDIter++ ){ 
    NGDIter.current()->dumpH5(dumpObj,nnnn);
    nnnn++;
  }

// 
// dump Species
// 

  int isp= 0;
  oopicListIter<Species> speciesIter(*speciesList);
  for (speciesIter.restart(); !speciesIter.Done(); speciesIter++){
        
    int specID = speciesIter.current()->getID();
    int ptclGrpCount = 0;

    //    strstream sp;

    //stringstream sp;


    //sp << "/" << speciesIter.current()->get_name().c_str() <<ends;    
    //string spGroupName = sp.str();
    //   hid_t spGroupId = H5Gcreate(fileId,spGroupName.c_str(),0);
     hid_t spGroupId = H5Gcreate(fileId,speciesIter.current()->get_name().c_str(),0);

     oopicListIter<ParticleGroup> pgscan(*particleGroupList[specID]);
     for(pgscan.restart();!pgscan.Done();pgscan++){
       //      strstream s;

      stringstream s;
    
      // s << spGroupName <<"/pGrp"<< ptclGrpCount <<ends;
      s << speciesIter.current()->get_name() << "/pGrp"<< ptclGrpCount <<ends;
      string pGroupName = s.str();
           
      pgscan.current()->dumpH5(dumpObj, grid,pGroupName);
      ptclGrpCount++;
    }
    
    attrdataspaceId = H5Screate(H5S_SCALAR);
    attributeId = H5Acreate(spGroupId, "nPtclGrp",H5T_NATIVE_INT, attrdataspaceId, H5P_DEFAULT);
    status = H5Awrite(attributeId, H5T_NATIVE_INT, &ptclGrpCount );
    status = H5Aclose(attributeId);
    status = H5Sclose(attrdataspaceId);

    H5Gclose(spGroupId);
    isp++;
  }
//  H5Dclose(datasetId);
  H5Gclose(groupId);
  H5Fclose(fileId);
   
  return (1);
}
#endif

int SpatialRegion::Dump(FILE* DMPFile)
{

  cerr << "Entering sptlrgn::Dump"<<endl;

  static int end=-1;
  int nn;
  FILE *DropFile;
  // char dropfile[50]; // this is too small
  char dropfile[180];
  XGWrite(&frandseed, 4, 1, DMPFile, "long");
	
  int RnameLength = Name.length();
  // Write the length of the region name
  XGWrite(&RnameLength,4,1,DMPFile,"int");

  // Write the region name
  XGWrite(Name.c_str(),1,RnameLength+1,DMPFile,"char");

  // Write simulation time
  XGWrite(&t, ScalarInt, 1, DMPFile, ScalarChar);
	 
  // Write the physical dimensions of the system
  {
    Scalar xw[4];  // in this format:  xs, ys, xf, yf
    Vector2 **X=grid->getX();
    xw[0] = X[0][0].e1();
    xw[1] = X[0][0].e2();
    xw[2] = X[getJ()][getK()].e1();
    xw[3] = X[getJ()][getK()].e2();
    XGWrite(xw,ScalarInt,4,DMPFile,ScalarChar);
  }



  // make a drop file entry
/*  commented out by W.Y.  This is not used.  I also commented out extern const char * theDumpFile above.
  strcpy(dropfile, theDumpFile);
  nn = strlen(dropfile);
  dropfile[nn-2] = 'r';
  if ((DropFile = fopen(dropfile, "a")) == NULL)
    {
      printf("filename = %s\n", dropfile);
      puts("Drop: open failed error#1");
      return (0);
    }
  fprintf(DropFile, "%g ", t);

  for (int i=0; i<nSpecies; i++){
    int number = CountParticles(i);
    fprintf(DropFile, " %d ", number);
  }
  
  // Drop the number of shifts the spatial region has done.
  fprintf(DropFile," %lu ", shiftNum);

  fprintf(DropFile, "\n");
  fclose(DropFile);
*/	
 
  //  Dump Boundaries
  
  // dump the # of bounaries

  { int n;
  n = boundaryList->nItems();


  XGWrite(&n,4,1,DMPFile,"int");

  }
  oopicListIter<Boundary>	nextb(*boundaryList);
  int nnnn = 0;
  for (nextb.restart(); !nextb.Done(); nextb++)
    {
      if (!(nextb.current()->Dump(DMPFile)))
        printf("Dump: dump failed to save boundaries");
    }
	
  //  Dump asked for diagnostic
  // dump the # of diagnostics
  { int n;
  n = diagList->nItems();
  XGWrite(&n,4,1,DMPFile,"int");
  }
  oopicListIter<Diag>	nextd(*diagList);
  for (nextd.restart(); !nextd.Done(); nextd++)
    {
      if (!(nextd.current()->Dump(DMPFile)))
        cout << "Dump: dump failed to save diagnostics." << endl;
 }	

  end = -1;
  
  fields->Dump(DMPFile);


  //
  // dump of the NGDs
  // 
  oopicListIter<NGD> NGDIter(NGDList);

  for ( NGDIter.restart(); !NGDIter.Done(); NGDIter++ ){ 
    NGDIter.current()->Dump(DMPFile); 
  }
  XGWrite(&nSpecies, 4, 1, DMPFile, "int");

  for (int isp=0; isp<nSpecies; isp++){
    oopicListIter<ParticleGroup> pgscan(*particleGroupList[isp]);
    for(pgscan.restart();!pgscan.Done();pgscan++)
      pgscan.current()->Dump(DMPFile,grid);
  }

  XGWrite(&end, 4, 1, DMPFile, "int");  //end of particle marker
  XGWrite(&end, 4, 1, DMPFile, "int");
  XGWrite(&end, 4, 1, DMPFile, "int");	
 
  return (1);
	
}


int SpatialRegion::Restore_2_00(FILE* DMPFile)
  /* throw(Oops) */{
  int j1test, k1test, j2test, k2test;
  int found;

  XGRead(&frandseed, 4, 1, DMPFile, "long");
	
  cerr << "\nentered  SpatialRegion::Restore_2_00\n";

  int ReadNameLength;
  char buf[512];
	
  XGRead(&ReadNameLength,4,1,DMPFile,"int");   // Read length of the region name
  XGRead(buf,1,ReadNameLength+1,DMPFile,"char"); // Read region name

  XGRead(&t, ScalarInt, 1, DMPFile, ScalarChar); // Read simulation time

  //  Restore Boundaries must be in the same order as dumped
  // Should give them titles as in diagnostics

  XGRead(&j1test, 4, 1, DMPFile, "int");
  XGRead(&k1test, 4, 1, DMPFile, "int");
  XGRead(&j2test, 4, 1, DMPFile, "int");
  XGRead(&k2test, 4, 1, DMPFile, "int");  
  oopicListIter<Boundary>	nextb(*boundaryList);
  for (nextb.restart(); !nextb.Done(); nextb++){
    if (!(nextb.current()->Restore_2_00(DMPFile, j1test, k1test, j2test, k2test))){
      stringstream ss (stringstream::in | stringstream::out);
      ss << "SpatialRegion::Restore_2_00: Error: \n"<<
        "Input file has boundaries that can't be matched to dump file." << endl;
     
      std::string msg;
      ss >> msg;
      Oops oops(msg);
      throw oops;    // exit() dump::Restore  dump::Restore_2_00

    }
    XGRead(&j1test, 4, 1, DMPFile, "int");
    XGRead(&k1test, 4, 1, DMPFile, "int");
    XGRead(&j2test, 4, 1, DMPFile, "int");
    XGRead(&k2test, 4, 1, DMPFile, "int");
  } 

	
  //  Restore diagnostic
  XGRead(&j1test, 4, 1, DMPFile, "int");
  XGRead(&k1test, 4, 1, DMPFile, "int");
  XGRead(&j2test, 4, 1, DMPFile, "int");
  XGRead(&k2test, 4, 1, DMPFile, "int"); 
  int titleLength;
  XGRead(&titleLength, 4,1,DMPFile,"int");
  XGRead(buf,1,titleLength+1, DMPFile, "char");
  ostring titleTest=ostring(buf);
  while (j1test>=0){
    found = FALSE;
    oopicListIter<Diag>	nextd(*diagList);
    for (nextd.restart(); !nextd.Done(); nextd++){
      if (nextd.current()->Restore_2_00(DMPFile, j1test, k1test, j2test, k2test, titleTest)){
        found = TRUE;
        break;
      }
    }
    if(!found){
       printf("Input file has diagnostics that can't be matched to dump file.");
      sprintf(buf, "Assuming that you dropped %s \n",titleTest());
      sprintf(buf, "If you 'save' again %s's data will be lost. \n", titleTest());
      //			cout << "Assuming that you dropped " << (titleTest.()) << endl;
      //			cout << "If you 'save' again " << (titleTest.()) << "'s data will be lost. << endl;
    }	
		
    XGRead(&j1test, 4, 1, DMPFile, "int");
    XGRead(&k1test, 4, 1, DMPFile, "int");
    XGRead(&j2test, 4, 1, DMPFile, "int");
    XGRead(&k2test, 4, 1, DMPFile, "int"); 
    XGRead(&titleLength, 4,1,DMPFile,"int");
    XGRead(buf,1,titleLength+1, DMPFile, "char");
    titleTest=ostring(buf);
  }
 
  fields->Restore_2_00(DMPFile);

  int speciesID;
  Scalar np2c0;
  int vary_np2c;
  Species* species=0;

  XGRead(&nSpecies, 4, 1, DMPFile, "int");
  XGRead(&speciesID, 4, 1, DMPFile, "int");
  while (!(speciesID==-1)){

    oopicListIter<Species> speciesIter(*speciesList);
    for (speciesIter.restart(); !speciesIter.Done(); speciesIter++)
      if (speciesIter.current()->getID() == speciesID)
        species = speciesIter.current();

    XGRead(&vary_np2c, 4, 1, DMPFile, "int");
    XGRead(&np2c0, ScalarInt, 1, DMPFile, ScalarChar);

    fprintf(stderr, "\nduplicateParticles=%d, np2c0=%f", 
	    duplicateParticles, np2c0);
		
    // Making a new particleGroup
		
    ParticleGroup* pg;
    

    if(getInfiniteBFlag() && getElectrostaticFlag())
      pg = new ParticleGroupIBES(maxN, species, np2c0, vary_np2c);
    else if (getInfiniteBFlag()) // relativistic Infinite B mover
      pg = new ParticleGroupIBEM(maxN, species, np2c0, vary_np2c);  
    else if (getElectrostaticFlag()) // add ES group
      pg = new ParticleGroupES(maxN, species, np2c0, vary_np2c);
    else if (getSynchRadiationFlag()) // add SR group
      pg = new ParticleGroupSR(maxN, species, np2c0, vary_np2c);
    else if(getNonRelFlag())
      pg = new ParticleGroupNR(maxN, species, np2c0, vary_np2c);
    else        //	add EM group
      pg = new ParticleGroup(maxN, species, np2c0, vary_np2c);

    pg->Restore_2_00(DMPFile);

    ParticleGroup** k;
    if(duplicateParticles > 0) 
      { 
	fprintf(stderr, "\nduplicateParticles = %d", duplicateParticles);
	k = pg->duplicateParticles(duplicateParticles); 
	if(k != NULL)
	  {
	    for(int i=0; i < duplicateParticles; i++)
	      {
		particleGroupList[speciesID]->add(k[i]);
	      }
	  }
	else
	  {
	    fprintf(stderr, "\nError duplicating particles");
	  }
      }
    particleGroupList[speciesID]->add(pg);
    
    XGRead(&speciesID, 4, 1, DMPFile, "int");
  }

  return (1);

}

#ifdef HAVE_HDF5
int SpatialRegion::restoreH5(dumpHDF5 restoreObj)
/* throw(Oops) */{

  // cerr << "entered SpatialRegion::restoreH5().\n"; 

  hid_t fileId, groupId; // datasetId,dataspaceId;
  herr_t status;
  string outFile;
//  hsize_t rank; 
//  hsize_t     dimsf[1];   
//  hsize_t dims;
  hid_t scalarType = restoreObj.getHDF5ScalarType();
  hid_t attrId;// attrspaceId
  hid_t bMetaGroupId; 
  string restoreFile = restoreObj.getDumpFileName();

  fileId = H5Fopen(restoreObj.getDumpFileName().c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
  if(fileId<0) {
    std::cerr << "Unable to open file '" << restoreObj.getDumpFileName()
	      << "'.  Error # " << fileId << std::endl;
    return(-1);
  }
  
// Open the group for the region
  groupId = H5Gopen(fileId, "/SpatialRegion");

// Read in the field data from the file

// Read the randomseed attribute
  attrId = H5Aopen_name(groupId, "frandseed");
  status = H5Aread(attrId,H5T_NATIVE_INT, &frandseed);
  //  cerr << "read random seed: "<<frandseed<<endl;
// Close the attribute
  status = H5Aclose(attrId);

// Read the time attribute
  attrId = H5Aopen_name(groupId, "simulationTime");
  status = H5Aread(attrId,scalarType , &t);
  // cerr << " read time: "<<time<<endl;
// Close the attribute
  status = H5Aclose(attrId);

  //  cerr << "read time.\n";

//read dimensions of region
  attrId = H5Aopen_name(groupId, "dims");
  // attrspaceId = H5Aget_space(attrId );
  Scalar xw[4];
  status = H5Aread(attrId, scalarType, xw);
  H5Aclose(attrId);
 
// Read the number boundaries attribute
  int numBoundaries;
  attrId = H5Aopen_name(groupId, "numBoundaries");
  status = H5Aread(attrId,H5T_NATIVE_INT , &numBoundaries);
  //  cerr << " read numBoundaries: "<<numBoundaries<<endl;
  status = H5Aclose(attrId);

  // status = H5Dclose(datasetId);
   
  bMetaGroupId = H5Gopen(fileId, "/Boundaries");
  for(int i=1;i<numBoundaries;i++) {
//    Scalar A1,A2,B1,B2;  // phys. dimensions of boundary
    Scalar bDims[4];
    hid_t bGroupId;
    int BoundaryType; // Type of this boundary
    int WasRestored=0; //Did one of the boundaries restore from this boundary?
    int nQuads;  // number of ordered quads of floats to restore
    
    // read in the boundary extent
   //read dimensions of boundary

    //    strstream s;

    stringstream s;

    s <<  "boundary" << i <<ends;    // ends provides the null terminator (don`t forget that)
    string bname = s.str();

    bGroupId = H5Gopen(bMetaGroupId, bname.c_str());

    bname = "/Boundaries/" + bname;
    attrId = H5Aopen_name(bGroupId, "dims" );
    // attrspaceId = H5Dget_space(datasetId );
    status = H5Aread(attrId, scalarType, bDims);
    H5Aclose(attrId);
    
// read in boundary type
    attrId = H5Aopen_name(bGroupId, "boundaryType" );
    // attrspaceId = H5Dget_space(datasetId );
    status = H5Aread(attrId, H5T_NATIVE_INT, &BoundaryType);
    H5Aclose(attrId);
    
    attrId = H5Aopen_name(bGroupId, "nQuads" );
    // attrspaceId = H5Dget_space(datasetId );
    status = H5Aread(attrId, H5T_NATIVE_INT, &nQuads);
    H5Aclose(attrId);

    if(nQuads>0) {
// try to find a matching boundary
      oopicListIter<Boundary>	nextb(*boundaryList);
      for (nextb.restart(); !nextb.Done()&& WasRestored==0; nextb++){
	//if(BoundaryType != -4)
		WasRestored=nextb.current()->restoreH5(restoreObj,BoundaryType,bname,nQuads);
      
 }
    }
  H5Gclose(bGroupId);
  }
  H5Gclose(bMetaGroupId);

  //
// restore diagnostics (empty for now)
  //

  //
// restore fields
  //
   fields->restoreH5(restoreObj);

   //
// restore NGD
   //

  oopicListIter<NGD> NGDIter(NGDList);
  int nnnn = 0;
  for ( NGDIter.restart(); !NGDIter.Done(); NGDIter++ ){ 
    //    strstream s;

    stringstream s;

    s << "NGD" << nnnn <<ends;   
    string ngdGrpName = s.str();
    try{
      NGDIter.current()->restoreH5(restoreObj,ngdGrpName); 
    }
    catch(Oops& oops){
      oops.prepend(" SpatialRegion::restoreH5: Error: \n");//dumpHDF5::restoreH5
      throw oops;
    }
    nnnn++;
}

   //
// restore species
   //
  //  cerr << "restoring Species";


  int speciesID;
  Scalar np2c0;
  int vary_np2c;
  

// Read the nSpecies attribute
  attrId = H5Aopen_name(groupId, "nSpecies");
  status = H5Aread(attrId,H5T_NATIVE_INT , &nSpecies);
  status = H5Aclose(attrId);

  // for(int i=0;i<nSpecies;i++)


  oopicListIter<Species> speciesIter(*speciesList);
  for (speciesIter.restart(); !speciesIter.Done(); speciesIter++)
    {
      
//        strstream sp;
//        sp << "/species" << i <<ends;
      ostring spGrpName = speciesIter.current()->get_name();
      Species* species = speciesIter.current();

      hid_t spGroupId = H5Gopen(fileId,speciesIter.current()->get_name().c_str());
// Read the nPtclGrp attribute
       int nPtclGrp;
       attrId = H5Aopen_name(spGroupId, "nPtclGrp");
       status = H5Aread(attrId,H5T_NATIVE_INT , &nPtclGrp);
       status = H5Aclose(attrId);

      for(int j=0;j<nPtclGrp;j++) 
	{
          //	  strstream s;

         stringstream s;


	  s << spGrpName <<"/pGrp"<<j<<ends;  
	  string pGrpName = s.str();
	  hid_t pGroupId = H5Gopen(fileId,pGrpName.c_str());
// Read the speciesID attribute
	  attrId = H5Aopen_name(pGroupId, "speciesID");
	  status = H5Aread(attrId,H5T_NATIVE_INT , &speciesID);
	  status = H5Aclose(attrId);
      
//  	  oopicListIter<Species> speciesIter(*speciesList);
//  	  for (speciesIter.restart(); !speciesIter.Done(); speciesIter++)
//  	    if (speciesIter.current()->getID() == speciesID)
//  	      species = speciesIter.current();
      
// Read the vary_np2c attribute
	  attrId = H5Aopen_name(pGroupId, "vary_np2c");
	  status = H5Aread(attrId,H5T_NATIVE_INT , &vary_np2c);
	  status = H5Aclose(attrId);
      
// Read the np2c attribute
	  attrId = H5Aopen_name(pGroupId, "np2c");
	  status = H5Aread(attrId,scalarType , &np2c0);
	  status = H5Aclose(attrId);

// Making a new particleGroup
	  np2c0 *= np2cFactor/(Scalar(1+duplicateParticles));;
	  ParticleGroup* pg;
	  if(getInfiniteBFlag() && getElectrostaticFlag())
	    pg = new ParticleGroupIBES(maxN, species, np2c0, vary_np2c);
	  else if (getInfiniteBFlag()) // relativistic Infinite B mover
	    pg = new ParticleGroupIBEM(maxN, species, np2c0, vary_np2c);  
	  else if (getElectrostaticFlag()) // add ES group
	    pg = new ParticleGroupES(maxN, species, np2c0, vary_np2c);
	  else if (getSynchRadiationFlag()) // add SR group
	    pg = new ParticleGroupSR(maxN, species, np2c0, vary_np2c);
	  else if(getNonRelFlag())
	    pg = new ParticleGroupNR(maxN, species, np2c0, vary_np2c);
	  else        //	add EM group
	    pg = new ParticleGroup(maxN, species, np2c0, vary_np2c);

	  pg->restoreH5(restoreObj, grid, np2cFactor,pGrpName);
	  
	  ParticleGroup** k;
	  if(duplicateParticles > 0) 
	    { 
	      fprintf(stderr, "\nduplicateParticles = %d", duplicateParticles);
	      k = pg->duplicateParticles(duplicateParticles); 
	      if(k != NULL)
		{
		  for(int i=0; i < duplicateParticles; i++)
		    {
		      particleGroupList[speciesID]->add(k[i]);
		    }
		}
	      else
		{
		  fprintf(stderr, "\nError duplicating particles");
		}
	    }
	  
	  particleGroupList[speciesID]->add(pg);
	  H5Gclose(pGroupId);
	}
      H5Gclose(spGroupId);
    }
 
  H5Gclose(groupId);
  H5Fclose(fileId);
  return(1);
}
#endif

int SpatialRegion::Restore(FILE* DMPFile)
{
  Scalar x1min, x2min, x1max, x2max;
  Scalar gx1min, gx2min, gx1max, gx2max;
  Vector2 **X=grid->getX();

  int N_Boundaries;
//  int found;
  int i;

  XGRead(&frandseed, 4, 1, DMPFile, "long");
	
  int ReadNameLength;
  char buf[512];
	
  XGRead(&ReadNameLength,4,1,DMPFile,"int");   // Read length of the region name
  XGRead(buf,1,ReadNameLength+1,DMPFile,"char"); // Read region name

  XGRead(&t, ScalarInt, 1, DMPFile, ScalarChar); // Read simulation time

  //  Restore Boundaries must be in the same order as dumped
  // Should give them titles as in diagnostics


  // read in the dimensions of the dumpfile being read
  XGRead(&x1min, ScalarInt, 1, DMPFile, ScalarChar);
  XGRead(&x2min, ScalarInt, 1, DMPFile, ScalarChar);
  XGRead(&x1max, ScalarInt, 1, DMPFile, ScalarChar);
  XGRead(&x2max, ScalarInt, 1, DMPFile, ScalarChar);

  // decide if this is in our domain so we need to restore some of it
  gx1min = X[0][0].e1();
  gx2min = X[0][0].e2();
  gx1max = X[getJ()][getK()].e1();
  gx2max = X[getJ()][getK()].e2();

  // all these checks exit if the file we're reading is out
  // of our domain.
  if(x1min > gx1max) return 1;
  if(x2min > gx2max) return 1;
  if(x1max < gx1min) return 1;
  if(x2max < gx2min) return 1;

	
  //read in the number of boundaries
  XGRead(&N_Boundaries, 4, 1, DMPFile, "int"); 

  // read in the dump information for each boundary
  for(i=0;i<N_Boundaries;i++) {
    Scalar A1,A2,B1,B2;  // phys. dimensions of boundary
    int BoundaryType; // Type of this boundary
    int WasRestored=0; //Did one of the boundaries restore from this boundary?
    int nQuads;  // number of ordered quads of floats to restore
		
    // read in the boundary extent
    XGRead(&A1,ScalarInt,1,DMPFile,ScalarChar);
    XGRead(&A2,ScalarInt,1,DMPFile,ScalarChar);
    XGRead(&B1,ScalarInt,1,DMPFile,ScalarChar);
    XGRead(&B2,ScalarInt,1,DMPFile,ScalarChar);

    XGRead(&BoundaryType,4,1,DMPFile,"int");
    // This next int tells us if there is anything to restore
    XGRead(&nQuads,4,1,DMPFile,"int");
    if(nQuads>0) {
      // try to find a matching boundary
      oopicListIter<Boundary>	nextb(*boundaryList);
      for (nextb.restart(); !nextb.Done()&& WasRestored==0; nextb++){
        WasRestored=nextb.current()->Restore(DMPFile,BoundaryType,A1,A2,B1,B2,nQuads);
				
      }
    }
    if(WasRestored==0)  { //nothing to restore:  load in the dummy stuff
      XGRead(&nQuads,4,1,DMPFile,"int");
      XGRead(&nQuads,4,1,DMPFile,"int");
      if(nQuads > 0) {  //wasn't restored, but there's data we have to read
        int l;
        Scalar QuadIn[4];
        for(l=0;l<nQuads;l++)
          XGRead(&QuadIn,ScalarInt,4,DMPFile,ScalarChar);
      }
    }
  }
	
  //  Restore diagnostic

  int N_Diags=0;
  XGRead(&N_Diags, 4, 1, DMPFile, "int");

  for(i=0;i<N_Diags;i++) {
    int WasRestored=0;
    Scalar A1,A2,B1,B2;
    int xlength;
    int ylength;
    int tlength;
    char title[256];
    int HistFlag;

    XGRead(&A1,ScalarInt,1,DMPFile,ScalarChar);
    XGRead(&A2,ScalarInt,1,DMPFile,ScalarChar);
    XGRead(&B1,ScalarInt,1,DMPFile,ScalarChar);
    XGRead(&B2,ScalarInt,1,DMPFile,ScalarChar);
		
    XGRead(&xlength,4,1,DMPFile,"int");
    XGRead(&ylength,4,1,DMPFile,"int");

    XGRead(&tlength,4,1,DMPFile,"int");

    XGRead(title,1,tlength+1,DMPFile,"char"); // +1 to get the null
    XGRead(&HistFlag,4,1,DMPFile,"int");

    if(HistFlag) {
      oopicListIter<Diag>	nextd(*diagList);
      for (nextd.restart(); !nextd.Done() && WasRestored==0; nextd++){
        WasRestored=nextd.current()->Restore(DMPFile,A1,A2,B1,B2,xlength,ylength,title);
      }
      if(WasRestored==0) { //nothing to restore:  load in the data
        int datlen;
        int dummy[11];
        int l;
				// first read the generic history stuff
        XGRead(&datlen,4,1,DMPFile,"int");
				// Read in two unused ints (hist_hi and alloc_size)
        XGRead(dummy,4,2,DMPFile,"int");
				// read in the unused data
        for(l=0;l<datlen;l++) { 
          Scalar dummy2;
          XGRead(&dummy2,ScalarInt,1,DMPFile,ScalarChar);
        }
				// Now read the particular history stuff
        XGRead(&datlen,4,1,DMPFile,"int");
				// Read in 11 unused ints 
        XGRead(dummy,4,11,DMPFile,"int");
				// read in the unused data
        for(l=0;l<datlen;l++) { 
          Scalar dummy2;
          XGRead(&dummy2,ScalarInt,1,DMPFile,ScalarChar);
        }
				


      }


    }
  }
 
  fields->Restore(DMPFile);
  //
  // restore of the neutral gas densities
  // 
  oopicListIter<NGD> NGDIter(NGDList);
  for ( NGDIter.restart(); !NGDIter.Done(); NGDIter++ ) 
    NGDIter.current()->Restore(DMPFile); 

  int speciesID;
  Scalar np2c0;
  int vary_np2c;
  Species* species=0;

  XGRead(&nSpecies, 4, 1, DMPFile, "int");
  XGRead(&speciesID, 4, 1, DMPFile, "int");
  while (!(speciesID==-1)){

    oopicListIter<Species> speciesIter(*speciesList);
    for (speciesIter.restart(); !speciesIter.Done(); speciesIter++)
      if (speciesIter.current()->getID() == speciesID)
        species = speciesIter.current();
    
    XGRead(&vary_np2c, 4, 1, DMPFile, "int");
    XGRead(&np2c0, ScalarInt, 1, DMPFile, ScalarChar);
		
    // Making a new particleGroup
		
    np2c0 *= np2cFactor/(Scalar(1+duplicateParticles));
    ParticleGroup* pg;
    if(getInfiniteBFlag() && getElectrostaticFlag())
      pg = new ParticleGroupIBES(maxN, species, np2c0, vary_np2c);
    else if (getInfiniteBFlag()) // relativistic Infinite B mover
      pg = new ParticleGroupIBEM(maxN, species, np2c0, vary_np2c);  
    else if (getElectrostaticFlag()) // add ES group
      pg = new ParticleGroupES(maxN, species, np2c0, vary_np2c);
    else if (getSynchRadiationFlag()) // add SR group
      pg = new ParticleGroupSR(maxN, species, np2c0, vary_np2c);
    else if(getNonRelFlag())
      pg = new ParticleGroupNR(maxN, species, np2c0, vary_np2c);
    else        //	add EM group
      pg = new ParticleGroup(maxN, species, np2c0, vary_np2c);

    pg->Restore(DMPFile, grid, np2cFactor);
    ParticleGroup** k;
    if(duplicateParticles > 0) 
      { 
	//	fprintf(stderr, "\nduplicateParticles = %d", duplicateParticles);
	k = pg->duplicateParticles(duplicateParticles); 
	if(k != NULL)
	  {
	    for(int i=0; i < duplicateParticles; i++)
	      {
		particleGroupList[speciesID]->add(k[i]);
	      }
	  }
	else
	  {
	    fprintf(stderr, "\nError duplicating particles");
	  }
      }
    particleGroupList[speciesID]->add(pg);

    XGRead(&speciesID, 4, 1, DMPFile, "int");
  }

  return (1);
}

void SpatialRegion::shiftRegion() 
  /* throw(Oops) */{

#ifdef MPI_VERSION
  extern int MPI_RANK;
#endif

  //
  // Update the next shift time
  //
  next_shift_time += shift_dt;

  // Increment the shift count
  shiftNum++;

  //
  // Shift all of the species and send out
  //
  // cout << "MPI_RANK = " << MPI_RANK << ": shifting particles." << endl;
  oopicListIter<Species> siter(*speciesList);
  for (siter.restart(); !siter.Done(); siter++){

    // Get the species
    Species* species = siter.current();
    int i = species->getID();

    // Shift all of the particle groups for that species
    // This collects all of the particles into the boundaries
    oopicListIter<ParticleGroup> nextpg(*particleGroupList[i]);
    for (nextpg.restart(); !nextpg.Done(); nextpg++) 
      nextpg()->shiftParticles(*fields, shift);
  }

  //
  // Pass the particles to the other region.
  //

  // Determine the boundary that is shifted out
  Boundary* bPtrOut = 0;
  switch(shiftDir){
   
  case 1:
    bPtrOut=grid->GetBC2()[0][1];
    break;

  case 2:
    bPtrOut=grid->GetBC2()[grid->getJ()][1];
    break;

  case 3:
    bPtrOut=grid->GetBC1()[1][0];
    break;

  case 4:
    bPtrOut=grid->GetBC1()[1][grid->getK()];
    break;

  }

  // Pass the particles.  This does nothing except for spatial
  // region boundaries.  Presumably, none of the other regions
  // collected particles.
  if(!bPtrOut){
    static bool outPrinted = false;
    if(!outPrinted) cout << "Error - no outward boundary for shiftDir = " << 
                      shiftDir << endl;
    outPrinted = true;
  }
  else{
    // cout << "MPI_RANK = " << MPI_RANK << ": passing shifted particles." << endl;
    bPtrOut->passShiftedParticles();
  }

  // Determine the boundary that is shifted in.
  // Same as above, but switch ends.
  Boundary* bPtrIn = 0;
  switch(shiftDir){

  case 2:
    bPtrIn=grid->GetBC2()[0][1];
    break;

  case 1:
    bPtrIn=grid->GetBC2()[grid->getJ()][1];
    break;

  case 4:
    bPtrIn=grid->GetBC1()[1][0];
    break;

  case 3:
    bPtrIn=grid->GetBC1()[1][grid->getK()];
    break;

  }

  // Better have a boundary
  if(!bPtrIn){
    static bool inPrinted = false;
    if(!inPrinted) cout << "Error - no inward boundary for shiftDir = " << 
                     shiftDir << endl;
    inPrinted = true;
  }
  else{
    // cout << "MPI_RANK = " << MPI_RANK << ": asking for stripe." << endl;
    bPtrIn->askStripe();

    //
    // Send fields to right, then shift internal
    // cout << "MPI_RANK = " << MPI_RANK << ": sending fields." << endl;
    //
    // We want to shift the data associated with the neutral gas density
    // when we do the Fields send and shift so we will pass a pointer to 
    // the neutral gas density. dad 05/16/01.
    //
 
    if(bPtrOut) fields->sendFieldsAndShift(shiftDir, bPtrOut);

    //
    // Receive any particles from the boundary
    //
    if(bPtrIn->getBCType() == SPATIAL_REGION_BOUNDARY){
      // cout << "MPI_RANK = " << MPI_RANK << ": receiving particles." << endl;
      ParticleList& pl = bPtrIn->recvShiftedParticles();
      // cout << "MPI_RANK = " << MPI_RANK << ": adding particles." << endl;
      addParticleList(pl);	// This empties the particle list
    }
    
    /**
     * We want to follow the approach taken in communicating the 
     * fields when transfering the NGD's. However, we will do this
     * work only if the oopicList with NGD's is not empty. dad-05.21.01.
     */
    if ( !NGDList.isEmpty() ) {
      /**
       * ask first for a stripe of NGDs. 
       */

      bPtrIn->askNGDStripe(); 

      //
      // Send NGDs to right, then shift internal. 
      //
      if(bPtrOut) 
        try{
          ptrMapNGDs->sendNGDsAndShift(shiftDir, bPtrOut);
        }
        catch(Oops& oops2){
          oops2.prepend("SpatialRegion::shiftRegion: Error: \n");//done
          throw oops2;
        }
     }
  }

  //
  // Load any shiftLoads
  //
  // cout << "MPI_RANK = " << MPI_RANK << ": loading shift loads." << endl;
  oopicListIter<Load> nextload(*theLoads);
  for (nextload.restart(); !nextload.Done(); nextload++)
    if(nextload()->name == ostring("shiftLoad")) {
      try{
        nextload()->load_it();
      }
      catch(Oops& oops){
        oops.prepend("SpatialRegion::shiftRegion: Error: \n"); //SpatialRegion::advance
        throw oops;
      }
    }
  //
  // Collect the shifted fields
  //
  // cout << "MPI_RANK = " << MPI_RANK << ": receiving stripe." << endl;
  if(bPtrIn) fields->recvShiftedFields(shiftDir, bPtrIn,t,dt);
  // cout << "MPI_RANK = " << MPI_RANK << ": shift completed." << endl;

  /**
   * Collect the shifted NGDs
   */
  if ( !NGDList.isEmpty() ) {
    if ( bPtrIn )
      ptrMapNGDs->recvShiftedNGDs(shiftDir, bPtrIn);
  }

}
