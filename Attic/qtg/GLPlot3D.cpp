/****************************************************************************
**
** Implementation of GLPlot3D
** The GLPlot3D is a Qt OpenGL widget which creates an OpenGL plot context
** to plot 3D data
**
** The Qt code is mostly borrowed from the qt/extensions/opengl/examples/glbox.cpp
**
** $Id: GLPlot3D.cpp 922 1999-02-10 22:09:12Z kgl $
**
****************************************************************************/

#include "iostream.h"
#include "xpoint.h"
#include "GLPlot3D.h"
//#include <malloc/malloc.h>
#define  DBL_MIN  1E-200
extern "C" {
#include <math.h>
}


#ifdef OPENGL


/*!
  Create a GLPlot3D widget
*/

GLPlot3D::GLPlot3D( QWidget* parent, const char* name )
    : QGLWidget( parent, name )
{
// set the class variables;
// default object rotation
    xRot = yRot = zRot = 0.0;		

// default object scale
    scale = 1.25;			

//  the GL object reference
    object = 0;

// Defalut object eye translation
    trans = -10.0;
}


/*!
  Release allocated resources
*/

GLPlot3D::~GLPlot3D()
{
    cout << "Deleting GLPlot3D: " << thisLabel << endl;
    // (this fails but in original code ) glDeleteLists( object, 1 );
}


/*!
  Paint the box with the OOPIC data . 
  The actual openGL commands for drawing are performed here.
*/

void GLPlot3D::paintGL()
{
    glClear( GL_COLOR_BUFFER_BIT );

    glLoadIdentity();
    glTranslatef( 0.0, 0.0, trans );
    glScalef( scale, scale, scale );

    glRotatef( xRot, 1.0, 0.0, 0.0 ); 
    glRotatef( yRot, 0.0, 1.0, 0.0 ); 
    glRotatef( zRot, 0.0, 0.0, 1.0 );

    glColor3f( 1.0, 1.0, 1.0 );
    glCallList( object );
}


/*!
  Set up the OpenGL rendering state, and define display list
*/

void GLPlot3D::initializeGL()
{
    glClearColor( 0.0, 0.0, 0.0, 0.0 ); // Let OpenGL clear to black
    object = makeObject();		// Generate an OpenGL display list
    glShadeModel( GL_FLAT );
}



/*!
  Set up the OpenGL view port, matrix mode, etc.
*/

void GLPlot3D::resizeGL( int w, int h )
{
    glViewport( 0, 0, (GLint)w, (GLint)h );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glFrustum( -1.0, 1.0, -1.0, 1.0, 5.0, 15.0 );
    glMatrixMode( GL_MODELVIEW );

// the glFrustum controls the viewing matrix.  these options did not work well.
// Later might want to size the Y-axis down to -.5 to .5 for OOPIC compatibility

    // glFrustum( 0.0, 1.0, 0.0, 1.0, 5.0, 15.0 );
    // glFrustum( -.5, .5, -.5, .5, 5.0, 15.0 );
    // glFrustum( xmin, xmax, ymin, ymax, zmin, zmax);
    // cout << " Frustum: min, max " 
    //   << xmin << " " << xmax << " " << ymin << " " << ymax << " " << zmin << " " << zmax << " " << endl; 
}


/*!
  Generate an OpenGL display list for the object to be shown, i.e. the box
*/

