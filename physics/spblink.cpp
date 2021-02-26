/*
====================================================================

spblink.cpp

Purpose:        Defines implementation for SPBoundLink, which sends and
                receives particles and fields through a boundary


The communication link between pairs of spatial region boundary

0.99    (PeterM 9-3-97) Inception
1.01    (PeterM 1/15/01) Deeper look, allowing Enode/Bnode continuity.
====================================================================
*/

#include "spblink.h"
#ifdef MPI_VERSION
extern int MPI_RANK;  //globally defined MPI_RANK
#endif

// To get the precision of MPI transfers
#include <precision.h>


#ifdef _MSC_VER
using std::cout;
using std::cerr;
using std::endl;
#endif

SPBoundLink::SPBoundLink(Vector3 *_lE, Vector3 *_lB, Vector3 *_lJ,
                         int _lenIn,int _lenDest, 
                         Scalar* ptrlNGDbuf, int numElemNGDsbufArg) {
  lE=_lE;
  lB=_lB;
  lJ=_lJ;
  lenIn = _lenIn;
  lenDest = _lenDest;
  LinkType = SAME_PROCESS;  //  default, may be overridden by makeLink()
  PinArraySize=100;
  PinArray = new particlePassDat[PinArraySize];

// Buffers for shifted particles
  shftPtclsSize = 100;
  shftPtcls = new ShiftedParticlePassData[shftPtclsSize];
  numShftPtcls = 0;
#ifdef MPI_VERSION
  shftPtclSendRequest = MPI_REQUEST_NULL;
#endif

  /**
   * Note that the following two buffers allocate three times the
   * memory we need. The reason for this is explained by the
   * following email from PeterM:
   *
   * "I resized all these buffers
   * to be consistent with the deeper redundant field-solve I put in
   * last year.
   * 
   * With the deeper redundant field solve the possiblity exists of making the 
   * MPI moving window implementation more efficient, and the buffers would
   * also have to be larger for that. 
   * 
   * That code never ended up materializing however, and the MPI moving window
   * implementation is essentially unchanged from what John Cary wrote
   * except for larger buffers.
   * 
   * PeterM" May, 2001. 
   */
// Buffers for usual fields
  fieldsBufout = new Vector3[9 * _lenIn + 21];
  fieldsBufin = new Vector3[9 * _lenIn + 21];
  int i;
  for(i=0; i < 9 * _lenIn +20; i++) 
	fieldsBufin[i]=fieldsBufout[i]=Vector3(0,0,0);

// Buffers for shifted fields
#ifdef MPI_VERSION
  shftFldSendRequest = MPI_REQUEST_NULL;
  shftFldRecvRequest = MPI_REQUEST_NULL;
#endif
  shiftedFieldsSent = false;
  shftFldsBufout = new Vector3[9 * _lenIn + 20];
  shftFldsBufin = new Vector3[9 * _lenIn + 20];
  for(i=0; i < 9 * _lenIn +20; i++) 
    shftFldsBufin[i]=shftFldsBufout[i]=Vector3(0,0,0);

  //
  // initialization of data related to the NGD
  // 
#ifdef MPI_VERSION
  shftNGDSendRequest = MPI_REQUEST_NULL;
  shftNGDRecvRequest = MPI_REQUEST_NULL;
#endif
  shiftedNGDsSent = false;
  lNGD = ptrlNGDbuf; // the NGD data we'll need for this boundary
  numElemNGDsBuf = numElemNGDsbufArg; // size of the following two buffers. 
  if ( numElemNGDsBuf ) {
    cout << "Allocating memory in SPBoundLink with numElem = " 
         << numElemNGDsBuf << endl;
    shftNGDsBufin = new Scalar[numElemNGDsBuf];
    shftNGDsBufout= new Scalar[numElemNGDsBuf];
    for ( i = 0; i < numElemNGDsBuf; i++ ) {
      shftNGDsBufin[i] = 0.0;
      shftNGDsBufout[i] = 0.0;
    }
  } else { // numElemNGDsBuf == 0
    shftNGDsBufin  = 0;
    shftNGDsBufout = 0;
  }

}
// 
// Destructor - return memory
//
SPBoundLink::~SPBoundLink(){
  delete [] PinArray;
  delete [] shftPtcls;
  delete [] fieldsBufout;
  delete [] fieldsBufin;
  delete [] shftFldsBufout;
  delete [] shftFldsBufin;
  if (shftNGDsBufout )
    delete [] shftNGDsBufout;
  if (shftNGDsBufin )
    delete [] shftNGDsBufin;
}

