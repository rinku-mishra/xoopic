////////////////////////////////////////////////////////
//
//  PlotWindow.h:   Header file
//		Inherits TxqPlotBorder
//		Creates a Plot Window for OOPIC
//		Has methods to plot data, axis, labels
//		Creates titles for window
//
// PlotWindow.h is needed for initwin.cpp
//  the same as TxqPlotWindow.h minus the xg header files
//  and the Unix header files
//
//  Copyright 1998  Tech-X Corporation
//
//  CVS: $Id: PlotWindow.h 872 1999-01-15 01:27:05Z kgl $
//
///////////////////////////////////////////////////////

#ifndef  PLOTWINDOW_H
#define  PLOTWINDOW_H
 
// Txq Includes
#include "TxqPlotBorder.h"

// Qt Includes
#include <qlabel.h>

extern QLabel      *timelabel;

class TxqPlotWindow: public TxqPlotBorder {
  Q_OBJECT

public:

  TxqPlotWindow( QWidget *, char *Label, WindowType myWindow);
  virtual ~TxqPlotWindow();

  void plotPoints();

  char *thisLabel;
  WindowType thisWindow;

 
private:
  void plotStructure();
};


#endif
