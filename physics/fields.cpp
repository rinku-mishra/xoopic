/*
   ====================================================================

   fields.cpp

   Purpose:	Member functions for Fields class.  This class provides 
   the full set of electromagnetic fields, as well as 
   methods for advancing the PDEs in time.


   Revision/Programmer/Date
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
   0.970	(JohnV 08-01-94) Add prototypes for dadi() and init_dadi().
   0.980	(JohnV 08-17-95) Add compute_iC_iL(), generalize for arbitrary meshed epsilon.
   0.99  (KeithC 08-25-95) Lack of precision caused Bz0 to 'leak` into 
   Etheta and Br.  Added BNodeStatic to separate the Static part
   from the B fields that that Fields
   updates. Changed Fields::setBz0() to Fields::setBNodeStatic.
   0.991	(JohnV 09-10-95) Add setBeamDump() to create a magnetic beam dump;
   required making BNodeStatic a Vector3.
   0.992	(Ron Cohen 09-14-95) More parameters for BNodeStatic added to
   allow for more general initial magnetic fields.
   1.001	(JohnV, KC 04-19-96) Optimized innermost loop of advanceE() +3% speed.
   1.002	(JohnV, 04-25-96) Fixed nomenclature in compute_iC_iL().
   1.003 (JohnV 05-03-96) Modified toNodes() to set interior fields only, and call 
   Boundary::toNodes() for edges and internal boundaries.
   1.0035 (KC 5-15-96) Added EMdamping.  Naoki will tells us what we are doing to the 
   physics.
   1.004	(JohnV 05-15-96) Add speciesList, Ispecies to improve subcycling.
   1.01  (JohnV 09-30-97) Add freezeFields, which uses initial fields.
   1.02  (KC 01-13-98) Changed Compute_ic_iL to use dl, dlPrime, dS, and dSPrime from Grid.
   2.01  (JohnV 03-19-98) Add nSmoothing to apply binomial filter to rho within computeRho.
   2.02  (JohnV 02-04-99) MarderFlag->MarderIter 
   2.03  (Bruhwiler 11-08-99) added more vector and scalar quantities 
   to the ShiftFields() method, so that moving window algorithm 
   doesn't break emdamping algorithm
   2.0?  (Cary 22 Jan 00) Broke up shift fields into two parts in anticipation
   of MPI installation.
   CVS1.80.2.6 (Cary 23 Jan 00) Reindented toNodes so that is easily seen on
   80 char telnet window.  No significant changes here.  For 
   sendFieldsAndShift, moved the send to after the shift.  Now 
   detect if boundary is SPATIAL_REGION_BOUNDARY, if so, call
   putCharge_and_Current.  Similarly, in recvShiftedFields, I check
   to see if boundary is SPATIAL_REGION_BOUNDARY.  If so, I call
   applyFields.
   CVS1.80.2.9 (Cary 26 Jan 00) changed sendFieldsAndShift to shiftFieldsAndSend
   for accuracy.  
   CVS1.87 (JohnV 05 Jan 2001) Added check for magnetic field file in setBNodeStatic(),
   improved efficiency.
   CVS1.88 (JohnV 21 Mar 2001) Re-ordered field components to B1, B2, B3 when
   magnetic field file

   ====================================================================
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef PETSC
#include "parpoi.h"
#endif

#include "fields.h"
#include "ptclgrp.h"
#include "dadirz.h"
#include "dadixy.h"
#include "dadixyp.h"
#include "boltzman.h"
#include "multigrid.h"
#include "dump.h"

//#include "cgxy.h"
// morgan
#include "domain.h"
#include "electrostatic_operator.h"
#include "cg.h"
#include "gmres.h"

#include "eval.h"
#include <cstdio>

#ifdef _MSC_VER
#include <iomanip>
#include <iostream>
#include <fstream>
using std::cout;
using std::endl;
#else

using namespace std;
#endif

// io includes

#ifdef HAVE_HDF5
#include <hdf5.h>
#include "dumpHDF5.h"
#endif



#ifdef PHI_TEST
Scalar analytic_rho(int j, int k);
#endif
extern Evaluator *adv_eval;


//--------------------------------------------------------------------
//	Allocate arrays for the electromagnetic fields and the capacitance
//	and inductance matrices.

Fields::Fields(Grid* _grid, BoundaryList* _boundaryList, SpeciesList* _speciesList, 
		Scalar epsR, int _ESflag,Scalar _presidue, int _BoltzmannFlag, int _SRflag)
{
	int i, j, k;
	grid = _grid;
	boundaryList = _boundaryList;
	speciesList = _speciesList;
	nSpecies = speciesList->nItems();
	presidue = _presidue;
	// Initialize flags:
	MarderIter=0;
	BGRhoFlag=0;
	DivergenceCleanFlag=0;
	CurrentWeightingFlag=0;
	ElectrostaticFlag=_ESflag;
	SynchRadiationFlag = _SRflag;
	freezeFields = 0;
	BoltzmannFlag = _BoltzmannFlag;
	ShowInitialDensityFlag = FALSE;
	MarderParameter=0;
	psolve=0;
	sub_cycle_iter = 0;
	EMdamping = 0;
	epsilonR = epsR;
	J = grid->getJ();
	K = grid->getK();
	//grid->setBoundaryMask(*boundaryList);
	//	allocate memory
	rho = new Scalar *[J + 1];
	Charge = new Scalar *[J + 1];
	backGroundRho = new Scalar *[J + 1];
	Phi = new Scalar *[J + 1];
	PhiP = new Scalar *[J + 1];
	DivDerror = new Scalar *[J + 1];
	ENode = new Vector3* [J + 1];
	BNode = new Vector3* [J + 1];
	BNodeStatic = new Vector3* [J+1];
	BNodeDynamic = new Vector3* [J+1];
	I = new Vector3* [J + 1];
	Ispecies = new Vector3** [nSpecies];
	oopicListIter<Species> spIter(*speciesList);
	for (spIter.restart(); !spIter.Done(); spIter++){
		i = spIter.current()->getID();
		if (spIter.current()->isSubcycled()) Ispecies[i] = new Vector3* [J+1];
		else Ispecies[i] = NULL;
	}
	Inode = new Vector3* [J + 1];
	intEdl = new Vector3* [J + 1];
	intEdlPrime = new Vector3* [J + 1];
	intEdlBar = new Vector3* [J + 1];
	intBdS = new Vector3* [J + 1];
	iC = new Vector3* [J + 1];
	iL = new Vector3* [J + 1];
	epsi = new Scalar *[J];  //cell centered
	rho_species = 0;
	accumulator = 0;

	for (j=0; j<=J; j++)
	{
		k = 0;
		rho[j] = new Scalar[K + 1];
		Charge[j] = new Scalar[K + 1];
		backGroundRho[j] = new Scalar[K + 1];
		Phi[j] = new Scalar [K + 1];
		PhiP[j] = new Scalar [K + 1];
		DivDerror[j]= new Scalar[K + 1];
		ENode[j] = new Vector3[K + 1];
		BNode[j] = new Vector3[K + 1];
		BNodeStatic[j] = new Vector3[K+1];
		BNodeDynamic[j] = new Vector3[K+1];
		I[j] = new Vector3[K + 1];
		for (i = 0; i < nSpecies; i++)
			if (Ispecies[i]) Ispecies[i][j] = new Vector3[K + 1];
		Inode[j] = new Vector3[K + 1];
		intEdl[j] = new Vector3[K + 1];
		intEdlPrime[j] = new Vector3[K + 1];
		intEdlBar[j] = new Vector3[K + 1];
		intBdS[j] = new Vector3[K + 1];
		iC[j] = new Vector3[K + 1];
		iL[j] = new Vector3[K + 1];
	}

	for(j=0; j<J; j++)
		epsi[j] = new Scalar[K];

	loaddensity = new Scalar **[nSpecies];
	for(i=0;i<nSpecies;i++) {
		loaddensity[i] = new Scalar *[J+1];
		for(j=0;j<=J;j++) {
			loaddensity[i][j]=new Scalar[K+1];
			//zero the memory
			memset(loaddensity[i][j],0,(K+1)*sizeof(Scalar));
		}
	}

	intEdlBasePtr = intEdl;
	//  The following initializations to zero are important
	//  correct answers depend upon them.
	for(j=0;j<=J;j++)
		for(k=0;k<=K;k++) {
			rho[j][k]=0;Phi[j][k]=0;backGroundRho[j][k]=0;
			Charge[j][k]=0;
			PhiP[j][k]=0;
			DivDerror[j][k]=0;
			//			epsi[j][k]=1/iEPS0;
		}; //initialize rho to zero
	for(j=0;j<J;j++)
		for(k=0;k<K;k++)
			epsi[j][k]=1/iEPS0;
	minusrho=0;
	d=0; delPhi=0;  //These initializations are important
	oopicListIter<Boundary>	nextb(*boundaryList);
	for(nextb.restart(); !nextb.Done(); nextb++)
		nextb.current()->setFields(*this);
	compute_iC_iL();
	// must be done AFTER setFields()
	initPhi();  // initialize the Poisson solver  MUST be done before the boundaries
	// are done below
	for (nextb.restart(); !nextb.Done(); nextb++)
		nextb.current()->setPassives();
	//  Initialize the Laplace solutions for the potential
	//  due to time-varying equipotential boundaries
#ifdef BCT_DEBUG
	printf("\n");
	for(k=K;k>=0;k--) {
		for(j=0;j<=J;j++) {
			Boundary *B = grid->GetNodeBoundary()[j][k];
			int type;
			if(B!=NULL) type = B->getBCType();
			else type = FREESPACE;

			printf("%d\t",type);
		}
		printf("\n");
	}
#endif
#undef RANDOM_FIELDS
#ifdef RANDOM_FIELDS
	// Here we want to set ALL the fields in the system to something random and predictable.
	for(j=0;j<=J;j++)
		for(k=0;k<=K;k++) {
			int j1,k1, seed;
			k1 = k;
			j1 = (int)(( (grid->getMKS(j,k).e1())/grid->dl1(1,1)) + 0.5);
			seed = k + j1 * K;
			srand(seed);
			/*			intEdl[j][k] = Vector3((rand()/(float)RAND_MAX - 0.5),(rand()/(float)RAND_MAX - 0.5),(rand()/(float)RAND_MAX - 0.5));
                                intBdS[j][k] = Vector3((rand()/(float)RAND_MAX - 0.5),(rand()/(float)RAND_MAX - 0.5),(rand()/(float)RAND_MAX - 0.5)); */

			intEdl[j][k] = Vector3(seed, 10*seed, 100*seed);
			intBdS[j][k] = 1e-7 * Vector3(.001*seed,.01*seed, .1*seed);
		}