void SPBoundLink::makeLink(Vector3 *_lEdest, Vector3 *_lBdest,
	Vector3 *_lJdest, Vector3 **_lE, Vector3 **_lB, Vector3 **_lJ)
{
  LinkType = SAME_PROCESS;
  
  *_lE = lE;  // set the other guy's pointer to point to our receiving area
  *_lB = lB;  // set the other guy's pointer to point to our receiving area
  *_lJ = lJ;  // set the other guy's pointer to point to our receiving area

  lEdest = _lEdest; // set our pointers to point at the remote receiving area
  lBdest = _lBdest; // set our pointers to point at the remote receiving area
  lJdest = _lJdest; // set our pointers to point at the remote receiving area

}

void SPBoundLink::makeLink(int _DestID,int _tag_modifier)
{
  LinkType = MPI_LINK;
  DestID = _DestID;  // tells us which boundary we're communicating with
  tag_modifier = _tag_modifier;  //

#ifdef MPI_DEBUG
  printf("\nSPBoundLink::makeLink: (%d)  DestID = %d, tag_modifier = %d",MPI_RANK,DestID,tag_modifier);
#endif

}

void SPBoundLink::sendParticles(int n, particlePassDat *passers) {
  switch(LinkType) {

    case SAME_PROCESS:
      break;

#ifdef MPI_VERSION
    case MPI_LINK:
      {
#ifdef MPI_DEBUG
        printf("\nSPBoundLink::sendParticles (%d):  DestID = %d, tag = %d\n",
		MPI_RANK,DestID,P_TAG+tag_modifier);
#endif

//#define DEBUG
#if defined(DEBUG) || defined(MPI_DEBUG)
        if(n > 0)
        printf("\nSBL:passParticles:  sending %d particles to %d",n,DestID);
#endif
        MPI_Isend(passers, (sizeof(particlePassDat)/sizeof(Scalar))*n,
		MPI_Scalar,DestID, P_TAG + tag_modifier,XOOPIC_COMM,&Psrequest);
      }
      break;

#endif
  }
}

void SPBoundLink::askParticles() {

  switch(LinkType) {

    case SAME_PROCESS:
      break;

#ifdef MPI_VERSION
    case MPI_LINK:
      {
// non-blocking receive NOT possible
      }
      break;
#endif

  }

}

particlePassDat * SPBoundLink::waitParticles(int *n_in) {

  switch(LinkType) {
    case SAME_PROCESS:
      break;

#ifdef MPI_VERSION
    case MPI_LINK:
      {
	int N_in;
  #ifdef MPI_DEBUG
        printf("\nSPBoundLink::waitParticles (E %d):  DestID = %d, tag = %d\n",
		MPI_RANK,DestID,P_TAG+tag_modifier);
  #endif

// wait for our send to complete
	MPI_Wait(&Psrequest,&Psstatus);

// count how many particles are incoming
	MPI_Probe(DestID,P_TAG+tag_modifier,XOOPIC_COMM,&Prstatus);
	MPI_Get_count(&Prstatus,MPI_Scalar,&N_in); 

// if our buffer isn't large enough, increase the size
	while((*n_in=N_in*sizeof(Scalar)/sizeof(particlePassDat)) > PinArraySize)
		{
	  particlePassDat *array = new particlePassDat[2*PinArraySize];
	  PinArraySize*=2;
	  delete [] PinArray;
	  PinArray=array;
	}

// receive the incoming particles
        MPI_Recv(PinArray, N_in, MPI_Scalar, DestID, P_TAG+tag_modifier,
		XOOPIC_COMM, &Prstatus);

  #if defined(DEBUG) || defined(MPI_DEBUG)
	printf("\nSBL:waitParticles:  getting %d particles from %d", *n_in,
		DestID);
  #endif

        return PinArray;
      }
      break;

#endif
  }

  return PinArray;

}

