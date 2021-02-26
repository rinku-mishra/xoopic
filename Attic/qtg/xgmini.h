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

void XGWriteLabel(LabelType theLabel, FILE *fp);
void XGWriteStructures(StructType *s, const char *filename);

void BXG_Write_2D(DataType theData,LabelType theLabel,StructType *structs,const char *filename, char type, double *theTimeStep);

void BXG_Write_3D(DataType theData, LabelType theLabel,const char *filename, char type, double *theTimeStep);

void  BXG_Write_Vector(DataType theData,LabelType theLabel, StructType *structs,const char *filename,char type, double *theTimeStep);




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

extern void            XGStart();
/* extern void            XGInit(int argc, char **argv, double *t); */
