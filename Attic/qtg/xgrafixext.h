#ifndef XGRAFIXINT_H
#define XGRAFIXINT_H

/************************/
/* Including the C libs */

#include <stdio.h>
#ifdef UNIX
#include <unistd.h>
#endif
//#include <malloc/malloc.h>
#include <string.h>
#include <time.h>
#include <math.h>

/************************/
/* Including the X libs */

/* 
#include <X11/Xlib.h>
#include <X11/Xutil.h>
*/

/*****************************/
/* Including the Tcl/Tk libx */

/*
#include <tcl.h>
#include <tk.h>
*/

/********************/
/* Some data macros */
#include "xgdatamacros.h"

/*****************************/
/* the non-X global variables and data types */
#include "xgminint.h"


/****************************************/
/* XGrafix version number               */
static const char *xgversion = "2.50";

/****************************************/
/* Definition of colors used in XGrafix */

static const char *WindowTextColor   = "cyan";
static const char *BorderColor       = "red";
static const char *TickMarkColor     = "yellow";
static const char *BackgroundColor   = "black";

#define		MAXGRAYSHADES	 11

static const char *theGrayShades[]={	"gray0",  "gray10", "gray20",
				"gray30", "gray40", "gray50",
				"gray60", "gray70", "gray80",
				"gray90", "gray100"};

#define		MAXCOLORS	 9

static const char *theColorNames[]={	"cyan", "wheat", "green", 
				"yellow", "orange", "pink",
				"lightblue", "brown", "red"};

#define		MAXTHREEDCOLORS	61

static const char *theTDColorNames[]={"00", "20", "35", "50", "70", "90",
				"a5", "b9", "d0", "e5", "ff"};

static const char *theTDPSColorNames[]={"0.0 ", "0.1 ", "0.2 ", "0.3 ",
				  "0.4 ", "0.5 ", "0.6 ", "0.7 ",
				  "0.8 ", "0.9 ", "1.0 "};

/****************************************/

static const char *theFontName = "variable";




typedef struct window_struct *WindowType;
struct window_struct {
  int             ulxc;
  int             ulyc;
  int             lrxc;
  int             lryc;
  int             state;
  DataType        data;
  LabelType       label;
  SCALAR          theta;
  SCALAR          phi;
  char            eps_file_name[MAX_BUFFER_SIZE];
  char            plot_title[MAX_BUFFER_SIZE];
/*
  GC              xwingc;
  Pixmap          pixmap_buffer;
  void            (*paint_function) ();
  void            (*print_function) ();
  void            (*ascii_print_function) ();
  void            (*xgrafix_print_function) ();
  Tk_Window       tkwin;
*/
  MeshType      **theMesh;
  double          c1,d1,c2,d2;
  int             mstart, mend, nstart, nend, xSize, ySize;
  StructType     *structures;
  // int            *openFlag;
  int            openFlag;
  // char            type;
  int		 type;
};

typedef struct string_struct {
  char            buffer[20];
  SCALAR          x;
  SCALAR          y;
}               StringType;

/********************************************/
/********************************************/
/* Global variables used in XGRAFIX         */


extern WindowType     *theWindowArray;
extern int             sizeIncrement;
extern int             sizeOfWindowArray;
extern int             numberOfWindows; 

/* Variables for Special Button List, Payam 7.92 */
/* List changed to array, Don 7.94 */
extern SpecialType    *theSpecialArray; 
extern int             specialSizeIncrement;
extern int             sizeOfSpecialArray;
extern int             numberOfSpecials;

extern double          theCrosshairmx, theCrosshairbx;
extern double          theCrosshairmy, theCrosshairby;
extern int             theCrosshairx1, theCrosshairx2;
extern int             theCrosshairy1, theCrosshairy2;

extern int             theWinTextColor;
extern int             theDBTextColor;
extern int             theBorderColor;
extern int             theTickMarkColor;
extern int             theBKGDColor;
extern int             GrayShade[MAXGRAYSHADES];
extern int             Color[MAXCOLORS];
extern float           RedColor[MAXCOLORS];
extern float           GreenColor[MAXCOLORS];
extern float           BlueColor[MAXCOLORS];
extern int             ThreeDColor[MAXTHREEDCOLORS];
extern char            ThreeDPSColor[MAXTHREEDCOLORS][20];
extern SCALAR          init_theta, init_phi;

extern WindowType      theNewWindow;
extern int             theRunWithXFlag;

void            XpmToGif(const char **xpmData, const char *outFile);
DataType        SetupDataStruct();
LabelType       SetupLabelStruct();

extern int             xindex_3d_plots;
extern int             yindex_3d_plots;
extern int             zindex_3d_plots;
extern int             windex_3d_plots;
extern SCALAR        **xplot3d[MAX_3D_PLOTS];
extern SCALAR        **yplot3d[MAX_3D_PLOTS];
extern SCALAR        **zplot3d[MAX_3D_PLOTS];
extern SCALAR        **wplot3d[MAX_3D_PLOTS];