void SPBoundLink::sendFields(Vector3 *_lEsend, Vector3 *_lBsend,
	Vector3 *_lJsend) {

  switch(LinkType) {

    case SAME_PROCESS:
      {
// Copy the data to be sent over to the other boundary
        for(int i=0;i<lenIn;i++) {
	  lEdest[i] = _lEsend[i];
	  lBdest[i] = _lBsend[i];
	  lJdest[i] = _lJsend[i];
// I think the following are not needed
#ifdef MPI_VERSION
	  // fieldsBufout[i] = _lEsend[i];
	  // fieldsBufout[i+lenIn+2] = _lBsend[i];
	  // fieldsBufout[i+2*lenIn+4] = _lJsend[i];
#endif //MPI_VERSION		
        }
      }
      break; 

#ifdef MPI_VERSION
    case MPI_LINK:
      {
        for(int i=0;i<lenIn;i++) {
			  /* obsolete format */
/*	  fieldsBufout[i] = _lEsend[i];
	  fieldsBufout[i+lenIn+2] = _lBsend[i];
	  fieldsBufout[i+2*lenIn+4] = _lJsend[i]; */

			  /* we're sending 3 layers of fields now. */
			  fieldsBufout[i] = _lEsend[i];
			  fieldsBufout[i+lenIn+2] = _lEsend[lenIn + 1 + i];
			  fieldsBufout[i+2*lenIn+4] = _lEsend[2*lenIn + 2 + i];

			  fieldsBufout[i+3*lenIn+6] = _lBsend[i];
			  fieldsBufout[i+4*lenIn+8] = _lBsend[lenIn + 1 + i];
			  fieldsBufout[i+5*lenIn+10] = _lBsend[2*lenIn + 2 + i];

			  fieldsBufout[i+6*lenIn+12] = _lJsend[i];
			  fieldsBufout[i+7*lenIn+14] = _lJsend[lenIn + 1 + i];
			  fieldsBufout[i+8*lenIn+16] = _lJsend[2*lenIn + 2 + i];
			  
	}
// send one message instead of 3
	MPI_Isend(fieldsBufout, 27 * lenIn+63, MPI_Scalar,
		DestID, E_TAG + tag_modifier, XOOPIC_COMM, &Esrequest);
      }
      break;
#endif

  }

}

void SPBoundLink::askFields() {

  switch(LinkType) {

    case SAME_PROCESS:
      {
      }
      break;  // nothing needs doing. 

#ifdef MPI_VERSION
    case MPI_LINK:
      {
	MPI_Irecv(fieldsBufin, 27*lenIn+63, MPI_Scalar,
		DestID, E_TAG + tag_modifier, XOOPIC_COMM, &Errequest);
      }
      break;
#endif

  }

}

void SPBoundLink::waitFields() {

  switch(LinkType) {

    case SAME_PROCESS:
      {
// nothing needs doing. 
      }
      break;

#ifdef MPI_VERSION
    case MPI_LINK:
      {

//  make sure all the sends finish
	MPI_Wait(&Esrequest,&Esstatus);

//  make sure all the receives finish
	MPI_Wait(&Errequest,&Erstatus);

// copy the fields back out of the receive buffer.
	for(int i=0;i<lenIn;i++) {
	  lE[i]=fieldsBufin[i];
	  lB[i]=fieldsBufin[i+lenIn+2];
	  lJ[i]=fieldsBufin[i+2*lenIn+4];
	}
      }
      break;
#endif

  }

}

//new version of this function 

void SPBoundLink::waitFields(Vector3 *lEa,Vector3 *lEb,Vector3 *lEc,
									  Vector3 *lBa,Vector3 *lBb,Vector3 *lBc,
									  Vector3 *lJa,Vector3 *lJb,Vector3 *lJc) {

  switch(LinkType) {

    case SAME_PROCESS:
      {
// nothing needs doing. 
      }
      break;

#ifdef MPI_VERSION
    case MPI_LINK:
      {

//  make sure all the sends finish
	MPI_Wait(&Esrequest,&Esstatus);

//  make sure all the receives finish
	MPI_Wait(&Errequest,&Erstatus);

// copy the fields back out of the receive buffer.
	for(int i=0;i<lenIn;i++) {
/*	  lE[i]=fieldsBufin[i];
	  lB[i]=fieldsBufin[i+lenIn+2];
	  lJ[i]=fieldsBufin[i+2*lenIn+4]; */

	  lEa[i] = fieldsBufin[i];
	  lEb[i] = fieldsBufin[i + lenIn + 2];
	  lEc[i] = fieldsBufin[i + 2*lenIn +4];

	  lBa[i] = fieldsBufin[i + 3*lenIn +6];
	  lBb[i] = fieldsBufin[i + 4*lenIn +8];
	  lBc[i] = fieldsBufin[i + 5*lenIn +10];
	  
	  lJa[i] = fieldsBufin[i + 6*lenIn +12];
	  lJb[i] = fieldsBufin[i + 7*lenIn +14];
	  lJc[i] = fieldsBufin[i + 8*lenIn +16];

	}
      }
      break;
#endif
  }
} 

