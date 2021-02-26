//spatialg.cpp

#include "advisman.h"
#include "spatialg.h"
#include "abswag.h"
#include "pmlportg.h"
#include "lnchwag.h"
#include "speciesg.h"
#include "maxwelln.h"
#include "fields.h"
#include "diags.h"
#include "mcc.h"
#include "loadg.h"
#include "varloadg.h"
#include "diagg.h"
#include "sptlrgn.h"
#include "conductg.h"
#include "cylaxisg.h"
#include "femitg.h"
#include "bemitg.h"
#include "obemitg.h"
#include "fowler_nordheim_emit_g.h"
#include "vbemitg.h"
#include "plsmsrcg.h"
#include "laserplasmasrcg.h"
#include "eportg.h"
#include "portteg.h"
#include "porttmg.h"
#include "portgaussg.h"
#include "gapg.h"
#include "eqpotg.h"
#include "foilg.h"
#include "emitprtg.h"
#include "polarg.h"
#include "iloopg.h"
#include "iregiong.h"
#include "varg.h"
#include "spboundg.h"
#include "spemitg.h"
#include "periodic.h"
#include "ptclgrp.h"
#include "porttemmwg.h"
#include "loadpwg.h"
#include "cursrcg.h"

#if defined(_MSC_VER)
extern "C" int WasDumpFileGiven;
#else
extern int WasDumpFileGiven;
#endif


void Restore(SpatialRegion*); 

extern "C++" void printf(char *);

#ifdef MPI_VERSION
#include "mpi.h"
extern MPI_Comm XOOPIC_COMM;
extern int MPI_RANK;
#define SRB_ANNOUNCE_TAG 100
#define SRB_LINK_TAG 200
typedef struct {
   char *name;
   int index;
   int linkedP;
} SRBdat;
#endif

using namespace std;

//=================== SpatialRegionGroup Class

ostring SpatialRegionGroup::CreateParamGroup(ifstream &fin, ostring &groupname)
  /* throw(Oops) */{
  ostring message;
 if (groupname ==(ostring) "Grid")
    message = GP.InitializeFromStream(fin);
 else if (groupname == (ostring)"Variables")
    message = VG.InitializeFromStream(fin);
 else if (groupname == (ostring)"Control")
    message = CP.InitializeFromStream(fin);
 else if (groupname == (ostring)"ParticleGroup")
    message = PGP.InitializeFromStream(fin);
 else if (groupname == (ostring)"Species")
    message = readSpecies(fin);
 else if (groupname == (ostring)"MCC")
    message = AddMCCParamsList(fin);
 else if ( groupname == (ostring)"Load" ) 
    message = AddLoadParamsList(fin); 
 else if ( groupname == (ostring)"Load_PW")
    message = AddLoadPWParamsList(fin);
 else if ( groupname == (ostring)"VarWeightLoad" ) 
    message = AddVarWeightLoadParamsList(fin); 
 else if (groupname == (ostring)"Diagnostic") 
    message = AddDiagParamsList(fin); 
 else if (advisorp->KnownBoundary(groupname)){
   try{
     message = CreateBoundaryParams(fin, groupname);
   }
   catch(Oops& oops3){
     oops3.prepend("SpatialRegionGroup::CreateParamGroup: Error: \n");
     throw oops3;
   }
 }
 else if (groupname == (ostring)"H5Diagnostic") 
   message = AddH5DiagDumpParamsList(fin);
 else message = "Unknown name ecountered for parameterGroup in input deck";
 return(message);
}

ostring SpatialRegionGroup::CreateBoundaryParams(ifstream &fin, ostring groupname)
/* throw(Oops) */{
   BoundaryParams* bptr = 0;

   try{
     bptr = constructBoundaryParams(groupname);
   }
   catch(Oops& oops2){
     oops2.prepend("SpatialRegionGroup::CreateBoundaryParams: Error: \n");//done
     throw oops2;
   }

   ostring message("WARNING:  Invalid boundary type was requested!");
   if (bptr) {
     message = bptr->InitializeFromStream(fin);                  
     Boundaries.add(bptr);
   }
   return(message);
}

ostring SpatialRegionGroup::AddMCCParamsList(ifstream& fin)
{
   MCCParams* mPtr = new MCCParams(this);
   ostring message = mPtr->InitializeFromStream(fin);
   MCCParamsList.add(mPtr);
   return message;
}

ostring SpatialRegionGroup::AddLoadParamsList(ifstream &fin)
{
   LoadParams* lptr = new LoadParams(this);
   ostring message = lptr->InitializeFromStream(fin);
   Loads.add(lptr);
   return(message);
}

ostring SpatialRegionGroup::AddLoadPWParamsList(ifstream &fin)
{
   LoadPWParams* lptr = new LoadPWParams(this);
   ostring message = lptr->InitializeFromStream(fin);
   Loads.add(lptr);
   return(message);
}

ostring SpatialRegionGroup::AddVarWeightLoadParamsList(ifstream &fin)
{
   VarWeightLoadParams* lptr = new VarWeightLoadParams(this);
   ostring message = lptr->InitializeFromStream(fin);
   Loads.add(lptr);
   return(message);
}

ostring SpatialRegionGroup::AddDiagParamsList(ifstream &fin)
{
   DiagParams* dptr = new DiagParams(&(this->GP),this); 
   ostring message = dptr->InitializeFromStream(fin);
   Diags.add(dptr);
   return(message);
}