void            Paint_Window(WindowType);
void            Paint_Vector_Window(WindowType);
void            Paint_ThreeD_Window(WindowType);
void            PostScriptOpenWindow(WindowType, char*, char*);
void            PostScript_ThreeD_Window(WindowType, char*, char*);
void            PostScript_Vector_Window(WindowType, char*, char*);
void            Ascii_ThreeD_Window(WindowType, char*, char);
void            Ascii_TwoD_Window(WindowType, char*, char);
void            Ascii_Vector_Window(WindowType, char*, char);
void            XGInit(int argc, char **argv, double *t);
void            XGInitX();
void            XGInitTclTk();
void 	        XGSetVec(const char *type, const char *xlabel, const char *ylabel, 
			 const char *zlabel, const char *state, int ulx, 
			 int uly, SCALAR xscale, SCALAR yscale, 
			 int xautorescale, int yautorescale, 
			 SCALAR xmin, SCALAR xmax, SCALAR ymin,
			 SCALAR ymax);
void		XGSet2D(const char *type, const char *xlabel, const char *ylabel, 
			const char *state, int ulx, int uly, SCALAR xscale,
			SCALAR yscale, int xautorescale, 
			int yautorescale, SCALAR xmin, SCALAR xmax, 
			SCALAR ymin, SCALAR ymax);
void 	        XGSet2DC(const char *type, const char *xlabel, const char *ylabel, 
			 const char *zlabel, const char *state, int ulx,
			 int uly, SCALAR xscale, SCALAR yscale,
			 SCALAR zscale, int xautorescale, 
			 int yautorescale, int zautorescale, 
			 SCALAR xmin, SCALAR xmax, SCALAR ymin, 
			 SCALAR ymax, SCALAR zmin, SCALAR zmax);
void 	        XGSet3D(const char *type, const char *xlabel, const char *ylabel, 
			const char *zlabel, SCALAR theta, SCALAR phi,
			const char *state, int ulx, int uly, SCALAR xscale, 
			SCALAR yscale, SCALAR zscale, 
			int xautorescale, int yautorescale, 
			int zautorescale, SCALAR xmin, SCALAR xmax, 
			SCALAR ymin, SCALAR ymax, SCALAR zmin, 
			SCALAR zmax);
void 	        XGVector(SCALAR *xdata, SCALAR *ydata, 
			 SCALAR **zdata, SCALAR **wdata, int *mpoints,
			 int *npoints, int color);
void            XGVectorVector(SCALAR *xdata, SCALAR *ydata, 
			       SCALAR **zdata, SCALAR **wdata, 
			       int *mpoints, int *npoints, int color,
			       int xSize, int xOffset, int ySize, 
			       int yOffset, int zSize, int zOffset, 
			       int wSize, int wOffset);
     
void 	        XGSurf(SCALAR *xdata, SCALAR *ydata, 
		       SCALAR **zdata, int *mpoints, int *npoints, 
		       int color);
void  	        XGSurfVector(SCALAR *xdata, SCALAR *ydata, 
			     SCALAR **zdata, int *mpoints, int *npoints,
			     int color, int xSize, int xOffset, 
			     int ySize, int yOffset, int zSize, 
			     int zOffset);

void 	        XGCurve(SCALAR *xdata, SCALAR *ydata, 
			int *npoints, int color);
void  	        XGCurveVector(SCALAR *xdata, SCALAR *ydata, 
			int *npoints, int color, int xSize, int xOffset,
			int ySize, int yOffset);

void 	        XGIRSurf(SCALAR **xdata, SCALAR **ydata, 
			 SCALAR **zdata, int *mpoints, int *nponits, 
			 int color);
void 	        XGIRSurfVector(SCALAR **xdata, SCALAR **ydata, 
			       SCALAR **zdata, int *mpoints, 
			       int *npoints, int color, int xSize, 
			       int xOffset, int ySize, int yOffset,
			       int zSize, int zOffset);

void            XGScat2D(SCALAR *xdata, SCALAR *ydata, 
			  int *npoints, int color);

void            XGScat3D(SCALAR *xdata, SCALAR *ydata, 
			  SCALAR *zdata, int *npoints, int color);

void            XGCont(SCALAR *xdata, SCALAR *data, 
			SCALAR **zdata, int *mpoints, int *npoints, 
			int color);

void 		XGStart();
void            XGMainLoop();
void 		XG_Setup_Pixmap(WindowType);
void 		Setup();
void 		Main();
void 		RedrawWindow(WindowType);
void 		DoGraphics();
void 		DoMain();
void 		SortWindows();
int 		Search(const char *);
void            XGSetupWindow(const char *, const char *, int, int, int, DataType, 
		LabelType, int);
void 		InitTclCommands();
void 		PSHeader(const char *);
void 		XG_Copy_Pixmap(WindowType);
void 		ReallocateWindows();
void 		SetUpNewVar(void *, const char *, const char *);
void		Dump(const char *);
void		Quit();
void            DrawCrosshair(WindowType, int, int);
void            RescaleTwoDWindow(WindowType theWindow);
void            RescaleVectorDWindow(WindowType theWindow, int *ms, int *me, 
				     int *ns, int *ne);
void            RescaleThreeDWindow(WindowType theWindow, int *ms, int *me, 
				    int *ns, int *ne);

void XGWriteLabel(LabelType theLabel, FILE *fp);
void XGWriteStructures(StructType *structures, const char *filename);
void XGRead(void *ptr,int size,int nitems,FILE * stream,const char * type);
void Bin_Vector_Window(WindowType theWindow, const char *filename, char type);
void Bin_ThreeD_Window(WindowType theWindow, const char *filename, char type);
void Bin_TwoD_Window(WindowType theWindow, const char *filename, char type);
extern void	       XGStructureArray(int npoints, STRUCT_FILL fillFlag, int linecolor, int fillcolor, SCALAR *points);

extern void QtInit(int argc, char **argv);

#endif /* XGRAFIXINT_H */