GLuint GLPlot3D::makeObject()
{	
// the OpenGL object
    GLuint list;

// OOPIC declarations
    MeshType      **theMeshPoint=thisWindow->theMesh, theBox[8];
    XPoint          thePolygon[5];
    int             m, n, mstart, mend, nend, nstart;
    int             i, j, k, x1, y1, x2, y2, colornumber;
    double          a1, a2, a3, b1, b2, b3;
    double          n1, n2, n3, v1, v2, v3;
    double          u1, u2, u3, up1, up2, up3;
    double          xtemp, ytemp, ztemp;

// Create the OpenGL lists
    list = glGenLists( 1 );
    glNewList( list, GL_COMPILE );
    glLineWidth( 2.0 );

/* setup the OOPIC data set
   the following code is mostly borrowed from xgrafix/xgpaint3d.c 
   in Paint_ThreeD_Window 
*/
    data_Ptr  = thisWindow->data;
    label_Ptr = thisWindow->label;

// get number of points in data set
    if (!Is_Scatter(data_Ptr)) {
      m = *(data_Ptr->mpoints);
    }
    else {
      m = *(data_Ptr->npoints);
    }
    n = *(data_Ptr->npoints);

    if(m<3 || n<2) {cout << "m, n error " << m << " " << n << endl; }

    // cout << "Number of m data points " << m << endl;
    // cout << "Number of n data points " << n << endl;

// setup the messh

    if (thisWindow->theMesh == NULL) {
      thisWindow->theMesh = (MeshType **) malloc(m * sizeof(MeshType *));
      theMeshPoint = thisWindow->theMesh;
      for (i = 0; i < m; i++) {
        theMeshPoint[i] = (MeshType *) malloc(n * sizeof(MeshType));
      }
 
      thisWindow->xSize = m;
      thisWindow->ySize = n;
    } else if ((m != thisWindow->xSize) || n != (thisWindow->ySize)) {
      for (i=0; i<thisWindow->xSize; i++) {
        free(theMeshPoint[i]);
      }
      free(theMeshPoint);
 
      thisWindow->theMesh = (MeshType **) malloc(m * sizeof(MeshType *));
      theMeshPoint = thisWindow->theMesh;
      for (i = 0; i < m; i++) {
        theMeshPoint[i] = (MeshType *) malloc(n * sizeof(MeshType));
      }
 
      thisWindow->xSize = m;
      thisWindow->ySize = n;
    }
 
    RescaleThreeDWindow(thisWindow,&mstart,&mend,&nstart,&nend);
 
// compute min/max
// (This error needs to be corrected! in Phase II)

    if (label_Ptr->Z_Max == label_Ptr->Z_Min ||
        label_Ptr->Y_Max == label_Ptr->Y_Min ||
        label_Ptr->X_Max == label_Ptr->X_Min)
      cout << "Error in min/max " << endl;;

    xmin = label_Ptr->X_Min;
    xmax = label_Ptr->X_Max;
    ymin = label_Ptr->Y_Min;
    ymax = label_Ptr->Y_Max;
    zmin = label_Ptr->Z_Min;
    zmax = label_Ptr->Z_Max;

    // cout << "xmin, xmax : " << xmin << " " << xmax << endl;
    // cout << "ymin, ymax : " << ymin << " " << ymax << endl;
    // cout << "zmin, zmax : " << zmin << " " << zmax << endl;

// Used for scaling the arrays before rotating 
 
    if (Is_X_Log(thisWindow)) {
      a1 = 1.0 / log10(max(label_Ptr->X_Max / (label_Ptr->X_Min + DBL_MIN),
                           DBL_MIN));
      b1 = log10(max(label_Ptr->X_Min / (label_Ptr->X_Scale+DBL_MIN),DBL_MIN))/
           log10(max(label_Ptr->X_Max / (label_Ptr->X_Min+DBL_MIN), DBL_MIN));
    } else {
      a1 = label_Ptr->X_Scale / (label_Ptr->X_Max - label_Ptr->X_Min +DBL_MIN);
      b1 = label_Ptr->X_Min / (label_Ptr->X_Max - label_Ptr->X_Min + DBL_MIN);
    }
    if (Is_Y_Log(thisWindow)) {
      a2 = 1.0 / log10(max(label_Ptr->Y_Max / (label_Ptr->Y_Min+DBL_MIN),
                           DBL_MIN));
      b2 = log10(max(label_Ptr->Y_Min/(label_Ptr->Y_Scale+DBL_MIN),DBL_MIN)) /
           log10(max(label_Ptr->Y_Max / (label_Ptr->Y_Min+DBL_MIN),DBL_MIN));
    } else {
      a2 = label_Ptr->Y_Scale / (label_Ptr->Y_Max - label_Ptr->Y_Min +DBL_MIN);
      b2 = label_Ptr->Y_Min / (label_Ptr->Y_Max - label_Ptr->Y_Min + DBL_MIN);
    }
    if (Is_Z_Log(thisWindow)) {
      a3 = 1.0 / log10(max(label_Ptr->Z_Max / (label_Ptr->Z_Min+DBL_MIN),
                           DBL_MIN));
      b3 = log10(max(label_Ptr->Z_Min/(label_Ptr->Z_Scale+DBL_MIN),DBL_MIN)) /
           log10(max(label_Ptr->Z_Max / (label_Ptr->Z_Min+DBL_MIN),DBL_MIN));
    } else {
      a3 = label_Ptr->Z_Scale / (label_Ptr->Z_Max - label_Ptr->Z_Min+DBL_MIN);
      b3 = label_Ptr->Z_Min / (label_Ptr->Z_Max - label_Ptr->Z_Min + DBL_MIN);
    }

    // cout << " a1, a2, a3 " << a1 << " " << a2 << " " << a3 << endl;
    // cout << " b1, b2, b3 " << b1 << " " << b2 << " " << b3 << endl;
    // cout << " x,y,z scale " << label_Ptr->X_Scale << " " << label_Ptr->Y_Scale << " " << label_Ptr->Z_Scale << endl;


// OOPIC min and max is between 0 and 1
// reset to be between -.5 and .5 for OpenGL visualization

    xtemp = a1 * xmin - b1;
    xmin = xtemp - .5;
    xtemp = a1 * xmax - b1;
    xmax = xtemp-.5;

    ytemp= a2 * ymin - b2;
    ymin = ytemp-.5;
    ytemp = a2 * ymax - b2;
    ymax = ytemp-.5;

    ztemp = a3 * zmin - b3;
    zmin = ztemp-.5;
    ztemp = a3 * zmax - b3;
    zmax = ztemp-.5;

    // cout << "xmin, xmax : " << xmin << " " << xmax << endl;
    // cout << "ymin, ymax : " << ymin << " " << ymax << endl;
    // cout << "zmin, zmax : " << zmin << " " << zmax << endl;


//  actual 3D data drawing

// do in Phase II the Scatter data plots 
    if (Is_Scatter(data_Ptr)) {
     cout << "Scatter data not implemented" << endl;
    } 
    else {
      for (i=0; i<m; i++) {
 
// this is not done correctly yet, do in Phase II

        if (Is_Irr_Surf(data_Ptr)) {
          for (j = 0; j < n; j++) {
            if (Is_X_Log(thisWindow))
              xtemp = max(a1 * log10(fabs(IrregularX(data_Ptr,i,j))+ DBL_MIN) -
                          b1,DBL_MIN);
            else
              xtemp = a1 * IrregularX(data_Ptr,i,j) - b1 -.5;
            if (Is_Y_Log(thisWindow))
              ytemp = max(a2 * log10(fabs(IrregularY(data_Ptr,i,j))+ DBL_MIN) -
                          b2,DBL_MIN);
            else
              ytemp = a2 * IrregularY(data_Ptr,i,j) - b2 -.5;
            if (Is_Z_Log(thisWindow))
              ztemp = max(a3 * log10(fabs(IrregularZ(data_Ptr,i,j))+ DBL_MIN) -
                          b3,DBL_MIN);
            else
              ztemp = a3 * IrregularZ(data_Ptr,i,j) - b3 -.5;
/* do this in phase II
            if (ztemp > 1.0)
              ztemp = 1.0;
            else if (ztemp < 0.0)
              ztemp = 0.0;
            theMeshPoint[i][j].x = u1 * xtemp + u2 * ytemp + u3 * ztemp;
            theMeshPoint[i][j].y = v1 * xtemp + v2 * ytemp + v3 * ztemp;
*/
          }
        } else {
          for (j = 0; j < n; j++) {
            if (Is_X_Log(thisWindow))
              xtemp = max(a1 * log10(fabs(SurfaceX(data_Ptr,i)+DBL_MIN)) -
                          b1,DBL_MIN);
            else {
              xtemp = a1 * SurfaceX(data_Ptr,i) - b1 -.5;
	      // cout << "xtemp, SurfaceX(data_Ptr,i) " << xtemp << " " << SurfaceX(data_Ptr,i) << endl;
            }
            if (Is_Y_Log(thisWindow))
              ytemp = max(a2 * log10(fabs(SurfaceY(data_Ptr,j)+DBL_MIN)) -
                          b2,DBL_MIN);
            else
	    {
              ytemp = a2 * SurfaceY(data_Ptr,j) - b2 -.5;
	      // cout << "ytemp, SurfaceY(data_Ptr,j) " << ytemp << " " << SurfaceY(data_Ptr,j) << endl;
 	    }
            if (Is_Z_Log(thisWindow))
              ztemp = max(a3 * log10(fabs(SurfaceZ(data_Ptr,i,j)+DBL_MIN)) -
                          b3,DBL_MIN);
            else
            {
              ztemp = a3 * SurfaceZ(data_Ptr,i,j) - b3 - .5;
	      // cout << "ztemp, SurfaceZ(data_Ptr,i,j) " << ztemp << " " << SurfaceZ(data_Ptr,i,j) << endl;
	      // cout << "ztemp, SurfaceZ(data_Ptr,i,j) ,a3, b3, zmin, zmax" << endl;
              // cout << ztemp << " " << SurfaceZ(data_Ptr,i,j) << " " << a3 << " " << b3 << " " << zmin << " " << zmax << endl;
            }
/**
            if (ztemp > 1.0)
              ztemp = 1.0;
            else if (ztemp < 0.0)
              ztemp = 0.0;
            theMeshPoint[i][j].x = u1 * xtemp + u2 * ytemp + u3 * ztemp;
            theMeshPoint[i][j].y = v1 * xtemp + v2 * ytemp + v3 * ztemp;
*/
            theMeshPoint[i][j].x = xtemp; 
            theMeshPoint[i][j].y = ytemp; 
            theMeshPoint[i][j].z = ztemp; 
	    // cout << "mesh.x " << theMeshPoint[i][j].x << endl;
	    // cout << "mesh.y " << theMeshPoint[i][j].y << endl;
          }
        }
      }
     }

// OpenGL draw
/*
  This part should be re-visited.  
  Hidden lines / surface should be implemented
  Is there a way to draw lines around QUADS with the double do-loop?
  Color implementation would go here
  Flags to turn on/off grid, surface, color here too 
*/

// Draw the QUADS first

     glBegin( GL_QUADS);
     glColor3f( 1.0, 1.0, 1.0 );
     for (i=0; i<m-1; i++) {
          for (j = 0; j < n-1; j++) {
            glVertex3f(  theMeshPoint[i][j].x,  theMeshPoint[i][j].y , theMeshPoint[i][j].z);
            glVertex3f(  theMeshPoint[i+1][j].x,  theMeshPoint[i+1][j].y , theMeshPoint[i+1][j].z);
            glVertex3f(  theMeshPoint[i+1][j+1].x,  theMeshPoint[i+1][j+1].y , theMeshPoint[i+1][j+1].z);
            glVertex3f(  theMeshPoint[i][j+1].x,  theMeshPoint[i][j+1].y , theMeshPoint[i][j+1].z);
          }
      }
      glEnd();

// Now draw the Lines around the QUADS

     for (i=0; i<m-1; i++) {
          for (j = 0; j < n-1; j++) {
            glBegin( GL_LINE_LOOP);
     	    glColor3f( 0.0, 0.0, 0.0 );
            glVertex3f(  theMeshPoint[i][j].x,  theMeshPoint[i][j].y , theMeshPoint[i][j].z);
            glVertex3f(  theMeshPoint[i+1][j].x,  theMeshPoint[i+1][j].y , theMeshPoint[i+1][j].z);
            glVertex3f(  theMeshPoint[i+1][j+1].x,  theMeshPoint[i+1][j+1].y , theMeshPoint[i+1][j+1].z);
            glVertex3f(  theMeshPoint[i][j+1].x,  theMeshPoint[i][j+1].y , theMeshPoint[i][j+1].z);
            glEnd();
          }
      }
      glColor3f( 1.0, 1.0, 1.0 );



// Draw the Box around the data
// (Revisit, see above, also need Text / Labels)

// x-y plane at zmin
    glBegin( GL_LINE_LOOP );
    glVertex3f(  xmax,  ymax, zmin );
    glVertex3f(  xmax, ymin,  zmin );
    glVertex3f(  xmin, ymin,  zmin );
    glVertex3f(  xmin, ymax,  zmin );
    glEnd();

 
// x-y plane at zmax
    glBegin( GL_LINE_LOOP );
    glVertex3f(  xmax,  ymax, zmax );
    glVertex3f(  xmax,  ymin, zmax );
    glVertex3f(  xmin,  ymin, zmax );
    glVertex3f(  xmin,  ymax, zmax );
    glEnd();
 
    glBegin( GL_LINES );
    glVertex3f(  xmax,  ymax, zmin );   glVertex3f(  xmax,  ymax, zmax );
    glVertex3f(  xmax,  ymin, zmin );   glVertex3f(  xmax,  ymin, zmax );
    glVertex3f(  xmin,  ymin, zmin );   glVertex3f(  xmin,  ymin, zmax );
    glVertex3f(  xmin,  ymax, zmin );   glVertex3f(  xmin,  ymax, zmax );
    glEnd();


    glEndList();

    return list;
}