#endif			



	for (nextb.restart(); !nextb.Done(); nextb++)
		nextb.current()->InitializeLaplaceSolution();
}

//--------------------------------------------------------------------
//	Deallocate memory for Fields object

Fields::~Fields()
{
	int i;
	int j;
	for (j=0; j<=J; j++)
	{
		delete[] ENode[j];
		delete[] BNode[j];
		delete[] BNodeStatic[j];
		delete[] BNodeDynamic[j];
		delete[] I[j];
		delete[] Inode[j];
		for (i=0; i < nSpecies; i++) if (Ispecies[i]) delete[] Ispecies[i][j];
		delete[] intEdl[j];
		delete[] intEdlPrime[j];
		delete[] intEdlBar[j];
		delete[] intBdS[j];
		delete[] iC[j];
		delete[] iL[j];
		delete[] DivDerror[j];
		delete[] rho[j];
		delete[] Charge[j];
		delete[] backGroundRho[j];
		delete[] Phi[j];
		delete[] PhiP[j];
		if(delPhi!=0) delete[] delPhi[j] ;
		if(minusrho!=0) delete[] minusrho[j];
		if(d!=0) delete[] d[j];
	}
	for (j=0; j<J; j++)
		delete[] epsi[j];
	delete[] ENode;
	delete[] BNode;
	delete[] BNodeStatic;
	delete[] BNodeDynamic;
	delete[] I;
	delete[] Inode;
	for (i=0; i < nSpecies; i++) {
		if(Ispecies[i])
			delete[] Ispecies[i];
		else
			delete Ispecies[i];
	}
	delete[] Ispecies;
	delete[] intEdl;
	delete[] intEdlPrime;
	delete[] intEdlBar;
	delete[] intBdS;
	delete[] iC;
	delete[] iL;
	delete[] DivDerror;
	delete[] rho;
	delete[] Charge;
	delete[] backGroundRho;
	delete[] Phi;
	delete[] PhiP;
	delete[] epsi;
	if(delPhi!=0) delete[] delPhi;
	if(minusrho!=0)   delete[] minusrho;
	if(d!=0) delete[] d;
	delete psolve;
	if(rho_species)
	{
		int J=grid->getJ();
		for(i=0; i<nSpecies; i++)
		{
			for(j=0; j<J+1; j++)
				delete [] rho_species[i][j];
			delete [] rho_species[i];
		}
		delete [] rho_species;
	}
	int J=grid->getJ();
	for(i=0; i<nSpecies; i++)
	{
		for(j=0; j<=J; j++)
			delete [] loaddensity[i][j];
		delete [] loaddensity[i];
	}
	delete [] loaddensity;
	/* begin
      if (poisson) {
      delete poisson;
      poisson = 0;
      } 
	 */
	 /*
      if (dom) {
      delete dom;
      dom = 0;
      }

      end debug */
}

//--------------------------------------------------------------------
//	Advance the fields from t-dt->t by solving Maxwell's equations.

void Fields::advance(Scalar t, Scalar dt)
/* throw(Oops) */{

	/*
      int kout = K/4;
      #ifdef MPI_VERSION
      extern int MPI_RANK;
      if(MPI_RANK == 0){
      cout << "MPI_RANK = " << MPI_RANK << ", Fields::advance entered.  kout = "
      << kout << "." << endl;
      #else
      cout << "Fields::advance entered.  kout = " <<
      kout << "." << endl;
      #endif
      cout << "E1 =";
      for(int j1=0; j1<=J; j1++)  cerr << " " << intEdl[j1][kout].e1();
      cout << endl;
      cout << "E2 =";
      for(int j1=0; j1<=J; j1++)  cerr << " " << intEdl[j1][kout].e2();
      cout << endl;
      cout << "I1 =";
      for(int j1=0; j1<=J; j1++)  cout << " " << I[j1][kout].e1();
      cout << endl;

      #ifdef MPI_VERSION
      }
      #endif
	 */

	int	j, k;
	local_dt = dt;  // so that objects needing to manipulate
	// fields directly can get_dt() easily
	Scalar local_t = t;

	//  get any charge on any dielectrics into rho, put any currents generated by loops
	// this current weighting does not subcycle properly

	//ApplyToList(putCharge_and_Current(t),*boundaryList,Boundary);

	if(ElectrostaticFlag)
		ElectrostaticSolve(t,dt);

	else
	{
		if(DivergenceCleanFlag) {
			updateDivDerror();
			DivergenceClean();
			updateDivDerror();
		}

		if(CurrentWeightingFlag)  InodeToI();  //  the bilinear weighting needs this
		if (grid->axis()) radialCurrentCorrection(); // correct current at 0 and K

		for (int i = 0; i < FieldSubFlag; i++, local_t += dt)
		{
			if (EMdamping>0)
				intEdl = intEdlPrime;

			if (i==0) advanceB(0.5*dt);		   //	advance B^(n-1) -> B^(n-1/2)
			else advanceB(dt);		   			//	advance B^(n-3/2) -> B^(n-1/2)

			if (EMdamping>0)
			{
				intEdl = intEdlBasePtr;
				for (j=0; j<=J; j++)
					for (k=0; k<=K; k++) {
						intEdlBar[j][k] *= EMdamping;
						intEdlBar[j][k] += (1-EMdamping)*intEdl[j][k];
						intEdlPrime[j][k] = .5*((1-EMdamping)*intEdlBar[j][k] -intEdl[j][k]);
					}
			}

			advanceE(dt);                   //	advance E^(n-1) -> E^n

			//	Apply Boundary Conditions at t
			try{
				ApplyToList(applyFields(local_t,dt),*boundaryList,Boundary);
			}
			catch(Oops& oops3){
				oops3.prepend("Fields::advance: Error: \n");
				throw oops3;
			}
			if (EMdamping>0)
				for (j=0; j<=J; j++)
					for (k=0; k<=K; k++)
						intEdlPrime[j][k] += (1+.5*EMdamping)*intEdl[j][k];
		}

		if (EMdamping>0)
			intEdl = intEdlPrime;

		advanceB(0.5*dt);

		if (EMdamping>0)
			intEdl = intEdlBasePtr;
	}

	if(FieldSubFlag==1)   //ask peter about this
		if(MarderIter>0) MarderCorrection(dt);

	/*
      #ifdef MPI_VERSION
      extern int MPI_RANK;
      if(MPI_RANK == 0){
      cout << "MPI_RANK = " << MPI_RANK << ", leaving Fields::advance.  E1 =";
      #else
      cout << "Leaving Fields::advance.  E1 =";
      #endif
      for(int i1=0; i1<=J; i1++)  cerr << " " << intEdl[i1][kout].e1();
      cout << endl;
      cout << "E1 =";
      for(int j1=0; j1<=J; j1++)  cout << " " << intEdl[j1][kout].e2();
      cout << endl;
      cout << "I1 =";
      for(int j1=0; j1<=J; j1++)  cout << " " << I[j1][kout].e1();
      cout << endl;

      #ifdef MPI_VERSION
      }
      #endif
	 */

}

void Fields::ElectrostaticSolve(Scalar t, Scalar dt)
/* throw(Oops) */{
	if(BoltzmannFlag)	{
		oopicListIter<Boundary> nextb(*boundaryList);
		for (nextb.restart(); !nextb.Done(); nextb++){
			try{
				nextb.current()->applyFields(t,dt);
			}
			catch(Oops& oops3){
				oops3.prepend("Fields::ElectrostaticSolve: Error: \n");
				throw oops3;
			}
		}
		theBoltzmann->updatePhi(rho,Phi,t,dt);
	}
	else updatePhi(rho,Phi,t,dt);
	setEfromPhi();
}

//--------------------------------------------------------------------
//	Advances B by the time step specified.  This function is called twice
//	per time step by advance() with a half timestep each time.

void Fields::advanceB(Scalar dt2)
{
	int	j, k;					//	indices
	for (j=0; j<J; j++)
	{
		for (k=0; k<K; k++)
		{
			intBdS[j][k] -= dt2*Vector3(intEdl[j][k+1].e3() - intEdl[j][k].e3(),
					-intEdl[j+1][k].e3() + intEdl[j][k].e3(),
					intEdl[j][k].e1() - intEdl[j][k+1].e1()
					+ intEdl[j+1][k].e2() - intEdl[j][k].e2());
		}
		//	get intBdS[j][K].e2 only
		intBdS[j][K] -= dt2*Vector3(0, intEdl[j][K].e3() - intEdl[j+1][K].e3(), 0);
	}
	for (k=0; k<K; k++)				//	intBdS[J][k].e1
		intBdS[J][k] -= dt2*Vector3(intEdl[J][k+1].e3()	- intEdl[J][k].e3(), 0, 0);

}

