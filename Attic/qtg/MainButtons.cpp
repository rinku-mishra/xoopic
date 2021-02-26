////////////////////////////////////////////////////////
//
//  MainButtons.cpp:  Creates Main buttons
//
//  Copyright 1998  Tech-X Corporation
//
//  CVS: $Id: MainButtons.cpp 1012 2000-01-07 00:32:23Z kgl $
//
///////////////////////////////////////////////////////

// Includes
// #include "ButtonList.h"

#include "MainButtons.h"

MainButtons::MainButtons (QWidget *parent, const char *name) 
 : QWidget(parent, name)
{
}
MainButtons::MainButtons (ButtonList *bl ) : QWidget()
{

	cout << "Entered MainButtons" << endl;

// Make input file as label
	filelabel = new QLabel(" ", bl);
        filelabel->setText("  ");
	filelabel->setAlignment(AlignCenter);
	filelabel->setFixedSize( filelabel->sizeHint() );
	bl->addWidget(filelabel);

// Make Time= as label
	timelabel = new QLabel("Time = 0.0 e-00", bl);
        timelabel->setText("Time = 0.0 e-00");
        timelabel->setAlignment(AlignLeft);
        timelabel->setFixedSize( timelabel->sizeHint() );
        timelabel->setMargin(0);
        bl->addWidget(timelabel, AlignLeft);

// Create button "Close Diagnostics"
	diagButton = new QPushButton ("Close Diagnostics", bl);
	// diagButton->setMinimumSize( diagButton->sizeHint() );
	diagButton->setMinimumWidth( bl->width() );
        diagButton->setAutoDefault(FALSE);
        diagButton->setDefault(FALSE);
	bl->addWidget( diagButton );

// Create button "Run"
	runButton = new QPushButton ("Run", bl);
	// runButton->setMinimumSize( runButton->sizeHint() );
	runButton->setMinimumWidth( bl->width() );
        runButton->setAutoDefault(TRUE);
        runButton->setDefault(TRUE);
	bl->addWidget( runButton );

// Create button "Step"
	stepButton = new QPushButton ("Step", bl);
	//stepButton->setMinimumSize( stepButton->sizeHint() );
	stepButton->setMinimumWidth( bl->width() );
	bl->addWidget( stepButton );

// Create button "Save"
	saveButton = new QPushButton ("Save", bl);
	// saveButton->setMinimumSize( saveButton->sizeHint() );
	saveButton->setMinimumWidth( bl->width() );
	bl->addWidget( saveButton );

// Create button "Open Input File"
	inputButton = new QPushButton ("Open Input File", bl);
	// inputButton->setMinimumSize( inputButton->sizeHint() );
	inputButton->setMinimumWidth( bl->width() );
	bl->addWidget( inputButton );

// Create button "Special"
	specialButton = new QPushButton ("Special", bl);
	// specialButton->setMinimumSize( specialButton->sizeHint() );
	specialButton->setMinimumWidth( bl->width() );
	bl->addWidget( specialButton);

}
MainButtons::~MainButtons()
{
  debug( "Deleted MainButtons" );
}

