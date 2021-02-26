////////////////////////////////////////////////////////
//
//  TxqMainWin.cpp:  Creates Main Window button list
//
//  Copyright 1998  Tech-X Corporation
//
//  CVS: $Id: TxqMainWin.cpp 1023 2000-01-11 22:37:25Z kgl $
//
///////////////////////////////////////////////////////

// TxqMainWin Includes
#include "TxqMainWin.h"
#include "WindowList.h"
#include "MainButtons.h"

void TxqMainWin (QApplication *qtapp, int argc, char **argv, WindowList *wl) {

// Create the Button List
   ButtonList bl;

/* 
 Create the Buttons
   Make input file as label
   Make Time= as label
   Create button "Close Diagnostics"
   Create button "Run"
   Create button "Step"
   Create button "Save"
   Create button "Open Input File"
   Create button "Special"
   Create the Diagnostic list selector
*/

   MainButtons *ml = new MainButtons(&bl);

// set the input filename
   ml->filelabel->setText(theInputFile);
   ml->filelabel->setFixedSize(ml->filelabel->sizeHint());

// Create the Diagnostic List
   DiagnosticList *dl = new DiagnosticList(ml);

// Create the ButtonCmds 
   ButtonCmds *cmds = new ButtonCmds(wl,dl,ml); 

// Coneect Button clicks to correct slot
   QObject::connect( ml->diagButton, SIGNAL( clicked() ),  cmds, SLOT( DoDiag() ) );
   QObject::connect( ml->runButton, SIGNAL( clicked() ),  cmds, SLOT( DoRun() ) );
   QObject::connect( ml->stepButton, SIGNAL( clicked() ),  cmds, SLOT( DoStep() ) );

// Set this as main widget
   bl.setGeometry(0, 0, 190, 250);
   qtapp->setMainWidget( &bl);

// Create the Diagnostic Window 
   QListBox *lb = new QListBox( dl, "listBox" );

   for (int i=0; i<wl->plotLabel.Size(); i++) {
	QString str(wl->plotLabel[i]);
        lb->insertItem( str );
   }

#ifdef OPENGL
   for (int i=0; i<wl->plot3DLabel.Size(); i++) {
	QString str(wl->plot3DLabel[i]);
        lb->insertItem( str );
   }
#endif

   // lb->setFixedVisibleLines(wl->plotLabel.Size()-1);
   lb->setRowMode(lb->numRows());
   lb->setCurrentItem(0);
   // lb->setFixedVisibleLines(11);
   dl->addWidget(lb);
   QObject::connect( lb, SIGNAL( selected(int) ),     cmds, SLOT( DoWindow(int) ) );
   QObject::connect( lb, SIGNAL( highlighted(int) ),  cmds, SLOT( SelectWindow(int) ) );
   QToolTip::add( lb, "Single click selects, double click opens" );

   QPushButton *diagOpen = new QPushButton("Open",dl);
//   diagOpen->setMinimumSize(diagOpen->sizeHint());
   diagOpen->setFixedHeight(diagOpen->sizeHint().height());
   dl->addHWidget(diagOpen);

   QPushButton *diagClose = new QPushButton("Close All",dl);
   // diagClose->setMinimumSize(diagClose->sizeHint());
   diagClose->setFixedHeight(diagClose->sizeHint().height());
   dl->addHWidget(diagClose);

// connect the buttons
   QObject::connect( diagOpen,  SIGNAL( clicked() ),  cmds, 
     SLOT( OpenSelectWindow() ) );
   QObject::connect( diagClose, SIGNAL( clicked() ),  cmds, 
     SLOT( CloseAll() ) );


// Show the button list 
// Show the Diagnostic window 
// and enter qt event loop
   bl.show();
   dl->show();
   dl->move(0, 325);
   dl->updateGeometry();
   qtapp->exec();
}
