extern "C++" void Physics(void);
extern "C++" void Maintain_Diagnostics();
// extern "C" void XGMainLoop();
#include "ovector.h"
#include "oopiclist.h"
#include "diagn.h"
extern oopicList<Diagnostics> *theDiagnostics;

// #define TIMING 1

#ifdef TIMING
extern "C" {
#include <sys/time.h>
}
double Time0=-1;
long int TimeIndex = 0;
#endif

// extern double simulation_time;

void XGMainLoop()
{

#ifdef TIMING
	struct timeval TheTime;
	double TimeUsed;

	gettimeofday(&TheTime,0);

	// initialize the first time
	if(Time0==-1)  Time0 = 1.0e-6*(double)TheTime.tv_usec + (double)TheTime.tv_sec;

	TimeUsed =  1.0e-6*(double)TheTime.tv_usec + (double)TheTime.tv_sec - Time0;

	// fprintf(stderr,"\n%ld %f",TimeIndex,(float)TimeUsed);
	TimeIndex++;

#endif TIMING

	// fprintf(stderr,"\n Simulation time: %g",simulation_time);
	Physics();  // fields then particles
	{
		oopicListIter <Diagnostics> Diter(*theDiagnostics);
		for(Diter.restart();!Diter.Done();Diter++)
			Diter.current()->UpdatePreDiagnostics();
	}
	{
		oopicListIter <Diagnostics> Diter(*theDiagnostics);
		for(Diter.restart();!Diter.Done();Diter++)
			Diter.current()->UpdatePostDiagnostics();
	}


}

void taskYield()
{}
