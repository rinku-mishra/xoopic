////////////////////////////////////////////////////////
//
//  MainButtons.h:  Creates Main buttons 
//
//  Copyright 1998  Tech-X Corporation
//
//  CVS: $Id: MainButtons.h 900 1999-01-26 21:59:26Z kgl $
//
///////////////////////////////////////////////////////

#ifndef MAINBUTTONS_H
#define MAINBUTTONS_H

// Unix includes
#include <iostream.h>
#include <stdlib.h>

// Qt Includes
#include <qapplication.h>
#include <qlabel.h>
#include <qtooltip.h>
 
// Txq Qt Includes
#include "ButtonList.h"

/**
  -- This class creates the main buttons for OOPIC
 *
 *  Copyright 1998, 1999 by Tech-X Corporation
 *
 *  @author  Kelly G. Luetkemeyer
 *
 *  @version $Id: MainButtons.h 900 1999-01-26 21:59:26Z kgl $
 */

class MainButtons : public QWidget
{
   Q_OBJECT

public:
   
  /** This MainButtons constructor takes a QWidget w and a name
      nm and uses those to build the QWidget base class.  All other
      options must be set from member methods. */

   MainButtons (QWidget *parent=0, const char *name=0) ;

  /** This MainButtons constructor takes a ButtonList bl.
      The ButtonList bl is the parent for the created buttons. */

   MainButtons (ButtonList *bl ) ;

   /** Destruction is done at Quit time */
   virtual ~MainButtons();

   /** public label for the input file */
   QLabel *filelabel; 
  
   /** public label Time = */
   QLabel *timelabel; 

   /** public button Close Diagnotics */
   QPushButton *diagButton;

   /** public button Run */
   QPushButton *runButton;

   /** public button Step */
   QPushButton *stepButton;

   /** public button Save */
   QPushButton *saveButton;
 
   /** public button Open Input File */
   QPushButton *inputButton;

   /** public button Special */
   QPushButton *specialButton;

};

   
#endif