void SPBoundLink::sendShiftedParticles(vector<Particle*>& pl){

// Make sure local buffer is at least as large as the particle list
  if(shftPtclsSize < pl.size()){
    delete [] shftPtcls;
    shftPtcls = new ShiftedParticlePassData[pl.size()];
    shftPtclsSize = pl.size();
  }

// Copy the particles into the list
  for(size_t i=0; i<pl.size(); i++){
    shftPtcls[i].speciesID = (Scalar) pl[i]->get_speciesID();
    shftPtcls[i].x = pl[i]->get_x();
    shftPtcls[i].u = pl[i]->get_u();
    shftPtcls[i].np2c = pl[i]->get_np2c();
    if ( pl[i]->isVariableWeight() ) shftPtcls[i].vary_np2c = 1.;
    else shftPtcls[i].vary_np2c = 0.;
  }
  numShftPtcls = pl.size();

// Send the particle over
  switch(LinkType) {

    case SAME_PROCESS:
      cout << "sendShiftedParticles not implemented for same process.\n";
      assert(0);
      break;

#ifdef MPI_VERSION
    case MPI_LINK:
      {

#if defined(DEBUG) || defined(MPI_DEBUG)
        // cout << "\nSBL:passParticles:  sending " << n << " particles to " <<
		// DestID << endl;
#endif
        MPI_Isend(shftPtcls, 
	  (sizeof(ShiftedParticlePassData)/sizeof(Scalar))*numShftPtcls,
		MPI_Scalar, DestID, SHIFT_PTCL_TAG + tag_modifier, XOOPIC_COMM,
		&shftPtclSendRequest);
  	// cout << "MPI_RANK = " << MPI_RANK << 
	// ": SPBoundLink::sendShiftedParticles: shftPtclSendRequest = " <<
		// shftPtclSendRequest << endl;
      }
      break;
#endif

  }

}

// Wait for the particles from the boundary.
// Return reference to list with particles.

ParticleList& SPBoundLink::recvShiftedParticles(Species** speciesArray){

// Empty out old particles
  // cout << "MPI_RANK = " << MPI_RANK <<
        // ": deleting old particles in SPBoundLink::recvShiftedParticles." << endl;
  shftPtclList.deleteAll();
  // cout << "MPI_RANK = " << MPI_RANK <<
        // ": old particles deleted in SPBoundLink::recvShiftedParticles." << endl;

// Get new particles from boundary
  switch(LinkType) {

    case SAME_PROCESS:
      cout << "waitShiftedParticles not implemented for same process.\n";
      assert(0);
      break;

#ifdef MPI_VERSION
    case MPI_LINK:
      {

// Wait for our send to complete
  	// cout << "MPI_RANK = " << MPI_RANK << ": waiting for receive " <<
        	// "to complete in SPBoundLink::recvShiftedParticles." << endl;
  	// cout << "MPI_RANK = " << MPI_RANK << ": shftPtclSendRequest = " <<
		// shftPtclSendRequest << endl;
	if(shftPtclSendRequest != MPI_REQUEST_NULL)
		MPI_Wait(&shftPtclSendRequest, &shftPtclSendStatus);

// Count how many particles are incoming
  	// cout << "MPI_RANK = " << MPI_RANK << ": probing " <<
        	// "in SPBoundLink::recvShiftedParticles." << endl;
	MPI_Probe(DestID, SHIFT_PTCL_TAG + tag_modifier, XOOPIC_COMM,
		&shftPtclRecvStatus);
        int numScalarIn;
  	// cout << "MPI_RANK = " << MPI_RANK << ": getting count " <<
        	// "in SPBoundLink::recvShiftedParticles." << endl;
	MPI_Get_count(&shftPtclRecvStatus, MPI_Scalar, &numScalarIn); 
        int numPtclsIn = numScalarIn * sizeof(Scalar) /
		sizeof(ShiftedParticlePassData);
  	// cout << "MPI_RANK = " << MPI_RANK << ": receiving " << numPtclsIn <<
        	// " particles in SPBoundLink::recvShiftedParticles." << endl;

// Local buffer must be at least as large as the number of incoming particles
        if(shftPtclsSize < numPtclsIn){
          delete [] shftPtcls;
          shftPtcls = new ShiftedParticlePassData[numPtclsIn];
          shftPtclsSize = numPtclsIn;
        }

// Receive the incoming particles
        MPI_Recv(shftPtcls, numScalarIn, MPI_Scalar, DestID, 
		SHIFT_PTCL_TAG + tag_modifier, XOOPIC_COMM, 
		&shftPtclRecvStatus);

// Add the particles to the list
        BOOL _vary_np2c;
        for(int i=0; i<numPtclsIn; i++) {
          if ( shftPtcls[i].vary_np2c > 0. ) _vary_np2c = TRUE;
          else _vary_np2c = FALSE;
	  Particle* pp = new Particle(shftPtcls[i].x, shftPtcls[i].u,
		speciesArray[(int)(shftPtcls[i].speciesID+0.1)],
		shftPtcls[i].np2c, _vary_np2c);
          shftPtclList.add(pp);
        }

      }
      break;
#endif	// MPI_VERSION

  }
  return shftPtclList;
}

