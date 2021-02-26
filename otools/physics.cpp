#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if defined(_MSC_VER)
#include <iomanip>
#include <iostream>
using std::ends;
#else

#include <stdlib.h>
using namespace std;
#endif

#include <cmath>

#include <oopiclist.h>
#include <plsmadev.h>
#include <sptlrgn.h>
#include <ptclgrp.h>

//global variables!
extern PlasmaDevice *theDevice;
extern SpatialRegion *theSpace;
extern int electrostaticFlag;
Scalar dt;

// sim_time has to be double b/c of the GUI
double simulation_time = 0;

void Physics(void)
{
    if (theDevice) simulation_time = theDevice->AdvancePIC();
}