ostring SpatialRegionGroup::AddH5DiagDumpParamsList(ifstream &fin)
 {
    H5DiagParams* dptr = new H5DiagParams(); 
    ostring message = dptr->InitializeFromStream(fin);
    H5DiagDumpParams.add(dptr);
    return(message);
 }





void SpatialRegionGroup::describe()
{
   cout << "testing echoFlag..." << endl;
   if (CP.get_echoFlag()==0) {cout << "echoFlag=0" << endl; return;}
   GP.describe();
   CP.describe();
   PGP.describe();
   {
      oopicListIter<LoadParams> nL(Loads);
      for (nL.restart(); !nL.Done(); nL++)
         nL.current()->describe();
   }
   {
      oopicListIter<DiagParams> nD(Diags);
      for (nD.restart(); !nD.Done(); nD++)
         nD.current()->describe();
   }
   {
      oopicListIter<BoundaryParams> nB(Boundaries);
      for (nB.restart(); !nB.Done(); nB++)
         nB.current()->describe();
   }
}

void SpatialRegionGroup::writeOutputFile(ofstream &fout)
{GP.writeOutputFile(fout);
 CP.writeOutputFile(fout);
 oopicListIter<BoundaryParams> nB(Boundaries);
 for (nB.restart(); !nB.Done(); nB++)
    {nB.current()->writeOutputFile(fout);}
}

void SpatialRegionGroup::checkRules()
{
}

