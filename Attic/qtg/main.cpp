#include "main.h"
#include "Information.h"

SpatialRegion *theSpace;
PlasmaDevice *theDevice;
AdvisorManager *theAdvisor;
SpatialRegionList *theRegions;
extern oopicList<Diagnostics> *theDiagnostics;

int electrostaticFlag;
int number_of_species;
Information *inform; 
extern "C++" void printf(char *);
#ifdef MPI_VERSION
#include <mpi.h>
int MPI_RANK;
MPI_Comm XOOPIC_COMM;
MPI_Group MPI_GROUP_WORLD;
int GlobalArgc;
char **GlobalArgv;
#endif

int main(int argc, char **argv)
{

#ifdef MPI_VERSION
	GlobalArgc = argc; GlobalArgv = argv;
	MPI_Init(&argc,&argv);
	MPI_Comm_group(MPI_COMM_WORLD,&MPI_GROUP_WORLD);
	MPI_Comm_create(MPI_COMM_WORLD,MPI_GROUP_WORLD,&XOOPIC_COMM);
	MPI_Comm_rank(XOOPIC_COMM, &MPI_RANK);
#endif

	QApplication::setPalette( QPalette( Qt::lightGray ),TRUE );
	QApplication qtapp(argc,argv);
	inform = new Information();
	inform->show();
	inform->move(0,610);
	inform->updateGeometry();

	printf("\nStarting QtOOPIC version 1.0 ");
	printf("XOOPIC (c) Copyright 1995-98 The Regents of the University of California.");
	printf("Plasma Theory and Simulation Group");
	printf("University of California at Berkeley");
	printf("http://ptsg.eecs.berkeley.edu");

	/**
  inform->write("\nStarting QtOOPIC version 1.0 \n");
  inform->write("XOOPIC (c) Copyright 1995-98 The Regents of the University of California.");
  inform->write("Plasma Theory and Simulation Group");
  inform->write("University of California at Berkeley");
  inform->write("http://ptsg.eecs.berkeley.edu");
	 */

	//  Initializations

	printf("\nStarting program. ");
	printf("Reading input file. ");

	/**
  inform->write("\nStarting program. ");
  inform->write("Reading input file. ");
	 */
	XGInit(argc,argv,&simulation_time);

	theAdvisor= new AdvisorManager(theInputFile);

	printf("\nAdvisorManager::Check rules()");
	// inform->write("\nAdvisorManager::Check rules()");
	theAdvisor->checkRules();

	if(theAdvisor->GetErrorMessage().length() > 0) return 1;

	printf("AdvisorManager::createDevice()");
	// inform->write("AdvisorManager::createDevice()");
	theRegions = theAdvisor->createDevice();

	delete theAdvisor;  //don't need it anymore

	theDevice = new PlasmaDevice(theRegions);
	theDiagnostics = new oopicList<Diagnostics>;
	theSpace = theRegions->head->data;
	//theDevice->setParticleLimit(theRegions->head->data->getParticleLimit());

	dt = theRegions->head->data->get_dt();

#ifndef BENCHMARK
	printf("\nEntering Set_diags... ");
	// inform->write("Entering Set_diags... ");

	oopicListIter <SpatialRegion> Siter(*theRegions);
	for(Siter.restart();!Siter.Done();Siter++)
		theDiagnostics->add(new Diagnostics(Siter.current()));
	electrostaticFlag = theRegions->head->data->getElectrostaticFlag();
	number_of_species = theRegions->head->data->get_nSpecies();



	printf("Entering InitWin..");
	// inform->write("\nEntering InitWin..");
	//  if(theRunWithXFlag)
	oopicListIter <Diagnostics> Diter(*theDiagnostics);
	for(Diter.restart();!Diter.Done();Diter++) {
		Diter.current()->InitWin();
		//		Diter.current()->Update();
		Diter.current()->UpdatePreDiagnostics();
	}

	for(Siter.restart();!Siter.Done();Siter++) {
		// check all diagnostics and write initial state/create directories/files
//		Siter.current()->;
	}


	XGStart();
	CreateWindows *cl;
	// cl = new CreateWindows();
	cl = new CreateWindows(numberOfWindows,  theWindowArray);

	TxqMainWin(qApp, argc, argv, cl);


#else

	/**
  int i = 200;
  // if(argc==3) { i=atoi(argv[2]);}
  printf("\nRunning for %d timesteps\n",i);
  oopicListIter <Diagnostics> Diter(*theDiagnostics);
  for(Diter.restart();!Diter.Done();Diter++)
		Diter.current()->Update();
  while(i--) XGMainLoop();
	 */
	// TxqRun();
	write_validation();
	Quit();  // belongs here, xgrafix calls this itself.
#endif  // BENCHMARK

}

/*
int qMain(int argc, char **argv) {
  return thismain(argc,argv);
}
 */
