////////////////////////////////////////////////////////
//
//  ButtonCmds.h:  Class that controls the exectution
//                 of the main window buttons
//
//  Copyright 1998  Tech-X Corporation
//
//  CVS: $Id: ButtonCmds.h 1294 2000-07-06 16:07:33Z bruhwile $
//
///////////////////////////////////////////////////////

#ifndef BUTTONCMDS_H
#define BUTTONCMDS_H

// Borland needs this
#ifndef UNIX
#define CINTERFACE
#endif
 
// Unix includes
#include <iostream.h>
extern "C" {
#include <stdio.h>
}


// Qt Includes
#include <qapplication.h>
#include <qwidget.h>
#include <qpushbutton.h>
#include <qobject.h>
#include <qtimer.h>
#include <qlabel.h>

// Txq includes
#include "xgdatamacros.h"
#include "windowtype.h"
#include "WindowList.h"
#include "DiagnosticList.h"
#include "MainButtons.h"

// Externals
extern double simulation_time;
extern "C++" void  XGMainLoop();
extern "C" WindowType *theWindowArray;


/**
  -- This class controls the buttons from the Main widget and the DiagnosticList widget
 *
 *  Copyright 1998, 1999 by Tech-X Corporation
 *
 *  @author  Kelly G. Luetkemeyer
 *
 *  @version $Id: ButtonCmds.h 1294 2000-07-06 16:07:33Z bruhwile $
 */

class ButtonCmds : public QWidget 
{
  Q_OBJECT

public:

    /** The TxqPlotBorder constructor takes a QWidget w and a name
      nm and uses those to build the QWidget base class.  All other
      options must be set from member methods. */

    ButtonCmds( QWidget *parent=0, const char *name=0 );

    /** This constructor takes a WindowList wl, DiagnosticList dl,
      and MainButtons mb to build the QWidget class. */
    ButtonCmds( WindowList *wl, DiagnosticList *dl, MainButtons *mb);

    /** Destructor kills the timer */
    virtual ~ButtonCmds();

    /** Start the  Qt Timer */
    void start();

    /** Step the timer */
    void step();

    /** kill the Qt timer */
    void stop();
   
    /** delete the widget */
    void destroy();

    /** the timer will do a plot step */
    void timerEvent( QTimerEvent * );

    /** public WindowList available for all methods */
    WindowList *winList;
  
    /** public DiagnosticList available for all methods */
    DiagnosticList *diagList;

    /** public MainButtons available for all methods */
    MainButtons *btns;

    /** flag to indicate state of Run text */
    bool runtext;

    /**  public int indicating the selected window index */
    int  winIndex;


private:

    /** the timer id number */
    int timer_id;


public slots:

  /** DoRun Executed after Run is pushed */
  void DoRun();

  /** Executed after Step is pushed */
  void DoStep();
  
  /** Executed after Open is pushed in diagnostic list window or double clicked
      index is the selected window number */
  void DoWindow(int index);

  /**  Executed after single click in DiagnosticList window */
  void SelectWindow(int index);

  /** Executed after Close Diagnostics is pushed */
  void DoDiag();

  /** Executed after Open is pushed in DiagnosticList */
  void OpenSelectWindow();

  /** Executed after Close ALl in DiagnosticList */
  void CloseAll();
};

#endif