SpatialRegion* SpatialRegionGroup::CreateCounterPart()
  /* throw(Oops) */{
  if (CP.getFrandseed())
    frandseed = CP.getFrandseed();
  GP.checkRules();
  GP.showRuleMessages();
  CP.checkRules();
  CP.showRuleMessages();
  Grid *G;
#ifdef MPI_VERSION
  int MPI_MAX_RANK;
  MPI_Comm_size(XOOPIC_COMM,&MPI_MAX_RANK);
  if (MPI_MAX_RANK > 1) {  //  if we have more than one region.
#ifdef MPI_DEBUG
    printf("\n%d: Creating grid with %d partitions.",MPI_RANK,MPI_MAX_RANK);
#endif
    G = GP.CreateCounterPart(CP.getMPIpartition());
  }
  else
#endif /* MPI_VERSION */
    G = GP.CreateCounterPart();
  
  oopicList<Boundary> *BList = new BoundaryList;
  
  //put the periodic boundary on the boundary list MUST BE FIRST HERE SO THAT IT IS LAST IN PHYSICS
  if ((GP.getPeriodicFlagX1())||(GP.getPeriodicFlagX2())){
    oopicList<LineSegment> *LineSegmentList = new oopicList<LineSegment>;
    LineSegment *newSeg = new LineSegment(Vector2(0,0),Vector2(GP.getJ(),GP.getK()),1);
    LineSegmentList->add(newSeg);
    Boundary* B = 
      new Periodic(LineSegmentList, GP.getPeriodicFlagX1(), GP.getPeriodicFlagX2());
    BList->add(B);
  }
  
  oopicListIter<BoundaryParams> nB(Boundaries);
  for (nB.restart(); !nB.Done(); nB++)
    {
      BoundaryParams *bParam=nB.current();
      try{
        bParam->toGrid(G);  //cause the boundary to 'snap to' j,k's if given MKS
      // moved toGrid to checkRules because some of the rules are for j1,j2,k1,k2.
      // moved back to CreateCounter Part and removed checkRules
      }
      catch(Oops& oops){
        oops.prepend("SpatialRegionGroup::CreateCounterPart: Error:\n"); //AdvisorManager::createDevice
        throw oops;
      }

#ifdef MPI_VERSION
      if(! (bParam->j1.getValue()==-1 && bParam->A1.getValue()==-1)) {
        // we don't want to create boundaries which are out-of-bounds!
#endif /* MPI_VERSION */
        bParam->checkRules();
        bParam->showRuleMessages();
        Boundary* bPtr = 0;
        try{
          bPtr = bParam->CreateCounterPart();
        }
        catch(Oops& oops2){
          oops2.prepend("SpatialRegionGroup::CreateCounterPart: Error: \n");//done
          throw oops2;
        }
        bParam->setBoundaryParams(bPtr);  //should probably cascade CreateCounterPart()
        BList->add(bPtr);
#ifdef MPI_VERSION
      }
#endif /*MPI_VERSION */
    }
  
  // Move this call here.  It saves trouble with deciding where to put any SRB's.
  G->setBoundaryMask(*BList);
#ifdef MPI_VERSION
  /*  At this point, we should add all the necessary SRB's to our region.  */
  try{
    addNeededSRBstoRegion(BList,G);
  }
  catch(Oops& oops3){
    oops3.prepend("SpatialRegionGroup::CreateCounterPart: Error: \n");
    throw oops3;
  }

#endif /*MPI_VERSION*/

  SpeciesParamsList* speciesParamsList = advisorp->get_speciesParamsList();
  int tmpDim = speciesParamsList->nItems();
  ParticleGroupList** pgListArray = new ParticleGroupList*[tmpDim];
  for (int i=0; i<tmpDim; i++)
    pgListArray[i] = new ParticleGroupList;
  
  SpeciesList* speciesList = advisorp->get_speciesList();
  
  Scalar epsilonR = 1.0;
  Fields *F = new Fields(G, BList, speciesList, epsilonR, 
                         CP.getElectrostaticFlag(),CP.get_presidue(),
                         CP.getBoltzmannFlag(),CP.getSynchRadiationFlag());
  
  F->set_bSpecies(get_speciesPtr(CP.getBoltzSpecies()));
  if (!F->setBNodeStatic(Vector3(CP.getB01(),CP.getB02(),CP.getB03()),
                         CP.getbetwig(), CP.getzoff(),CP.getBf().c_str(),
                         CP.getB01analytic(),CP.getB02analytic(),
			 CP.getB03analytic()))
    abort();;
  
  
  F->setBeamDump(CP.get_j1BeamDump(), CP.get_j2BeamDump());
  
  F->setMarderIter(CP.getMarderIter());
  F->setDivergenceCleanFlag(CP.getDivergenceCleanFlag());
  F->setCurrentWeightingFlag(CP.getCurrentWeighting());
  F->set_freezeFields(CP.get_FreezeFields());
  F->setNonRelativisticFlag(CP.getNonRelativisticFlag());
  F->set_nSmoothing(CP.get_nSmoothing());
  F->setInfiniteBFlag(CP.getInfiniteBFlag());
  F->setFieldSubFlag(CP.getFieldSubFlag());
  F->setMarderParameter(CP.getMarderParameter());
  F->setEMdamping(CP.getEMdamping());
  
  if ( (CP.getElectrostaticFlag()) && (CP.getBoltzmannFlag()) ) {
    Species* species = get_speciesPtr(CP.getBoltzSpecies());
    Boltzmann* boltzmann= new Boltzmann(F,CP.getBoltzmannTemp(), species, CP.getBoltzmannEBFlag());
    F->setBoltzmann(boltzmann);
  }
  MCC*   mcc = 0;
  MCTI* mcti = 0;
  SpatialRegion *region = new SpatialRegion(F,BList, pgListArray, speciesList, 
                                            mcc, mcti, get_dt());
  
  if (MCCParamsList.nItems() > 0) {
    mcc  = new MCC;
    mcti = new MCTI; 
    oopicListIter<MCCParams> mIter(MCCParamsList);
    for (mIter.restart(); !mIter.Done(); mIter++) {
      mIter()->checkRules();
      mIter()->showRuleMessages();
      try{
        mIter()->CreateCounterPart(region, mcc, mcti);
      }
      catch(Oops& oops2){
        oops2.prepend("SpatialRegionGroup::CreateCounterPart: Error: \n");//OK
        throw oops2;
      }

      /**
       * note the difference in the call to 
       * CreateCoungerPart(...) in the line of code above
       * from its earlier use:
       * mcc->addPackage(mIter()->CreateCounterPart(region));
       * which was the case when tunneling ionization was not 
       * coded. For the tunneling ionization we have a class
       * MCTI analogous to the MCC class and the pointers to
       * these two classes are now passed to the 
       * CreateCounterPart for proper initialization of the 
       * list objects these clases contain.
       * dad: 12/20/2000.
       */
    }
    region->setMCC(mcc);
    region->setMCTI(mcti);
    region->setNGDListPtrs();
#ifdef MPI_VERSION
    //
    // allocate memory for the buffers in the SpatialRegionBoundary
    // objects for the NGD data
    // 
    oopicListIter<Boundary> nextb(*BList);
    for(nextb.restart(); !nextb.Done(); nextb++)
      nextb.current()->allocMemNGDbuffers();
#endif // MPI_VERSION
  }
  
  region->setParticleLimit( CP.get_particleLimit() );
  region->set_np2cFactor( CP.get_np2cFactor() );
  region->setduplicateParticles( (int)(CP.getduplicateParticles()+0.1) );

  region->set_histmax(CP.get_histmax());
  
#ifdef MPI_VERSION
  char buf[512];
  if(Name.getValue()==ostring("Noname") )
    sprintf(buf,"Region %d",MPI_RANK);
  else
    sprintf(buf,"%s Region %d",Name.getValue().c_str(),MPI_RANK);
  region->setName(buf);
#else  /* MPI_VERSION */
  region->setName(Name.getValue());
#endif  /* MPI_VERSION */
  oopicList<Diag> *DList = new DiagList;
  oopicListIter<DiagParams> nD(Diags);
  for (nD.restart(); !nD.Done(); nD++){
    try{
      nD.current()->toGrid(G);
    }
    catch(Oops& oops){
      oops.prepend("SpatialRegionGroup::CreateCounterPart: Error:\n"); //OK
      throw oops;
    }

#ifdef MPI_VERSION
    if(! (nD.current()->j1.getValue()==-1 && nD.current()->A1.getValue()==-1)) {
      // we don't want to create boundaries which are out-of-bounds!
#endif /* MPI_VERSION */
      Diag* dPtr = 0;
      try{
        dPtr = nD.current()->CreateCounterPart(region);
      }
      catch(Oops& oops2){
        oops2.prepend("SpatialRegionGroup::CreateCounterPart: Error: \n");//OK
        throw oops2;
      }
      DList->add(dPtr);
#ifdef MPI_VERSION
    }
#endif
    
  }
  region->setDiagList(DList);

// create a pointer to list of H5Diagnostic parameters

  oopicList<H5DiagParams> *H5DList = new oopicList<H5DiagParams>;
  oopicListIter<H5DiagParams> nDD(H5DiagDumpParams);
  for (nDD.restart(); !nDD.Done(); nDD++){
     H5DList->add(nDD.current());
  }
  region->setH5DiagDumpList(H5DList);
    
  if (WasDumpFileGiven)	{
    try{
      Restore(region);
    }
    catch(Oops& oops2){
      oops2.prepend("SpatialRegionGroup::CreateCounterPart: Error: \n");//OK
      throw oops2;
    }
  }
  
  //	else 
  try {
    ApplyToList(CreateCounterPart(region), Loads, LoadParams);
  }
  catch(Oops& oops2){
      oops2.prepend("SpatialRegion::CreateCounterPart: Error: \n");
      throw oops2;
  }
  
  F->compute_rho(region->getParticleGroupListArray(),
                 region->get_nSpecies());
  
#ifdef MPI_VERSION
  linkSRB();
#endif
  try{
    ApplyToList(putCharge_and_Current(region->getTime()),*BList,Boundary);
  }
  catch(Oops& oops2){
      oops2.prepend("SpatialRegion::CreateCounterPart: Error: \n");//done
      throw oops2;
  }
  
  // Need to receive a message sent by putCharge_and_Current
  // in MPI, to set up toNodes applyFields with dt=0 just receives
  // sent fields and sets up the SRB for toNodes
#ifdef MPI_VERSION
  try{
    ApplyToList(applyFields(region->getTime(),0),SpatialRegionBoundaryList, SpatialRegionBoundary);
  }
  catch(Oops& oops3){
    oops3.prepend("SpatialRegionGroup::CreateCounterPart: Error: \n");
    throw oops3;
  }

#endif

   // Some boundaries need feedback info from SpatialRegion
   //
   //
   ApplyToList(setRegion(region),*BList,Boundary);
   
   /* this line need to follow compute_rho so that the boundary 
      get rho correct */

   if ((CP.getElectrostaticFlag() || !WasDumpFileGiven) && CP.getInitPoissonSolveFlag()){

#ifndef MPI_VERSION  /* ES field not working in MPI version yet! */
    F->ElectrostaticSolve(region->getTime(),region->get_dt());
#endif /* MPI_VERSION */
    
  }

#ifdef MPI_VERSION /* Since the ES field solve is not working, at least apply BC */
  if( CP.getInitPoissonSolveFlag()) {
    try{
      ApplyToList(applyFields(region->getTime(),region->get_dt()),*BList,Boundary);
    }
    catch(Oops& oops3){
      oops3.prepend("SpatialRegionGroup::CreateCounterPart: Error: \n");
      throw oops3;
    }
  }
#endif
  
  F->setdt(region->get_dt());
  
  //  the next two lines set up initial EM fields
  //  which are added to any previous fields.
  //  since it is additive, we should do it only once.
  if(!WasDumpFileGiven) {
    F->setEinit(CP.getE1init(),CP.getE2init(),CP.getE3init());
    F->setBinit(CP.getB1init(),CP.getB2init(),CP.getB3init());
  }
  
  F->toNodes(region->getTime());
  
  
  // set up the moving window.
  switch(CP.getMovingWindow()){
  case 1:
    region->setShift(Vector2(-1,0));
    break;
  case 2:
    region->setShift(Vector2(1,0));
    break;
  case 3:
    region->setShift(Vector2(-1,0));
    break;
  case 4:
    region->setShift(Vector2(1,0));
    break;
  }

  region->setShiftDelayTime(CP.getShiftDelayTime());
  region->setGasOffTime(CP.getGasOffTime());

  
  return region;
}


