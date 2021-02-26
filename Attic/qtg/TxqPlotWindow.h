////////////////////////////////////////////////////////
//
//  TxqPlotWindow.h:   Header file
//		Inherits TxqPlotBorder
//		Creates a Plot Window for OOPIC
//		Has methods to plot data, axis, labels
//		Creates titles for window
//
//  Copyright 1998  Tech-X Corporation
//
//  CVS: $Id: TxqPlotWindow.h 1294 2000-07-06 16:07:33Z bruhwile $
//
///////////////////////////////////////////////////////

#ifndef  TXQPLOTWINDOW_H
#define  TXQPLOTWINDOW_H
 
// Unix includes
#include <iostream.h>
#include <stdlib.h>
#ifdef UNIX
#include <unistd.h>
#endif
extern "C" {
#include <stdio.h>
}

// XG Includes
#include "xgdatamacros.h"
#include "windowtype.h"

// Txq Includes
#include "TxqPlotBorder.h"

// Qt Includes
#include <qlabel.h>

extern "C" void RescaleTwoDWindow(WindowType theWindow);

/**
  --
  This class creates a Qt Plot widget used for OOPIC
 *
 *  Copyright 1998, 1999 by Tech-X Corporation
 *
 *  @author  Kelly G. Luetkemeyer
 *
 *  @version $Id: TxqPlotWindow.h 1294 2000-07-06 16:07:33Z bruhwile $
 */

class TxqPlotWindow: public TxqPlotBorder {
  Q_OBJECT

public:

  /** The TxqPlotWindow constructor takes a QWidget w and a name
      nm and uses those to build the QWidget base class.  All other
      options must be set from member methods. */

  TxqPlotWindow( QWidget *, char *Label, WindowType myWindow);

  /** Destroys the TxqPlotWindow */
  virtual ~TxqPlotWindow();

  /** public method to plot the OOPIC data values and structures */
  void plotPoints();

  /** this method should overload the TxqPlotBorder method */
  void TxqPlotWindow::getYTicks( TxqList<double> &ticks );

 
private:
  
  /** the plot icon title and plot label */
  char *thisLabel;

  /** the OOPIC data structure */
  WindowType thisWindow;

  /** method to plot the structure(s) */
  void plotStructure();

  /** closeEvent will hide the window */
  void closeEvent( QCloseEvent * );
};


#endif
