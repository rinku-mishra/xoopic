//--------------------------------------------------------------------
//
// File:	mapNGDs.cpp
//
// Purpose:	implementations of the NGD (Neutral Gas Density) class
//
// Version:	$Id: mapNGDs.cpp 2294 2006-09-18 17:51:45Z yew $
//
// Copyright 2001, Tech-X Corporation, all rights reserved
//
//--------------------------------------------------------------------

#include "mapNGDs.h"
#include "sptlrgn.h"

//
// a function to initiate the NGD send stripe and then
// shift the NGDs. 
//
void MapNGDs::sendNGDsAndShift(int shiftDir, Boundary* bPtrOut) 
  /* throw(Oops) */{
  // auxiliary variables: 
  int J, K, jp1; 
  NGD* ptrNGD;
  //
  // Send cells over. This should send j=1.
  // 
  if ( bPtrOut ) {
    try{
      bPtrOut->sendNGDStripe(shiftDir);
    }
    catch(Oops& oops){
      oops.prepend("MapNGDs::sendNGDsAndShift: Error:\n"); //SpatialRegion::shiftRegion
      throw oops;
    }
  }
  //
  // Shift the NGDs. NOTE the comment at this point in the
  // corresponding function that does the send and shift for
  // the fields (fields.cpp): "Shift fields.  We do this first 
  // because the before-shift edge cells were already sent over 
  // at the field solve. I am not sure that this is correct."
  // 
  switch(shiftDir) {
    //
    // implement only shift to the left, i.e. right moving window
    // 
  case 1:
    {
      // 
      // loop over the NGD list and do the shift for each NGD
      // 
      oopicListIter<NGD> NGDIter(*ptrNGDList);
      for ( NGDIter.restart(); !NGDIter.Done(); NGDIter++ ) {
        ptrNGD = NGDIter.current(); 
        J = ptrNGD->getJ();
        K = ptrNGD->getK();
        // 
        // Note that j ranges from 0 to J-2, while jp1 ranges from
        // 1 to J-1.  Unlike the other fields, the neutral gas
        // density is cell-centered and only ranges from 0 to
        // J-1 and 0 to K-1.  With this logic, the values in
        // cells with j=J-1 are left unchanged.  This is
        // a reasonable default, but should be generalized in
        // the future.
        //      
        for(int j=0; j<J-1; j++) {
          jp1 = j + 1;
          for(int k=0; k<K; k++) {
            try{
              ptrNGD->set(j, k, ptrNGD->getNGD(jp1, k));
              if ( ptrNGD->getisTIOn() ) {
                ptrNGD->setNGD_excessNumIons(j, k, 
                                            ptrNGD->getNGD_excessNumIons(jp1, k));
                ptrNGD->setNGD_TI_np2c(j, k, ptrNGD->getNGD_TI_np2c(jp1, k) );
              }
            }
            catch(Oops& oops){
              oops.prepend("MapNGDs::sendNGDsAndShift: Error: \n"); //OK
              throw oops;
            }
          }
        }

        // if (time gt gasOffTime)
        if (checkGasOffTime && ptrSpatialRegion->isGasSwitchOffSet()) {
          checkGasOffTime = false;
          for(int k=0; k<K; k++) {
            ptrNGD->set(J-1, k, 0.);
            if ( ptrNGD->getisTIOn() ) {
              try{
                ptrNGD->setNGD_excessNumIons(J-1, k, 0.);
                ptrNGD->setNGD_TI_np2c(J-1, k, 1.);
              }
              catch(Oops& oops){
                oops.prepend("MapNGDs::sendNGDsAndShift: Error: \n"); //OK
                throw oops;
              }
            }
          }
        }
      }
    }
    break;
  default:
    {
        std::cerr << "Only right moving window (shiftDir = 1 in " << std::endl
           << "MapNGDs::sendNGDsAndShift(int, Boundary*)) is " << std::endl
           << "implemented for now! You have requested shiftDir = " 
           << shiftDir << std::endl;
    }
    break;
  }
}

// 
// a function to initiate the NGD receive stripe and wait 
// until the completion of the MPI messages
// 
void MapNGDs::recvShiftedNGDs(int shiftDir, Boundary* bPtrIn) {
  // 
  // note that the corresponding function in Fields uses
  // assert(bPtrIn); which I think is not needed since
  // the bPtrIn was first checked to evaluate to true in 
  // the SpatialRegion before being passed to 
  // recvShiftedNGDs(...) dad 05/21/01
  //

  //
  // Receive a new stripe of NGDs from the boundary.
  // 
  bPtrIn->recvNGDStripe(shiftDir);
}

void MapNGDs::setNumNGDs() {
  oopicListIter<NGD> NGDIter(*ptrNGDList);
  for ( NGDIter.restart(); !NGDIter.Done(); NGDIter++ ) 
    numNGDs++;
}
