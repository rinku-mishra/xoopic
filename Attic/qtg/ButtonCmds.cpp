////////////////////////////////////////////////////////
//
//  ButtonCmds.cpp:  Class that executes the buttons
//
//  Copyright 1998  Tech-X Corporation
//  Author: Kelly G. Luetkemeyer
//
//  CVS: $Id: ButtonCmds.cpp 914 1999-02-08 22:23:54Z kgl $
//
///////////////////////////////////////////////////////
 
// Class definitions
#include "ButtonCmds.h"

/*!
 \class ButtonCmds ButtonCmds.h
 \brief The ButtonCmds widget controls the button on main widget
 */
 
/*!
  This constructor for ButtonCmds takes a parent widget \a w and a
name \a nm and passes them on to the base class QWidget.
*/

ButtonCmds::ButtonCmds( QWidget *parent, const char *name )
    : QWidget( parent, name )

{
   cout << "Entered buttoncmds " << endl; 
   runtext=0;
}

/*!
  This constructor for ButtonCmds takes a WindowList \a wl 
  a DiagnosticList \a name and a MainButtons \a wb 

  public class variables are assigned to these input pointers 
*/

ButtonCmds::ButtonCmds( WindowList *wl, DiagnosticList *dl, MainButtons *mb) : QWidget()
{
   winList = wl;
   diagList = dl;
   btns = mb;
}

/*!
  DoRun: Executed after Run is pushed
 */

void ButtonCmds::DoRun() 
{

   if (runtext) {
      btns->runButton->setText("Run");
           runtext = FALSE;
           stop();
   }
   else {
      btns->runButton->setText("Stop");
           runtext = TRUE;
           start();
   }
}

/*!
  DoStep: Executed after Step is pushed
 */

void ButtonCmds::DoStep() 
{
   char buf[512];
   
// Execute the physics loop
   XGMainLoop();
   sprintf(buf, "Time = %g ",simulation_time);

// Check size of Time = widget, adjust if necessary
   QSize oldSize = btns->timelabel->size();
   btns->timelabel->setText(buf);
   QSize newSize = btns->timelabel->sizeHint();
   if (newSize.width() > oldSize.width()) 
      btns->timelabel->setFixedSize( newSize );

// Plot points for opened windows only
   for (int i=0; i<winList->plotWindows.Size(); i++)  {
      if(*winList->TWODopenFlag[i]) {
         winList->plotWindows[i]->plotPoints();
      }
    }

#ifdef OPENGL

// plot 3D windows
  for (int i=0; i<winList->plot3DWindows.Size(); i++)  {
      if(*winList->THREEDopenFlag[i]) {
         winList->plot3DWindows[i]->plotPoints();
      }
    }
#endif

}

/*!
  DoWindow: Executed after Open is pushed in diagnostic list window
            or double clicked
 */

void ButtonCmds::DoWindow(int index) 
{

   QString str;
   str.sprintf( "List box item %d selected", index );
   cout << str << endl;


#ifdef OPENGL

   if (index >= winList->plotLabel.Size()) {
    cout << "Index is 3D" << endl;
    index = index - winList->plotWindows.Size();
    cout << "Opening " << winList->plot3DLabel[index] << endl;
    winList->plot3DWindows[index]->show();
    *winList->THREEDopenFlag[index] = 1;
    return;
   }
#endif

// Set open flag of requested window
   cout << "Opening " << winList->plotLabel[index] << endl;
   int allReadyOpen = 0;
   if (*winList->TWODopenFlag[index]) allReadyOpen=1;
   *winList->TWODopenFlag[index] = 1;

/*
   Get the number of windows opened
   compute this windows location: UL, UR, LL, LR
*/
   int ip=-1;
   int numberOfOpenPlotWindows=-1;
   int i;
   for (i=0; i<winList->plotWindows.Size(); i++) {
      if(*winList->TWODopenFlag[i]) {
         numberOfOpenPlotWindows++;
         if (numberOfOpenPlotWindows < 4) ip++;
         else { ip=0; numberOfOpenPlotWindows=0;}
      }
   }

/* Open the window and move it to correct location:
   0:  Upper Left
   1:  Upper Right
   2:  Lower left
   3:  Lower Right
*/
   if (allReadyOpen) return;
   else {
     winList->plotWindows[index]->show();
     switch(ip) {
     case 0: {
         winList->plotWindows[index]->move(200, 20);
         break;
       }
      case 1: {
         winList->plotWindows[index]->move(630, 20);
	 break;
       }
      case 2: {
         winList->plotWindows[index]->move(200, 375);
         break;
       }
      case 3: {
         winList->plotWindows[index]->move(630, 375);
         break;
       }
     }
   }

}

/*!
  SelectWindow: Executed after single click in DiagnosticList window
 */

void ButtonCmds::SelectWindow(int index) 
{
/* DEBUG INFORMATION
   cout << "Entered Select Window" << endl;
   QString str;
   str.sprintf( "List box item %d selected", index );
   cout << str << endl;
*/

// Set the public window index for this single clicked selection
   winIndex = index;

}

/*!
  OpenSelectWindow: Open selected window 
  Executed after Open is pushed in DiagnosticList
 */

void ButtonCmds::OpenSelectWindow() 
{
/* DEBUG INFORMATION
   cout << "Entered Open Select Window" << endl;
   QString str;
   str.sprintf( "List box item %d selected", winIndex);
   cout << str << endl;
*/

// Open the saved selected window

   DoWindow(winIndex);

}


/*!
  CloseAll: Executed after Close ALl in DiagnosticList
 */

void ButtonCmds::CloseAll()
{

   int i;

/*
  Check all 2D Plot windows 
  If the window flag is set to be opened, 
  then hide the window and set open flag to close
*/

   for (i=0; i<winList->plotWindows.Size(); i++) {
      if(*winList->TWODopenFlag[i]) {
        winList->plotWindows[i]->hide();
        *winList->TWODopenFlag[i] = 0;
       }
    }

/*
  Check all 3D Plot windows 
  If the window flag is set to be opened, 
  then hide the window and set open flag to close
*/

#ifdef OPENGL

   for (i=0; i<winList->plot3DWindows.Size(); i++) {
      if(*winList->THREEDopenFlag[i]) {
        winList->plot3DWindows[i]->hide();
        *winList->THREEDopenFlag[i] = 0;
       }
   }

#endif

}

/*!
  DoDiag: Executed after Close Diagnostics is pushed
 */

void ButtonCmds::DoDiag()
{
 
   if (diagList->diagFlag) {
      btns->diagButton->setText("Close Diagnostics");
      diagList->diagFlag = FALSE;
      diagList->show();
    }
    else {
      btns->diagButton->setText("Open Diagnostics");
      diagList->diagFlag = TRUE;
      diagList->hide();
    }
}


/*!
  start: start the plotting and Qt timer
*/

void ButtonCmds::start() {
 
   stop();
   timer_id = startTimer(0);
 
}

/*!
  Destructor: stop
*/

ButtonCmds::~ButtonCmds()
{
   debug( "Deleting ButtonCmds\n");
   stop();
}

/*!
  destroy: delete this widget
*/

void ButtonCmds::destroy()
{
  delete this;
}

/*!
  stop: stop and kill the timer
*/

void ButtonCmds::stop()
{
  killTimer( timer_id );
}

/*!
  timerEvent: do a step
*/

void ButtonCmds::timerEvent( QTimerEvent * e)
{
  ButtonCmds::DoStep();
}





