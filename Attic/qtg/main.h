// #include <iostream.h>
#include <stdlib.h>
 
 
#include <plsmadev.h>
#include <advisman.h>
#include <sptlrgn.h>
#include <ptclgrp.h>
#include "diagn.h"
#include "globalvars.h"
#include "ostring.h"
 
extern "C" {
#include <math.h>
// #include <stdio.h>
//#include <malloc/malloc.h>
 
#ifdef UNIX
// #include <unistd.h>
#endif
#include <xgrafix.h>
 
// #include <xgmini.h>
void Quit(void);
 
}
 
// #define BENCHMARK 1
#ifdef BENCHMARK
void write_validation();
#endif

// Qt and Txq Includes

#include <qapp.h>
#include <qapplication.h>
#include "windowtype.h"
#include "CreateWindows.h"

// External definitions

extern "C" void XGInit(int argc, char **argv, double *t);
extern "C++" void XGMainLoop();
extern void TxqMainWin(QApplication* qtapp, int argc, char **argv, WindowList *wl);
extern Scalar nfft;
extern "C" WindowType *theWindowArray;
extern "C" int numberOfWindows;

 
void Physics(void);
void InitSim(void);