void Fields::advanceE(Scalar dt) {
	int j,k;

	Vector3*	iL0 = &iL[0][0];
	intEdl[0][0] += dt*iC[0][0].jvMult(Vector3(
			intBdS[0][0].e3()*iL0->e3(),
			-intBdS[0][0].e3()*iL0->e3(),
			-intBdS[0][0].e1()*iL0->e1() + intBdS[0][0].e2()*iL0->e2())
			- I[0][0]);
	iL0 = &iL[0][K];
	Vector3*	iLk1 = &iL[0][K-1];
	intEdl[0][K] += dt*iC[0][K].jvMult(Vector3(
			-intBdS[0][K-1].e3()*iLk1->e3(),
			0,
			intBdS[0][K-1].e1()*iLk1->e1()	+ intBdS[0][K].e2()*iL0->e2())
			- I[0][K]);
	iL0 = &iL[J][0];
	Vector3*	iLj1 = &iL[J-1][0];
	intEdl[J][0] += dt*iC[J][0].jvMult(Vector3(
			0,
			intBdS[J-1][0].e3()*iLj1->e3(),
			-intBdS[J][0].e1()*iL0->e1() - intBdS[J-1][0].e2()*iLj1->e2())
			- I[J][0]);
	iL0 = &iL[J][K];
	iLk1 = &iL[J][K-1];
	iLj1 = &iL[J-1][K];
	intEdl[J][K] += dt*iC[J][K].jvMult(Vector3(
			0,
			0,
			intBdS[J][K-1].e1()*iLk1->e1() - intBdS[J-1][K].e2()*iLj1->e2())
			- I[J][K]);
	for (k=1; k<K; k++)
	{
		iL0 = &iL[0][k];
		iLk1 = &iL[0][k-1];
		intEdl[0][k] += dt*iC[0][k].jvMult(Vector3(
				intBdS[0][k].e3()*iL0->e3() - intBdS[0][k-1].e3()*iLk1->e3(),
				-intBdS[0][k].e3()*iL0->e3(),
				intBdS[0][k-1].e1()*iLk1->e1() - intBdS[0][k].e1()*iL0->e1()
				+ intBdS[0][k].e2()*iL0->e2())
				- I[0][k]);
		iL0 = &iL[J][k];
		iLk1 = &iL[J][k-1];
		iLj1 = &iL[J-1][k];
		intEdl[J][k] += dt*iC[J][k].jvMult(Vector3(
				//								 intBdS[J][k].e3()*iL0->e3() - intBdS[J][k-1].e3()*iLk1->e3(),
				0,   // component 1 outside system
				intBdS[J-1][k].e3()*iLj1->e3(),
				intBdS[J][k-1].e1()*iLk1->e1() - intBdS[J][k].e1()*iL0->e1()
				- intBdS[J-1][k].e2()*iLj1->e2())
				- I[J][k]);
	}
	Vector3 *Bk1, *Bj1, *B0;
	for (j=1; j<J; j++)
	{
		iL0 = &iL[j][0];
		iLj1 = &iL[j-1][0];
		B0 = &intBdS[j][0];
		Bj1 = &intBdS[j-1][0];
		intEdl[j][0] += dt*iC[j][0].jvMult(Vector3(
				intBdS[j][0].e3()*iL0->e3(),
				-intBdS[j][0].e3()*iL0->e3() + intBdS[j-1][0].e3()*iLj1->e3(),
				-intBdS[j][0].e1()*iL0->e1() + intBdS[j][0].e2()*iL0->e2()
				- intBdS[j-1][0].e2()*iLj1->e2())
				- I[j][0]);
		for (k=1; k<K; k++)
		{
			iLk1 = iL0;
			iLj1 = &iL[j-1][k];
			iL0 = &iL[j][k];
			Bk1 = B0;
			Bj1 = &intBdS[j-1][k];
			B0 = &intBdS[j][k];
			intEdl[j][k] += dt*iC[j][k].jvMult(Vector3(
					B0->e3()*iL0->e3() - Bk1->e3()*iLk1->e3(),
					-B0->e3()*iL0->e3() + Bj1->e3()*iLj1->e3(),
					Bk1->e1()*iLk1->e1() - B0->e1()*iL0->e1()
					+ B0->e2()*iL0->e2() - Bj1->e2()*iLj1->e2())
					- I[j][k]);
		}
		iL0 = &iL[j][K];
		iLk1 = &iL[j][K-1];
		intEdl[j][K] += dt*iC[j][K].jvMult(Vector3(
				-intBdS[j][K-1].e3()*iLk1->e3(),
				0,
				intBdS[j][K-1].e1()*iLk1->e1()
				+ intBdS[j][K].e2()*iL0->e2() - intBdS[j-1][K].e2()*iLj1->e2())
				- I[j][K]);
	}

}

//--------------------------------------------------------------------
//	Translates to new locations by incrementally locating cell crossings
//	and accumulates current along the trajectory.  Initial position passed
//	in x is updated to final position on return.  Return value is a
//	pointer to a boundary if encountered, NULL otherwise.
//	qOverDt = q/dt*particleWeight for this particle.

Boundary* Fields::translateAccumulate(Vector2& x, Vector3& dxMKS, Scalar qOverDt)
{
	Boundary*	boundary = NULL;
	int	which_dx = (dxMKS.e1() != 0.0) ? 1 : 2;
	Scalar	prev_dxMKS = (which_dx == 1) ? dxMKS.e1() : dxMKS.e2();
	Scalar	frac_v3dt;

	while (fabs(dxMKS.e1()) + fabs(dxMKS.e2()) > 1E-25){
		Vector2 	xOld = x;
		boundary = grid->translate(x, dxMKS);

		//	If the particle translates in x1 or x2, apply the fraction of
		//	rotation for this cell only; otherwise it spins only and all
		//	current is collected at this location.
		if (prev_dxMKS != 0) frac_v3dt = dxMKS.e3()*(prev_dxMKS -
				((which_dx==1) ? dxMKS.e1() : dxMKS.e2()))/prev_dxMKS;
		else frac_v3dt = dxMKS.e3();
		accumulate(xOld, x, frac_v3dt, qOverDt);
		prev_dxMKS = (which_dx == 1) ? dxMKS.e1() : dxMKS.e2();
		if (boundary) break;
	}

	return boundary;
}

//--------------------------------------------------------------------
//	Weight intEdl and intBdS to nodes to obtain Enode and Bnode in
//	physical units.  This routine has been modified to set only the 
// interior points; edges and internal boundaries are set by calling
// Boundary::toNodes().

void Fields::toNodes(Scalar t) {

	int j, k;
	Scalar w1p, w1m, w2p, w2m;

	// all interior points in 1-direction
	for (k=j=1; j<J; j++) {

		// Calculate weight factors for 1-directions
		w1m = grid->dl1(j, k)/(grid->dl1(j-1, k) + grid->dl1(j, k));
		w1p = 1 - w1m;

		// Not sure what this is doing
		BNode[j][0]=BNode[j][K]=0;  // unbounded edges.

		// All interior points in the 2-direction

		// JRC: Could this be accelerated through use of smaller loops?
		for (k=1; k<K; k++) {

			// Get weights in 2-direction
			w2m = grid->dl2(j, k)/(grid->dl2(j, k-1) + grid->dl2(j, k));
			w2p = 1 - w2m;

			// Weight each of the fields

			// E1 defined on 2-node values, but weight in 1-direction
			ENode[j][k].set_e1(w1p*intEdl[j][k].e1()/grid->dl1(j, k)
					+ w1m*intEdl[j-1][k].e1()/grid->dl1(j-1, k));

			// E2 defined on 1-node values, but weight in 2-direction
			ENode[j][k].set_e2(w2p*intEdl[j][k].e2()/grid->dl2(j, k)
					+ w2m*intEdl[j][k-1].e2()/grid->dl2(j, k-1));

			// E3 defined at the nodes
			ENode[j][k].set_e3(intEdl[j][k].e3()/grid->dl3(j,k));

			// B1 defined on 1-node values, but weight in 2-direction
			BNode[j][k].set_e1(intBdS[j][k].e1()*(w2p/grid->dS1(j, k))
					+ intBdS[j][k-1].e1()*w2m/grid->dS1(j, k-1));

			// B2 defined on 2-node values, but weight in 1-direction
			BNode[j][k].set_e2(intBdS[j][k].e2()*(w1p/grid->dS2(j, k))
					+ intBdS[j-1][k].e2()*w1m/grid->dS2(j-1, k));

			// B3 weighted in both directions (a cell center quantity)
			BNode[j][k].set_e3(intBdS[j][k].e3()*w1p*w2p/grid->dS3(j, k)
					+ intBdS[j-1][k].e3()*w1m*w2p/grid->dS3(j-1, k)
					+ intBdS[j][k-1].e3()*w1p*w2m/grid->dS3(j, k-1)
					+ intBdS[j-1][k-1].e3()*w1m*w2m/grid->dS3(j-1, k-1));
		}
	}

	// Not sure what this is doing.  Shouldn't this be set by boundaries?
	for (k=0; k<=K; k++)	BNode[0][k] = BNode[J][k] = Vector3(0,0,0);  // unbounded edges.
	// JK, 2016-03-11 - used to be: for (j=1; j<J; j++) BNode[j][K] = BNode[j][K] = Vector3(0,0,0);
	for (j=1; j<J; j++) BNode[j][0] = BNode[j][K] = Vector3(0,0,0);

	// boundaries interpolate coincident fields (assumes interior previously set):
	ApplyToList(toNodes(),*boundaryList,Boundary);

	// Add in the static magnetic field
	for (j=0; j<=J; j++)
		for (k=0; k<=K; k++) {
			BNodeDynamic[j][k] = BNode[j][k];
			BNode[j][k] += BNodeStatic[j][k];
		}

}

//--------------------------------------------------------------------
//	Zero the current accumulator array, I

void Fields::clearI(int i)
{
	Vector3** Iptr;
	if (ElectrostaticFlag) return;
	if (i == -1) Iptr = I;
	else Iptr = Ispecies[i];
	if (!Iptr) return;
	for (int j=0; j<=J; j++){
		if (CurrentWeightingFlag!=0) memset(Inode[j], 0, (K+1)*sizeof(Inode[j][0]));
		memset(Iptr[j], 0, (K+1)*sizeof(Iptr[j][0]));
	}
}

// Accumulate current for species I:

void Fields::setAccumulator(int i)
{
	if (Ispecies[i]) accumulator = Ispecies[i];
	else if (CurrentWeightingFlag) accumulator = Inode;
	else accumulator = I;
}

void Fields::addtoI(int i)
{
	if (!Ispecies[i]) return;
	for (int j=0; j<=J; j++)
		for (int k=0; k<=K; k++)
			I[j][k] += Ispecies[i][j][k];
}

void Fields::Excite() //ntg8-16 excites a field component at t = 0
{
	this->setIntEdl(J/2, K/2, 1, 1.0);
	this->setIntEdl(J/2, K/2, 2, -1.0);
	this->setIntEdl(J/2, K/2+1, 1, -1.0);
	this->setIntEdl(J/2+1, K/2, 2, 1.0);
	/*
      Scalar freq = 20E9;
      Scalar z = getMKS(J/2, 0).e1();
      Scalar outerradius = getMKS(J/2, K).e2();
      for(int k = 0; k <= K; k++)
      {Scalar r = getMKS(J/2, k).e2();
      Scalar Ezval = AnalyticEz(z, r, freq, 0.0, outerradius); 
      this->setIntEdl(J/2, k, 1, Ezval);}
	 */
}

//--------------------------------------------------------------------
//	Seed the field emitter with a large negative field to get it
//	started emitting.

void Fields::SeedFieldEmitter()
{
	for (int j=0; j<J; j++)
		for (int k=0; k<=K; k++)
			setIntEdl(j, k, 1, -1000);
}

//--------------------------------------------------------------------
//	Set initial uniform magnetic field in x1. 
//	Here, Bz0 is in Tesla.

