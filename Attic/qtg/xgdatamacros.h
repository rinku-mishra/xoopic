/*********************************************************************/
/* Change the following typedef to the type desired: float or double */
/* Also change in xgrafix.h                                          */

#ifdef SCALAR_DOUBLE
typedef double SCALAR;
#else
typedef float SCALAR;
#endif


#define LINE_PLOT 1
#define SCATTER_PLOT 2
#define VECTOR_PLOT 3
#define SURFACE_PLOT 4
#define SCATTER3D 5
#define IRREGULAR3D 6



/*****************/
/* For 2D graphs */

#define CurveX(data,index) ((data)->x[(index)*(data)->xArraySize+(data)->xArrayOffset])
#define CurveY(data,index) ((data)->y[(index)*(data)->yArraySize+(data)->yArrayOffset])

/*********************/
/* For Vector graphs */

#define VectorX(data,index) ((data)->x[(index)*(data)->xArraySize+(data)->xArrayOffset])
#define VectorY(data,index) ((data)->y[(index)*(data)->yArraySize+(data)->yArrayOffset])
#define VectorW(data,index1,index2) ((data)->w[(index1)][(index2)*(data)->wArraySize+(data)->wArrayOffset])
#define VectorZ(data,index1,index2) ((data)->z[(index1)][(index2)*(data)->zArraySize+(data)->zArrayOffset])

/*************************/
/* For 3D surface graphs */

#define SurfaceX(data,index) ((data)->x[(index)*(data)->xArraySize+(data)->xArrayOffset])
#define SurfaceY(data,index) ((data)->y[(index)*(data)->yArraySize+(data)->yArrayOffset])
#define SurfaceZ(data,index1,index2) ((data)->z[(index1)][(index2)*(data)->zArraySize+(data)->zArrayOffset])

/***************************/
/* For 3D irregular graphs */

#define IrregularX(data,index1,index2) ((data)->x_irr[(index1)][(index2)*(data)->xArraySize+(data)->xArrayOffset])
#define IrregularY(data,index1,index2) ((data)->y_irr[(index1)][(index2)*(data)->yArraySize+(data)->yArrayOffset])
#define IrregularZ(data,index1,index2) ((data)->z[(index1)][(index2)*(data)->zArraySize+(data)->zArrayOffset])

/*************************/
/* For 3D scatter graphs */

#define ScatterX(data,index) ((data)->x[(index)*(data)->xArraySize+(data)->xArrayOffset])
#define ScatterY(data,index) ((data)->y[(index)*(data)->yArraySize+(data)->yArrayOffset])
#define ScatterZ(data,index) ((data)->zscat[(index)*(data)->zArraySize+(data)->zArrayOffset])

typedef struct data *DataType;
struct data {
  SCALAR**        w;
  SCALAR**        z;
  SCALAR*         zscat;
  SCALAR*         y;
  SCALAR**        y_irr;
  SCALAR*         x;
  SCALAR**        x_irr;
  SCALAR          xCoord;
  SCALAR          yCoord;
  int            *mpoints;
  int            *npoints;
  int             color;
  int             state;
  DataType        next;
  int             wArraySize;
  int             xArraySize;
  int             yArraySize;
  int             zArraySize;
  int             wArrayOffset;
  int             xArrayOffset;
  int             yArrayOffset;
  int             zArrayOffset;
};

typedef struct label *LabelType;
struct label {
  double          Z_Scale;
  double          Z_Min;
  double          Z_Max;
  char           *Z_Label;
  int             Z_Auto_Rescale;
  double          Y_Scale;
  double          Y_Min;
  double          Y_Max;
  char           *Y_Label;
  int             Y_Auto_Rescale;
  double          X_Scale;
  double          X_Min;
  double          X_Max;
  char           *X_Label;
  int             X_Auto_Rescale;
};


typedef enum { FILLED, HOLLOW } STRUCT_FILL;

typedef struct struct_object {
  int numberPoints;
  STRUCT_FILL fillFlag;
  int lineColor;
  int fillColor;
  double *x;
  double *y;
  struct struct_object *next;
}               StructType;


typedef enum { LITTLE, BIG } ENDIAN;
extern ENDIAN		theEndian;

/***************************************************/
/* Defining macros used in XGRAFIX macros         */

#define Is_Zeroth_Set(x)	(((x)&  1)  ==    1)
#define Is_First_Set(x)		(((x)&  2)  ==    2)
#define Is_Second_Set(x)	(((x)&  4)  ==    4)
#define Is_Third_Set(x)		(((x)&  8)  ==    8)
#define Is_Fourth_Set(x)	(((x)& 16)  ==   16)
#define Is_Fifth_Set(x)		(((x)& 32)  ==   32)
#define Is_Sixth_Set(x)		(((x)& 64)  ==   64)
#define Is_Seventh_Set(x)	(((x)&128)  ==  128)
#define Is_Eighth_Set(x)	(((x)&256)  ==  256)
#define Is_Ninth_Set(x)         (((x)&512)  ==  512)
#define Is_Tenth_Set(x)         (((x)&1024) == 1024)
#define Is_Eleventh_Set(x)      (((x)&2048) == 2048)
#define Is_Twelveth_Set(x)      (((x)&4096) == 4096)
#define Is_Thirteenth_Set(x)    (((x)&8192) == 8192)
#define Is_Fourteenth_Set(x)    (((x)&16384)==16384)
#define Is_Fifteenth_Set(x)     (((x)&32768)==32768)
#define Is_Sixteenth_Set(x)     (((x)&65536)==65536)

