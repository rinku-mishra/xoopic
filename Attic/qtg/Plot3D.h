/****************************************************************************
**
** Definition of GLPlot3D
** This is a simple QGLWidget displaying an openGL wireframe box
** $Id: Plot3D.h 915 1999-02-08 22:32:57Z kgl $
**
****************************************************************************/

#ifndef GLPLOT3D_H
#define GLPLOT3D_H

#ifdef OPENGL

#include <qgl.h>

// XG Includes
/*
#include "xgdatamacros.h"
#include "windowtype.h"
*/
extern "C" void RescaleThreeDWindow(WindowType theWindow, int *ms, int *me, int *ns, int *ne);



class GLPlot3D : public QGLWidget
{
    Q_OBJECT

public:

    GLPlot3D( QWidget* parent, const char* name );
    ~GLPlot3D();
    void		setData( char *Label, WindowType myWindow);
    void                plotPoints();

public slots:

    void		setXRotation( int degrees );
    void		setYRotation( int degrees );
    void		setZRotation( int degrees );
    void		setTranslation ( int units);

protected:

    void		initializeGL();
    void		paintGL();
    void		resizeGL( int w, int h );

    virtual GLuint 	makeObject();

private:

    GLuint object;
    GLfloat xRot, yRot, zRot, scale, trans;
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