BOOL Fields::setBNodeStatic(Vector3 B0,Scalar betwig, Scalar zoff, char* BFname,const ostring &B01analytic,const ostring &B02analytic, const ostring &B03analytic)
{
	FILE *openfile;
	int status;
	ostring B01a = B01analytic;
	ostring B02a = B02analytic;
	ostring B03a = B03analytic;
	Vector3 Btemp;
	if (strcmp(BFname, "NULL")) {
		if ((openfile = fopen (BFname, "r"))  != NULL) {
			// read in
			Scalar dum1, dum2, B1, B2, B3; // dummies, remove later
			cout << "starting to read B field file" << endl;
			for (int j=0; j<=J; j++) {
				for (int k=0; k<=K; k++) {
#ifdef SCALAR_DOUBLE
					// status = fscanf (openfile, "%lg %lg %lg %lg %lg", &dum1, &dum2, &B2, &B3, &B1);
					status = fscanf (openfile, "%lg %lg %lg %lg %lg", &dum1, &dum2, &B1, &B2, &B3);
#else
					// status = fscanf (openfile, "%g %g %g %g %g", &dum1, &dum2, &B2, &B3, &B1);
					status = fscanf (openfile, "%g %g %g %g %g", &dum1, &dum2, &B1, &B2, &B3);
#endif
					Btemp.set_e1(B1*1e-4);
					Btemp.set_e2(B2*1e-4);
					Btemp.set_e3(B3*1e-4);
					BNodeStatic[j][k] = Btemp;
				}
				//				status = fscanf(openfile, "%e", &dum1);
			}
			cout << "finished reading B field file" << endl;
			fclose (openfile);
		}
		else {
			cout << "open failed on magnetic field file Bf in Control group";
			return FALSE;
		}
	}
	else if (B0.e1()!=0 || B0.e2()!=0 || B0.e3()!=0) {
		for (int j=0; j<=J; j++)
			for (int k=0; k<=K; k++)
			{Bz0 = (grid->query_geometry() == ZXGEOM) ? B0.e1() : B0.e1()
					+ B0.e2()*(cos(betwig*(getMKS(j,k).e1()-zoff)));
			Bx0 = (grid->query_geometry() == ZXGEOM) ? B0.e2() :
					+ .5*B0.e2()*betwig*getMKS(j,k).e2()*(sin(betwig*(getMKS(j,k).e1()-zoff)));
			Btemp.set_e1(Bz0);
			Btemp.set_e2(Bx0);
			Btemp.set_e3(B0.e3());
			BNodeStatic[j][k] = Btemp;
			}
	}
	else {
		Scalar x1,x2;  // These can now be scalar -- RT, 2003/12/09
		adv_eval->add_indirect_variable("x1",&x1);
		adv_eval->add_indirect_variable("x2",&x2);
		B01a+='\n';
		B02a+='\n';
		B03a+='\n';
		for(int j=0;j<=J;j++)
			for(int k=0;k<=K;k++) {
				x1 = grid->getMKS(j,k).e1();
				x2 = grid->getMKS(j,k).e2();
				Btemp.set_e1(adv_eval->Evaluate(B01a.c_str()));
				Btemp.set_e2(adv_eval->Evaluate(B02a.c_str()));
				Btemp.set_e3(adv_eval->Evaluate(B03a.c_str()));
				BNodeStatic[j][k] = Btemp;
			}
	}
	return TRUE;
}

void Fields::setBeamDump(int _j1, int _j2)
{
	int j1 = MIN(_j1, _j2);
	int j2 = MAX(_j1, _j2);
	if (j1 < 0 || j2 > J) return;			// default values or error
	Scalar x1 = getMKS(j1).e1();
	Scalar x2 = getMKS(j2).e1();
	Scalar beta = M_PI/(2*(x2 - x1));
	Scalar geomMult = (grid->query_geometry() == ZXGEOM) ? 1 : 0.5;
	int j,k;
	for (j=j1; j<=j2; j++)
		for (k=0; k<=K; k++)
			BNodeStatic[j][k] = Bz0*Vector3(cos(beta*(getMKS(j,k).e1()-x1)),
					geomMult*beta*getMKS(j,k).e2()*sin(beta*(getMKS(j,k).e1()-x1)),
					BNodeStatic[j][k].e3());
	for (j=j2+1; j<=J; j++)
		for (k=0; k<=K; k++)
			BNodeStatic[j][k] = Vector3(0, 0, 0);
}

//--------------------------------------------------------------------
//	Update the DivD array of divergences.  This is not intended to
//      be kept updated but rather to be updated by request.

void Fields::updateDivDerror(void)
{
	int j,k;
	Vector3 D1,D2;
	Scalar Div,ic;
	//This section of the code computes the divergence of D everywhere.

	for(j=1;j<=J-1;j++) {  // bounds are temporary, need to check for metals eventually
		for(k=1 ;k<=K-1;k++) {

			if( j>0&&(ic=iC[j-1][k].e1())!=0.0)
				D1.set_e1( intEdl[j-1][k].e1()/ic );
			else
				D1.set_e1(0);

			if( k>0&&(ic=iC[j][k-1].e2())!=0.0)
				D1.set_e2( intEdl[j][k-1].e2()/ic );
			else
				D1.set_e2(0);

			if( j<J&& (ic=iC[j][k].e1())!=0.0)
				D2.set_e1( intEdl[j][k].e1()/ic );
			else
				D2.set_e1(0);
			if( k<K&&(ic=iC[j][k].e2()) !=0.0)
				D2.set_e2( intEdl[j][k].e2()/ic );
			else
				D2.set_e2(0);

			//		D1.set_e2(D2.e2()); //from the previous
			//D1.set_e1( intEdl[j][k].e1()*iC[j][k].e1() );
			//D2.set_e1( intEdl[j+1][k].e1()*iC[j+1][k].e1() );
			//D2.set_e2( intEdl[j][k+1].e2()*iC[j][k+1].e2() );

			Div = D2.e1() - D1.e1() + D2.e2() - D1.e2();
			Div /= grid->get_halfCellVolumes()[j][k];
			DivDerror[j][k]=-Div + rho[j][k];
		}
	}
}


//--------------------------------------------------------------------
//        compute the potential everywhere using Fields::rho as a 
//	  source term


void Fields::updatePhi(Scalar **source,Scalar **dest,Scalar t,Scalar dt)
/* throw(Oops) */{
	int j,k;

	//	presiduetol_test=1e-4;  //tolerance for the poisson solver
	static int itermax=100;  //maximum # of iterations for psolve
	oopicListIter<Boundary> nextb(*boundaryList);

	// set up the boundary arrays if necessary
	//  they will not have been set up if u_z0 = 0
	if(minusrho==0) {
		minusrho = new Scalar* [J+1];
		for(j=0;j<=J;j++) {
			minusrho[j]=new Scalar[K+1];
			memset(minusrho[j],0,(K+1)*sizeof(Scalar));
		}
		itermax = 100;
	}

	//  Scale the source term by -1
	for(j=0;j<=J;j++) for(k=0;k<=K;k++)
		minusrho[j][k]=-(Scalar)source[j][k];

	//actually DO the solve
	psolve->solve(PhiP,minusrho,itermax,presidue);

	// To add all the LaPlace solutions, we first need to start
	//  with the Poisson solution.
	for(j=0;j<=J;j++) for(k=0;k<=K;k++)
		dest[j][k]=PhiP[j][k];

	// apply any laplace boundary conditions to phi
	for (nextb.restart(); !nextb.Done(); nextb++)
	{
		try{
			nextb.current()->applyFields(t,dt);
		}
		catch(Oops& oops3){
			oops3.prepend("Fields::updatePhi: Error: \n");
			throw oops3;
		}
	}
	//  assign the solution to dest
	for(j=0;j<=J;j++) for(k=0;k<=K;k++)
		dest[j][k]=Phi[j][k];


}

// -----------------------------
//  complete those tasks necessary to initialize the poisson
//  solver--symmetry boundary at zero, dirichlet otherwise

void Fields::initPhi() 
/* throw(Oops) */{

#ifndef PETSC
	if(grid->getPeriodicFlagX1()&&ElectrostaticFlag == DADI_SOLVE)
		ElectrostaticFlag = PERIODIC_X1_DADI;
	switch(ElectrostaticFlag) {
	default:
		fprintf(stderr,"WARNING, poisson solve defaulting to Multigrid\n");

	case ELECTROMAGNETIC_SOLVE:
		psolve=new Multigrid(epsi,grid);
		break;
	case DADI_SOLVE:

		switch(grid->query_geometry()) {
		case ZRGEOM:
			psolve=new Dadirz(epsi,grid);
			break;
		case ZXGEOM:
			psolve=new Dadixy(epsi,grid);
			break;
		}
		break;

		// morgan
		// begin Krylov inverters
		case CG_SOLVE:
			try{
				set_up_inverter(grid);
			}
			catch(Oops& oops3){
				oops3.prepend("Fields::initPhi: Error: \n");
				throw oops3;
			}

			//Conjugate_Gradient *cg = new Conjugate_Gradient(dom,poisson);
			psolve = new Conjugate_Gradient(dom,poisson);
			break;
		case GMRES_SOLVE:
			try{
				set_up_inverter(grid);
			}
			catch(Oops& oops3){
				oops3.prepend("Fields::initPhi: Error: \n");  //OK
				throw oops3;
			}

			psolve = new GMRES(dom,poisson);
			break;
			// end Krylov inverters
		case MGRID_SOLVE:
			psolve=new Multigrid(epsi,grid);
			break;
		case PERIODIC_X1_DADI:
			try{
				psolve=new DadixyPer(epsi,grid);
			}
			catch(Oops& oops3){
				oops3.prepend("Fields::initPhi: Error: \n");//OK
				throw oops3;
			}
			break;


	}
#else // ndef-PETSC
	try{
		psolve = new ParallelPoisson(epsi,grid);
	}
	catch(Oops& oops3){
		oops3.prepend("Fields::initPhi: Error: \n");//OK
		throw oops3;
	}

#endif // ndef-PETSC
}



//--------------------------------------------
//  this collects the charge from the particle group
//  into rho[][]  If the group == 0 then it clears rho[][]

