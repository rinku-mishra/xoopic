////////////////////////////////////////////////////////
//
//  ButtonList.h:  
//
//  Header file describing a compound widget used to 
//  create a button list.  A button list is initially 
//  intended as the main window for qtoopic.  
//  Each button will be tied to a creation method for the button
//  exectution
//
//  Copyright 1998  Tech-X Corporation
//
//  CVS:  $Id: ButtonList.h 1294 2000-07-06 16:07:33Z bruhwile $
//
///////////////////////////////////////////////////////

// Qt includes
#include <qpushbt.h>
#include <qwidget.h>
#include <qmenubar.h>
#include <qpopmenu.h>
#include <qlayout.h>

// Create a compound widget.  
// This is used for the initial menubar and the list of buttons

/**
  -- This class creates a widget which manages a vertical stacking of 
     child widgets with a menu that allows the user to quit
 *
 *  Copyright 1998, 1999 by Tech-X Corporation
 *
 *  @author  Ryan Mclean and Kelly G. Luetkemeyer
 *
 *  @version $Id: ButtonList.h 1294 2000-07-06 16:07:33Z bruhwile $
 */

class ButtonList : public QFrame
{
  Q_OBJECT

public:

  /** The constructor takes the arguments parent and  nm are passed on to the QWidget base class 
      during creation.  The ButtonList constuctor builds the toplevel menu bar with the
      Quit option under the file menu and starts a layout manager for future widget additions. */
#ifdef UNIX
  ButtonList(QWidget *parent=0, const char *nm = 0, WFlags f = WType_TopLevel) ;
#else
  ButtonList(QWidget *parent=0, const char *nm = 0) ;
#endif

  /** Adds the given button to the area below the MenuBar. This call is forwarded to the layout */
  void addWidget(QWidget *w, int stretch = 5, int align = AlignCenter);  

  /** resize the widget, activate the layout, and make visible */
  void show();

  /** activates widget geometry management for the topLayout */
  bool activate();

protected:
    
  /**  menuBar is the QMenuBar that runs across the top of the ButtonList. */
  QMenuBar *menuBar;

  /**  file is the QPopupmenu that has the quit command quit. */
  QPopupMenu *file;

  /** layout is a QVBoxLayout that holds all of the child widgets. */
  QBoxLayout *layout;

  /** topLayout does geometry management for the menuBar and the layout.  */
  QBoxLayout *topLayout;


private:

  /** close and quit the window  */
  bool close( bool forceKill = FALSE) ;

private slots:

  // Queries the user if quit is desired
  bool quit();
 

};

