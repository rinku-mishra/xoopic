//-----------------------------------------------------------------------
//
// File:	Oops.h
//
// Purpose:	Basic exception that one can add strings to
//
// Version:	$Id: oops.h 2296 2006-09-18 20:20:31Z yew $
//
// Copyright 1999-2001, Tech-X Corporation
//
//-----------------------------------------------------------------------

#ifndef OOPS_H
#define OOPS_H

// system includes
#include <stdio.h>
#include <string>
#include <sstream>

// From configure
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef _MSC_VER
#pragma warning ( disable: 4290) 
#endif

/** 
 * Exception holding a string.
 *
 */
class Oops {

  public:

/// Default constructor
    Oops();

/// Constructor taking a string.
    Oops(const std::string&);

/// Copy constructor
    Oops(const Oops&);

/// Destructor: virtual -- All resources released implicitly
    virtual ~Oops();

/// Assign from another of the same
    Oops& operator=(const Oops&);

/** Sets the message */
    Oops& operator=(const std::string& s) {
      msg = s;
      return *this;
    }


/** pre-pend */
    void prepend(const std::string &s);

/** Clear out the message */
    void clear() {
      msg = "";
    }

/** Returns a string of the accumulated messages */
    std::string getMessage() const {
      return msg;
    }

/** Appends a string */
    Oops& operator<<(std::string str);

  protected:

/// Output stream for debug info
    std::string msg;

  private:

// Prevent use

};

#endif