#define Turn_On(x, flag)	((x) |= (flag))
#define Turn_Off(x, flag) 	((x) &= ~(flag))

/*************************************************/
/* XGRAFIX  macros.                              */

#define Is_Open(object)			    Is_Zeroth_Set((object)->state)
#define Was_Open_Before(object)		    Is_First_Set((object)->state)
#define Is_Trace_On(object)		    Is_Second_Set((object)->state)

#define Is_Z_Log(object)		    Is_Fifteenth_Set((object)->state)
#define Is_Y_Log(object)		    Is_Third_Set((object)->state)
#define Is_X_Log(object)		    Is_Fourth_Set((object)->state)

#define Is_Y_AutoRescale(object)            Is_Fifth_Set((object)->state)
#define Is_X_AutoRescale(object)            Is_Sixth_Set((object)->state)

#define Is_Y_AutoRescale_Before(object)     Is_Seventh_Set((object)->state)
#define Is_X_AutoRescale_Before(object)     Is_Eighth_Set((object)->state)

#define Is_Crosshair_On(object)             Is_Ninth_Set((object)->state)

#define Is_Z_AutoRescale(object)            Is_Tenth_Set((object)->state)
#define Is_Shading_On(object)               Is_Eleventh_Set((object)->state)
#define Is_ColorCode_On(object)             Is_Twelveth_Set((object)->state)
#define Is_Grid_On(object)                  Is_Thirteenth_Set((object)->state)
#define Is_ThreeD(object)                   Is_Fourteenth_Set((object)->state)

#define Open(object)			    Turn_On((object)->state,  OPEN)
#define Iconify(object)			    Turn_Off((object)->state, OPEN)

#define Open_Before(object)		    Turn_On((object)->state,  OPEN_BEFORE)
#define Iconic_Before(object)		    Turn_Off((object)->state, OPEN_BEFORE)

#define Trace_On(object)		    Turn_On((object)->state,  TRACE)
#define Trace_Off(object)		    Turn_Off((object)->state, TRACE)

#define Set_Z_Log(object)		    Turn_On((object)->state,  Z_LOG)
#define Set_Y_Log(object)		    Turn_On((object)->state,  Y_LOG)
#define Set_X_Log(object)		    Turn_On((object)->state,  X_LOG)

#define Set_Z_Lin(object)		    Turn_Off((object)->state,  Z_LOG)
#define Set_Y_Lin(object)		    Turn_Off((object)->state,  Y_LOG)
#define Set_X_Lin(object)		    Turn_Off((object)->state,  X_LOG)

#define Y_AutoRescale(object)               Turn_On((object)->state,  Y_AUTORESCALE)
#define X_AutoRescale(object)               Turn_On((object)->state,  X_AUTORESCALE)
#define Y_AutoRescale_Before(object)        Turn_On((object)->state,  Y_AUTORESCALE_BEFORE)
#define X_AutoRescale_Before(object)        Turn_On((object)->state,  X_AUTORESCALE_BEFORE)

#define Y_AutoRescale_Off(object)           Turn_Off((object)->state, Y_AUTORESCALE)
#define X_AutoRescale_Off(object)           Turn_Off((object)->state, X_AUTORESCALE)
#define Y_AutoRescale_Off_Before(object)    Turn_Off((object)->state, Y_AUTORESCALE_BEFORE)
#define X_AutoRescale_Off_Before(object)    Turn_Off((object)->state, X_AUTORESCALE_BEFORE)


#define Z_AutoRescale(object)               Turn_On((object)->state,  Z_AUTORESCALE)
#define Z_AutoRescale_Off(object)           Turn_Off((object)->state, Z_AUTORESCALE)

#define Crosshair_On(object)                Turn_On((object)->state, CROSSHAIR)
#define Crosshair_Off(object)               Turn_Off((object)->state,CROSSHAIR)
#define Shading_On(object)                  Turn_On((object)->state,  SHADING)
#define Shading_Off(object)                 Turn_Off((object)->state, SHADING)

#define ColorCode_On(object)                Turn_On((object)->state,  COLORCODE)
#define ColorCode_Off(object)               Turn_Off((object)->state, COLORCODE)

#define Grid_On(object)                     Turn_On((object)->state,  GRID)
#define Grid_Off(object)                    Turn_Off((object)->state, GRID)

#define Set_ThreeD(object)                  Turn_On((object)->state,  THREED)

#define Is_Scatter(object)                  Is_Zeroth_Set((object)->state)
#define Set_Scatter(object)                 Turn_On((object)->state,  SCATTER)
#define Set_Vector(object)                  Turn_On((object)->state,  VECTOR)
#define Is_Vector(object)                   Is_Sixteenth_Set((object)->state)

#define Is_Irr_Surf(object)                 Is_First_Set((object)->state)
#define Set_Irr_Surf(object)                Turn_On((object)->state,  IRR_SURFACE)

#ifndef max
#define max(x, y)                           (((x) > (y)) ? (x) : (y))
#endif

#ifndef min
#define min(x, y)                           (((x) < (y)) ? (x) : (y))
#endif


// void XGWriteLabel(LabelType theLabel, FILE *fp);
void XGWriteStructures(StructType *s, const char *filename);

