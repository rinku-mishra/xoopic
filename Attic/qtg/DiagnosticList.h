////////////////////////////////////////////////////////
//
//  DiagnosticList.h:  
//
//  Header file describing a compound widget used to 
//  create a Diagnostic selection widget. 
//
//  Copyright 1998  Tech-X Corporation
//  Author:  Kelly G. Luetkemeyer
//
//  CVS:  $Id: DiagnosticList.h 1294 2000-07-06 16:07:33Z bruhwile $
//
///////////////////////////////////////////////////////

// Qt includes
#include <qpushbt.h>
#include <qwidget.h>
#include <qmenubar.h>
#include <qpopmenu.h>
#include <qlayout.h>
#include <qlistbox.h>

// Txq includes
#include "MainButtons.h"


// Create a compound widget.  
// This is used for the initial menubar and the editor

/**
  -- This class creates the Diagnostic editor widget
 *
 *  Copyright 1998, 1999 by Tech-X Corporation
 *
 *  @author  Kelly G. Luetkemeyer
 *
 *  @version $Id: DiagnosticList.h 1294 2000-07-06 16:07:33Z bruhwile $
 */


class DiagnosticList : public QFrame
{
  Q_OBJECT

public:

  /** This DiagnosticList constructor takes a QWidget w and a name
      nm and uses those to build the QWidget base class.  All other
      options must be set from member methods. */

#ifdef UNIX
  DiagnosticList(QWidget *parent=0, const char *nm = 0, WFlags f = WType_TopLevel) ;
#else
  DiagnosticList(QWidget *parent=0, const char *nm = 0) ;
#endif

  /** This DiagnosticList constructor takes a MainButtons ml 
      to build the DiagnosticList base class.  
      The MainButtons are used to change the text of 
      Open / Close Diagnostic */

  DiagnosticList(MainButtons *ml);

  /** Adds the given button to the area below the MenuBar. 
      This call is forwarded to the layout */
  void addWidget(QWidget *w, int stretch = 5, int align = AlignCenter);  

  /** Adds the given button to the area below the add Widget. 
      This call is forwarded to the layout */
  void addHWidget(QWidget *w, int stretch = 0, int align = AlignCenter);  

  /** resize the widget, activate the layout, and make visible. */
  void show();

  /** activate teh layout manager. */
  bool activate();

  /** Public pointer to the Main widget buttons */
  MainButtons *btns;

  /** Public flag for the Open Close Diagnostic text */
  bool diagFlag;


protected:
    
  /**  menuBar is the QMenuBar that runs across the top of the DiagnosticList. */
  QMenuBar *menuBar;

  /**  file is the QPopupmenu that has the quit command quit. */
  QPopupMenu *file;

  /** layout is a QVBoxLayout that holds the editor. */
  QBoxLayout *layout;

  /** hozlayout holds the Open and Close All buttons */
  QBoxLayout *hozlayout;

  /** topLayout does geometry management for the menuBar and the layout.  */
  QBoxLayout *topLayout;

private:
  /** closeEvent will execute close */
  void closeEvent( QCloseEvent * );
  
private slots:

  /** close will reset the main buttons text */
  void close( ) ;
 

};