//-------------------------------------------------------------------
//	temp mechanism to show rule messages - should work on the entire
//	SRG?  Should SRG inherit from ParameterGroup?

void	SpatialRegionGroup::showRuleMessages()
{
   if (GP.ruleMessages.nItems()) GP.showRuleMessages();
   if (CP.ruleMessages.nItems()) CP.showRuleMessages();
   oopicListIter<BoundaryParams> nB(Boundaries);
   for (nB.restart(); !nB.Done(); nB++)
      {
         BoundaryParams *bp = nB.current();
         if (bp->ruleMessages.nItems()) bp->showRuleMessages();
      }
}                               

// the following are used in GeometryEditor::setSpatialRegion
//
GridParams* SpatialRegionGroup::getGridParams()
{
   return &GP;
}

ControlParams* SpatialRegionGroup::getControlParams()
{
   return &CP;
}

oopicList<BoundaryParams>* SpatialRegionGroup::getBoundaryParamsList()
{
   return &Boundaries;
}

oopicList<DiagnosticParams>* SpatialRegionGroup::getProbeParamsList()
{
   return &Probes;
}

// the following are used in GeometryEditor createDragObject
BoundaryParams* SpatialRegionGroup::constructBoundaryParams(ostring groupname)
  /* throw(Oops) */{
   BoundaryParams* bptr = NULL;

   if (groupname == (ostring)"Conductor")
      bptr = new ConductorParams(&GP,this);

   if (groupname == (ostring)"Polarizer")
      bptr = new PolarParams(&GP,this);

   if (groupname == (ostring)"SpatialRegionBoundary"){
     try{    
       bptr = new SpatialRegionBoundaryParams(&GP,this);
     }
     catch(Oops& oops){
       oops.prepend("SpatialRegionGroup::constructBoundaryParams: Error:\n"); //SpatialRegionGroup::CreateBoundaryParams
       throw oops;
     }
   }

   if (groupname == (ostring)"CylindricalAxis")
      bptr = new CylindricalAxisParams(&GP,this);

   if (groupname == (ostring)"FieldEmitter")
      bptr = new FieldEmitterParams(&GP,this);

   if (groupname == (ostring)"FieldEmitter2")
      bptr = new FieldEmitterParams2(&GP,this);

   if (groupname == (ostring)"BeamEmitter")
      bptr = new BeamEmitterParams(&GP,this);

   if (groupname == (ostring)"OpenBoundaryBeamEmitter")
      bptr = new OpenBoundaryBeamEmitterParams(&GP,this);

   if (groupname == (ostring)"FowlerNordheimEmitter")
      bptr = new FowlerNordheimEmitterParams(&GP,this);

   if (groupname == (ostring)"SingleParticleEmitter")
      bptr = new SingleParticleEmitterParams(&GP,this);

   if (groupname == (ostring)"PlasmaSource")
      bptr = new PlasmaSourceParams(&GP,this);

   if (groupname == (ostring)"LaserPlasmaSource")
      bptr = new LaserPlasmaSourceParams(&GP,this);

   if (groupname == (ostring)"EmitPort")
      bptr = new EmitPortParams(&GP,this);
   
   if (groupname == (ostring)"VarWeightBeamEmitter")
      bptr = new VarWeightBeamEmitterParams(&GP,this);

   if (groupname == (ostring)"ExitPort")
      bptr = new ExitPortParams(&CP, &GP,this);

   if (groupname == (ostring)"Iloop")
      bptr = new IloopParams(&GP, this);

   if (groupname == (ostring)"CurrentRegion")
      bptr = new CurrentRegionParams(&GP,this);

   if (groupname == (ostring)"PortTE")
      bptr = new PortTEParams(&GP,this);

   if (groupname == (ostring)"PortTM")
      bptr = new PortTMParams(&GP,this);

   if (groupname == (ostring)"PortTEM_MW")
      bptr = new PortTEM_MW_Params(&GP,this);

   if (groupname == (ostring)"PortGauss")
      bptr = new PortGaussParams(&GP,this);

   if (groupname == (ostring)"AbsorbWave")
      bptr = new AbsorbWaveParams(&GP,this);

   if (groupname == (ostring)"PMLPort")
     bptr = new PMLportParams(&GP,this);

   if (groupname == (ostring)"LaunchWave")
      bptr = new LaunchWaveParams(&GP,this);

   if (groupname == (ostring)"Gap")
      bptr =	new GapParams(&GP,this);

   if (groupname == (ostring)"Equipotential")
      bptr = new EquipotentialParams(&GP,this);
   
   if (groupname == (ostring)"Dielectric")
      bptr = new DielectricParams(&GP,this);

   if (groupname == (ostring)"DielectricRegion")
      bptr = new DielectricRegionParams(&GP,this);

   if (groupname == (ostring)"DielectricTriangle")
      bptr = new DielectricTriangleParams(&GP,this);

   if (groupname == (ostring)"Foil")
      bptr = new FoilParams(&GP,this);

   if (groupname == (ostring)"CurrentSource")
     bptr = new CurrentSourceParams(&GP,this);

   if (bptr) {
     bptr->setName(groupname);
   }
   else {
     cerr << "\n\nWARNING:  Invalid boundary type!" << endl;
     cerr << "  " << groupname << " does not exist..." << endl;
     cerr << "  You must modify your input file." << endl;
   }
   return bptr;
}