void Fields::collect_charge_to_grid(ParticleGroup *group,Scalar **_rho)
{
	int i,j,k;
	Scalar q,jx,kx;
	Vector2 *x;

	if(group==0) {  // reset flag - zero this rho
		for(j=0;j<=J;j++) memset(_rho[j],0,(K+1)*sizeof(Scalar));
		return;
	}

	q = group->get_q();  //charge * np2c
	x = group->get_x();
	if (q==0.) {
		Scalar pweight=0;
		for(i=0; i<group->get_n(); i++) {
			jx = x[i].e1();
			kx = x[i].e2();
			j = (int)jx; k=(int)kx;  //get the integer part
			jx -= j; kx -= k;  //get the fractional part only
			//if (group->vary_np2c()) pweight = group->get_np2c(i);
			pweight = group->get_np2c(i);

			_rho[j][k] += (1-jx)*(1-kx)*pweight;
			_rho[j][k+1] += (1-jx)*(kx)*pweight;
			_rho[j+1][k] += (jx)*(1-kx)*pweight;
			_rho[j+1][k+1] += (jx)*(kx)*pweight;
		}
	}
	else {
		for(i=0; i<group->get_n(); i++) {
			jx = x[i].e1();
			kx = x[i].e2();
			j = (int)jx; k=(int)kx;  //get the integer part
			jx -= j; kx -= k;  //get the fractional part only
			if (group->vary_np2c()) q = group->get_q(i);

			_rho[j][k] += (1-jx)*(1-kx)*q;
			_rho[j][k+1] += (1-jx)*(kx)*q;
			_rho[j+1][k] += (jx)*(1-kx)*q;
			_rho[j+1][k+1] += (jx)*(kx)*q;
		}
	}
}

//----------------------------------------------------------------------
//function iterates through the particle group list given it
//to sum up all the charge using collect_charge_to_grid()
//

void Fields::compute_rho(ParticleGroupList **list,int nSpecies) 
{
	int j;

	if (rho_species==0){ // not initialized, we need to allocate and initialize
		int J=grid->getJ();
		int K=grid->getK();
		rho_species = new Scalar **[nSpecies];
		for(int i=0;i<nSpecies;i++) {
			rho_species[i] = new Scalar *[J+1];
			for(j=0;j<J+1;j++) {
				rho_species[i][j]=new Scalar[K+1];
				//zero the memory
				memset(rho_species[i][j],0,(K+1)*sizeof(Scalar));
			}
		}
	}

#ifndef PHI_TEST
	collect_charge_to_grid(0,rho);  //reset the global rho
	for (int i=0; i<nSpecies; i++){
		collect_charge_to_grid(0,rho_species[i]);  //reset the species rho
		oopicListIter<ParticleGroup> pgscan(*list[i]);
		for(pgscan.restart();!pgscan.Done();pgscan++)
			collect_charge_to_grid(pgscan(),rho_species[i]);
	}
	charge_to_charge_density(nSpecies);
	grid->binomialFilter(rho, nSmoothing);
	addBGRho();

#else

	for(j=0;j<J;j++)
		for(int k=0;k<K;k++)
			rho[j][k] = analytic_rho(j,k);
#endif
}

void Fields::ZeroCharge(void)
{
	for(int j=0;j<=J;j++)
		for(int k=0;k<=K;k++)
			Charge[j][k] = 0.0;
}

Scalar Fields::get_rhospecies(int speciesID, int j, int k){
	Scalar cellvolume = grid -> cellVolume(j,k);

	//  return rho_species[speciesID][j][k]/=cellvolume;
	return rho_species[speciesID][j][k];
}

void Fields::charge_to_charge_density(int nSpecies)
{ 
	int i,j,k;
	Scalar **CellVolumes=grid->get_halfCellVolumes();
	oopicListIter<Species> spIter(*speciesList);
	for (spIter.restart();!spIter.Done();spIter++) {
		i = spIter.current()->getID();
		for (j=0; j<=J; j++)
			for (k=0; k<=K; k++) {
				rho_species[i][j][k]/=CellVolumes[j][k];
				if (spIter.current()->get_q()!=0.)
					rho[j][k] += rho_species[i][j][k];
			}
	}

	/* Old code commented off to enable
   neutral(zero charge) species to be counted
   in the number density calculations 
   for(i=0;i<nSpecies;i++) 
     for(j=0;j<=J;j++) 
         for(k=0;k<=K;k++) {
            rho_species[i][j][k]/=CellVolumes[j][k];
            rho[j][k]+=rho_species[i][j][k];
         }
	 */
	if (grid->getPeriodicFlagX1()){
		for (k=0; k<=K; k++) {
			rho[0][k] += rho[J][k]; // note: CellVol along these edges is doubled
			rho[J][k] = rho[0][k];
		}
		for (i=0; i<nSpecies; i++)
			for (k=0; k<=K; k++) {
				rho_species[i][0][k] += rho_species[i][J][k];
				rho_species[i][J][k] = rho_species[i][0][k];
			}
	}
	if (grid->getPeriodicFlagX2()){
		for (j=0; j<=J; j++) {
			rho[j][0] += rho[j][K];
			rho[j][K] = rho[j][0];
		}
		for (i=0; i<nSpecies; i++)
			for (j=0; j<=J; j++) {
				rho_species[i][j][0] += rho_species[i][j][K];
				rho_species[i][j][K] = rho_species[i][j][0];
			}
	}
}

void Fields::addBGRho()
{
	if(BGRhoFlag)
		for(int j=0;j<=J;j++)
			for(int k=0;k<=K;k++)
				rho[j][k] += backGroundRho[j][k];
}
//----------------------------------------------------------------------
//  Given a z-r solution of poisson\'s equation from updatePhi in
//  Scalar **Phi, make the electric fields consistent with it.
//  This is an initialization of the E-fields.  It will destroy any
//  previous E fields.  This function cannot touch e3.

void Fields::setEfromPhi()
{ 
	int j,k;

	//  phi solution is for phi at nodes.  This makes it convenient to
	//  get E at half-cells.  Fortunately, this is what intEdl needs.

	for(j=0;j<J;j++)
		for(k=0;k<K;k++) {
			setIntEdl(j,k,1,  (Phi[j][k]-Phi[j+1][k])   );
			setIntEdl(j,k,2,  (Phi[j][k]-Phi[j][k+1])   );
		}

	for(j=0;j<J;j++) setIntEdl(j,K,1, (Phi[j][K]-Phi[j+1][K])  );
	for(k=0;k<K;k++) setIntEdl(J,k,2, (Phi[J][k]-Phi[J][k+1])  );

}


//----------------------------------------------------------------------
//  Use the current rho and a poisson solver to correct the errors in the
//  irrotational E fields.  compute_rho and updateDivDerror should be called first.
//  Also initPhi should be called.
//  

void Fields::DivergenceClean(void)
{
	int j,k;

	if(!delPhi) {  // get space if this is the first call.
		delPhi= new Scalar *[J +1];
		for(j=0;j<=J;j++)
			delPhi[j] = new Scalar [K+1];
		for(j=0;j<=J;j++) for(k=0;k<=K;k++) delPhi[j][k]=0;
	}

	// get the phi we need
	psolve->solve(delPhi,DivDerror,5,presidue); // use the Poisson solver directly,
	//delPhi is actually returned as -delPhi

	// Now we need to perform the actual correction to the intEdl everywhere.

	for(j=0;j<J;j++) {
		for(k=0;k<K;k++) {
			setIntEdl(j,k,1,
					intEdl[j][k].e1() - (delPhi[j][k]-delPhi[j+1][k])   );
			setIntEdl(j,k,2,
					intEdl[j][k].e2() - (delPhi[j][k]-delPhi[j][k+1])   );
		}
	}
}

void Fields::InodeToI(void)
{
	int j,k;

	for(j=0;j<J;j++) {
		for(k=0;k<K;k++) {
			I[j][k] = 0.5*Vector3(
					((j==0)?2:1)*Inode[j][k].e1() + ((j==J-1)?2:1)*Inode[j+1][k].e1(),
					Inode[j][k].e2() + Inode[j][k+1].e2(),
					2*Inode[j][k].e3());
		}
	}
}

//---------------------------------------------------------------------
//  Do a marder correction pass.  Actual algorithm is based on
//  Bruce Langdon's paper "On Enforcing Gauss' Law"

void Fields::MarderCorrection(Scalar dt)
{
	int i,j,k;

	static Scalar** d=0;
	Vector2 **X=grid->getX();
	if(d==0) {

		d = new Scalar* [J+1];
		for(j=0;j<=J;j++)
		{
			d[j] = new Scalar[K+1];

			for(k=0;k<=K;k++) {
				if(j==J||k==K) d[j][k]=0;
				else

					d[j][k] = 0.5 * MarderParameter*iEPS0
					/( 1.0/sqr( X[j+1][k].e1()-X[j][k].e1())
							+1.0/sqr( X[j][k+1].e2()-X[j][k].e2()));
			}
		}
	}

	for(i=0;i<MarderIter;i++) {

		updateDivDerror();

		for(j=0;j<J;j++)
			for(k=0;k<K;k++) {

				setIntEdl(j,k,1,intEdl[j][k].e1() - (d[j+1][k]*DivDerror[j+1][k]
																			  - d[j][k]*DivDerror[j][k]));
				setIntEdl(j,k,2,intEdl[j][k].e2() - (d[j][k+1]*DivDerror[j][k+1]
																			- d[j][k]*DivDerror[j][k]));

			}
	}
}

void Fields::compute_iC_iL()
{
	Vector3** dl = grid->dl();
	Vector3** dlPrime = grid->dlPrime();
	Vector3** dS = grid->dS();
	Vector3** dSPrime = grid->dSPrime();
	Scalar epsjk, epsjmk, epsjkm, epsjmkm;

	for (int j=0; j<=J; j++)
		for (int k = 0; k<=K; k++){
			iL[j][k] = iMU0*dlPrime[j][k].jvDivide(dS[j][k]+Vector3(1e-30,1e-30,1e-30));
			epsjk = epsi[MIN(j,J-1)][MIN(k,K-1)];
			epsjmk = epsi[MAX(j-1, 0)][MIN(k,K-1)];
			epsjkm = epsi[MIN(j,J-1)][MAX(k-1, 0)];
			epsjmkm = epsi[MAX(j-1, 0)][MAX(k-1, 0)];
			iC[j][k] = dl[j][k].jvDivide(dSPrime[j][k].jvMult(
					Vector3(.5*(epsjk+epsjkm),.5*(epsjk+epsjmk),.25*(epsjk+epsjmk+epsjkm+epsjmkm))));
		}
}

// morgan
void Fields::set_up_inverter(Grid* grid)
/* throw(Oops) */{
	int nc1 = grid->getJ();
	int nc2 = grid->getK();

	// calculate uniform mesh widths for passing to Domain object
	Vector2 x;
	x = grid->getMKS(nc1,nc2);
	Scalar len1 = x.e1();
	Scalar len2 = x.e2();

	bool periodic1 = (grid->getPeriodicFlagX1() != 0);
	try{
		dom = new Domain(grid,len1,len2,nc1,nc2,periodic1);
	}
	catch(Oops& oops2){
		oops2.prepend("Fields::set_up_inverter: Error: \n"); //Fields::initPhi
		throw oops2;
	}

	poisson = new Electrostatic_Operator(dom,epsi);

} 



