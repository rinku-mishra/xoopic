//////////////////////////////////////////////////////////
//
//  ButtonList.cpp
//
//  Source file describing a compound widget used to 
//  create a button list.  A button list is initially 
//  intended as the main window for qtoopic.  
//
//  Copyright 1998  Tech-X Corporation
//
//  CVS: $Id: ButtonList.cpp 1294 2000-07-06 16:07:33Z bruhwile $
//
//////////////////////////////////////////////////////////

// Qt includes
#include <qapp.h>
#include <qmsgbox.h>

#include "ButtonList.h"

/*!
  \class ButtonList ButtonList.h
  \brief  The widget manages a vertical stacking of child widgets with a menu that
allows the user to quit.

The ButtonList is used in mapa.C to create a button for each possible TxID that 
can be created.  (Add reference showing mapa.C here).

*/

/*!
  The arguments \a parent and \a nm are passed on to the QWidget base class during
creation.  The ButtonList constuctor builds the toplevel menu bar with the 
Quit option under the file menu and starts a layout manager for future widget 
additions.
*/

#ifdef UNIX
ButtonList::ButtonList(QWidget *parent, const char *nm ,WFlags f = WType_TopLevel) :
#else
ButtonList::ButtonList(QWidget *parent, const char *nm ) :
#endif
    QFrame(parent, nm)
    // QWidget(parent, nm) 
{
  setPalette( QPalette( Qt::lightGray ) );

  setCaption("Control Panel");
  setIconText("Control Panel");

  // setGeometry(3, 70, 190, 250);
  setGeometry(3, 3, 190, 250);

  topLayout = new QVBoxLayout( this, 5 );
  // (default )topLayout = new QVBoxLayout( this );
  CHECK_PTR( topLayout );

  menuBar = new QMenuBar(this);
  CHECK_PTR( menuBar );

  file = new QPopupMenu();
  CHECK_PTR( file );
  file->setPalette( QPalette( Qt::lightGray ) );

  // This should perhaps be replaced by a popup asking
  // if we are sure we want to quit
  file->insertItem( "&Quit", this, SLOT( quit() ) );

  // Put the submenus in the main menu
  menuBar->insertItem( "&File", file );

  // Put the menuBar on the layout
  topLayout->setMenuBar( menuBar );


  // Add a second layout to hold added buttons
  // Activate the layout
  layout = new QVBoxLayout();
  CHECK_PTR( layout );

  topLayout->addLayout( layout );

}

/*!
  This method adds the QWidget \a w to the list of child widgets
managed by the ButtonList.  The arguments \a s and \a a are passed
onto the \link QBoxLayout addWidget \endlink method of the layout member.
*/
void ButtonList::addWidget(QWidget *w, int stretch, int align )
{
  // Put the given widget in the row/column area
     layout->addWidget( w, stretch, align );
}

/*! 
  activate activates widget geometry management for the topLayout
*/
bool ButtonList::activate()
{
  return topLayout->activate();
}

/*!
  This draws the ButtonList onto the screen.
*/
void ButtonList::show()
{
  // Start management with the widget at its smallest
  // This should set the widget to have minimum size, but still
  // be resizeable
  // resize( 0, 0);
  // resize( 200, 250);
  // resize( 190, 250);
  topLayout->activate();
  setGeometry(3, 3, 190, 250);
  updateGeometry();
  QWidget::show();
}

bool ButtonList::close( bool forceKill ) {
  if (forceKill == TRUE) {
    QWidget::close( forceKill );
    return TRUE;
  }
  else return quit();
}

bool ButtonList::quit()
{
  switch( QMessageBox::information( this, 
				    name(),
				    "Really quit?", 
				    QMessageBox::Ok | QMessageBox::Default,
				    QMessageBox::Cancel | QMessageBox::Escape 
				    ) )
				    
    {
    case QMessageBox::Ok:
      qApp->quit();
      return TRUE;
      break;
    case QMessageBox::Cancel:
      // Cancel clicked, do nothing
      return FALSE;
      break;
    }
}
  
