/*
====================================================================
xgdefs.h
  Minimum set of xgrafix definitions for otools to compile.
  This will be removed later, but for now it is required.
  (kgl) 1.31.00
====================================================================
*/

#include <precision.h>
#include "xgio.h"

extern "C" void            XGSet2D(const char *type, const char *xlabel, const char *ylabel,
                               const char *state, int ulx, int uly, Scalar xscale,
                               Scalar yscale, int xautorescale,
                               int yautorescale, Scalar xmin, Scalar xmax,
                               Scalar ymin, Scalar ymax);
extern "C" void            XGCurve(Scalar *xdata, Scalar *ydata,
                               int *npoints, int color);
extern "C" void            XGSet3D(const char *type, const char *xlabel, const char *ylabel,
                               const char *zlabel, Scalar theta, Scalar phi,
                               const char *state, int ulx, int uly, Scalar xscale,
                               Scalar yscale, Scalar zscale,
                               int xautorescale, int yautorescale,
                               int zautorescale, Scalar xmin, Scalar xmax,
                               Scalar ymin, Scalar ymax, Scalar zmin,
                               Scalar zmax);
extern "C" void            XGSurf(Scalar *xdata, Scalar *ydata,
                              Scalar **zdata, int *mpoints, int *npoints,
                              int color);
// extern "C" void            XGWrite(void *,int,int,FILE *,const char *);
// extern "C" void            XGRead(void *,int,int,FILE *,const char *);

