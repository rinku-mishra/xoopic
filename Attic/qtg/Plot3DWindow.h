/****************************************************************************
**
** Definition of TxqPlot3DWindow widget class
** The TxqPlot3DWindow contains a GLPlot3D and three sliders connected to
** the GLPlot3D's rotation slots.
** $Id: Plot3DWindow.h 920 1999-02-09 23:37:43Z kgl $
**
****************************************************************************/

#ifndef PLOT3DWINDOW_H
#define PLOT3DWINDOW_H

#ifdef OPENGL

// Unix includes
#include <iostream.h>

#include <qwidget.h>
#include "Plot3D.h"


class TxqPlot3DWindow : public QWidget
{
    Q_OBJECT
public:
    TxqPlot3DWindow( QWidget* parent = 0, const char* name = 0 );
    TxqPlot3DWindow( char *Label, WindowType myWindow);
    virtual ~TxqPlot3DWindow();
    void plotPoints();
 
private:
    void closeEvent( QCloseEvent * );
    GLPlot3D * plot3D;
    WindowType thisWindow;

private slots:
    bool close();
};


#endif // OPENGL
#endif // PLOT3DWINDOW_H
