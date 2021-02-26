////////////////////////////////////////////////////////
//
//  WindowList.h:   List of window attribute Header file
//
//  Copyright 1998  Tech-X Corporation
//
//  CVS: $Id: WindowList.h 1294 2000-07-06 16:07:33Z bruhwile $
//
///////////////////////////////////////////////////////

#ifndef  WINDOWLIST_H
#define  WINDOWLIST_H

// Borland needs this
#ifndef UNIX
#define CINTERFACE
#endif
 
// Txq Includes
#include "TxqList.h"
#include "PlotWindow.h"
#include "Plot3DWindow.h"


/**
  -- This is a data class to hold public data required by ButtonCmds
 *   and CreateWindows
 *
 *  Copyright 1998, 1999 by Tech-X Corporation
 *
 *  @author  Kelly G. Luetkemeyer
 *
 *  @version $Id: WindowList.h 1294 2000-07-06 16:07:33Z bruhwile $
 */

class WindowList
{


public:

  /** Default constructor takes no arguements */
  WindowList();

  /** deletes class */
  virtual ~WindowList();

  /** public plotWindows contains the pointers to the created TxqPlotWindow */
  TxqList<TxqPlotWindow *>   plotWindows;

#ifdef OPENGL

  /** public plot3DWindows contains the pointers to the created TxqPlot3DWindow */
  TxqList<TxqPlot3DWindow *>   plot3DWindows;
#endif

  /** public plotType contains the TWOD or THREED plot type information */
  TxqList<int>               plotType;

  /** public TWODopenFlag pointer determines if the TWOD window is opened. */
  TxqList<int *>             TWODopenFlag;

  /** public THREEDopenFlag pointer determines of the THREED window is opened */
  TxqList<int *>             THREEDopenFlag;
  
  /** public plotLabel pointer is the name of the plot */
  TxqList<char *>            plotLabel;

  /** public plot3DLabel pointer is the name of the 3D plot */
  TxqList<char *>            plot3DLabel;


protected:

 
private:
};


#endif
