/****************************************************************************
**
** xgifc.c
** xgifc.c implements the required xg interface for Qt graphics
**  This file is composed of the former files:
**    xgnox.c
**    xginitc.c
**    xginitvar.c
**    xgreadrit.c
**    xgrescale.c
**    xgsetup.c
**
** CVS: $Id: xgifc.c 1294 2000-07-06 16:07:33Z bruhwile $
**
** Author: Kelly G. Luetkemeyer, Tech-X Corporation
**
****************************************************************************/

#include "xgnox.h"
#include "xgrafixint.h"
// #include "xgrafixext.h"
#include <stdlib.h>
#include <varargs.h>
#include <stdio.h>
#ifdef UNIX
#include <unistd.h>
#endif
//#include <malloc/malloc.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <precision.h>

#ifndef UNIX
#define F_OK 0
#endif

ENDIAN          theEndian;

void PrintUsage(char *string)
{
  printf("Usage: %s [options]\n",string);
  printf("\t-d dumpfile[.dmp]:\n\t\tSpecify dump file name. Default is output.dmp\n");
  printf("\t-display displayname:\n\t\tSpecify the display. Default is DISPLAY environment variable.\n");
  printf("\t-dp n:\n\t\tSpecify the dump period. Default is 0 (no periodic dumps)\n");
  printf("\t-exit:\n\t\tExit after done with iterations\n");
  printf("\t-h:\n\t\tThis help message\n");
  printf("\t-i inputfile[.inp]:\n\t\tSpecify input file name. Default is no input file\n");
  printf("\t-nox:\n\t\tRun without X\n");
  printf("\t-p epsfile[.eps]:\n\t\tSpecify ps file name. Default is output.eps\n");
  printf("\t-s n:\n\t\tSpecify the number of iterations. Default is 0 (run forever)\n");
  printf("\t-u n:\n\t\tSpecify the number of iterations per X update. Default is 1\n");
}

void PrintUnknownOption(char *string, char *string2)
{
  printf("Unknown option specified: %s\n",string);
  PrintUsage(string2);
}

void PrintNoArg(char *string, char *string2)
{
  printf("No argument given to option: %s\n",string);
  PrintUsage(string2);
}

void XGInit(int argc, char **argv, double *t)
{
  int index, endianInt = 1;
  char *endianChar;
  printf("\n Entered: XGInit\n");
  printf("argc: %d \n",argc);
  printf("argv[0]: %s \n",argv[0]);
  printf("t: %g \n",t);
  
  /*******************/
  /* The C stuff */
  
  theTimeStep = t;
  theRunFlag = FALSE;
  theStepFlag = FALSE;
  theExitFlag = FALSE;
  init_theta = init_phi = 45;
  theRunWithXFlag = TRUE; 
  theNumberOfSteps = 0;
  iterationsPerXUpdate = 1;
  theCurrentStep = 1;
  theDumpPeriod = 0;
  WasDumpFileGiven = FALSE;
  WasInputFileGiven = FALSE;
  theExitFlag = FALSE;
  // ReqDisp[0] = 0;

  /***********************************/
  /* Checking the endianness of host */

  endianChar = (char*)&endianInt;

  if (*endianChar == (char)1) {
     theEndian = LITTLE;
  } else {
     theEndian = BIG;
  }

  /*****************************************/
  /* Getting the code and input file names */

  sprintf(theCodeName, "%s", argv[0]);
  theInputFile[0]='\0';

/* Try to read the input parameters, i.e. -i -d -dp, etc. options */
   for(index=1;index<argc;index++) {
    if(argv[index][0] != '-') {
      PrintUnknownOption(argv[index],argv[0]);
      exit(1);
    }
    switch(argv[index][1]) {
    case 'd':
      if(argv[index][2]!='\0' && argv[index][2]!='p' && 
	 strcmp(argv[index],"-display")) {
	PrintUnknownOption(argv[index],argv[0]);
	exit(1);
      }
      if(index+1 >= argc) {
	PrintNoArg(argv[index],argv[0]);
	exit(1);
      }
      switch(argv[index][2]) {
      case '\0':
	strcpy(theDumpFile, argv[++index]);
	if(access(theDumpFile,F_OK)) {
	  strcat(theDumpFile, theDumpExtension);
	  if(access(theDumpFile,F_OK)) {
	    printf("Error: %s does not exist\n",theDumpFile);
	    exit(1);
	  }
	}

	printf("Dump file is '%s'\n",theDumpFile);
	WasDumpFileGiven = TRUE;
	break;
      case 'p':
	theDumpPeriod = atoi(argv[++index]);
	printf("Dumping every %d steps\n",theDumpPeriod);
	break;
      case 'i':
	// strcpy(ReqDisp, argv[++index]);
	// printf("Setting display to %s.\n",ReqDisp);
	break;
      }
      break;
    case 'e':
      if (!strcmp(argv[index],"-exit")) {
	theExitFlag = TRUE;
      } else {
	PrintUnknownOption(argv[index],argv[0]);
	exit(1);
      }
      break;
    case 'h':
      PrintUsage(argv[0]);
      exit(0);
      break;
    case 'i':
      if(argv[index][2]!='\0') {
	PrintUnknownOption(argv[index],argv[0]);
	exit(1);
      }
      if(index+1 >= argc) {
	PrintNoArg(argv[index],argv[0]);
	exit(1);
      }
      strcpy(theInputFile, argv[++index]);

      if(access(theInputFile,F_OK)) {
	strcat(theInputFile,theInputExtension);
	if(access(theInputFile,F_OK)) {
	  printf("Error: %s does not exist\n",theInputFile);
	  exit(1);
	}
      }

      WasInputFileGiven = TRUE;
      printf("\nInput file is '%s'\n",theInputFile);
      break;
    case 'n':
      if(!strcmp(argv[index],"-nox")) {
	printf("Running without X\n");
	theRunWithXFlag = FALSE;
      } else {
	PrintUnknownOption(argv[index],argv[0]);
	exit(1);
      }
      break;
    case 'p':
      if(argv[index][2]!='\0') {
	PrintUnknownOption(argv[index],argv[0]);
	exit(1);
      }
      if(index+1 >= argc) {
	PrintNoArg(argv[index],argv[0]);
	exit(1);
      }
      strcpy(theEPSFile, argv[++index]);
      printf("Postscript file is '%s'\n",theEPSFile);
      break;
    case 's':
      if(argv[index][2]!='\0') {
	PrintUnknownOption(argv[index],argv[0]);
	exit(1);
      }
      if(index+1 >= argc) {
	PrintNoArg(argv[index],argv[0]);
	exit(1);
      }
      theNumberOfSteps = atoi(argv[++index]);
      printf("Running for %d steps\n",theNumberOfSteps);
      break;

	 case 'u':  /*  set the number of iterations per X update */
      if(argv[index][2]!='\0') {
		  PrintUnknownOption(argv[index],argv[0]);
		  exit(1);
      }
      if(index+1 >= argc) {
		  PrintNoArg(argv[index],argv[0]);
		  exit(1);
      }
      iterationsPerXUpdate = atoi(argv[++index]);
		if(iterationsPerXUpdate < 1) iterationsPerXUpdate = 1;
      printf("Iterations per X update: %d steps\n",iterationsPerXUpdate);
      break;

    default:
      PrintUnknownOption(argv[index],argv[0]);
      exit(1);
      break;
    }      
  }      
/* End of trying to read parameters */

  if (!WasInputFileGiven) {
    strcpy(theEPSFile,theDefaultEPSFile);
    strcpy(theDumpFile,theDefaultDumpFile);
  } else if (!WasDumpFileGiven) {
    strncpy(theEPSFile,theInputFile,findlen(theInputFile));
    strncpy(theDumpFile,theInputFile,findlen(theInputFile));
    strcat(theEPSFile,theEPSExtension);
    strcat(theDumpFile,theDumpExtension);
  } else {
    strncpy(theEPSFile,theInputFile,findlen(theInputFile));
    strcat(theEPSFile,theEPSExtension);
  }

  /*********************************/
  /* Initializing the Window array */

  sizeOfWindowArray = 10;
  printf("xgnox: sizeOfWindowArray %d\n",sizeOfWindowArray);
  theWindowArray = (WindowType *)malloc(sizeof(WindowType)*sizeOfWindowArray);
  numberOfWindows = 0;
  sizeIncrement = 10;

  /**********************************/
  /* Initializing the Special array */

  theSpecialArray = NULL;
  sizeOfSpecialArray = 0;
  specialSizeIncrement = 10;
  numberOfSpecials = 0;

  // QtInit(argc, argv);
/*
  if(theRunWithXFlag==TRUE) {
    XGInitX();
    XGInitTclTk();
  }
*/
}

