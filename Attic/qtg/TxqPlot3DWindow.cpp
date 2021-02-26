/****************************************************************************
**
** Implementation of TxqPlot3DWindow widget class
**
** The TxqPlot3DWindow contains an OpenGL GLPlot3D and three sliders connected to
** the GLPlot3D's rotation slots and eye translation slots.
**
** OPENGL must be defined at compilation for this code to be implemented.
**
** Copyright 1999  Tech-X Corporation
**
** Author: Kelly Luetkemeyer
**
** $Id: TxqPlot3DWindow.cpp 1017 2000-01-10 21:25:29Z kgl $
**
****************************************************************************/

// Qt Includes
#include <qpushbutton.h>
#include <qslider.h>
#include <qlayout.h>
#include <qframe.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qapplication.h>
#include <qkeycode.h>

// Header includee
#include "TxqPlot3DWindow.h"

// Define to implement this code
#ifdef OPENGL


/*!
  \class TxqPlot3DWindow TxqPlot3DWindow.h
  \brief The TxqPlot3DWindow is the base class for plotting 3D OOPIC data

   The TxqPlot3DWindow widgets implements an OpenGL Graphics context
   for plotting 3D data.  
*/

/*!
  This constructor for TxqPlot3DWindow takes a parent widget \a w and a
  name \a nm and passes them on to the base class QWidget.
  Not used currently. 
*/
 

TxqPlot3DWindow::TxqPlot3DWindow( QWidget* parent, const char* name )
    : QWidget( parent, name )
{
}


/*!
  This constructor for TxqPlot3DWindow takes a char * \a thisLabel  and a
  WindowType \a myWindow to construct the TxqPlot3DWindow.
  thisLabel is the label name for the contstructed window.
  myWindow is the data structure.
*/

TxqPlot3DWindow::TxqPlot3DWindow( char *thisLabel, WindowType myWindow )
    : QWidget( )
{

    // set thisWindow so that all class members may access
    thisWindow = myWindow;

    // Set the title
    setPalette( QPalette( Qt::lightGray ) );
    setCaption(thisLabel);
    setIconText(thisLabel);

    // Create top-level layout manager
    QHBoxLayout* hlayout = new QHBoxLayout( this, 20, 20, "hlayout");

    // Create a popup menu containing Close
    QPopupMenu *file = new QPopupMenu();
    file->setPalette( QPalette( Qt::lightGray ) );
    file->insertItem( "Close",  this, SLOT( close() ), CTRL+Key_Q );

    // Create a menu bar
    QMenuBar *m = new QMenuBar( this );
    m->setSeparator( QMenuBar::InWindowsStyle );
    m->insertItem("&File", file );
    hlayout->setMenuBar( m );

    // Create a layout manager for the sliders
    QVBoxLayout* vlayout = new QVBoxLayout( 20, "vlayout");
    hlayout->addLayout( vlayout );

    // Create a nice frame tp put around the openGL widget
    QFrame* f = new QFrame( this, "frame" );
    f->setFrameStyle( QFrame::Sunken | QFrame::Panel );
    f->setLineWidth( 2 );
    hlayout->addWidget( f, 1 );

    // Create a layout manager for the openGL widget
    QHBoxLayout* flayout = new QHBoxLayout( f, 2, 2, "flayout");

    // Create an openGL widget
    plot3D = new GLPlot3D( f, "glbox");
    plot3D->setData(thisLabel, myWindow);
    plot3D->setMinimumSize( 50, 50 );
    flayout->addWidget( plot3D, 1 );
    flayout->activate();

    // Create the three sliders; one for each rotation axis
    QSlider* x = new QSlider ( 0, 360, 60, 0, QSlider::Vertical, this, "xsl" );
    x->setTickmarks( QSlider::Left );
    x->setMinimumSize( x->sizeHint() );
    vlayout->addWidget( x );
    QObject::connect( x, SIGNAL(valueChanged(int)),plot3D,SLOT(setXRotation(int)) );

    QSlider* y = new QSlider ( 0, 360, 60, 0, QSlider::Vertical, this, "ysl" );
    y->setTickmarks( QSlider::Left );
    y->setMinimumSize( y->sizeHint() );
    vlayout->addWidget( y );
    QObject::connect( y, SIGNAL(valueChanged(int)),plot3D,SLOT(setYRotation(int)) );

    QSlider* z = new QSlider ( 0, 360, 60, 0, QSlider::Vertical, this, "zsl" );
    z->setTickmarks( QSlider::Left );
    z->setMinimumSize( z->sizeHint() );
    vlayout->addWidget( z );
    QObject::connect( z, SIGNAL(valueChanged(int)),plot3D,SLOT(setZRotation(int)) );

    // create the slider for the eye translation
    QSlider* t = new QSlider ( -15, -5, 1, -15, QSlider::Vertical, this, "tsl" );
    t->setTickmarks( QSlider::Left );
    t->setMinimumSize( x->sizeHint() );
    vlayout->addWidget( t );
    QObject::connect( t, SIGNAL(valueChanged(int)),plot3D,SLOT(setTranslation(int)) );

    // Start the geometry management
    hlayout->activate();
}

/*!
  Destroys the 3D Plot and OpenGL context.
  Destruction is done at Exit time by Qt
*/

TxqPlot3DWindow::~TxqPlot3DWindow()
{
    cout << "Deleting " << thisWindow->label->Z_Label << endl;
}


/*!
  A Txq3DWindow responds to the closeEvent by hiding itself.  
  The openFlag of this window is set to false.
*/

bool TxqPlot3DWindow::close() 
{
    cout << "Closing " << thisWindow->label->Z_Label << endl;
    thisWindow->openFlag = 0;
    hide();
}

/*!
  A Txq3DWindow responds to the closeEvent by hiding itself.  
  The openFlag of this window is set to false.
*/

void TxqPlot3DWindow::closeEvent( QCloseEvent * )
{
    cout << "Closing " << thisWindow->label->Z_Label << endl;
    thisWindow->openFlag = 0;
    hide();
}

/*!
  This method forwards to the OpenGL graphics context (GLPlot3D) * plot3D 
*/

void TxqPlot3DWindow::plotPoints()
{
    plot3D->plotPoints();
}	

#endif