void SPBoundLink::sendShiftedFields(Vector3 *_lEsend, Vector3 *_lBsend,
	Vector3 *_lJsend) {

  switch(LinkType) {

    case SAME_PROCESS:
      {
// Copy the data to be sent over to the other boundary
        for(int i=0;i<lenIn;i++) {
	  lEdest[i] = _lEsend[i];
	  lBdest[i] = _lBsend[i];
	  lJdest[i] = _lJsend[i];
// I think the following are not needed
#ifdef MPI_VERSION
	  // fieldsBufout[i] = _lEsend[i];
	  // fieldsBufout[i+lenIn+2] = _lBsend[i];
	  // fieldsBufout[i+2*lenIn+4] = _lJsend[i];
#endif //MPI_VERSION		
        }
      }
      break; 

#ifdef MPI_VERSION
    case MPI_LINK:
      {
        for(int i=0;i<lenIn;i++) {
	  shftFldsBufout[i] = _lEsend[i];
	  shftFldsBufout[i+lenIn+2] = _lBsend[i];
	  shftFldsBufout[i+2*lenIn+4] = _lJsend[i];
	}
// send one message instead of 3
	MPI_Isend(shftFldsBufout, 9 * lenIn+20, MPI_Scalar,
		DestID, SHIFT_FLD_TAG + tag_modifier, XOOPIC_COMM, 
		&shftFldSendRequest);
        shiftedFieldsSent = true;
      }
      break;
#endif

  }

}

void SPBoundLink::askShiftedFields() {

  switch(LinkType) {

    case SAME_PROCESS:
      {
      }
      break;  // nothing needs doing. 

#ifdef MPI_VERSION
    case MPI_LINK:
      {
	MPI_Irecv(shftFldsBufin, 9*lenIn+20, MPI_Scalar,
		DestID, SHIFT_FLD_TAG + tag_modifier, XOOPIC_COMM, 
		&shftFldRecvRequest);
        extern int MPI_RANK;
        // cout << "Request to receive fields made by MPI_RANK = " << 
		// MPI_RANK << endl;
      }
      break;
#endif

  }

}