/* A procedure called by XGInit to determine the rootlength of filenames */
int findlen(char *string){
 int i, len;

 len = strlen(string);
 i = len-1;
 while (string[i] != '.' && i  >=0) {
   if(string[i]=='/') return len;
   i = i - 1;
 }
 if(i>0)  return (i);
 return len;

}


void XGInitX() {}

void XGInitTclTk() {}

void XGStructure(va_alist) va_dcl {
  int i, numPoints, lineColor, fillColor;
  va_list ap;
  STRUCT_FILL fillFlag;
  StructType *structure = (StructType *)malloc(sizeof(StructType));

  va_start(ap);
  numPoints = va_arg(ap,int);
  fillFlag = va_arg(ap,int);
  lineColor = va_arg(ap,int);
  fillColor = va_arg(ap,int);

  structure->x = (double *)malloc(numPoints * sizeof(double));
  structure->y = (double *)malloc(numPoints * sizeof(double));

  structure->numberPoints = numPoints;
  structure->fillFlag = fillFlag;
  structure->lineColor = lineColor;
  structure->fillColor = fillColor;

  for (i=0;i<numPoints;i++) {
    structure->x[i] = va_arg(ap,double);
    structure->y[i] = va_arg(ap,double);
  }
  va_end(ap);

/*
  structure->next = theNewWindow->structures;
  theNewWindow->structures = structure;
*/
/*
  structure->next = winType2D->structures;
  winType2D->structures = structure;
*/

  structure->next = theWindowArray[numberOfWindows-1]->structures;
  theWindowArray[numberOfWindows-1]->structures = structure;
}

void XGStructureArray(int numPoints,STRUCT_FILL fillFlag,int lineColor,
                                                         int fillColor, SCALAR *
points ) 
{
  int i;
  StructType *structure = (StructType *)malloc(sizeof(StructType));

  structure->x = (double *)malloc(numPoints * sizeof(double));
  structure->y = (double *)malloc(numPoints * sizeof(double));

  structure->numberPoints = numPoints;
  structure->fillFlag = fillFlag;
  structure->lineColor = lineColor;
  structure->fillColor = fillColor;

  for (i=0;i<numPoints;i++) {
    structure->x[i] = points[2*i];
    structure->y[i] = points[2*i+1];
  }

/*
  structure->next = theNewWindow->structures;
  theNewWindow->structures = structure;
*/
/*
  structure->next = winType2D->structures;
  winType2D->structures = structure;
*/

  structure->next = theWindowArray[numberOfWindows-1]->structures;
  theWindowArray[numberOfWindows-1]->structures = structure;
}


void XGSetVec(char *PlotType, char *X_Label, char *Y_Label, char *Z_Label,
	      char *State, int ulx, int uly, SCALAR X_Scale, SCALAR Y_Scale,
	      int X_Auto_Rescale, int Y_Auto_Rescale, SCALAR X_Min, 
	      SCALAR X_Max, SCALAR Y_Min, SCALAR Y_Max)
{
  int             plottype;
  DataType        data_ptr = NULL;
  LabelType       label_ptr;
  WindowType      winType;
  
  label_ptr = SetupLabelStruct();
  
  label_ptr->Z_Label = Z_Label;
  label_ptr->Z_Auto_Rescale = 0;

  label_ptr->Y_Label = Y_Label;
  label_ptr->Y_Min = Y_Min;
  label_ptr->Y_Max = Y_Max;
  label_ptr->Y_Scale = Y_Scale;
  label_ptr->Y_Auto_Rescale = Y_Auto_Rescale;
  
  label_ptr->X_Label = X_Label;
  label_ptr->X_Min = X_Min;
  label_ptr->X_Max = X_Max;
  label_ptr->X_Scale = X_Scale;
  label_ptr->X_Auto_Rescale = X_Auto_Rescale;

  if (!strcmp(PlotType, "vecvec"))
    plottype = VEC_VEC;
  else {
    printf("Unrecognized plot string '%s' for Window '%s'.\n", PlotType, 
	   Y_Label);
    exit(-1);
  }

  XGSetupWindow(Z_Label, State, ulx, uly, VECD, data_ptr, label_ptr, plottype);
}

void XGSetVecFlag(char *PlotType, char *X_Label, char *Y_Label, char *Z_Label,
		  char *State, int ulx, int uly, SCALAR X_Scale,SCALAR Y_Scale,
		  int X_Auto_Rescale, int Y_Auto_Rescale, SCALAR X_Min, 
		  SCALAR X_Max, SCALAR Y_Min, SCALAR Y_Max, int *openFlag)
{
  XGSetVec(PlotType, X_Label, Y_Label, Z_Label, State, ulx, uly, X_Scale,
	   Y_Scale, X_Auto_Rescale, Y_Auto_Rescale, X_Min, X_Max, Y_Min,Y_Max);

  // theWindowArray[numberOfWindows-1]->openFlag = openFlag;
  theWindowArray[numberOfWindows-1]->openFlag = *openFlag;
}  