#ifdef HAVE_HDF5
int Fields::dumpH5(dumpHDF5 &dumpObj)
{

	// cerr << "Entered fields::dumpH5(dumpObj) .\n";

	hid_t fileId, groupId, datasetId,dataspaceId;
	herr_t status;
	hid_t scalarType = dumpObj.getHDF5ScalarType();
	hsize_t RANK  = 3;
	//  hsize_t     dimsf[3];              /* dataset dimensions */

	//  hssize_t start1[1]; /* Start of hyperslab */
	//  hsize_t stride1[1]; /* Stride of hyperslab */
	//  hsize_t count1[1];  /* Block count */
	//  hsize_t block1[1];  /* Block sizes */

	//  hssize_t start2[2]; /* Start of hyperslab */
	//  hsize_t stride2[2]; /* Stride of hyperslab */
	//  hsize_t count2[2];  /* Block count */
	//  hsize_t block2[2];  /* Block sizes */

	//  hssize_t start[3]; /* Start of hyperslab */
	//  hsize_t stride[3]; /* Stride of hyperslab */
	//  hsize_t count[3];  /* Block count */
	//  hsize_t block[3];  /* Block sizes */


	//       /* data to read */
	//    int         i, j, num;
	//    Scalar         val;

	//    Scalar temp;
	Vector2 **X = grid->getX();
	//  Scalar x,y;


	fileId= H5Fopen(dumpObj.getDumpFileName().c_str(), H5F_ACC_RDWR, H5P_DEFAULT);
	groupId = H5Gcreate(fileId,"Fields",0);

	if(!ElectrostaticFlag)
	{
		cout << "Dumping fields" << endl;
		cout << "J = " << J << endl;
		cout << "K = " << K << endl;

		hsize_t dim1 = 2;
		hsize_t dim3 = 3;
		//	hid_t posSlabId = H5Screate_simple(1, &dim1, NULL); // dimensions of position hyperslab
		//     * Create dataspace for the dataset in the file.
		hsize_t posDim[3];
		posDim[0] = J+1; posDim[1] = K+1; posDim[2] = 2;

		dataspaceId = H5Screate_simple(3, posDim, NULL);
		datasetId = H5Dcreate(groupId, "positions", scalarType, dataspaceId, H5P_DEFAULT);

		hsize_t fieldDim[3];
		fieldDim[0] = J+1; fieldDim[1] = K+1; fieldDim[2] = 3;
		//hid_t fieldSlabId = H5Screate_simple(1, &dim3, NULL);  // dimensions of field hyperslab

		hid_t EdlspaceId = H5Screate_simple(3, fieldDim, NULL);
		hid_t EdlsetId = H5Dcreate(groupId, "Edl", scalarType, EdlspaceId, H5P_DEFAULT);

		hid_t BdSspaceId = H5Screate_simple(3, fieldDim, NULL);
		hid_t BdSsetId = H5Dcreate(groupId, "BdS", scalarType, BdSspaceId, H5P_DEFAULT);

		hid_t IspaceId = H5Screate_simple(3, fieldDim, NULL);
		hid_t IsetId = H5Dcreate(groupId, "I", scalarType, IspaceId, H5P_DEFAULT);


		Scalar *posData = new Scalar[(J+1)*(K+1)*2]; //contiguous values needed


		for (int j=0; j<=J;j++)
			for (int k=0; k<=K;k++)
			{
				//   if(j*(K+1) +k  < 10){
				//  		cerr << X[j][k].e1()<<"\n";
				//  		cerr << X[j][k].e2()<<"\n";
				//  	      }

				posData[(K+1)*2*j + k*2]= X[j][k].e1();
				posData[(K+1)*2*j + k*2 + 1]= X[j][k].e2();
			}


		status = H5Dwrite(datasetId, scalarType, H5S_ALL,H5S_ALL , H5P_DEFAULT, posData);
		delete[] posData;

		Scalar *data = new Scalar[(J+1)*(K+1)*3];

		for (int j=0; j<=J;j++)
			for (int k=0; k<=K;k++)
			{
				data[(K+1)*3*j + k*3] = intEdl[j][k].e1();
				data[(K+1)*3*j + k*3 + 1] = intEdl[j][k].e2();
				data[(K+1)*3*j + k*3 + 2] = intEdl[j][k].e3();
			}

		status = H5Dwrite(EdlsetId, scalarType, H5S_ALL, EdlspaceId, H5P_DEFAULT, data);

		for (int j=0; j<=J;j++)
			for (int k=0; k<=K;k++)
			{
				data[(K+1)*3*j + k*3]= intBdS[j][k].e1();
				data[(K+1)*3*j + k*3 + 1]= intBdS[j][k].e2();
				data[(K+1)*3*j + k*3 + 2]= intBdS[j][k].e3();




			}
		status = H5Dwrite(BdSsetId, scalarType, H5S_ALL, BdSspaceId, H5P_DEFAULT, data);

		for (int j=0; j<=J;j++)
			for (int k=0; k<=K;k++)
			{
				data[(K+1)*3*j + k*3]= I[j][k].e1();
				data[(K+1)*3*j + k*3 + 1]= I[j][k].e2();
				data[(K+1)*3*j + k*3 + 2]= I[j][k].e3();
			}
		status = H5Dwrite(IsetId, scalarType, H5S_ALL, IspaceId, H5P_DEFAULT, data);



		status = H5Sclose(dataspaceId);
		status = H5Sclose(EdlspaceId);
		status = H5Sclose(BdSspaceId);
		status = H5Sclose(IspaceId);
		/*
		 * Close datasets.
		 */
		 status = H5Dclose(datasetId);
		 status = H5Dclose(EdlsetId);
		 status = H5Dclose(BdSsetId);
		 status = H5Dclose(IsetId);


		 // dump EM damping stuff

		 // write EMdamping flag as attribute

		 hsize_t     dims;
		 //	int         attrData;
		 hid_t  attrdataspaceId,attributeId;
		 dims = 1;
		 attrdataspaceId = H5Screate_simple(1, &dims, NULL);
		 attributeId = H5Acreate(groupId, "EMdampingFlag",scalarType, attrdataspaceId, H5P_DEFAULT);

		 status = H5Awrite(attributeId, scalarType, &EMdamping);
		 status = H5Aclose(attributeId);
		 status = H5Sclose(attrdataspaceId);



		 EdlspaceId = H5Screate_simple(3, fieldDim, NULL);
		 EdlsetId = H5Dcreate(groupId, "EdlBar", scalarType, EdlspaceId, H5P_DEFAULT);




		 if((EMdamping)>0)
			 for (int j=0; j<=J;j++)
				 for (int k=0; k<=K;k++) {

					 data[(K+1)*3*j + k*3] = intEdlBar[j][k].e1();
					 data[(K+1)*3*j + k*3 + 1] = intEdlBar[j][k].e2();
					 data[(K+1)*3*j + k*3 + 2] = intEdlBar[j][k].e3();
				 }

		 status = H5Dwrite(EdlsetId, scalarType, H5S_ALL, EdlspaceId, H5P_DEFAULT, data);
		 status = H5Sclose(EdlspaceId);
		 status = H5Dclose(EdlsetId);
	}
	/*
	 * Close the file.
	 */
	status = H5Gclose(groupId);
	status = H5Fclose(fileId);
	// dump boltzmann

	if (BoltzmannFlag)
		theBoltzmann->dumpH5(dumpObj);

	return(1);
}

#endif

int Fields::Dump(FILE *DMPFile)
{
	//   Scalar temp;
	Vector2 **X = grid->getX();
	Scalar x,y;
	//   char theDumpFileName[256];
	// char *fdname ={"fdmp"};

	XGWrite(&J,4,1,DMPFile,"int");
	XGWrite(&K,4,1,DMPFile,"int");

	if(!ElectrostaticFlag)
	{
		// ~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.
		/*
       #ifdef MPI_VERSION
           extern int MPI_RANK;
           sprintf(theDumpFileName,"%s.%d",fdname,MPI_RANK);
       #else
           sprintf(theDumpFileName,"%s",fdname);
       #endif
       ofstream dataField(theDumpFileName, ios::app);

		 */
		cout << "Dumping fields" << endl;
		cout << "J = " << J << endl;
		cout << "K = " << K << endl;
#ifdef MPI_VERSION
		extern int MPI_RANK;
		cout << "MPI rank " << MPI_RANK << endl;
#endif

		cout << "\nField positions:\n";

		for (int j=0; j<=J;j++)
			for (int k=0; k<=K;k++)
			{
				x = X[j][k].e1();
				y = X[j][k].e2();
				XGWrite(&x,ScalarInt,1,DMPFile,ScalarChar);
				XGWrite(&y,ScalarInt,1,DMPFile,ScalarChar);

				//E
				XGVector3Write(intEdl[j][k], DMPFile);
				//B
				XGVector3Write(intBdS[j][k], DMPFile);
				//Enode
				/*XGVector3Write(ENode[j][k], DMPFile); // not needed, recalculable.
                    //Bnode
                    XGVector3Write(BNode[j][k], DMPFile); */		  
				//J
				XGVector3Write(I[j][k], DMPFile);
				/*
	          //TEXT
		  dataField << x <<"  "<< y <<"  "<< ENode[j][k].e1() <<"  "<<
                           ENode[j][k].e2()<<"  "<<ENode[j][k].e3()<<endl;
				 */
			}
		//        dataField.close();
		XGWrite(&EMdamping, ScalarInt, 1, DMPFile, ScalarChar);
		if((EMdamping)>0)
			for (int j=0; j<=J;j++)
				for (int k=0; k<=K;k++) {
					x = X[j][k].e1();
					y = X[j][k].e2();

					XGWrite(&x,ScalarInt,1,DMPFile,ScalarChar);
					XGWrite(&y,ScalarInt,1,DMPFile,ScalarChar);

					XGVector3Write(intEdlBar[j][k], DMPFile);
				}
	}
	if (BoltzmannFlag)
		theBoltzmann->Dump(DMPFile);

	// ~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.~.

	return (1);

}

