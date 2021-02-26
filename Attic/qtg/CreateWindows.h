////////////////////////////////////////////////////////
//
//  CreateWindows.h:   Header file
//
//  Copyright 1998  Tech-X Corporation
//
//  CVS: $Id: CreateWindows.h 1294 2000-07-06 16:07:33Z bruhwile $
//
///////////////////////////////////////////////////////

#ifndef  CREATEWINDOWS_H
#define  CREATEWINDOWS_H

// Borland needs this
#ifndef UNIX
#define CINTERFACE
#endif
 
// Txq Includes
#include "TxqPlotBorder.h"
#include "WindowList.h"

/**
  -- This class creates the TxqPlotWindows, given the input WindowType window array 
 *
 *  Copyright 1998, 1999 by Tech-X Corporation
 *
 *  @author  Kelly G. Luetkemeyer
 *
 *  @version $Id: CreateWindows.h 1294 2000-07-06 16:07:33Z bruhwile $
 */



class CreateWindows : public WindowList
{


public:


  /** The constructor requires the number of windows in the window array
      and the window array, then creates the TxqPlotWindow .
      Currently only the 2D windows are created */
  CreateWindows(int inNumberOfWindows,  WindowType *intheWindowArray);

  virtual ~CreateWindows();

};


#endif
