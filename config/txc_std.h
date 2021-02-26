//-----------------------------------------------------------------------------
//
// File:	txc_std.h
//
// Purpose:	Define configuration dependent variables
//
// Version: $Id: txc_std.h 1049 2003-09-23 20:25:12Z cary $
//
// Copyright (c) 1997-1999 by Tech-X Corporation.  All rights reserved.
//
//-----------------------------------------------------------------------------

// Include this file after your own config.h to get the definition
// of TXSTD correct.  Your configuration process should determine
// whether complex numbers are in namespace std.  If so, 
// HAVE_STD_COMPLEX should be set.

#ifndef TXC_CONFIG_H
#define TXC_CONFIG_H

// We are using complex to see if std works at all
#if defined(HAVE_STD_COMPLEX) || defined(__MWERKS__) || defined(WIN32)
  #define TXSTD std
#else
  #define TXSTD
#endif

#endif  // TXC_CONFIG_H