int Fields::Restore_2_00(FILE *DMPFile)
{
	Scalar temp;

	if(!ElectrostaticFlag)
	{
		// restoring fields
		for (int j=0; j<=J;j++)
			for (int k=0; k<=K;k++)
			{
				//E
				XGVector3Read(intEdl[j][k], DMPFile);
				//B
				XGVector3Read(intBdS[j][k], DMPFile);
				//J
				XGVector3Read(I[j][k], DMPFile);

			}
		XGRead(&temp, ScalarInt, 1, DMPFile, ScalarChar);
		if(temp>0){
			if(EMdamping==temp)
				for (int j=0; j<=J;j++)
					for (int k=0; k<=K;k++)
						XGVector3Read(intEdlBar[j][k], DMPFile);
			else if(EMdamping>=0)
			{
				cout << "**********************warning**********************" << endl;
				cout << "EMdamping value has been changed, using new value." << endl;
				cout << "Keeping old weighted average." << endl;
				for (int j=0; j<J;j++)
					for (int k=0; k<K;k++)
						XGVector3Read(intEdlBar[j][k], DMPFile);
			}
		}
		else if (EMdamping != temp){
			cout << "**********************warning**********************" << endl;
			cout << "EMdamping value has been turned on, using damping." << endl;
			cout << "Weighted average is beening started with the instantaneous" << endl;
			cout << "value of the fields." << endl;
			for (int j=0; j<=J;j++)
				for (int k=0; k<=K;k++)
					intEdlBar[j][k] = intEdl
					[j][k];
		}
	}

	if (BoltzmannFlag)
		theBoltzmann->Restore(DMPFile);

	return(1);
}


#ifdef HAVE_HDF5
int Fields::restoreH5(dumpHDF5 &restoreObj)
{

	//cerr << "entering Fields::restoreH5()\n";

#ifdef MPI_VERSION
	extern int MPI_RANK;
	//  cerr << " ,MPI_RANK "<<MPI_RANK<<"   "<<restoreObj.getDumpFileName()<<"\n";

#endif

	hid_t fileId, groupId, datasetId,dataspaceId;
	herr_t status;
	string outFile;
	//  hsize_t rank;
	//  hsize_t     dimsf[1];
	//  hsize_t dims;
	hid_t scalarType = restoreObj.getHDF5ScalarType();
	//  hid_t  attrspaceId,
	hid_t attrId;
	//  hid_t bMetaGroupId;
	//  hssize_t offset[2]; /* Offset of hyperslab */
	//  hsize_t count[2];  /* Block count */
	//  hssize_t offsetFile[3]; /* Offset of hyperslab */
	//  hsize_t countFile[3];  /* Block count */
	//  int EMdampingFlag;
	int jl,kl,Jl,Kl;
	//Scalar data,dataE,dataB,dataI;

	fileId = H5Fopen(restoreObj.getDumpFileName().c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);

	groupId = H5Gopen(fileId, "/Fields");


	if(!ElectrostaticFlag) {

		//get size of Fields:
		datasetId = H5Dopen(groupId,"positions");
		dataspaceId = H5Dget_space(datasetId );
		int dataRank = H5Sget_simple_extent_ndims(dataspaceId);
		hsize_t *sizes = new hsize_t[dataRank];
		int res = H5Sget_simple_extent_dims(dataspaceId, sizes, NULL);
		Jl = sizes[0] -1;
		Kl = sizes[1] - 1; //the -1 is to set the max index

		int third  = sizes[2];


		hsize_t dim2 = 2;
		// get dataspaces fro fields


		hid_t EdlsetId = H5Dopen(groupId, "Edl");
		hid_t EdlspaceId = H5Dget_space(EdlsetId);
		dataRank = H5Sget_simple_extent_ndims(EdlspaceId);
		delete [] sizes;
		sizes = new hsize_t[dataRank];
		res = H5Sget_simple_extent_dims(EdlspaceId, sizes, NULL);

		hid_t BdSsetId = H5Dopen(groupId, "BdS");
		hid_t BdSspaceId = H5Dget_space(BdSsetId);

		hid_t IsetId = H5Dopen(groupId, "I");
		hid_t IspaceId = H5Dget_space(IsetId);


		Scalar *pos = new Scalar[(Jl+1)*(Kl+1)*2];
		status = H5Dread(datasetId, scalarType,H5S_ALL, dataspaceId, H5P_DEFAULT, pos);


		Scalar *data = new Scalar[(Jl+1)*(Kl+1)*3];
		status = H5Dread(EdlsetId, scalarType,H5S_ALL, EdlspaceId, H5P_DEFAULT, data);

		Scalar *BdSdata = new Scalar[(Jl+1)*(Kl+1)*3];
		status = H5Dread(BdSsetId, scalarType,H5S_ALL, BdSspaceId, H5P_DEFAULT, BdSdata);

		Scalar *Idata = new Scalar[(Jl+1)*(Kl+1)*3];
		status = H5Dread(IsetId, scalarType,H5S_ALL, IspaceId, H5P_DEFAULT, Idata);


		//ofstream out;
		int compNum = 3;
		int maxNum = (Jl+1)*(Kl+1);

		for (jl=0; jl<=Jl;jl++){
			for (kl=0; kl<=Kl;kl++)
			{
				Scalar A1,A2;  // coordinates of the fields read in
				Vector2 Xloc;
				Vector3 E,B,EN,BN,Il;

				A1 = pos[(Kl+1)*2*jl + kl*2];
				A2 = pos[(Kl+1)*2*jl + kl*2 + 1];
				Xloc = grid->getNearestGridCoords(Vector2(A1,A2));



				if(Xloc.e1() >= 0) // valid location
				{
					int ji,ki;
					// the following operations make sure
					// stuff doesn't get rounded down when
					// converted from float to int
					ji = (int)(Xloc.e1()+0.5);
					ki = (int)(Xloc.e2()+0.5);

					intEdl[ji][ki].set_e1(data[(Kl+1)*3*jl + kl*3]);
					intEdl[ji][ki].set_e2(data[(Kl+1)*3*jl + kl*3 + 1]);
					intEdl[ji][ki].set_e3(data[(Kl+1)*3*jl + kl*3 + 2]);

					intBdS[ji][ki].set_e1(BdSdata[(Kl+1)*3*jl + kl*3]);
					intBdS[ji][ki].set_e2(BdSdata[(Kl+1)*3*jl + kl*3 + 1]);
					intBdS[ji][ki].set_e3(BdSdata[(Kl+1)*3*jl + kl*3 + 2]);

					I[ji][ki].set_e1(Idata[(Kl+1)*3*jl + kl*3]);
					I[ji][ki].set_e2(Idata[(Kl+1)*3*jl + kl*3 + 1]);
					I[ji][ki].set_e3(Idata[(Kl+1)*3*jl + kl*3 + 2]);
				}
			}
		}


		delete[] BdSdata;
		delete[] Idata;

		status = H5Sclose(BdSspaceId);
		status = H5Dclose(BdSsetId);
		status = H5Sclose(IspaceId);
		status = H5Dclose(IsetId);
		status =    H5Sclose(dataspaceId);
		status =    H5Dclose(datasetId);
		status = H5Sclose(EdlspaceId);
		status = H5Dclose(EdlsetId);

		// Read the EM damping flag attribute

		attrId = H5Aopen_name(groupId, "EMdampingFlag");
		Scalar EMdampingFlag;
		status = H5Aread(attrId,scalarType ,&EMdampingFlag);
		status = H5Aclose(attrId);

		if(EMdampingFlag>0){

			EdlsetId = H5Dopen(groupId,"EdlBar" );
			EdlspaceId = H5Dget_space(EdlsetId );
			status = H5Dread(EdlsetId, scalarType,H5S_ALL, EdlspaceId, H5P_DEFAULT, data);


			// read in the EMDamping stuff
			for (jl=0; jl<=Jl;jl++)
				for (kl=0; kl<=Kl;kl++)
				{
					Scalar A1,A2;  // coordinates of the fields read in
					Vector2 Xloc;
					Vector3 Emd;
					A1 = pos[(Kl+1)*2*jl + kl*2];
					A2 = pos[(Kl+1)*2*jl + kl*2 + 1];

					// find the grid locations here
					Xloc = grid->getNearestGridCoords(Vector2(A1,A2));
					//Emd
					if(Xloc.e1() >= 0) // valid location
					{
						int ji,ki;
						// the following operations make sure
						// stuff doesn't get rounded down when
						// converted from float to int
						ji = (int)(Xloc.e1()+0.5);
						ki = (int)(Xloc.e2()+0.5);

						intEdlBar[ji][ki].set_e1(data[(Kl+1)*3*jl + kl*3]);
						intEdlBar[ji][ki].set_e2(data[(Kl+1)*3*jl + kl*3 + 1]);
						intEdlBar[ji][ki].set_e3(data[(Kl+1)*3*jl + kl*3 + 2]);
					}
				}

			// if we've just turned EMDamping on.
			if (EMdamping != EMdampingFlag){
				cout << "**********************warning**********************" << endl;
				cout << "EMdamping value has been turned on, using damping." << endl;
				cout << "Weighted average is beening started with the instantaneous" << endl;
				cout << "value of the fields." << endl;
				for (int j=0; j<=J;j++)
					for (int k=0; k<=K;k++)
						intEdlBar[j][k] = intEdl[j][k];
			}

			status = H5Sclose(EdlspaceId);
			status = H5Dclose(EdlsetId);

		}
		delete [] sizes;
	}
	H5Gclose(groupId);
	H5Fclose(fileId);
	//    cerr <<"exiting fields::rstoreH5.\n";
	return(1);
}
#endif


