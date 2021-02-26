/*  Xgrafix mini-include-- excludes all the X stuff.  */


/****************************************/

static char *theDefaultDumpFile= "output.dmp";
static char *theDefaultEPSFile  = "output.eps";
static char *theDumpExtension  = ".dmp";
static char *theEPSExtension    = ".eps";
static char *theInputExtension = ".inp";

/**********************************************/
/* Definition of constants used in XGRAFIX    */

#ifndef TRUE
#define    TRUE                             1
#endif
#ifndef FALSE
#define    FALSE                            0
#endif

#define    MAX_BUFFER_SIZE                 512

#define    NULLTYPE                         0
#define    DOUBLE                           1
#define	   FLOAT			    3
#define	   INTEGER			    5
#define	   CHAR				    7

/******   Window states                 ****/
#define	   OPEN				    1
#define	   OPEN_BEFORE			    2
#define	   TRACE			    4
#define    Y_LOG                            8
#define    X_LOG                           16
#define    Y_AUTORESCALE		   32
#define    X_AUTORESCALE		   64
#define    Y_AUTORESCALE_BEFORE	          128
#define    X_AUTORESCALE_BEFORE	          256
#define    CROSSHAIR                      512
#define    Z_AUTORESCALE		 1024
#define    SHADING                       2048
#define    COLORCODE                  	 4096
#define    GRID                 	 8192
#define    THREED                       16384
#define    TWOD                             0
#define    VECD                         65536
#define    Z_LOG                        32768

/******   Data states                   ****/
#define    CURVE                            0
#define    SURFACE                          0
#define    SCATTER                          1
#define    IRR_SURFACE                      2
#define    VECTOR                           3

/******    Plot types                   ****/
#define    LIN_LIN                          0
#define    LIN_LOG                          1
#define    LOG_LOG                          2
#define    LOG_LIN                          3
#define    LIN_LIN_LIN                      4
#define    LIN_LIN_LOG                      5
#define    LIN_LOG_LIN                      6
#define    LIN_LOG_LOG                      7
#define    LOG_LIN_LIN                      8
#define    LOG_LIN_LOG                      9
#define    LOG_LOG_LIN                     10
#define    LOG_LOG_LOG                     11
#define    VEC_VEC                         12

/******  MAX NUMBER OF FORTRAN 3D PLOTS ****/
#define    MAX_3D_PLOTS                    50

#ifndef    DBL_MIN
#define	   DBL_MIN		       1E-200
#endif

/**********************************************/
/* Definition of structures used in XGRAFIX   */

/* data type for variable changer dealie, Payam 7.92 */

typedef struct special *SpecialType;
struct special {
  void            *data;
  char            Varname[20];
  int             Type;
};


typedef struct mesh_struct {
  SCALAR           x;
  SCALAR           y;
}               MeshType;


/********************************************/
/********************************************/
/* Global variables used in XGRAFIX         */

int             theRunFlag;
int             thePrevRunFlag;
int             theStepFlag;
int             theExitFlag;

double         *theTimeStep;

char            theCodeName[80];
char            theInputFile[80];
char            theDumpFile[80];
char            theEPSFile[80];

int             theNumberOfSteps;
int             theCurrentStep;
int             iterationsPerXUpdate;
int             theDumpPeriod;
int             WasDumpFileGiven;
int             WasInputFileGiven;