/*!
  Set the rotation angle of the object to \e degrees around the X axis.
*/

void GLPlot3D::setXRotation( int degrees )
{
    xRot = (GLfloat)(degrees % 360);
    updateGL();
}


/*!
  Set the rotation angle of the object to \e degrees around the Y axis.
*/

void GLPlot3D::setYRotation( int degrees )
{
    yRot = (GLfloat)(degrees % 360);
    updateGL();
}


/*!
  Set the rotation angle of the object to \e degrees around the Z axis.
*/

void GLPlot3D::setZRotation( int degrees )
{
    zRot = (GLfloat)(degrees % 360);
    updateGL();
}

/*!
  Set the eye translation of the object 
*/

void GLPlot3D::setTranslation( int units)
{
    trans = (GLfloat)units;
    // cout << "new translation " << trans << endl;
    updateGL();
}

/*!
  Set the OOPIC data
*/

void GLPlot3D::setData(char *Label, WindowType myWindow) {

    thisLabel = Label;
    thisWindow = myWindow;
}

/*!
  Plot the data points
*/

void GLPlot3D::plotPoints() {

// delete the old list
    glDeleteLists(object, 1);

// this 'should' not be necessary, but without 2 or more windows
// are not plotted.  Qt docs do not require
    makeCurrent();

// re-make the object
    initializeGL();

// update and plot
    updateGL();
}
#endif