int Fields::Restore(FILE *DMPFile)
{
	Scalar temp;
	int Jl,Kl;  // dimensions of the data in the dump file
	int jl,kl;  // indices over the data in the dump file


	XGRead(&Jl,4,1,DMPFile,"int");
	XGRead(&Kl,4,1,DMPFile,"int");

	if(!ElectrostaticFlag)
	{
		cout << "Restoring Fields:" << endl;
		cout << "Jl = " << Jl << endl;
		cout << "Kl = " << Kl << endl;

		// restoring fields
		for (jl=0; jl<=Jl;jl++)
			for (kl=0; kl<=Kl;kl++)
			{
				Scalar A1,A2;  // coordinates of the fields read in
				Vector2 Xloc;
				Vector3 E,B,EN,BN,Il;


				// read in the coordinates
				XGRead(&A1,ScalarInt,1,DMPFile,ScalarChar);
				XGRead(&A2,ScalarInt,1,DMPFile,ScalarChar);

				// find the grid locations here
				Xloc = grid->getNearestGridCoords(Vector2(A1,A2));


				//E
				XGVector3Read(E, DMPFile);
				//B
				XGVector3Read(B, DMPFile);
				/*		  //ENode
                          XGVector3Read(EN, DMPFile);  // not needed, recalculable.
                          //BNode
                          XGVector3Read(BN, DMPFile); */
				//J
				XGVector3Read(Il, DMPFile);
				if(Xloc.e1() >= 0) // valid location
				{
					int ji,ki;
					// the following operations make sure
					// stuff doesn't get rounded down when
					// converted from float to int
					ji = (int)(Xloc.e1()+0.5);
					ki = (int)(Xloc.e2()+0.5);

					intEdl[ji][ki]=E;
					intBdS[ji][ki]=B;
					/*ENode[ji][ki]=EN;
                  BNode[ji][ki]=BN; */ // Not needed: recalculable.
					I[ji][ki]=Il;
				}
			}
		XGRead(&temp, ScalarInt, 1, DMPFile, ScalarChar);
		if(temp>0){

			// red in the EMDamping stuff
			for (jl=0; jl<=Jl;jl++)
				for (kl=0; kl<=Kl;kl++)
				{
					Scalar A1,A2;  // coordinates of the fields read in
					Vector2 Xloc;
					Vector3 Emd;

					// read in the coordinates
					XGRead(&A1,ScalarInt,1,DMPFile,ScalarChar);
					XGRead(&A2,ScalarInt,1,DMPFile,ScalarChar);



					// find the grid locations here
					Xloc = grid->getNearestGridCoords(Vector2(A1,A2));

					//Emd
					XGVector3Read(Emd, DMPFile);
					if(Xloc.e1() >= 0) // valid location
					{
						int ji,ki;
						// the following operations make sure
						// stuff doesn't get rounded down when
						// converted from float to int
						ji = (int)(Xloc.e1()+0.5);
						ki = (int)(Xloc.e2()+0.5);

						intEdlBar[ji][ki]=Emd;
					}
				}


			// if we've just turned EMDamping on.
			if (EMdamping != temp){
				cout << "**********************warning**********************" << endl;
				cout << "EMdamping value has been turned on, using damping." << endl;
				cout << "Weighted average is beening started with the instantaneous" << endl;
				cout << "value of the fields." << endl;
				for (int j=0; j<=J;j++)
					for (int k=0; k<=K;k++)
						intEdlBar[j][k] = intEdl[j][k];
			}
		}
	}

	if (BoltzmannFlag)
		theBoltzmann->Restore(DMPFile);

	return(1);
}


//
// Routines for moving fields around:  left, right, down, or up.
//

void Fields::sendFieldsAndShift(int dir, Boundary* bPtr){
	//  int jp1; // temporary variable...
	// Send cells over.  (JRC 22 Jan 00)
	// This should send j=1/2 and j=1
	if(bPtr) bPtr->sendStripe(dir);

	//
	// Shift fields.  We do this first because the before-shift
	// edge cells were already sent over at the field solve.
	// I am not sure that this is correct.
	//
	switch(dir) {

	// Shift fields to the left
	case 1:
	{
		for(int j=0;j<J;j++) {
			for(int k=0;k<=K;k++) {
				ENode[j][k] = ENode[j+1][k];
				BNode[j][k] = BNode[j+1][k];
				BNodeDynamic[j][k] = BNodeDynamic[j+1][k];
				BNodeStatic[j][k] = BNodeStatic[j+1][k];
				I[j][k] = I[j+1][k];
				intEdl[j][k] = intEdl[j+1][k];
				intBdS[j][k] = intBdS[j+1][k];
				if(accumulator)
					accumulator[j][k] = accumulator[j+1][k];

				// stuff added by Bruhwiler (11/08/99) in attempt to prevent
				// moving window algorithm from crashing emdamping algorithm
				intEdlBar[j][k] = intEdlBar[j+1][k];
				intEdlPrime[j][k] = intEdlPrime[j+1][k];
				intEdlBasePtr[j][k] = intEdlBasePtr[j+1][k];
				Inode[j][k] = Inode[j+1][k];
				rho[j][k] = rho[j+1][k];
				backGroundRho[j][k] = backGroundRho[j+1][k];
				DivDerror[j][k] = DivDerror[j+1][k];
				Phi[j][k] = Phi[j+1][k];
				PhiP[j][k] = PhiP[j+1][k];
				Charge[j][k] = Charge[j+1][k];
			}
		}
	}
	break;

	// Shift fields to the right
	case 2:
	{

		for(int j=J;j>1;j--) {
			for(int k=0;k<=K;k++) {
				ENode[j][k] = ENode[j-1][k];
				BNode[j][k] = ENode[j-1][k];
				BNodeDynamic[j][k] = BNodeDynamic[j-1][k];
				BNodeStatic[j][k] = BNodeStatic[j-1][k];
				I[j][k] = I[j-1][k];
				intEdl[j][k] = intEdl[j-1][k];
				intBdS[j][k] = intBdS[j-1][k];
				if(accumulator)
					accumulator[j][k] = accumulator[j-1][k];
			}
		}
	}
	break;

	// Shift fields upward (smaller k)
	case 3:
	{
		for(int j=0;j<=J;j++) {
			for(int k=0;k<K;k++) {
				ENode[j][k] = ENode[j][k+1];
				BNode[j][k] = ENode[j][k+1];
				BNodeDynamic[j][k] = BNodeDynamic[j][k+1];
				BNodeStatic[j][k] = BNodeStatic[j][k+1];
				I[j][k] = I[j][k+1];
				intEdl[j][k] = intEdl[j][k+1];
				intBdS[j][k] = intBdS[j][k+1];
				if(accumulator)
					accumulator[j][k] = accumulator[j][k+1];
			}
		}
	}
	break;

	// Shift fields downward (larger k)
	case 4:
	{
		for(int j=0;j<=J;j++) {
			for(int k=K;k>0;k--) {
				ENode[j][k] = ENode[j][k-1];
				BNode[j][k] = ENode[j][k-1];
				BNodeDynamic[j][k] = BNodeDynamic[j][k-1];
				BNodeStatic[j][k] = BNodeStatic[j][k-1];
				I[j][k] = I[j][k-1];
				intEdl[j][k] = intEdl[j][k-1];
				intBdS[j][k] = intBdS[j][k-1];
				if(accumulator)
					accumulator[j][k] = accumulator[j][k-1];
			}
		}
	}
	break;

	}

}

void Fields::recvShiftedFields(int dir, Boundary* bPtr,Scalar t, Scalar dt) {

	// There must be some kind of boundary
	assert(bPtr);

	// Receive a new stripe of fields from the boundary.
	bPtr->recvStripe(dir);

	// Load in any new fields
	loadShiftedFields(t, dt);

}

//
// Load in any fields coming from shifted boundary
//
void Fields::loadShiftedFields(Scalar t,Scalar dt) {
	ApplyToList(applyShiftBC(t,dt),*boundaryList,Boundary);
}


//--------------------------------------------------------------------
//	+= an  initial magnetic field as a function of x1,x2

void Fields::setBinit(const ostring &B1init, const ostring &B2init, 
		const ostring &B3init) {
	//   int status;
	ostring B01a = B1init;
	ostring B02a = B2init;
	ostring B03a = B3init;
	// need extra variables for computing fields at half-cells.
	Scalar x1,x2;  // These can now be Scalar -- RT, 2003/12/09
	Scalar x1a,x2a,x1b,x2b,x1s,x2s;
	Scalar Bx1,Bx2,Bx3;
	adv_eval->add_indirect_variable("x1",&x1);
	adv_eval->add_indirect_variable("x2",&x2);
	B01a+='\n';
	B02a+='\n';
	B03a+='\n';
	for(int j=0;j<=J;j++)
		for(int k=0;k<=K;k++) {
			x1s = grid->getMKS(j,k).e1();
			x2s = grid->getMKS(j,k).e2();
			x1a = grid->getMKS(MAX(j-1,0),k).e1();
			x1b = grid->getMKS(MIN(j+1,J),k).e1();
			x2a = grid->getMKS(j,MAX(k-1,0)).e2();
			x2b = grid->getMKS(j,MIN(k+1,K)).e2();

			// get the intBdS location, and the field.

			x2 = 0.5*(x2b + x2s); 		x1 = x1s;
			Bx1 = adv_eval->Evaluate(B01a.c_str()) * grid->dS1(j,k);

			x1 = 0.5*(x1b + x1s); 		x2 = x2s;
			Bx2 = adv_eval->Evaluate(B02a.c_str()) * grid->dS2(j,k);

			x2 = 0.5*(x2b + x2s); 		x1 = 0.5*(x1b + x1s);
			Bx3 = adv_eval->Evaluate(B03a.c_str()) * grid->dS3(j,k);
			intBdS[j][k] += Vector3(Bx1, Bx2, Bx3);
		}

}


//--------------------------------------------------------------------
//	+= an  initial electric field as a function of x1,x2


void Fields::setEinit(const ostring &E1init,const ostring &E2init, const ostring &E3init)
{
	//   int status;
	ostring E01a = E1init;
	ostring E02a = E2init;
	ostring E03a = E3init;
	// need extra variables for computing fields at half-cells.
	Scalar x1,x2;  // These can now be Scalar -- RT, 2003/12/09
	Scalar x1a,x2a,x1b,x2b,x1s,x2s;
	Scalar Ex1,Ex2,Ex3;
	adv_eval->add_indirect_variable("x1",&x1);
	adv_eval->add_indirect_variable("x2",&x2);
	E01a+='\n';
	E02a+='\n';
	E03a+='\n';
	for(int j=0;j<=J;j++)
		for(int k=0;k<=K;k++) {
			x1s = grid->getMKS(j,k).e1();
			x2s = grid->getMKS(j,k).e2();
			x1a = grid->getMKS(MAX(j-1,0),k).e1();
			x1b = grid->getMKS(MIN(j+1,J),k).e1();
			x2a = grid->getMKS(j,MAX(k-1,0)).e2();
			x2b = grid->getMKS(j,MIN(k+1,K)).e2();

			// get the intEdL location, and the field.
			x1 = 0.5*(x1b + x1s); 		x2 = x2s;
			Ex1 = adv_eval->Evaluate(E01a.c_str()) * grid->dl1(j,k);

			x2 = 0.5*(x2b + x2s); 		x1 = x1s;
			Ex2 = adv_eval->Evaluate(E02a.c_str()) * grid->dl2(j,k);

			x2 = x2s; 		x1 = x1s;
			Ex3 = adv_eval->Evaluate(E03a.c_str()) * grid->dl3(j,k);
			intEdl[j][k] += Vector3(Ex1, Ex2, Ex3);
		}

}

void Fields::radialCurrentCorrection()
{
	for (int j=0; j<=J; j++) {
		I[j][0] *= grid->get_radial0();
		I[j][K] *= grid->get_radialK();
	}
}