DiagnosticParams* SpatialRegionGroup::constructProbeParams(ostring groupname)
{
   DiagnosticParams* dptr=0;
#ifndef TEXT_OOPIC
   /*
      #ifndef UNIX
      if (groupname == (ostring)"E Field")
      dptr = new EFieldProbe(&GP);
      if (groupname == (ostring)"B Field")
      dptr = new BFieldProbe(&GP);
      if (groupname == (ostring)"Current")
      dptr = new CurrentProbe(&GP);
      if (groupname == (ostring)"DivDerror")
      dptr = new DivDerrorProbe(&GP);
      dptr->setName(groupname);
      #endif
      */
#endif
   return dptr;
}

ostring 	SpatialRegionGroup::readSpecies(ifstream& fin) 
{
   return advisorp->readSpecies(fin);
}

BOOL SpatialRegionGroup::KnownSpecies(ostring &speciesName)
{
   return advisorp->KnownSpecies(speciesName);
}

Species* SpatialRegionGroup::get_speciesPtr(const ostring &speciesName)
{
   return advisorp->get_speciesPtr(speciesName);
}

SpeciesList* SpatialRegionGroup::get_speciesList()
{
   return advisorp->get_speciesList();
}

SpatialRegionGroup::~SpatialRegionGroup() {
   Boundaries.deleteAll();
   //	Loads.deleteAll();   for moving window, want to keep these around.
   Diags.deleteAll();
   Probes.deleteAll();
   MCCParamsList.deleteAll();
};

#ifdef MPI_VERSION

