/****************************************************************************
**
** Definition of GLPlot3D
** The GLPlot3D is a Qt OpenGL widget which creates an OpenGL plot context
** to plot 3D data 
**
** OPENGL must be defined at compilation for this code to be implemented.
**
** Copyright 1999  Tech-X Corporation
**
** Author: Kelly Luetkemeyer
**
**
****************************************************************************/

#ifndef GLPLOT3D_H
#define GLPLOT3D_H

#ifdef OPENGL

#include <qgl.h>

// XG Includes
#include "xgdatamacros.h"
#include "windowtype.h"
extern "C" void RescaleThreeDWindow(WindowType theWindow, int *ms, int *me, int *ns, int *ne);


/**
  --
  This class creates a Qt OpenGL widget to plot 3D data
 *
 *  Copyright 1998, 1999 by Tech-X Corporation
 *
 *  @author  Kelly G. Luetkemeyer
 *
 *  @version $Id: GLPlot3D.h 922 1999-02-10 22:09:12Z kgl $
 */


class GLPlot3D : public QGLWidget
{
    Q_OBJECT

public:

   /** This GLPlot3D constructor takes a QWidget w and a name
      nm and uses those to build the QWidget base class.  All other
      options must be set from member methods. */

    GLPlot3D( QWidget* parent, const char* name );

    /** Destroys the GLPlot3D widget */
    ~GLPlot3D();

    /** public method to set the OOPIC data */
    void		setData( char *Label, WindowType myWindow);

    /** public method to plot the 3D data */
    void		plotPoints();

public slots:

    /** public slot to set the X-Axis rotation */
    void		setXRotation( int degrees );

    /** public slot to set the Y-Axis rotation */
    void		setYRotation( int degrees );

    /** public slot to set the Z-Axis rotation */
    void		setZRotation( int degrees );

    /** public slot to set the eye translation */
    void		setTranslation ( int units);

protected:

    /** protected method to initialize OpenGL, required by QGLWidget */
    void		initializeGL();

    /** protected method to paint OpenGL, required by QGLWidget */
    void		paintGL();

    /** protected method to resize the OpenGL window, required by QGLWidget */
    void		resizeGL( int w, int h );

    /** protected method to make the OpenGL graphics object */
    virtual GLuint 	makeObject();

private:

    /** the reference to the OpenGL graphics object */
    GLuint object;
   
    /** the X, Y, Z axis rotation, the GL scale value, and the eye translation value */
    GLfloat xRot, yRot, zRot, scale, trans;

    /** the X,Y,Z axis min and max */
    double xmin, xmax, ymin, ymax, zmin, zmax;
 
    /** the plot icon title and plot label */
    char *thisLabel;
 
    /** the OOPIC data structure */
    WindowType thisWindow;

    /** the OOPIC data pointer */
    DataType        data_Ptr;

    /** the OOPIC label pointer */
    LabelType       label_Ptr;

};


#endif // OPENGL
#endif // GLPLOT3D_H