void SPBoundLink::waitShiftedFields() {

  switch(LinkType) {

    case SAME_PROCESS:
      {
// nothing needs doing. 
      }
      break;

#ifdef MPI_VERSION
    case MPI_LINK:
      {

//  make sure all the sends finish
        if(shiftedFieldsSent) {
	  MPI_Wait(&shftFldSendRequest, &shftFldSendStatus);
	  shiftedFieldsSent = false;
	}

//  make sure all the receives finish
	MPI_Wait(&shftFldRecvRequest, &shftFldRecvStatus);

// copy the fields back out of the receive buffer.
	for(int i=0;i<lenIn;i++) {
	  lE[i]=shftFldsBufin[i];
	  lB[i]=shftFldsBufin[i+lenIn+2];
	  lJ[i]=shftFldsBufin[i+2*lenIn+4];
	}

/*
    int kout = 5;
    cout << "In SPBoundLink::waitShiftedFields:\n";
    cout << "E1 =";
    for(int j1=0; j1<=lenIn; j1++)  cerr << " " << shftFldsBufin[j1].e1();
    cout << endl;
    cout << "E2 =";
    for(int j1=0; j1<=lenIn; j1++)  cerr << " " << shftFldsBufin[j1].e2();
    cout << endl;
    cout << "I1 =";
    for(int j1=0; j1<=lenIn; j1++)  cout << " " << 
	shftFldsBufin[j1+2*lenIn+4].e1();
    cout << endl;
*/

      }
      break;
#endif
  }
}

//
// Communication of shifted NGDs
// 
void SPBoundLink::sendShiftedNGDs( Scalar* NGDSendBuffer, int numElements ) 
  /* throw(Oops) */{
  
  switch(LinkType) {
    
  case SAME_PROCESS:
    {
      stringstream ss (stringstream::in | stringstream::out);
      ss << "SPBoundLink::sendShiftedNGDs: Error: \n"<<
        "The case SAME_PROCESS: is not implemented yet in" << endl
           << "since at the time of writing of this function (05/24/01)" << endl
           << "this case was never reachable. D. A. D. " << endl;
           
      std::string msg;
      ss >> msg;
      Oops oops(msg);
      throw oops;    // exit() "SpatialRegionBoundary::sendNGDStripe:

    }
    break; 
    
#ifdef MPI_VERSION
  case MPI_LINK:
    { 
      //
      // Check that the number of elements requested to send is not more
      // than the number of elements of the shftNGDsBufout
      // 
      if ( numElements != numElemNGDsBuf ) {
        stringstream ss (stringstream::in | stringstream::out);
        ss <<"SPBoundLink::sendShiftedNGDs: Error: \n"<<
          "The number of elements = " << numElements << " requested is " << endl
          << "different from the number of elements numElemNGDsBuf = " 
          << numElemNGDsBuf << " in the MPI send buffer." << endl;
             
        std::string msg;
        ss >> msg;
        Oops oops(msg);
        throw oops;    // exit() "SpatialRegionBoundary::sendNGDStripe:

      }
      for(int i = 0; i < numElements; i++) {
        shftNGDsBufout[i] = NGDSendBuffer[i];
      }
      MPI_Isend(shftNGDsBufout, numElemNGDsBuf, MPI_Scalar,
		DestID, SHIFT_NGD_TAG + tag_modifier, XOOPIC_COMM, 
		&shftNGDSendRequest);
      shiftedNGDsSent = true;
    }
    break;
#endif
  }
}

// 
// ask the other boundary to send the needed NGD data
//
void SPBoundLink::askShiftedNGDs() {
  switch(LinkType) {
  case SAME_PROCESS:
    {
    }
    break;  // nothing needs doing. 
    
#ifdef MPI_VERSION
  case MPI_LINK:
    {
      MPI_Irecv(shftNGDsBufin, numElemNGDsBuf, MPI_Scalar,
		DestID, SHIFT_NGD_TAG + tag_modifier, XOOPIC_COMM, 
		&shftNGDRecvRequest);
      extern int MPI_RANK;
      // cout << "Request to receive NGDs made by MPI_RANK = " << 
      // MPI_RANK << endl;
    }
    break;
#endif
  }
}

//
// wait for the needed NGD data 
//
void SPBoundLink::waitShiftedNGDs() {
  switch(LinkType) {
  case SAME_PROCESS:
    {
      // nothing needs doing. 
    }
    break;
#ifdef MPI_VERSION
  case MPI_LINK:
    {
      //  make sure all the sends finish
      if( shiftedNGDsSent ) {
        MPI_Wait(&shftNGDSendRequest, &shftNGDSendStatus);
        shiftedNGDsSent = false;
      }
      //  make sure all the receives finish
      MPI_Wait(&shftNGDRecvRequest, &shftNGDRecvStatus); 
      // 
      // copy the NGDs back out of the receive buffer.
      // 
      for( int i = 0; i < numElemNGDsBuf; i++) {
        lNGD[i] = shftNGDsBufin[i];
      }
    }
    break;
#endif
  }
}