void SpatialRegionGroup::addNeededSRBstoRegion(oopicList<Boundary> *BList,Grid *G) 
/* throw(Oops) */{
   // Grid has a mask in it so
   // I can figure out where the SRB's need to go!
   int J = G->getJ();
   int K = G->getK();
   int j,k;
   int bcount;  // count of SRB along an edge, needed for naming
   int Js,Ks;   // beginning of the current SRB
   int makingSRB=0;  // flag on whether we're making an SRB or not.
   //  0->1 transitions set Js, Ks,
   //  1->0 transitions create the boundary.
   int Nindex;  // index to neighbor array
   int normal;  // normal
   Boundary ***bc1=G->GetBC1();
   Boundary ***bc2=G->GetBC2();
   Boundary *newSRB;

   // if we have a comp. partition below us, try to make SRB
   if(G->neighbor[Nindex=3]!= -1) {  
      k = 0;
      makingSRB=0;
      bcount = 0;
      normal = 1;
      for(j=0;j<=J;j++) {
         int BTYPE = FREESPACE;
         if(bc1[j][k]) BTYPE=bc1[j][k]->getBCType();
         if(BTYPE == FREESPACE || BTYPE == DIELECTRIC_BOUNDARY ) {
            // SRB needed here.
            if(makingSRB==0) {  //detected an up-transition
               Js = j; Ks = k; bcount++;
               makingSRB=1;
            }
         }
         else if(BTYPE == CONDUCTING_BOUNDARY && makingSRB!=0) {
            // We've detected the end of a valid SRB:
            // make it and add it to THE END of the BList (necessary for toNodes to work right)
           try{
             BList->addToEnd(makeSRB(G,Js,Ks,j,k,normal,G->neighbor,Nindex,bcount));
           }
           catch(Oops& oops2){
             oops2.prepend("SpatialRegionGroup::addNeededSRBstoRegion: Error: \n");//done
             throw oops2;
           }

           Js=0; Ks=0; makingSRB=0;
         }
         else {
            if(makingSRB) {
               // this is an error codition.
               printf("\n%d:ERROR: SpatialRegionBoundary attempted placement on",MPI_RANK);
               printf("\n%d:ERROR: another SpatialRegionBoundary, a CYLINDRICAL Axis, or",MPI_RANK);
               printf("\n%d:ERROR: a periodic boundary.  NOT ALLOWED!  Exiting.",MPI_RANK);
               printf("\n%d:ERROR: (happened at j=%d, k=%d)  Exiting.",MPI_RANK,j,k);
               abort();
            }
         }
      }
   }
   // need to back the index up one...
   j--;
   // we've fallen through the loop entirely
   if(makingSRB) {
     try{
       BList->addToEnd(makeSRB(G,Js,Ks,j,k,normal,G->neighbor,Nindex,bcount));
     }
     catch(Oops& oops2){
       oops2.prepend("SpatialRegionGroup::addNeededSRBstoRegion: Error: \n");//OK
       throw oops2;
     }
     makingSRB=0;
   }
   // if we have a comp. partition above us, try to make SRB.
   if(G->neighbor[Nindex=2]!= -1) {  
      k = K;
      makingSRB=0;
      bcount = 0;
      normal = -1;
      for(j=0;j<=J;j++) {
         int BTYPE = FREESPACE;
         if(bc1[j][k]) BTYPE=bc1[j][k]->getBCType();
         if(BTYPE == FREESPACE || BTYPE == DIELECTRIC_BOUNDARY ) {
            // SRB needed here.
            if(makingSRB==0) {  //detected an up-transition
               Js = j; Ks = k; bcount++;
               makingSRB=1;
            }
         }
         else if(BTYPE == CONDUCTING_BOUNDARY && makingSRB!=0) {
            // We've detected the end of a valid SRB:
            // make it and add it to the BList
           try{
             BList->addToEnd(makeSRB(G,Js,Ks,j,k,normal,G->neighbor,Nindex,bcount));
           }
           catch(Oops& oops2){
             oops2.prepend("SpatialRegionGroup::addNeededSRBstoRegion: Error: \n");//OK
             throw oops2;
           }
           Js=0; Ks=0; makingSRB=0;
         }
         else {
            if(makingSRB) {
               // this is an error condition
               printf("\n%d:ERROR: SpatialRegionBoundary attempted placement on",MPI_RANK);
               printf("\n%d:ERROR: another SpatialRegionBoundary, a CYLINDRICAL Axis, or",MPI_RANK);
               printf("\n%d:ERROR: a periodic boundary.  NOT ALLOWED!  Exiting.",MPI_RANK);
               printf("\n%d:ERROR: (happened at j=%d, k=%d)  Exiting.",MPI_RANK,j,k);
               abort();
            }
         }
      }
   }

   // need to back the index up one...
   j--;
   // we've fallen through the loop entirely
   if(makingSRB) {
     makingSRB=0;
     try{
       BList->addToEnd(makeSRB(G,Js,Ks,j,k,normal,G->neighbor,Nindex,bcount));
     }
     catch(Oops& oops2){
       oops2.prepend("SpatialRegionGroup::addNeededSRBstoRegion: Error: \n");//OK
       throw oops2;
     }
   }
   // if we have a comp. partition left of us, try to make SRB.
   if(G->neighbor[Nindex=1]!= -1) {  
      j = 0;
      makingSRB=0;
      bcount = 0;
      normal = 1;
      for(k=0;k<=K;k++) {
         int BTYPE = FREESPACE;
         if(bc2[j][k]) BTYPE=bc2[j][k]->getBCType();
         if(BTYPE == FREESPACE || BTYPE == DIELECTRIC_BOUNDARY ) {
            // SRB needed here.
            if(makingSRB==0) {  //detected an up-transition
               Js = j; Ks = k; bcount++;
               makingSRB=1;
            }
         }
         else if(BTYPE == CONDUCTING_BOUNDARY && makingSRB!=0) {
            // We've detected the end of a valid SRB:
            // make it and add it to the BList
           try{
             BList->addToEnd(makeSRB(G,Js,Ks,j,k,normal,G->neighbor,Nindex,bcount));
           }
           catch(Oops& oops2){
             oops2.prepend("SpatialRegionGroup::addNeededSRBstoRegion: Error: \n");//OK
             throw oops2;
           }
           Js=0; Ks=0; makingSRB=0;
         }
         else {
            if(makingSRB) {
               // this is an error condition and shouldn't happen.
               printf("\n%d:ERROR: SpatialRegionBoundary attempted placement on",MPI_RANK);
               printf("\n%d:ERROR: another SpatialRegionBoundary, a CYLINDRICAL Axis, or",MPI_RANK);
               printf("\n%d:ERROR: a periodic boundary.  NOT ALLOWED!  Exiting.",MPI_RANK);
               printf("\n%d:ERROR: (happened at j=%d, k=%d)  Exiting.",MPI_RANK,j,k);
               abort();
            }
         }
      }
   }

   // need to back the index up one...
   k--;
   // we've fallen through the loop entirely
   if(makingSRB) {
     makingSRB=0;
     try{
       BList->addToEnd(makeSRB(G,Js,Ks,j,k,normal,G->neighbor,Nindex,bcount));
     }
     catch(Oops& oops2){
       oops2.prepend("SpatialRegionGroup::addNeededSRBstoRegion: Error: \n");//OK
       throw oops2;
     }
   }
   // if we have a comp. partition right of us, try to make SRB.
   if(G->neighbor[Nindex=0]!= -1) {  
      j = J;
      makingSRB=0;
      bcount = 0;
      normal = -1;
      for(k=0;k<=K;k++) {
         int BTYPE = FREESPACE;
         if(bc2[j][k]) BTYPE=bc2[j][k]->getBCType();
         if(BTYPE == FREESPACE || BTYPE == DIELECTRIC_BOUNDARY ) {
            // SRB needed here.
            if(makingSRB==0) {  //detected an up-transition
               Js = j; Ks = k; bcount++;
               makingSRB=1;
            }
         }
         else if(BTYPE == CONDUCTING_BOUNDARY && makingSRB!=0) {
            // We've detected the end of a valid SRB:
            // make it and add it to the BList
           try{
             BList->addToEnd(makeSRB(G,Js,Ks,j,k,normal,G->neighbor,Nindex,bcount));
           }
           catch(Oops& oops2){
             oops2.prepend("SpatialRegionGroup::addNeededSRBstoRegion: Error: \n");//OK
             throw oops2;
           }

           Js=0; Ks=0; makingSRB=0;
         }
         else {
            if(makingSRB) {
               printf("\n%d:ERROR: SpatialRegionBoundary attempted placement on",MPI_RANK);
               printf("\n%d:ERROR: another SpatialRegionBoundary, a CYLINDRICAL Axis, or",MPI_RANK);
               printf("\n%d:ERROR: a periodic boundary.  NOT ALLOWED!  Exiting.",MPI_RANK);
               printf("\n%d:ERROR: (happened at j=%d, k=%d)  Exiting.",MPI_RANK,j,k);
               abort();
            }
         }
      }
   }
   // need to back the index up one...
   k--;


   // we've fallen through the loop entirely
   if(makingSRB) {
     makingSRB=0;
     try{
       BList->addToEnd(makeSRB(G,Js,Ks,j,k,normal,G->neighbor,Nindex,bcount));
     }
     catch(Oops& oops2){
       oops2.prepend("SpatialRegionGroup::addNeededSRBstoRegion: Error: \n");//OK
       throw oops2;
     }

   }
   

}

