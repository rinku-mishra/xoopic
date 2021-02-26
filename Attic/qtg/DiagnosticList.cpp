//////////////////////////////////////////////////////////
//
//  DiagnosticList.cpp
//
//  Source file describing a compound widget used to 
//  create a Diagnost list.  
//
//  Copyright 1998  Tech-X Corporation
//
//  CVS: $Id: DiagnosticList.cpp 1294 2000-07-06 16:07:33Z bruhwile $
//
//////////////////////////////////////////////////////////

// Qt includes
#include <qapp.h>
#include <qmsgbox.h>
#include <qlistbox.h>

#include "DiagnosticList.h"

/*!
The DiagnosticList widget contols the Diagnostic selection
*/

/*!
  The arguments \a parent and \a nm are passed on to the QWidget base class during
creation.  
*/

#ifdef UNIX
DiagnosticList::DiagnosticList(QWidget *parent, const char *nm , WFlags f = WType_TopLevel) :
#else
DiagnosticList::DiagnosticList(QWidget *parent, const char *nm ) :
#endif
    QFrame(parent, nm) 
{
}
DiagnosticList::DiagnosticList(MainButtons *ml) :
    QFrame() 
{
  btns = ml;
  diagFlag = FALSE;

  setPalette( QPalette( Qt::lightGray ) );
  setCaption("Diagnostics");
  setIconText("Diagnostics");
  // setGeometry(3, 350, 190, 250);
  setGeometry(3, 278, 190, 250);

  topLayout = new QVBoxLayout( this, 5 );
  CHECK_PTR( topLayout );

  menuBar = new QMenuBar(this);
  CHECK_PTR( menuBar );

  file = new QPopupMenu();
  CHECK_PTR( file );
  file->setPalette( QPalette( Qt::lightGray ) );

  file->insertItem( "&Close", this, SLOT( close() ) );

  // Put the submenus in the main menu
  menuBar->insertItem( "&File", file );

  // Put the menuBar on the layout
  topLayout->setMenuBar( menuBar );


  // Add a second layout to hold added buttons
  // Activate the layout
  layout = new QVBoxLayout();
  CHECK_PTR( layout );

  topLayout->addLayout( layout );

  // Add a third layout for the open buttons
  // activate
  hozlayout = new QHBoxLayout();
  CHECK_PTR( hozlayout );
  topLayout->addLayout( hozlayout );

}

/*!
  This method adds the QWidget \a w to the list of child widgets
managed by the DiagnosticList.  The arguments \a s and \a a are passed
onto the \link QBoxLayout addWidget \endlink method of the layout member.
*/
void DiagnosticList::addWidget(QWidget *w, int stretch, int align )
{
  // Put the given widget in the row/column area
     layout->addWidget( w, stretch, align );
}
void DiagnosticList::addHWidget(QWidget *w, int stretch, int align )
{
  // Put the given widget in the row/column area
     hozlayout->addWidget( w, stretch, align );
}

/*! 
  activate activates widget geometry management for the topLayout
*/
bool DiagnosticList::activate()
{
  return topLayout->activate();
}

/*!
  This draws the DiagnosticList onto the screen.
*/
void DiagnosticList::show()
{
  // Start management with the widget at its smallest
  // This should set the widget to have minimum size, but still
  // be resizeable
  // resize( 0, 0);
  // resize( 200, 250);
  topLayout->activate();
  // resize( 190, 250);
  setGeometry(3, 278, 190, 250);
  updateGeometry();
  QWidget::show();
}

void DiagnosticList::close( ) {
  hide();
  btns->diagButton->setText("Open Diagnostics");
  diagFlag =TRUE;
}

void DiagnosticList::closeEvent( QCloseEvent * )
{
  DiagnosticList::close();
}


