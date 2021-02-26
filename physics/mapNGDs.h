#ifndef	__MAPNGDS_H
#define	__MAPNGDS_H

//----------------------------------------------------------------------------
//
// File:        mapNGDs.h
//
// Purpose:     A class to handle the mapping of the Neutral Gas
//              Densities when passing MPI messages. The NGDs are
//              accessed via a pointer to an NGD list. It is
//              currently implemented only for right moving window. 
//
// Version:    $Id: mapNGDs.h 2295 2006-09-18 18:05:10Z yew $ 
//
// Copyright 2001 by Tech-X Corporation
//
//----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if defined(_MSC_VER)
#include <iomanip>
#include <iostream>
using std::ends;
#else

using namespace std;
#endif

#include "oopiclist.h"
#include "ngd.h"
#include  <boundary.h>

class SpatialRegion;

class MapNGDs {
 public:
  MapNGDs(oopicList<NGD>* ptrNGDListArg, 
          SpatialRegion* pSpatialRegion) : ptrNGDList(ptrNGDListArg),
    numNGDs(0), ptrSpatialRegion(pSpatialRegion), checkGasOffTime(true) {}
  ~MapNGDs() {}
  
  //
  // a function to initiate the NGD send stripe and then
  // shift the NGDs. 
  // 
  void sendNGDsAndShift(int shiftDir, Boundary* bPtrOut) /* throw(Oops) */; 

  // 
  // a function to initiate the NGD receive stripe and wait 
  // until the completion of the MPI messages
  // 
  void recvShiftedNGDs(int shiftDir, Boundary* bPtrIn);
  
  void setNumNGDs();
  int getNumNGDs() const {
    return numNGDs;
  }
  oopicList<NGD>* getPtrNGDList() {
    return ptrNGDList;
  }
 private:
  // 
  // do not allow copy and assignment of this class.
  // 
  MapNGDs(const MapNGDs&);
  MapNGDs& operator=(const MapNGDs&);

  // 
  // data members
  // 
  oopicList<NGD>* ptrNGDList;
  int numNGDs; // number of neutral gas densities initialized
  SpatialRegion* ptrSpatialRegion;
  bool checkGasOffTime;
};
#endif // __MAPNGDS_H
