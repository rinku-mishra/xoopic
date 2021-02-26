#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

// commented by JK, 2016-03-01; on GCC 5.3.1 complains that this is deprecated include file
//#include <txc_streams.h>
using namespace std;

#include <cstdlib>
#include <cmath>

#include <plsmadev.h>
#include <advisman.h>
#include <sptlrgn.h>
#include <ptclgrp.h>
#include "diagn.h"
extern Scalar nfft;
#include "globalvars.h"
#include "ostring.h"
#include "oops.h"

extern "C" {
#include <math.h>
#include <stdio.h>
//#include <malloc/malloc.h>
void Quit(void);
void XGMainLoop(void);
#include <xgmini.h>
}

#ifdef MPI_VERSION
#include <mpi.h>
#endif

void Physics(void);
void InitSim(void);
#ifdef BENCHMARK
void write_validation();
#endif

SpatialRegion *theSpace;
PlasmaDevice *theDevice;
AdvisorManager *theAdvisor;
SpatialRegionList *theRegions;
extern oopicList<Diagnostics> *theDiagnostics;

int electrostaticFlag;
int number_of_species;

#ifdef MPI_VERSION
int MPI_RANK;
MPI_Comm XOOPIC_COMM;
MPI_Group MPI_GROUP_WORLD;
int *GlobalArgc;
char ***GlobalArgv;
#endif

extern "C" {
int PetscInitialize(int *,char ***,char *, char []);
}

int main (int argc,char **argv)
{
	printf("\nStarting XOOPIC version 2.70");
	printf("(c) Copyright 1995-2010 The Regents of the University of California.");
	printf("Plasma Theory and Simulation Group");
	printf("University of California at Berkeley");
	printf("http://ptsg.eecs.berkeley.edu");

	//  Initializations

#ifdef MPI_VERSION
	GlobalArgc = &argc; GlobalArgv = &argv;
	MPI_Init(&argc,&argv);
#ifdef PETSC
	PetscInitialize(GlobalArgc,GlobalArgv,(char *)0,"RTFM");
#endif
	MPI_Comm_group(MPI_COMM_WORLD,&MPI_GROUP_WORLD);
	MPI_Comm_create(MPI_COMM_WORLD,MPI_GROUP_WORLD,&XOOPIC_COMM);
	MPI_Comm_rank(XOOPIC_COMM, &MPI_RANK);
#endif

	printf("\nStarting program. ");
	printf("Reading input file. ");
	XGInit(argc,argv,&simulation_time);

	try {
		theAdvisor= new AdvisorManager(theInputFile);
	}
	catch(Oops& oopsAll){
		cout << "\nError creating the AdvisorManager\n  "
				<< oopsAll.getMessage()
				<< "\n\nPlease check your input file." << endl;
		Quit();
		return 1;
	}

	cout << "\nCalling AdvisorManager::checkRules().\n";
	theAdvisor->checkRules();

	if (theAdvisor->GetErrorMessage().length() > 0 || theAdvisor->spatialRegionGroupListIsEmpty())
	{
		printf("Fatal errors encountered! Please check the input file!\n");
		Quit();
		return 1;
	}

	try {
		theRegions = theAdvisor->createDevice();
	}
	catch(Oops& oopsAll){
		cout << "\nAn internal error occurred.\n  "
				<< oopsAll.getMessage()
				<< "\n\nPlease check your input file." << endl;
		Quit();
		return 1;
	}

	delete theAdvisor;  //don't need it anymore

	theDevice = new PlasmaDevice(theRegions);
	theDiagnostics = new oopicList<Diagnostics>;
	theSpace = theRegions->headData();
	//theDevice->setParticleLimit(theRegions->headData()->getParticleLimit());

	dt = theRegions->headData()->get_dt();
	simulation_time = theRegions->headData()->getTime();

#ifndef BENCHMARK
	cout << "\nEntering Set_diags...\n";
	oopicListIter <SpatialRegion> Siter(*theRegions);
	for(Siter.restart();!Siter.Done();Siter++)
		theDiagnostics->add(new Diagnostics(Siter.current()));
	electrostaticFlag = theRegions->headData()->getElectrostaticFlag();
	number_of_species = theRegions->headData()->get_nSpecies();
	cout << "Entering InitWin...\n";
	//  if(theRunWithXFlag)
	oopicListIter <Diagnostics> Diter(*theDiagnostics);
	try {
		for(Diter.restart();!Diter.Done();Diter++) {
			Diter.current()->InitWin();
			//		Diter.current()->Update();
			Diter.current()->UpdatePreDiagnostics();
		}
	}
	catch(Oops& oopsDiag){
		cout << "\nA diagnostic error occurred.\n  "
				<< oopsDiag.getMessage()
				<< "\n\nPlease check your input file." << endl;
		Quit();
		return 1;
	}
	XGStart();
#else

	int i = 200;
	if(argc==3) { i=atoi(argv[2]);}
	printf("\nRunning for %d timesteps\n",i);
	oopicListIter <Diagnostics> Diter(*theDiagnostics);
	try {
		for(Diter.restart();!Diter.Done();Diter++)
			Diter.current()->Update();
	}
	catch(Oops& oopsDiag){
		cout << "\nA diagnostic error occurred.\n  "
				<< oopsDiag.getMessage()
				<< "\n\nPlease check your input file." << endl;
		Quit();
		return 1;
	}
	while(i--) XGMainLoop();
	write_validation();
	Quit();  // belongs here, xgrafix calls this itself.
#endif  // BENCHMARK

	return 0;
}

void printf(char * input) {
	// overload printf so that qtoopic can overload to print inside
	//  a Qt diagnostic window

	if (input[strlen(input)-1] == '\n')
		cout << input << flush;
	else cout << input << endl;
}

// Overload even more so the previous function is actually useful.
// This function prints to a non-const string which is then sent
// to previous function.
#include <stdarg.h>
int printf(const char * input,...) {
	va_list ap;
	char buf[512];

	va_start(ap,input);
	vsprintf(buf,input,ap); // convert to non-const string
	va_end(ap);
	printf(buf); // print that string
	return 0;
}

// Overload puts too 
int puts(const char *s) { printf((char*)s); return 0; }
