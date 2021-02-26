//-----------------------------------------------------------------------------
//
// File:	txc_streams.h
//
// Purpose:	Fix system dependencies for streams.
//
// Version: $Id: txc_streams.h 1049 2003-09-23 20:25:12Z cary $
//
// Copyright (c) 1998, 1999 by Tech-X Corporation.  All rights reserved.
//
//-----------------------------------------------------------------------------

// Include this file after your own config.h to get the definition
// of TXSTRSTD correct.  Your configuration process should determine
// whether streams are in namespace std for your compiler and set
// HAVE_STD_STREAMS if so, and it should determine whether you have
// the file sstream, and set HAVE_SSTREAM if true.

#ifndef TXC_STREAMS_H
#define TXC_STREAMS_H

#if defined(HAVE_STD_STREAMS) 

#define TXSTRSTD std
#include <iostream>
#include <fstream>
#include <iomanip>

#ifdef HAVE_SSTREAM
#include <iosfwd>
#include <sstream>
#else
#include <strstream>
#endif

#else

#include <iomanip.h>
#include <strstream.h>
#include <iostream.h>
#include <fstream.h>
#define TXSTRSTD

#endif

#endif  // TXC_STREAMS_H