void XGSet3D(char *PlotType, char *X_Label, char *Y_Label, char *Z_Label,
	     SCALAR Theta, SCALAR Phi, char *State, int ulx, int uly,
	     SCALAR X_Scale, SCALAR Y_Scale, SCALAR Z_Scale, 
	     int X_Auto_Rescale, int Y_Auto_Rescale, int Z_Auto_Rescale,
	     SCALAR X_Min, SCALAR X_Max, SCALAR Y_Min, SCALAR Y_Max, 
	     SCALAR Z_Min, SCALAR Z_Max)
{
  int             plottype;
  DataType        data_ptr = NULL;
  LabelType       label_ptr;
  WindowType      winType;
  
  init_theta = Theta;
  init_phi = Phi;

  label_ptr = SetupLabelStruct();
  label_ptr->Z_Label = Z_Label;
  label_ptr->Z_Min = Z_Min;
  label_ptr->Z_Max = Z_Max;
  label_ptr->Z_Scale = Z_Scale;
  label_ptr->Z_Auto_Rescale = Z_Auto_Rescale;

  label_ptr->Y_Label = Y_Label;
  label_ptr->Y_Min = Y_Min;
  label_ptr->Y_Max = Y_Max;
  label_ptr->Y_Scale = Y_Scale;
  label_ptr->Y_Auto_Rescale = Y_Auto_Rescale;
  
  label_ptr->X_Label = X_Label;
  label_ptr->X_Min = X_Min;
  label_ptr->X_Max = X_Max;
  label_ptr->X_Scale = X_Scale;
  label_ptr->X_Auto_Rescale = X_Auto_Rescale;
  
  if (!strcmp(PlotType, "linlinlin"))      plottype = LIN_LIN_LIN;
  else if (!strcmp(PlotType, "linlinlog")) plottype = LIN_LIN_LOG;
  else if (!strcmp(PlotType, "linloglin")) plottype = LIN_LOG_LIN;
  else if (!strcmp(PlotType, "linloglog")) plottype = LIN_LOG_LOG;
  else if (!strcmp(PlotType, "loglinlin")) plottype = LOG_LIN_LIN;
  else if (!strcmp(PlotType, "loglinlog")) plottype = LOG_LIN_LOG;
  else if (!strcmp(PlotType, "logloglin")) plottype = LOG_LOG_LIN;
  else if (!strcmp(PlotType, "logloglog")) plottype = LOG_LOG_LOG;
  else {
    printf("Unrecognized plot string '%s' for Window '%s'.\n", PlotType, Z_Label);
    exit(-1);
  }

  XGSetupWindow(Z_Label, State, ulx, uly, THREED, data_ptr, label_ptr, 
		plottype);
}

void XGSet3DFlag(char *PlotType, char *X_Label, char *Y_Label, char *Z_Label,
		 SCALAR Theta, SCALAR Phi, char *State, int ulx, int uly,
		 SCALAR X_Scale, SCALAR Y_Scale, SCALAR Z_Scale, 
		 int X_Auto_Rescale, int Y_Auto_Rescale, int Z_Auto_Rescale,
		 SCALAR X_Min, SCALAR X_Max, SCALAR Y_Min, SCALAR Y_Max, 
		 SCALAR Z_Min, SCALAR Z_Max, int *openFlag)
{
  XGSet3D(PlotType, X_Label, Y_Label, Z_Label, Theta, Phi, State, ulx, uly,
	  X_Scale, Y_Scale, Z_Scale, X_Auto_Rescale, Y_Auto_Rescale,
	  Z_Auto_Rescale, X_Min, X_Max, Y_Min, Y_Max, Z_Min, Z_Max);
  
  // theWindowArray[numberOfWindows-1]->openFlag = openFlag;
  theWindowArray[numberOfWindows-1]->openFlag = *openFlag;
}

void XGSet2D(char *PlotType, char *X_Label, char *Y_Label, char *State,
	     int ulx, int uly, SCALAR X_Scale, SCALAR Y_Scale, 
	     int X_Auto_Rescale, int Y_Auto_Rescale, SCALAR X_Min, 
	     SCALAR X_Max, SCALAR Y_Min, SCALAR Y_Max)
{
  int             plottype;
  WindowType      winType;
  
  DataType        data_ptr = NULL;
  LabelType       label_ptr;
  
  // printf("Entered XGSet2D\n");

  label_ptr = SetupLabelStruct();
  label_ptr->Y_Label = Y_Label;
  label_ptr->Y_Min = Y_Min;
  label_ptr->Y_Max = Y_Max;
  label_ptr->Y_Scale = Y_Scale;
  label_ptr->Y_Auto_Rescale = Y_Auto_Rescale;

  label_ptr->X_Label = X_Label;
  label_ptr->X_Min = X_Min;
  label_ptr->X_Max = X_Max;
  label_ptr->X_Scale = X_Scale;
  label_ptr->X_Auto_Rescale = X_Auto_Rescale;
  
  if (!strcmp(PlotType, "linlin"))      plottype = LIN_LIN;
  else if (!strcmp(PlotType, "linlog")) plottype = LIN_LOG;
  else if (!strcmp(PlotType, "loglog")) plottype = LOG_LOG;
  else if (!strcmp(PlotType, "loglin")) plottype = LOG_LIN;
  else {
    printf("Unrecognized plot string '%s' for Window '%s'.\n", PlotType, Y_Label);
    exit(-1);
  }
  
  XGSetupWindow(Y_Label, State, ulx, uly, TWOD, data_ptr, label_ptr, plottype);
}

void XGSet2DFlag(char *PlotType, char *X_Label, char *Y_Label, char *State,
		 int ulx, int uly, SCALAR X_Scale, SCALAR Y_Scale, 
		 int X_Auto_Rescale, int Y_Auto_Rescale, SCALAR X_Min, 
		 SCALAR X_Max, SCALAR Y_Min, SCALAR Y_Max, int *openFlag)
{
  XGSet2D(PlotType, X_Label, Y_Label, State, ulx, uly, X_Scale, Y_Scale,
	  X_Auto_Rescale, Y_Auto_Rescale, X_Min, X_Max, Y_Min, Y_Max);

  // theWindowArray[numberOfWindows-1]->openFlag = openFlag;
  theWindowArray[numberOfWindows-1]->openFlag = *openFlag;
}

void XGSet2DC(char *PlotType, char *X_Label, char *Y_Label, char *Z_Label,
	      char *State, int ulx, int uly, SCALAR X_Scale, SCALAR Y_Scale,
	      SCALAR Z_Scale, int X_Auto_Rescale, int Y_Auto_Rescale,
	      int Z_Auto_Rescale, SCALAR X_Min, SCALAR X_Max, SCALAR Y_Min, 
	      SCALAR Y_Max, SCALAR Z_Min, SCALAR Z_Max)
{
  XGSet3D(PlotType, X_Label, Y_Label, Z_Label, 0.0, 0.0, State, ulx, uly,
	  X_Scale, Y_Scale, Z_Scale, X_Auto_Rescale, Y_Auto_Rescale,
	  Z_Auto_Rescale, X_Min, X_Max, Y_Min, Y_Max, Z_Min, Z_Max);
}

void XGSet2DCFlag(char *PlotType, char *X_Label, char *Y_Label, char *Z_Label,
		  char *State, int ulx, int uly, SCALAR X_Scale,SCALAR Y_Scale,
		  SCALAR Z_Scale, int X_Auto_Rescale, int Y_Auto_Rescale,
		  int Z_Auto_Rescale, SCALAR X_Min, SCALAR X_Max, SCALAR Y_Min,
		  SCALAR Y_Max, SCALAR Z_Min, SCALAR Z_Max, int *openFlag)
{
  XGSet3DFlag(PlotType, X_Label, Y_Label, Z_Label, 0.0, 0.0, State, ulx, uly,
	      X_Scale, Y_Scale, Z_Scale, X_Auto_Rescale, Y_Auto_Rescale,
	      Z_Auto_Rescale, X_Min, X_Max, Y_Min, Y_Max, Z_Min, Z_Max, 
	      openFlag);
}

