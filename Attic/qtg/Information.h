////////////////////////////////////////////////////////
//
//  Information.h:  
//
//  Header file describing a widget used to 
//  create an Information window.  
//  The information window editor prints out stdout and
//  stderr messages from OOPIC
//
//  Copyright 1998  Tech-X Corporation
//  Author:  Kelly G. Luetkemeyer
//
//  CVS:  $Id: Information.h 1294 2000-07-06 16:07:33Z bruhwile $
//
///////////////////////////////////////////////////////

// Qt includes
#include <qpushbt.h>
#include <qwidget.h>
#include <qmenubar.h>
#include <qpopmenu.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qmultilinedit.h>


// Create a compound widget.  
// This is used for the initial menubar and the multiline editor

/**
  -- This class creates the Information text widget
 *   The information window editor prints out stdout and stderr messages from OOPIC
 *
 *  Copyright 1998, 1999 by Tech-X Corporation
 *
 *  @author  Kelly G. Luetkemeyer
 *
 *  @version $Id: Information.h 1294 2000-07-06 16:07:33Z bruhwile $
 */


class Information : public QFrame
{
  Q_OBJECT

public:

  /** This Information constructor takes a QWidget w and a name
      nm and uses those to build the QWidget base class.  All other
      options must be set from member methods. */

#ifdef UNIX
  Information(QWidget *parent=0, const char *nm = 0, WFlags f = WType_TopLevel) ;
#else
  Information(QWidget *parent=0, const char *nm = 0) ;
#endif

  /**  Add the widget to the layout */
  void addWidget(QWidget *w, int stretch = 5, int align = AlignCenter);  

  /**  show the widget */
  void show();

  /** activate the layout */
  bool activate();
  
  /**  write to the text area */
  void write(char *inut);

  /** destructor to kill the window */
  virtual ~Information();


protected:
    
  /**  menuBar is the QMenuBar that runs across the top of the Information. */
  QMenuBar *menuBar;

  /**  file is the QPopupmenu that has the Close command quit. */
  QPopupMenu *file;

  /** layout is a QVBoxLayout that holds all of the child widgets. */
  QBoxLayout *layout;

  /** topLayout does geometry management for the menuBar and the layout.  */
  QBoxLayout *topLayout;

  /** mle is the multi line editor widget containing the output text */
  QMultiLineEdit *mle;

private:

  /** closeEvent invokes close */
  void closeEvent( QCloseEvent * );
  
private slots:

  /** destroys the widget */
  void close( ) ;
 

};

