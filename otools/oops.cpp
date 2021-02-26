//-----------------------------------------------------------------------
//
// File:	oops.cpp
//
// Purpose:	Simple exception containing a string
//
// Version: $Id: oops.cpp 2294 2006-09-18 17:51:45Z yew $
//
// Copyright (c) 1999 by Tech-X Corporation.  All rights reserved.
//
//-----------------------------------------------------------------------

// txbase includes
#include <oops.h>

// default constructor
Oops::Oops() {
}

// constructor taking a string
Oops::Oops(const std::string& str) {
  msg = str;
}

// Copy constructor
Oops::Oops(const Oops& d) {
  msg = d.msg;
}

// Destructor: virtual -- All resources released implicitly
Oops::~Oops() {
}

// Assignment
Oops& Oops::operator=(const Oops& d) {
  if( this == &d ) return *this;
  msg = d.msg;
  return *this;
}

//
Oops& Oops::operator<<(std::string str) {
  msg +=  str;
  return *this;
}

void Oops::prepend(const std::string &s){
  std::string st = s;
  msg = st + msg; 
}