void XGCurve(SCALAR *x_array, SCALAR *y_array, int *npoints, int color)
{
  DataType        data_prevptr, data_ptr;

  data_prevptr = theWindowArray[numberOfWindows-1]->data;
  data_ptr = SetupDataStruct();
  data_ptr->y = y_array;
  data_ptr->x = x_array;
  data_ptr->npoints = npoints;
  data_ptr->color = color;
  data_ptr->next = data_prevptr;

  theWindowArray[numberOfWindows-1]->data = data_ptr;
}

void XGCurveVector(SCALAR *x_array, SCALAR *y_array, int *npoints, int color,
		    int xSize, int xOffset, int ySize, int yOffset)
{
  DataType data_ptr;

  XGCurve(x_array, y_array, npoints, color);

  data_ptr = theWindowArray[numberOfWindows-1]->data;
  data_ptr->xArraySize = xSize;
  data_ptr->yArraySize = ySize;
  data_ptr->xArrayOffset = xOffset;
  data_ptr->yArrayOffset = yOffset;
}

void XGScat2D(SCALAR *x_array, SCALAR *y_array, int *npoints, int color)
{
  DataType        data_prevptr, data_ptr;
  
  data_prevptr = theWindowArray[numberOfWindows-1]->data;
  data_ptr = SetupDataStruct();
  data_ptr->y = y_array;
  data_ptr->x = x_array;
  data_ptr->npoints = npoints;
  data_ptr->color = color;
  data_ptr->next = data_prevptr;

  Set_Scatter(data_ptr);

  theWindowArray[numberOfWindows-1]->data = data_ptr;
}

void XGScat3D(SCALAR *x_array, SCALAR *y_array, SCALAR *z_array,
	      int *npoints, int color)
{
  DataType        data_prevptr, data_ptr;
  
  data_prevptr = theWindowArray[numberOfWindows-1]->data;
  data_ptr = SetupDataStruct();
  data_ptr->zscat = z_array;
  data_ptr->y = y_array;
  data_ptr->x = x_array;
  data_ptr->npoints = npoints;
  data_ptr->mpoints = npoints;
  data_ptr->color = Color[color];
  data_ptr->next = data_prevptr;

  Set_Scatter(data_ptr);

  theWindowArray[numberOfWindows-1]->data = data_ptr;
}

void XGVector(SCALAR *x_array, SCALAR *y_array, SCALAR **z_array, 
	      SCALAR **w_array, int *mpoints, int *npoints, int color)
{
  DataType        data_prevptr, data_ptr;
  
  data_prevptr = theWindowArray[numberOfWindows-1]->data;
  data_ptr = SetupDataStruct();
  data_ptr->w = z_array; /* VV: notice these are switched */
  data_ptr->z = w_array;
  data_ptr->y = y_array;
  data_ptr->x = x_array;
  data_ptr->npoints = npoints;
  data_ptr->mpoints = mpoints;
  data_ptr->color = color;
  data_ptr->next = data_prevptr;

  Set_Vector(data_ptr);

  theWindowArray[numberOfWindows-1]->data = data_ptr;
}

void XGVectorVector(SCALAR *x_array, SCALAR *y_array, SCALAR **z_array, 
		    SCALAR **w_array, int *mpoints, int *npoints, int color,
		    int xSize, int xOffset, int ySize, int yOffset, int zSize,
		    int zOffset, int wSize, int wOffset)
{
  DataType        data_ptr;
  
  XGVector(x_array, y_array, z_array, w_array, mpoints, npoints, color);

  data_ptr = theWindowArray[numberOfWindows-1]->data;
  data_ptr->xArraySize = xSize;
  data_ptr->yArraySize = ySize;
  data_ptr->xArrayOffset = xOffset;
  data_ptr->yArrayOffset = yOffset;
  data_ptr->zArraySize = wSize; /*DKW: these are switched as well */
  data_ptr->wArraySize = zSize;
  data_ptr->zArrayOffset = wOffset;
  data_ptr->wArrayOffset = zOffset;
}

void XGSurf(SCALAR *x_array, SCALAR *y_array, SCALAR **z_array,
	    int *mpoints, int *npoints, int color)
{
  DataType        data_prevptr, data_ptr;
  
  data_prevptr = theWindowArray[numberOfWindows-1]->data;
  data_ptr = SetupDataStruct();
  data_ptr->z = z_array;
  data_ptr->y = y_array;
  data_ptr->x = x_array;
  data_ptr->mpoints = mpoints;
  data_ptr->npoints = npoints;
  data_ptr->color = Color[color];
  data_ptr->next = data_prevptr;

  theWindowArray[numberOfWindows-1]->data = data_ptr;
}

void XGSurfVector(SCALAR *x_array, SCALAR *y_array, SCALAR **z_array,
		  int *mpoints, int *npoints, int color, int xSize, 
		  int xOffset, int ySize, int yOffset, int zSize, int zOffset)
{
  DataType        data_ptr;
  
  XGSurf(x_array, y_array, z_array, mpoints, npoints, color);

  data_ptr = theWindowArray[numberOfWindows-1]->data;
  data_ptr->xArraySize = xSize;
  data_ptr->yArraySize = ySize;
  data_ptr->zArraySize = zSize;
  data_ptr->xArrayOffset = xOffset;
  data_ptr->yArrayOffset = yOffset;
  data_ptr->zArrayOffset = zOffset;
}

void XGCont(SCALAR *x_array, SCALAR *y_array, SCALAR **z_array,
	    int *mpoints, int *npoints, int color)
{ 
  XGSurf(x_array, y_array, z_array, mpoints, npoints, color);
}

void XGIRSurf(SCALAR **x_array, SCALAR **y_array, SCALAR **z_array,
	      int *mpoints, int *npoints, int color)
{
  DataType        data_prevptr, data_ptr;
  
  data_prevptr = theWindowArray[numberOfWindows-1]->data;
  data_ptr = SetupDataStruct();
  data_ptr->z = z_array;
  data_ptr->y_irr = y_array;
  data_ptr->x_irr = x_array;
  data_ptr->mpoints = mpoints;
  data_ptr->npoints = npoints;
  data_ptr->color = Color[color];
  data_ptr->next = data_prevptr;

  Set_Irr_Surf(data_ptr);

  theWindowArray[numberOfWindows-1]->data = data_ptr;
}

void XGIRSurfVector(SCALAR **x_array, SCALAR **y_array, SCALAR **z_array,
		    int *mpoints, int *npoints, int color, int xSize, 
		    int xOffset, int ySize, int yOffset, int zSize, 
		    int zOffset)
{
  DataType        data_ptr;
  
  XGIRSurf(x_array, y_array, z_array, mpoints, npoints, color);

  data_ptr = theWindowArray[numberOfWindows-1]->data;
  data_ptr->xArraySize = xSize;
  data_ptr->yArraySize = ySize;
  data_ptr->zArraySize = zSize;
  data_ptr->xArrayOffset = xOffset;
  data_ptr->yArrayOffset = yOffset;
  data_ptr->zArrayOffset = zOffset;
}

/****************************************************************/
/* Setup and malloc stuff for variable changer dealie, Payam 7.92 */

/****************************************************/