Boundary *SpatialRegionGroup::makeSRB(Grid *G,int js,int ks,int je,int ke,int normal,
                                      int *neighbor, int Nindex,int bcount) 
                                        /* throw(Oops) */{
   oopicList <LineSegment> *segments = new oopicList<LineSegment>;
   char PartnerName[512];
   SpatialRegionBoundary *newSRB;
   LineSegment *newSeg= new LineSegment(Vector2(js,ks),Vector2(je,ke),normal);
   segments->add(newSeg);

   try{
     newSRB= new SpatialRegionBoundary(segments);
   }
   catch(Oops& oops){
     oops.prepend("SpatialRegionGroup::makeSRB: Error:\n"); //SpatialRegionGroup::addNeededSRBstoRegion
     throw oops;
   }
   // set up the partner name so boundary matching can happen later.
   // it is in the format "RXXXYYY[V|H]NN"
   //  XXX = higher-rank, YYY = lower-rank, V means vertical, H means horizontal
   //  NN is the section number.  
   sprintf(PartnerName,"R%03d%03d%c%02d",
           MAX(MPI_RANK,neighbor[Nindex]),
           MIN(MPI_RANK,neighbor[Nindex]),
           ((js==je)?'V':'H'),bcount);
   newSRB->PartnerName = PartnerName;
   newSRB->setBoundaryName( PartnerName);
   addtoSpatialRegionBoundaryList(newSRB);
   newSRB->setBoundaryMask(*G);
#ifdef MPI_DEBUG
   printf("\n%d: Creating new SRB: js %d ks %d je %d ke %d normal %d, %s",
          MPI_RANK,js,ks,je,ke,normal,PartnerName);
#endif
   return newSRB;
}


