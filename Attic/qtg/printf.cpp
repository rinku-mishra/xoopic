////////////////////////////////////////////////////////
//
//  printf.cpp:  function that writes to the information
//               text editor
//
//  Copyright 1998  Tech-X Corporation
//  Author:  Kelly G. Luetkemeyer
//
//  CVS: $Id: printf.cpp 1294 2000-07-06 16:07:33Z bruhwile $
//
///////////////////////////////////////////////////////

/*!
  Include the Information widget definitions.
  Declare the global external inform pointer
  from main.cpp
 */

#include "Information.h"
#include <stdio.h>
extern "C++" Information *inform;

/*!
  The printf function writes the \a input to the text
  information editor window.
 
  Declare in C++ routines as:
  
  extern "C++" void printf(char *);

 */

void printf(char *input) {
   inform->write(input);
   puts(input);
}