void SetUpNewVar(void *spvar, const char *spname, const char *type)
{
/*
  SpecialType     SpTemp;

  if (!theRunWithXFlag)
    return;

  SpTemp = (SpecialType) malloc(sizeof(struct special));
  SpTemp->data = spvar;
  strcpy(SpTemp->Varname, spname);
  if (!strcmp(type, "integer"))
    SpTemp->Type = INTEGER;
  else if (!strcmp(type, "float"))
    SpTemp->Type = FLOAT;
  else if (!strcmp(type, "double"))
    SpTemp->Type = DOUBLE;
  else if (!strcmp(type, "char"))
    SpTemp->Type = CHAR;
  else {
    printf("Unknown special type: %s.  Must be 'integer', 'double', 'float', or 'char'.\n", type);
    free(SpTemp);
    return;
  }

  if (numberOfSpecials == sizeOfSpecialArray)
    ReallocateSpecials();

  theSpecialArray[numberOfSpecials] = SpTemp;

  sprintf(TclCommand,"SetUpNewVar %d \"%s\" \"%s\"\n",numberOfSpecials++,
	  spname,type);
  if(Tcl_Eval(interp,TclCommand)!=TCL_OK) {
    printf("Error setting up variable\n");
    printf("%s\n",interp->result);
  }
*/
}


/**********************/
/* Endianness of host */
/* defined in xgdatamacros.h */

/******************************************************/

void sun_read(ptr, size, nitems, stream)
  char           *ptr;
  int             size;
  int             nitems;
  FILE           *stream;
{
  int             i, j;

  for (j = 0; j < nitems; j++)
    for (i = size - 1; i >= 0; i--)
      fread(ptr + j * size + i, 1, 1, stream);
}

/******************************************************/

void sun_write(ptr, size, nitems, stream)
  char           *ptr;
  int             size;
  int             nitems;
  FILE           *stream;
{
  int             i, j;

  for (j = 0; j < nitems; j++)
    for (i = size - 1; i >= 0; i--)
      fwrite(ptr + j * size + i, 1, 1, stream);
}

/*****************************************************************/

void XGRead(ptr, size, nitems, stream, type)
  void           *ptr; 
  char      	  *type;
  FILE           *stream;
  int             size, nitems;
{
#ifdef UNICOS
  if (!strcmp(type, "float"))
    unicos_read_float(ptr, nitems, stream);
  else if (!strcmp(type, "int"))
    unicos_read_int(ptr, nitems, stream);
  else if (!strcmp(type, "char"))
    fread(ptr, size, nitems, stream);
  else
    puts("XGRead: unrecognized type");
#else
  if (theEndian == LITTLE) {
    fread(ptr, size, nitems, stream);
  } else {
    sun_read(ptr, size, nitems, stream);
  }
#endif
}

/******************************************************/

void XGWrite(ptr, size, nitems, stream, type)
  char           *ptr, *type;
  FILE           *stream;
  int             size, nitems;
{
#ifdef UNICOS
  if (!strcmp(type, "float"))
    unicos_write_float(ptr, nitems, stream);
  else if (!strcmp(type, "int"))
    unicos_write_int(ptr, nitems, stream);
  else if (!strcmp(type, "char"))
    fwrite(ptr, size, nitems, stream);
  else
    puts("XGWrite: unrecognized type");
#else
  if (theEndian == LITTLE) {
    fwrite(ptr, size, nitems, stream);
  } else {
    sun_write(ptr, size, nitems, stream);
  }
#endif
}

/******************************************************/

#ifdef UNICOS
unicos_read_float(ptr, nitems, stream)
  float          *ptr;
  FILE           *stream;
  int             nitems;
{
  unsigned long  *iptr;
  unsigned char   a, b, c, d, t;
  int             j;

  for (j = 0; j < nitems; j++) {
    iptr = (unsigned long *) (ptr + j);

    fread(&d, 1, 1, stream);
    fread(&c, 1, 1, stream);
    fread(&b, 1, 1, stream);
    fread(&a, 1, 1, stream);

    if (a == 0 && b == 0 && c == 0 && d == 0)
      *iptr = 0;
    else {
      t = ((a << 1) | (b >> 7));
      *iptr = ((a >> 7) << 63) | ((t - 126 + 040000) << 48);
      *iptr |= (b | 0x80) << 40;
      *iptr |= c << 32;
      *iptr |= d << 24;
    }
  }
}
#endif

/******************************************************/

#ifdef UNICOS
unicos_read_int(ptr, nitems, stream)
  int            *ptr;
  FILE           *stream;
  int             nitems;
{
  unsigned long  *iptr;
  unsigned char   a, b, c, d, t;
  int             j;

  for (j = 0; j < nitems; j++) {
    iptr = (unsigned long *) (ptr + j);

    fread(&a, 1, 1, stream);
    fread(&b, 1, 1, stream);
    fread(&c, 1, 1, stream);
    fread(&d, 1, 1, stream);

    *iptr = d << 24 | c << 16 | b << 8 | a;
    *iptr |= (d >> 7) ? 0xffffffff00000000 : 0x0;
  }
}
#endif

/******************************************************/

#ifdef UNICOS
unicos_write_float(ptr, nitems, stream)
  float          *ptr;
  FILE           *stream;
  int             nitems;
{
  unsigned long  *iptr;
  unsigned char   a, b, c, d, t;
  int             j;

  for (j = 0; j < nitems; j++) {
    iptr = (unsigned long *) (ptr + j);

    t = ((*iptr >> 48) & 0xff) - 040000 + 126;

    a = ((*iptr & (1 << 63)) ? 1 << 7 : 0) | t >> 1;
    b = (t & 0x1) << 7 | ((*iptr >> 40) & 0x7f);
    c = *iptr >> 32;
    d = *iptr >> 24;

    fwrite(&d, 1, 1, stream);
    fwrite(&c, 1, 1, stream);
    fwrite(&b, 1, 1, stream);
    fwrite(&a, 1, 1, stream);
  }
}
#endif

/******************************************************/

#ifdef UNICOS
unicos_write_int(ptr, nitems, stream)
  int            *ptr;
  FILE           *stream;
  int             nitems;
{
  unsigned long  *iptr;
  unsigned char   a, b, c, d, t;
  int             j;

  for (j = 0; j < nitems; j++) {
    iptr = (unsigned long *) (ptr + j);

    a = *iptr & 0xff;
    b = *iptr >> 8 & 0xff;
    c = *iptr >> 16 & 0xff;
    d = *iptr >> 24 & 0xff;
    fwrite(&a, 1, 1, stream);
    fwrite(&b, 1, 1, stream);
    fwrite(&c, 1, 1, stream);
    fwrite(&d, 1, 1, stream);
  }
}
#endif

/****************************************************************/

