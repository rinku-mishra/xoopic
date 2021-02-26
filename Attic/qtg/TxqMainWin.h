////////////////////////////////////////////////////////
//
//  TxqMainWin.cpp:  Creates Main Window button list
//
//  Copyright 1998  Tech-X Corporation
//
//  CVS: $Id: TxqMainWin.h 893 1999-01-23 00:13:20Z kgl $
//
///////////////////////////////////////////////////////

#ifndef TXQMAINWIN_H
#define TXQMAINWIN_H

// Unix includes
#include <iostream.h>
#include <stdlib.h>

// Qt Includes
#include <qapplication.h>
#include <qlabel.h>
#include <qtooltip.h>
 
// Txq Qt Includes
#include "ButtonCmds.h"

// External definitions
extern char  theInputFile[80];

// Member functions
void TxqMainWin(int argc, char **argv);

#endif
