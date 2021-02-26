////////////////////////////////////////////////////////
//
//  CreateWindows.cpp:  Class that creates the plot windows
//
//  Copyright 1998  Tech-X Corporation
//
//  CVS: $Id: CreateWindows.cpp 1294 2000-07-06 16:07:33Z bruhwile $
//
///////////////////////////////////////////////////////

// Includes

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

#include "CreateWindows.h"


CreateWindows::CreateWindows(int numberOfWindows,  WindowType *theWindowArray): WindowList()
{
   int i;

// Loop thru all windows

   for (i=0; i<numberOfWindows; i++) {

// save the window type
     plotType.Append(theWindowArray[i]->type);

// construct the window if 2D
     if (theWindowArray[i]->type == 2)  {
      TxqPlotWindow *p=  
         new TxqPlotWindow (0, theWindowArray[i]->label->Y_Label, theWindowArray[i]);

// save the open flag, and the TxqPlotWindow pointer and the label
      TWODopenFlag.Append(&theWindowArray[i]->openFlag);
      plotWindows.Append(p);
      plotLabel.Append(theWindowArray[i]->label->Y_Label);
     }

// construct the window if 3D

#ifdef OPENGL

     if (theWindowArray[i]->type == 3)  {
      cout << "3dWindow : " << theWindowArray[i]->label->Z_Label << endl;

      TxqPlot3DWindow *w = 
         new TxqPlot3DWindow(theWindowArray[i]->label->Z_Label, theWindowArray[i]);
      w->resize(400, 350);
      if(theWindowArray[i]->openFlag) w->show();

      plot3DWindows.Append(w);
      plot3DLabel.Append(theWindowArray[i]->label->Z_Label);
      THREEDopenFlag.Append(&theWindowArray[i]->openFlag);
     }

#endif

   }  // end of for loop

// Compute the number of open windows and move the just opened windows accordingly
   int ip=-1;
   int numberOfOpenPlotWindows=-1;
   for (i=0; i<plotWindows.Size(); i++) {
 	if(*TWODopenFlag[i]) {
         numberOfOpenPlotWindows++;
         if (numberOfOpenPlotWindows < 4) ip++;
         else
          ip=0;
	 if (ip==0) plotWindows[i]->move(200, 20);
	 if (ip==1) plotWindows[i]->move(630, 20);
	 if (ip==2) plotWindows[i]->move(200, 375);
	 if (ip==3) plotWindows[i]->move(630, 375);
       }
    }
}

CreateWindows::~CreateWindows()
{
  debug( "Deleted CreateWindows" );
}
 