void RescaleTwoDWindow(WindowType theWindow)
{
  DataType ptr, data_Ptr = theWindow->data;
  LabelType label_Ptr = theWindow->label;

  int i,n;
  double temp;

  /***************************/
  /* AutoRescaling the x-axis */

  // fprintf(stdout, "\n");
  // fprintf(stdout, "x_auto: %d \n", label_Ptr->X_Auto_Rescale);
  // if (Is_X_AutoRescale(theWindow) && (n = *(theWindow->data->npoints))) {
  if ((label_Ptr->X_Auto_Rescale) && (n = *(theWindow->data->npoints))) {
    label_Ptr->X_Max = label_Ptr->X_Min = CurveX(data_Ptr,0);
    for (ptr = data_Ptr; ptr != NULL; ptr = ptr->next) {
      n = *(ptr->npoints);
      ptr->w = (SCALAR**)0;
      ptr->z = (SCALAR**)n;
      for (i = 0; i < n; i++) {
	label_Ptr->X_Max = max(label_Ptr->X_Max, CurveX(ptr,i));
	label_Ptr->X_Min = min(label_Ptr->X_Min, CurveX(ptr,i));
      }
    }
    if(label_Ptr->X_Scale < 0.0) {
      temp = label_Ptr->X_Max;
      label_Ptr->X_Max = label_Ptr->X_Min;
      label_Ptr->X_Min = temp;
    }

    label_Ptr->X_Max *= label_Ptr->X_Scale;
    label_Ptr->X_Min *= label_Ptr->X_Scale;

    if (Is_X_Log(theWindow))
      label_Ptr->X_Min = max(label_Ptr->X_Min, label_Ptr->X_Max / 1e6);
  } else if (!Is_X_AutoRescale(theWindow) && (n = *(theWindow->data->npoints))) {
    label_Ptr->X_Max /= label_Ptr->X_Scale;
    label_Ptr->X_Min /= label_Ptr->X_Scale;

    for (ptr = data_Ptr; ptr != NULL; ptr = ptr->next) {
      n = *(ptr->npoints);
      ptr->w = (SCALAR**)0;
      ptr->z = (SCALAR**)n;
      for (i = 0; i < n; i++) {
	if (label_Ptr->X_Min <= CurveX(ptr,i)) {
	  ptr->w = (SCALAR**)i;
	  break;
	}
      }
      for (; i<n; i++) {
	if (label_Ptr->X_Max < CurveX(ptr,i)) {
	  ptr->z = (SCALAR**)i;
	  break;
	} else if (label_Ptr->X_Max == CurveX(ptr,i)) {
	  ptr->z = (SCALAR**)(i+1);
	  break;
	}
      }
    }

    label_Ptr->X_Max *= label_Ptr->X_Scale;
    label_Ptr->X_Min *= label_Ptr->X_Scale;
  }
  if (Is_X_Log(theWindow)) {
    label_Ptr->X_Max = pow(10.0, ceil(log10(max(label_Ptr->X_Max, DBL_MIN))));
    label_Ptr->X_Min = pow(10.0, floor(0.01 + log10(max(label_Ptr->X_Min,
							DBL_MIN))));
  }

  /****************************/
  /* AutoRescaling the y-axis */

  // fprintf(stdout, "y_auto: %d \n", label_Ptr->Y_Auto_Rescale);
  // fprintf(stdout, "n: %d \n", *(theWindow->data->npoints) );
  // fprintf(stdout, "Is_Y_AutoRescale(theWindow): %d \n", Is_Y_AutoRescale(theWindow) );
  // if (Is_Y_AutoRescale(theWindow) && (n = *(theWindow->data->npoints))) {
  if ( (label_Ptr->Y_Auto_Rescale) && (n = *(theWindow->data->npoints))) {
    label_Ptr->Y_Max = label_Ptr->Y_Min = CurveY(data_Ptr, (int)data_Ptr->w);
    for (ptr = data_Ptr; ptr != NULL; ptr = ptr->next) {
      n = *(ptr->npoints);
      for (i = (int)(ptr->w); i < (int)(ptr->z); i++) {
	label_Ptr->Y_Max = max(label_Ptr->Y_Max, CurveY(ptr,i));
	label_Ptr->Y_Min = min(label_Ptr->Y_Min, CurveY(ptr,i));
      }
    }
    if(label_Ptr->Y_Scale < 0.0) {
      temp = label_Ptr->Y_Max;
      label_Ptr->Y_Max = label_Ptr->Y_Min;
      label_Ptr->Y_Min = temp;
    }
    label_Ptr->Y_Max *= label_Ptr->Y_Scale;
    label_Ptr->Y_Min *= label_Ptr->Y_Scale;
	 
    if (Is_Y_Log(theWindow))
      label_Ptr->Y_Min = max(label_Ptr->Y_Min, label_Ptr->Y_Max / 1e6);
  }
  if (Is_Y_Log(theWindow)) {
    label_Ptr->Y_Max = pow(10.0, ceil(log10(max(label_Ptr->Y_Max, DBL_MIN))));
    label_Ptr->Y_Min = pow(10.0, floor(0.01 + log10(max(label_Ptr->Y_Min,
							DBL_MIN))));
  }
}


void RescaleVectorDWindow(WindowType theWindow, int *ms, int *me, int *ns, 
			  int *ne)
{
  DataType data_Ptr = theWindow->data;
  LabelType label_Ptr = theWindow->label;
  int i,m,n,mstart,mend,nstart,nend;

  m = *(data_Ptr->mpoints);
  n = *(data_Ptr->npoints);

  /****************************/
  /* AutoRescaling the x-axis */

  if (Is_X_AutoRescale(theWindow)) {
    if (n) {
      mstart = 0;
      mend = m - 1;
      label_Ptr->X_Max = label_Ptr->X_Min = VectorX(data_Ptr,0);
      for (i = 1; i < m; i++) {
	label_Ptr->X_Max = max(label_Ptr->X_Max, VectorX(data_Ptr,i));
	label_Ptr->X_Min = min(label_Ptr->X_Min, VectorX(data_Ptr,i));
      }
    }
    label_Ptr->X_Max *= label_Ptr->X_Scale;
    label_Ptr->X_Min *= label_Ptr->X_Scale;
  } else {
    mstart = 0;
    while (VectorX(data_Ptr,mstart) * label_Ptr->X_Scale < label_Ptr->X_Min - DBL_MIN)
      mstart++;
    mend = m - 1;
    while (VectorX(data_Ptr,mend) * label_Ptr->X_Scale > label_Ptr->X_Max + DBL_MIN)
      mend--;
  }

  /****************************/
  /* AutoRescaling the y-axis */

  if (Is_Y_AutoRescale(theWindow)) {
    if (m) {
      nstart = 0;
      nend = n - 1;
      label_Ptr->Y_Max = label_Ptr->Y_Min = VectorY(data_Ptr,0);
      for (i = 1; i < n; i++) {
	label_Ptr->Y_Max = max(label_Ptr->Y_Max, VectorY(data_Ptr,i));
	label_Ptr->Y_Min = min(label_Ptr->Y_Min, VectorY(data_Ptr,i));
      }
    }
    label_Ptr->Y_Max *= label_Ptr->Y_Scale;
    label_Ptr->Y_Min *= label_Ptr->Y_Scale;
  } else {
    nstart = 0;
    while (VectorY(data_Ptr,nstart) * label_Ptr->Y_Scale < label_Ptr->Y_Min - DBL_MIN)
      nstart++;
    nend = n - 1;
    while (VectorY(data_Ptr,nend) * label_Ptr->Y_Scale > label_Ptr->Y_Max + DBL_MIN)
      nend--;
  }

  *ms = mstart;
  *me = mend;
  *ns = nstart;
  *ne = nend;
}


