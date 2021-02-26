//////////////////////////////////////////////////////////
//
//  Information.cpp
//
//  Source file describing a widget used to 
//  create an information widget to print out diagnostics
//  from OOPIC startup
//
//  Copyright 1998  Tech-X Corporation
//  Author:  Kelly G. Luetkemeyer
//
//  CVS: $Id: Information.cpp 1294 2000-07-06 16:07:33Z bruhwile $
//
//////////////////////////////////////////////////////////


#include "Information.h"

/*!
The Information widget prints messages
*/

/*!
  The arguments \a parent and \a nm are passed on to the QWidget base class during
creation.  
*/

#ifdef UNIX
Information::Information(QWidget *parent, const char *nm ,  WFlags f = WType_TopLevel) :
#else
Information::Information(QWidget *parent, const char *nm ) :
#endif
    QFrame(parent, nm) 
{

  // Set caption and geometry
  setPalette( QPalette( Qt::lightGray ) );
  setCaption("Information");
  setIconText("Information");
  setGeometry(3, 555, 190, 250);
  updateGeometry();

  // create layout manager
  // topLayout = new QVBoxLayout( this, 5 );
  topLayout = new QVBoxLayout( this );
  CHECK_PTR( topLayout );

  // create menubar, fill with file and close
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
/*
  layout = new QVBoxLayout();
  CHECK_PTR( layout );

  topLayout->addLayout( layout );
*/

   // Create the multi-line editor to display the stdout and stderr messages
   mle = new QMultiLineEdit( this, "multiLineEdit" );

   addWidget( mle);
   // (orig qt1.44) mle->setMinimumHeight(mle->fontMetrics().height()*3);
   mle->setFixedVisibleLines(12);
   mle->setFrameStyle( QFrame::Panel | QFrame::Plain );
   mle->setText("Qt OOPIC\n"
                 "Modifications copyrighted by Tech-X Corporation\n"
                 "http://www.techxhome.com\n");
   QToolTip::add( mle, "Information window" );

}

/*!
  This method prints \a input to the information editor
*/

void Information::write(char *input) {

     mle->append(input);
}

/*!
  This method adds the QWidget \a w to the list of child widgets
managed by the Information.  The arguments \a s and \a a are passed
onto the \link QBoxLayout addWidget \endlink method of the layout member.
*/

void Information::addWidget(QWidget *w, int stretch, int align )
{
  // Put the given widget in the row/column area
     // layout->addWidget( w, stretch, align );
     topLayout->addWidget( w, stretch, align );
}

/*! 
  activate activates widget geometry management for the topLayout
*/

bool Information::activate()
{
  return topLayout->activate();
}

/*!
  This opens and shows the widget
*/

void Information::show()
{
  // Start management with the widget at its smallest
  // This should set the widget to have minimum size, but still
  // be resizeable
  topLayout->activate();
  // resize( 190, 250);
  setGeometry(3, 555, 190, 250);
  updateGeometry(); 
  QWidget::show();
}

/* !
  A close event destroys the widget 
*/

void Information::close( ) {
  delete this;

/* Once the Main widget has open/close for 
    information, then use this.
    in order to not destroy but hide the widget
*/
  // hide();
}

void Information::closeEvent( QCloseEvent * )
{
  Information::close();
}

Information::~Information() {
  debug("Deleting Information Window");
}