void SpatialRegionGroup::linkSRB() {

   //  It is necessary here to link pairs of SpatialRegionBoundaries

   //  First we check for regions on the same CPU
   //  Now we look for boundaries on other processes/CPU's
   oopicListIter <SpatialRegionBoundary> SRBIter(getSpatialRegionBoundaryList());
   for(SRBIter.restart();!SRBIter.Done();SRBIter++) {
      if(SRBIter.current()->isLinked()) continue;  // if this boundary is linked already, skip it.
      char buf[256];  //  the send buffer
      strncpy(buf,SRBIter.current()->PartnerName.c_str(),255);
      // send our ID to the root process
      MPI_Send(buf,255,MPI_CHAR,0,SRB_ANNOUNCE_TAG,XOOPIC_COMM);
   }



   //  We need to block execution until everyone has sent ID's for
   //  all their boundaries to everyone else.
   MPI_Barrier(XOOPIC_COMM);

   // Now the root process has to receive all the ID's and then distribute
   // them to everyone.
   int tag_modifier=0;
   if(MPI_RANK==0 ) {
      oopicList <SRBdat> SRBdatList;
      int incoming_messagesP;
      MPI_Status message_status;
      MPI_Iprobe(MPI_ANY_SOURCE,SRB_ANNOUNCE_TAG,XOOPIC_COMM,&incoming_messagesP,&message_status);

      //  load all the incoming SRB information into SRBdatList
      while(incoming_messagesP) {
	 SRBdat *SRBtemp = new SRBdat;
	 SRBtemp->name = new char[256];
	 MPI_Recv(SRBtemp->name,255,MPI_CHAR,MPI_ANY_SOURCE,SRB_ANNOUNCE_TAG,XOOPIC_COMM,&message_status);
	 SRBtemp->index = message_status.MPI_SOURCE;
	 SRBtemp->linkedP = 0;
	 SRBdatList.add(SRBtemp);
	 MPI_Iprobe(MPI_ANY_SOURCE,SRB_ANNOUNCE_TAG,XOOPIC_COMM,&incoming_messagesP,&message_status);
      }

      
      // Now go through the list we've created, linking any pairs we find.
      oopicListIter<SRBdat> SRBdatIter(SRBdatList);
      for(SRBdatIter.restart();!SRBdatIter.Done();SRBdatIter++)
	 {
	    SRBdat *SRBtmp = SRBdatIter.current();
	    // check and see if this boundary is linked already
	    if(!SRBtmp->linkedP) {  // search through the rest of the list for a match
	       oopicListIter<SRBdat> SRBdatIter2(SRBdatList);
	       for(SRBdatIter2.restart();!SRBdatIter2.Done();SRBdatIter2++)
		  {
		     SRBdat *SRBtmp2 = SRBdatIter2.current();
		     // if it's already linked, or if it is the same, it cannot be a match
		     // also check match on name
		     if(! (SRBtmp2->linkedP || SRBtmp2 == SRBtmp) && !strcmp(SRBtmp->name,SRBtmp2->name) ) {
			
			tag_modifier++;
			// message both machines, telling them their counterparts.
			// message has the format: (concatenated null-terminated strings)
			// name\0index" , "tag_modifier\0
			char buf[256];
			//  tell SRBtmp2 about SRBtmp
			sprintf(buf,"%s",SRBtmp->name);
			sprintf(buf+strlen(SRBtmp->name) +1, "%d , %d",SRBtmp->index,tag_modifier);
			MPI_Send(buf,255,MPI_CHAR,SRBtmp2->index,SRB_LINK_TAG,XOOPIC_COMM);

			// tell SRBtmp about SRBtmp2
			sprintf(buf,"%s",SRBtmp2->name);
			sprintf(buf+strlen(SRBtmp2->name) +1, "%d , %d",SRBtmp2->index,tag_modifier);
			MPI_Send(buf,255,MPI_CHAR,SRBtmp->index,SRB_LINK_TAG,XOOPIC_COMM);
			
			
		     }
		     
		  }
	    }
	 }
   }

   if(tag_modifier > 3900) {
      printf("XOOPIC is unable to handle so much job distriubtion.  Use fewer nodes.\n");
      exit(0); //MPI_EXIT
   }

   // synchronize everone after all the link messages have been sent
   MPI_Barrier(XOOPIC_COMM);

   // begin processing the link messages
   MPI_Status SRBincoming_status;
   int incoming_SRB_P;
   MPI_Iprobe(0,SRB_LINK_TAG,XOOPIC_COMM,&incoming_SRB_P,&SRBincoming_status);
   while(incoming_SRB_P) {
      char buf[256];
      MPI_Recv(buf,255,MPI_CHAR,0,SRB_LINK_TAG,XOOPIC_COMM,&SRBincoming_status);

      //  look for the local matching boundary
      oopicListIter <SpatialRegionBoundary> SRBIter(getSpatialRegionBoundaryList());
      for(SRBIter.restart();!SRBIter.Done();SRBIter++) {
	 if(SRBIter.current()->isLinked()) continue;  //  if it's linked already, this isn't it.
	 
	 // Does the linkname match the current boundary?  If so, link it.
	 if(!strcmp(SRBIter.current()->PartnerName.c_str(),buf)) {
	    SpatialRegionBoundary *SRB = SRBIter.current();
	    SPBoundLink *link = new SPBoundLink(SRB->lE,SRB->lB,SRB->lJ,
                                                SRB->getLength(),SRB->getLength(), 
                                                SRB->lNGDbuf, 
                                                SRB->getnumElementsNGDbuf());
	    link->makeLink(atoi(buf + strlen(buf) +1),atoi(strstr(buf+strlen(buf)+1,",")));
	    SRB->LinkBoundary(link);
	    SRB->initialize_passives();
	 }

         
      }
#ifdef MPI_DEBUG
      printf("\nAdvisman: MPI init, receiving incoming link info, #%d",MPI_RANK);
#endif
      MPI_Iprobe(0,SRB_LINK_TAG,XOOPIC_COMM,&incoming_SRB_P,&SRBincoming_status);
   }
   
}


#endif /* MPI_VERSION */