void RescaleThreeDWindow(WindowType theWindow, int *ms, int *me, int *ns, 
			 int *ne)
{
  DataType data_Ptr = theWindow->data;
  LabelType label_Ptr = theWindow->label;
  int i,j,n,m,mstart,mend,nstart,nend;

  /****************************/
  /* AutoRescaling the x-axis */
  
  if (!Is_Scatter(data_Ptr))
    m = *(data_Ptr->mpoints);
  n = *(data_Ptr->npoints);

  // if (Is_X_AutoRescale(theWindow)) {
  if (label_Ptr->X_Auto_Rescale) {
    if (Is_Scatter(data_Ptr)) {
      if (n) {
	label_Ptr->X_Max = label_Ptr->X_Min = ScatterX(data_Ptr,0);
	for (i = 1; i < n; i++) {
	  label_Ptr->X_Max = max(label_Ptr->X_Max, ScatterX(data_Ptr,i));
	  label_Ptr->X_Min = min(label_Ptr->X_Min, ScatterX(data_Ptr,i));
	}
	if(label_Ptr->X_Max == label_Ptr->X_Min) return;
      }
    } else if (Is_Irr_Surf(data_Ptr)) {
      if (n && m) {
	label_Ptr->X_Max = label_Ptr->X_Min = IrregularX(data_Ptr,0,0);
	for (i = 1; i < m; i++)
	  for (j = 0; j < n; j++) {
	    label_Ptr->X_Max = max(label_Ptr->X_Max, IrregularX(data_Ptr,i,j));
	    label_Ptr->X_Min = min(label_Ptr->X_Min, IrregularX(data_Ptr,i,j));
	  }
	  if(label_Ptr->X_Max == label_Ptr->X_Min) return;
      }
    } else if (m) {
      label_Ptr->X_Max = SurfaceX(data_Ptr,m - 1);
      label_Ptr->X_Min = SurfaceX(data_Ptr,0);
		if(label_Ptr->X_Max == label_Ptr->X_Min) return;
    }
    label_Ptr->X_Max *= label_Ptr->X_Scale;
    label_Ptr->X_Min *= label_Ptr->X_Scale;
    if (Is_X_Log(theWindow))
      label_Ptr->X_Min = max(label_Ptr->X_Min, 1e-6 * label_Ptr->X_Max);
  }
  if (Is_X_Log(theWindow)) {
    label_Ptr->X_Max = pow(10.0, ceil(log10(max(label_Ptr->X_Max, DBL_MIN))));
    label_Ptr->X_Min = pow(10.0, floor(0.01 + log10(max(label_Ptr->X_Min,
							DBL_MIN))));
  }
  if (!Is_Scatter(data_Ptr)) {
    if (Is_Irr_Surf(data_Ptr)) {
      for (i = 0, j = 0; j < n; j++) {
	while (label_Ptr->X_Min / label_Ptr->X_Scale > IrregularX(data_Ptr,i,j)
	       && i < m - 1)
	  i++;
	mstart = i;
      }
      for (i = m - 1, j = 0; j < n; j++) {
	while (label_Ptr->X_Max / label_Ptr->X_Scale < IrregularX(data_Ptr,i,j)
	       && i)
	  i--;
	mend = i + 1;
      }
    } else {
      mstart = m * (label_Ptr->X_Min / label_Ptr->X_Scale - SurfaceX(data_Ptr,0)) / (SurfaceX(data_Ptr,m-1) - SurfaceX(data_Ptr,0)) + .5;
      mend = m * (label_Ptr->X_Max / label_Ptr->X_Scale - SurfaceX(data_Ptr,0)) / (SurfaceX(data_Ptr,m-1) - SurfaceX(data_Ptr,0));
    }
    if (mstart < 0)
      mstart = 0;
    else if (mstart > m - 1)
      mstart = m - 1;
    if (mend < 0)
      mend = 0;
    else if (mend > m)
      mend = m;
  }
  /****************************/
  /* AutoRescaling the y-axis */

  // if (Is_Y_AutoRescale(theWindow)) {
  if (label_Ptr->Y_Auto_Rescale) {
    if (Is_Scatter(data_Ptr)) {
      if (n) {
	label_Ptr->Y_Max = label_Ptr->Y_Min = ScatterY(data_Ptr,0);
	for (i = 1; i < n; i++) {
	  label_Ptr->Y_Max = max(label_Ptr->Y_Max, ScatterY(data_Ptr,i));
	  label_Ptr->Y_Min = min(label_Ptr->Y_Min, ScatterY(data_Ptr,i));
	}
	if(label_Ptr->Y_Max == label_Ptr->Y_Min) return;
      }
    } else if (Is_Irr_Surf(data_Ptr)) {
      if (n && m) {
	label_Ptr->Y_Max = label_Ptr->Y_Min = IrregularY(data_Ptr,0,0);
	for (i = 1; i < m; i++)
	  for (j = 0; j < n; j++) {
	    label_Ptr->Y_Max = max(label_Ptr->Y_Max, IrregularY(data_Ptr,i,j));
	    label_Ptr->Y_Min = min(label_Ptr->Y_Min, IrregularY(data_Ptr,i,j));
	  }
	if(label_Ptr->Y_Max == label_Ptr->Y_Min) return;
      }
    } else if (n) {
      label_Ptr->Y_Max = SurfaceY(data_Ptr,n - 1);
      label_Ptr->Y_Min = SurfaceY(data_Ptr,0);
		if(label_Ptr->Y_Max == label_Ptr->Y_Min) return;
    }
    label_Ptr->Y_Max *= label_Ptr->Y_Scale;
    label_Ptr->Y_Min *= label_Ptr->Y_Scale;
    if (Is_Y_Log(theWindow))
      label_Ptr->Y_Min = max(label_Ptr->Y_Min, label_Ptr->Y_Max / 1e6);
 }
  if (Is_Y_Log(theWindow)) {
    label_Ptr->Y_Max = pow(10.0, ceil(log10(max(label_Ptr->Y_Max, DBL_MIN))));
    label_Ptr->Y_Min = pow(10.0, floor(0.01 + log10(max(label_Ptr->Y_Min,
							DBL_MIN))));
  }
  if (!Is_Scatter(data_Ptr)) {
    if (Is_Irr_Surf(data_Ptr)) {
      for (j = 0, i = 0; i < m; i++) {
	while (label_Ptr->Y_Min / label_Ptr->Y_Scale > IrregularY(data_Ptr,i,j)
	       && j < n - 1)
	  j++;
	nstart = j;
      }
      for (j = n - 1, i = 0; i < m; i++) {
	while (label_Ptr->Y_Max / label_Ptr->Y_Scale < IrregularY(data_Ptr,i,j)
	       && j)
	  j--;
	nend = j + 1;
      }
    } else {
      nstart = n * (label_Ptr->Y_Min / label_Ptr->Y_Scale - SurfaceY(data_Ptr,0)) / (SurfaceY(data_Ptr,n - 1) - SurfaceY(data_Ptr,0)) + .5;
      nend = n * (label_Ptr->Y_Max / label_Ptr->Y_Scale - SurfaceY(data_Ptr,0)) / (SurfaceY(data_Ptr,n - 1) - SurfaceY(data_Ptr,0));
    }
    if (nstart < 0)
      nstart = 0;
    else if (nstart > n - 1)
      nstart = n - 1;
    if (nend < 0)
      nend = 0;
    else if (nend > n)
      nend = n;
  }
  /****************************/
  /* AutoRescaling the z-axis */

  // if (Is_Z_AutoRescale(theWindow) && (n || m)) {
  if ((label_Ptr->Z_Auto_Rescale) && (n || m)) {
    if (Is_Scatter(data_Ptr)) {
      label_Ptr->Z_Max = label_Ptr->Z_Min = ScatterZ(data_Ptr,0);
      for (i = 1; i < n; i++) {
	label_Ptr->Z_Max = max(label_Ptr->Z_Max, ScatterZ(data_Ptr,i));
	label_Ptr->Z_Min = min(label_Ptr->Z_Min, ScatterZ(data_Ptr,i));
      }
    } else {
      label_Ptr->Z_Max = label_Ptr->Z_Min = SurfaceZ(data_Ptr,mstart,nstart);
      for (i = mstart; i < mend; i++)
	for (j = nstart; j < nend; j++) {
	  label_Ptr->Z_Max = max(label_Ptr->Z_Max, SurfaceZ(data_Ptr,i,j));
	  label_Ptr->Z_Min = min(label_Ptr->Z_Min, SurfaceZ(data_Ptr,i,j));
	}
    }
    label_Ptr->Z_Max *= label_Ptr->Z_Scale;
    label_Ptr->Z_Min *= label_Ptr->Z_Scale;
    if (Is_Z_Log(theWindow))
      label_Ptr->Z_Min = max(label_Ptr->Z_Min, label_Ptr->Z_Max / 1e6);
  }
  if (Is_Z_Log(theWindow)) {
    label_Ptr->Z_Max = pow(10.0, ceil(log10(max(label_Ptr->Z_Max, DBL_MIN))));
    label_Ptr->Z_Min = pow(10.0, floor(0.01 + log10(max(label_Ptr->Z_Min,
							DBL_MIN))));
  }

  *ms = mstart;
  *me = mend;
  *ns = nstart;
  *ne = nend;
}

