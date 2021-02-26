/****************************************************************************
**
** Definition of TxqPlot3DWindow widget class
** The TxqPlot3DWindow contains an OpenGL GLPlot3D and three sliders connected to
** the GLPlot3D's rotation slots and eye translation slots.
**
** OPENGL must be defined at compilation for this code to be implemented.
**
** Copyright 1999  Tech-X Corporation
**
** Author: Kelly Luetkemeyer
**
** $Id: TxqPlot3DWindow.h 1294 2000-07-06 16:07:33Z bruhwile $
**
****************************************************************************/

#ifndef TXQPLOT3DWINDOW_H
#define TXQPLOT3DWINDOW_H
#ifndef UNIX
#define CINTERFACE
#endif

#ifdef OPENGL

// Unix includes
#include <iostream.h>

#include <qwidget.h>

// XG Includes
#include "xgdatamacros.h"
#include "windowtype.h"
#include "Plot3D.h"

/**
  -- 
  This class creates a Qt widget to plot 3D data
 *
 *  Copyright 1998, 1999 by Tech-X Corporation
 *
 *  @author  Kelly G. Luetkemeyer
 *
 *  @version $Id: TxqPlot3DWindow.h 1294 2000-07-06 16:07:33Z bruhwile $
 */

class TxqPlot3DWindow : public QWidget
{
    Q_OBJECT

public:

    /** This TxqPlot3DWindow constructor takes a QWidget w and a name
      nm and uses those to build the QWidget base class.  All other
      options must be set from member methods. (not used currently) */

    TxqPlot3DWindow( QWidget* parent = 0, const char* name = 0 );

    /** This TxqPlot3DWindow constructor takes a char Label and 
      a WindowType myWindow and uses these to build the QWidget base class.  
      The Label is the name of the icon and window title, taken from the Z-Axis.
      myWindow is the XOOPIC WindowType */

    TxqPlot3DWindow( char *Label, WindowType myWindow);

    /** Destroys the TxqPlot3DWindow */
    virtual ~TxqPlot3DWindow();

    /** public method to plot the 3D OOPIC data values */
    void plotPoints();

private:
    
    /** closeEvent will hide the window */
    void closeEvent( QCloseEvent * );

    /** the public QtWidget that constructs an OpenGL graphics context */
    GLPlot3D * plot3D;

    /** the OOPIC data structure */
    WindowType thisWindow;

private slots:

    /** hides the window */
    bool close();
};


#endif // OPENGL
#endif // TXQPLOT3DWINDOW_H
