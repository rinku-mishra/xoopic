#ifndef XGRAFIX_H
#define XGRAFIX_H

#ifdef UNIX
#include <varargs.h>
#endif
#include <xgdatamacros.h>
#include "windowtype.h"

/*********************************************************************/
/* Change the following typedef to the type desired: float or double */
/* Also change in xgdatamacros.h                                     */

#ifdef SCALAR_DOUBLE
typedef double SCALAR;
#else
typedef float SCALAR;
#endif
 
// typedef enum { FILLED, HOLLOW } STRUCT_FILL;
#define LINE_PLOT 1
#define SCATTER_PLOT 2
#define VECTOR_PLOT 3
#define SURFACE_PLOT 4
#define SCATTER3D 5
#define IRREGULAR3D 6

extern int             theRunWithXFlag;
extern int             theNumberOfSteps;
extern int             theCurrentStep;
extern int             theDumpPeriod;
extern int             WasDumpFileGiven;
extern int             WasInputFileGiven;
extern int             iterationsPerXUpdate;

extern char            theCodeName[80];
extern char            theInputFile[80];
extern char            theDumpFile[80];
extern char            theEPSFile[80];

#ifdef __GNUG__
extern "C" {
#endif
extern void            XGInit(int argc, char **argv, double *t);
extern void 	       XGSetVec(const char *type, const char *xlabel, const char *ylabel, 
				const char *zlabel, const char *state, int ulx, 
				int uly, SCALAR xscale, SCALAR yscale, 
				int xautorescale, int yautorescale, 
				SCALAR xmin, SCALAR xmax, SCALAR ymin,
				SCALAR ymax);
extern void 	       XGSetVecFlag(const char *type, const char *xlabel, const char *ylabel, 
				    const char *zlabel, const char *state, int ulx, 
				    int uly, SCALAR xscale, SCALAR yscale, 
				    int xautorescale, int yautorescale, 
				    SCALAR xmin, SCALAR xmax, SCALAR ymin,
				    SCALAR ymax, int *openFlag);

extern void 	      XGSet2D(const char *type, const char *xlabel, const char *ylabel, 
			       const char *state, int ulx, int uly, SCALAR xscale,
			       SCALAR yscale, int xautorescale, 
			       int yautorescale, SCALAR xmin, SCALAR xmax, 
			       SCALAR ymin, SCALAR ymax);

extern void 	       XGSet2DFlag(const char *type, const char *xlabel, const char *ylabel, 
			       const char *state, int ulx, int uly, SCALAR xscale,
			       SCALAR yscale, int xautorescale, 
			       int yautorescale, SCALAR xmin, SCALAR xmax, 
			       SCALAR ymin, SCALAR ymax, int *openFlag);
extern void 	       XGSet2DC(const char *type, const char *xlabel, const char *ylabel, 
				const char *zlabel, const char *state, int ulx,
				int uly, SCALAR xscale, SCALAR yscale,
				SCALAR zscale, int xautorescale, 
				int yautorescale, int zautorescale, 
				SCALAR xmin, SCALAR xmax, SCALAR ymin, 
				SCALAR ymax, SCALAR zmin, SCALAR zmax);
extern void 	       XGSet2DCFlag(const char *type, const char *xlabel, const char *ylabel, 
				    const char *zlabel, const char *state, int ulx,
				    int uly, SCALAR xscale, SCALAR yscale,
				    SCALAR zscale, int xautorescale, 
				    int yautorescale, int zautorescale, 
				    SCALAR xmin, SCALAR xmax, SCALAR ymin, 
				    SCALAR ymax, SCALAR zmin, SCALAR zmax,
				    int *openFlag);
extern void 	       XGSet3D(const char *type, const char *xlabel, const char *ylabel, 
			       const char *zlabel, SCALAR theta, SCALAR phi,
			       const char *state, int ulx, int uly, SCALAR xscale, 
			       SCALAR yscale, SCALAR zscale, 
			       int xautorescale, int yautorescale, 
			       int zautorescale, SCALAR xmin, SCALAR xmax, 
			       SCALAR ymin, SCALAR ymax, SCALAR zmin, 
			       SCALAR zmax);
extern void 	       XGSet3DFlag(const char *type, const char *xlabel, const char *ylabel, 
				   const char *zlabel, SCALAR theta, SCALAR phi,
				   const char *state, int ulx, int uly,SCALAR xscale,
				   SCALAR yscale, SCALAR zscale, 
				   int xautorescale, int yautorescale, 
				   int zautorescale, SCALAR xmin, SCALAR xmax, 
				   SCALAR ymin, SCALAR ymax, SCALAR zmin, 
				   SCALAR zmax, int *openFlag);
#ifdef __cplusplus
extern void            XGStructure(...);
#else
extern void	       XGStructure();
#endif
extern void	       XGStructureArray(int npoints, STRUCT_FILL fillFlag, int linecolor, int fillcolor, SCALAR *points);
extern void            XGVector(SCALAR *xdata, SCALAR *ydata, 
				SCALAR **zdata, SCALAR **wdata, int *mpoints,
				int *npoints, int color);
extern void            XGVectorVector(SCALAR *xdata, SCALAR *ydata, 
				      SCALAR **zdata, SCALAR **wdata, 
				      int *mpoints, int *npoints, int color,
				      int xSize, int xOffset, int ySize, 
				      int yOffset, int zSize, int zOffset, 
				      int wSize, int wOffset);

extern void 	       XGSurf(SCALAR *xdata, SCALAR *ydata, 
			      SCALAR **zdata, int *mpoints, int *npoints, 
			      int color);
extern void 	       XGSurfVector(SCALAR *xdata, SCALAR *ydata, 
				    SCALAR **zdata, int *mpoints, int *npoints,
				    int color, int xSize, int xOffset, 
				    int ySize, int yOffset, int zSize, 
				    int zOffset);

extern void 	       XGCurve(SCALAR *xdata, SCALAR *ydata, 
			       int *npoints, int color);
extern void 	       XGCurveVector(SCALAR *xdata, SCALAR *ydata, 
			       int *npoints, int color, int xSize, int xOffset,
			       int ySize, int yOffset);

extern void 	       XGIRSurf(SCALAR **xdata, SCALAR **ydata, 
				SCALAR **zdata, int *mpoints, int *nponits, 
				int color);
extern void 	       XGIRSurfVector(SCALAR **xdata, SCALAR **ydata, 
				      SCALAR **zdata, int *mpoints, 
				      int *npoints, int color, int xSize, 
				      int xOffset, int ySize, int yOffset,
				      int zSize, int zOffset);

extern void            XGScat2D(SCALAR *xdata, SCALAR *ydata, 
				int *npoints, int color);

extern void            XGScat3D(SCALAR *xdata, SCALAR *ydata, 
				SCALAR *zdata, int *npoints, int color);

extern void            XGCont(SCALAR *xdata, SCALAR *data, 
			      SCALAR **zdata, int *mpoints, int *npoints, 
			      int color);
extern void 	       SetUpNewVar(SCALAR *, const char *, const char *);
extern void 	       XGStart();
#ifdef __GNUG__
}
#endif

#endif /* XGRAFIX_H */