void  XGSetupWindow(char *Win_Title, char *State, int ulx, int uly, int WinType,
		   DataType data_ptr, LabelType label_ptr, int PlotType)
{
  char type;
  if (!(theNewWindow = (WindowType) malloc(sizeof(struct window_struct)))) {
    puts("XGrafix(SetupWindow): Not enough memory to allocate a new window");
  }
 
  theNewWindow->ulxc = ulx;
  theNewWindow->ulyc = uly;
  theNewWindow->state = 0;
  theNewWindow->data = data_ptr;
  theNewWindow->label = label_ptr;
  theNewWindow->theta = init_theta;
  theNewWindow->phi = init_phi;
  // theNewWindow->pixmap_buffer = 0;
  theNewWindow->theMesh = 0;
  theNewWindow->structures = 0;
  theNewWindow->c1 = 0;
  theNewWindow->c2 = 0;
  theNewWindow->d1 = 0;
  theNewWindow->d2 = 0;
  theNewWindow->mstart = -1;
  theNewWindow->mend = -1;
  theNewWindow->nstart = -1;
  theNewWindow->nend = -1;
  theNewWindow->xSize = -1;
  theNewWindow->ySize = -1;
  theNewWindow->openFlag = 0;
 if(numberOfWindows > sizeOfWindowArray-1) {
    ReallocateWindows();
  }
 
  theWindowArray[numberOfWindows++] = theNewWindow;

// set for now theNewWindow->type = type = '2';
/*
  if (WinType == THREED) theNewWindow->type = type = '3';
  if (WinType == TWOD)   theNewWindow->type = type = '2';
*/
  if (WinType == THREED) theNewWindow->type = 3;
   type = '3';
  if (WinType == TWOD)   theNewWindow->type = 2;
   type = '2';

  if (!strcmp(State, "open")) theNewWindow->openFlag = 1;

  // if (!strcmp(State, "open")) *theNewWindow->openFlag = 1;
  // if (!strcmp(State, "open")) (*theNewWindow->openFlag) = 1;

#ifdef UNIX
 strcpy(theNewWindow->eps_file_name, theEPSFile);
/*
  printf("QtSetupWindow \"%s\" %s %d %d %c %.3g %.3g %d %.3g %.3g %d %.3g %.3g %d %s %.3g %.3g %d %d %d %d %d %d %s \"%s\" \"%s\"\n",
          Win_Title, State, ulx, uly, type, label_ptr->X_Min,
          label_ptr->X_Max, Is_X_AutoRescale(theNewWindow), label_ptr->Y_Min,
          label_ptr->Y_Max, Is_Y_AutoRescale(theNewWindow), label_ptr->Z_Min,
          label_ptr->Z_Max, Is_Z_AutoRescale(theNewWindow),
          theNewWindow->eps_file_name, init_theta, init_phi,
          Is_Shading_On(theNewWindow), Is_ColorCode_On(theNewWindow),
          Is_Grid_On(theNewWindow), Is_X_Log(theNewWindow),
          Is_Y_Log(theNewWindow), Is_Z_Log(theNewWindow), theDumpFile,
          label_ptr->X_Label, label_ptr->Y_Label);
*/
#endif  

}

  

LabelType 
SetupLabelStruct()
{
  LabelType       label;

  if (!(label = (LabelType) malloc(sizeof(struct label))))
    puts("Xgrafix.SetupLabelStruct: malloc error!");
  
  label->Z_Scale = 1;
  label->Z_Min = 0;
  label->Z_Max = 0;
  label->Z_Label = NULL;
  label->Z_Auto_Rescale = TRUE;

  label->Y_Scale = 1;
  label->Y_Min = 0;
  label->Y_Max = 0;
  label->Y_Label = NULL;
  label->Y_Auto_Rescale = TRUE;

  label->X_Scale = 1;
  label->X_Min = 0;
  label->X_Max = 0;
  label->X_Label = NULL;
  label->X_Auto_Rescale = TRUE;

  return (label);
}

DataType 
SetupDataStruct()
{
  DataType        data;

  if (!(data = (DataType) malloc(sizeof(struct data))))
    puts("Xgrafix.SetupDataStruct: malloc error!");

  data->z = NULL;
  data->zscat = NULL;
  data->y = NULL;
  data->x = NULL;
  data->mpoints = 0;
  data->npoints = 0;
  data->color = 0;
  data->state = 0;
  data->next = NULL;
  data->xCoord = 0;
  data->yCoord = 0;
  data->wArraySize = 1;
  data->xArraySize = 1;
  data->yArraySize = 1;
  data->zArraySize = 1;
  data->wArrayOffset = 0;
  data->xArrayOffset = 0;
  data->yArrayOffset = 0;
  data->zArrayOffset = 0;
  return (data);
}
void XGStart(){}
void ReallocateWindows()
{
  sizeOfWindowArray += sizeIncrement;
  theWindowArray = (WindowType *)realloc(theWindowArray,
                                         sizeof(struct window_struct)*
                                         sizeOfWindowArray);
  if(theWindowArray == NULL) {
    printf("Error: Not enough memory to reallocate new window array\n");
    exit(1);
  }
}

