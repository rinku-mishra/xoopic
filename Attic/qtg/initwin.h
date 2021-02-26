//  This initializes the windows for diagnostics.

#define True 1
#define False 0
 
// Standard include files

#include <iostream.h>
#include <stdlib.h>
#include "oopiclist.h"
#include "ovector.h"
#include "diagn.h"
#include "sptlrgn.h"
#include "ptclgrp.h"
#include "globalvars.h"
#include "boundary.h"
#include "gpdist.h"
#include "history.h"
 
// External C include files

extern "C" {
#include <math.h>
#include <stdio.h>
//#include <malloc/malloc.h>
#ifdef UNIX        // DLB 10-09-98
#include <unistd.h>
#endif
#include <xgrafix.h>
}
 
// External definitions

/*
extern "C" WindowType *theWindowArray;
extern "C" int numberOfWindows;
*/
 
