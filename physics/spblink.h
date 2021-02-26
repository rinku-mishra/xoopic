#ifndef	__SPBLINK_H
#define	__SPBLINK_H

/*
  ====================================================================
  
  spblink.h
  
  Purpose:	Defines interface for SPBoundLink, which sends and 
  receives particles and fields through a boundary
  
  
  The communication link between pairs of spatial region boundary
  
  0.99	(PeterM 9-3-97) Inception
  1.01    (PeterM 1/18/01) Deeper look, allowing Enode/Bnode continuity.
  ====================================================================
*/

// Standard includes
#include <stdio.h>
#if defined(_CRAYT3E)
#include <vector.h>
#else
#include <vector>
using std::vector;
#endif    //_CRAYT3E

#include <boundary.h>
#include <oops.h>
#include "xgio.h"

// Structure used to push particles across a boundary
typedef struct {
  Scalar x;  //position along the boundary, we already KNOW the other coord.
  Vector3 dxMKS;  // remaining push
  Vector3 u;  // velocity
  // Scalar q; extraneous
  Scalar speciesID;  // hopefully the same across CPU's, Scalar for easy passing
  // Scalar m; extraneous
  Scalar np2c;
  Scalar vary_np2c;
} particlePassDat;

// Structure used to shift particles across a boundary
// All data are scalars to facilitate the push
typedef struct {
  Scalar speciesID;  // hopefully the same across CPU's, Scalar for easy passing
  Vector2 x;  // postion
  Vector3 u;  // velocity
  Scalar np2c;
  Scalar vary_np2c;
} ShiftedParticlePassData;



class ostring;
class PDistCol;
class PoytFlux;


#define SAME_PROCESS 0
#define MPI_LINK 1
#define LINUX_THREAD 2
#define SYSV_THREAD 3
#define PVM 4
#define SHMEM 5

#ifdef MPI_VERSION
#include <mpi.h>
extern MPI_Comm XOOPIC_COMM;
#endif


//  Tags for sending fields via MPI.
#define E_TAG 1000
#define B_TAG 6000
#define J_TAG 11000
#define SHIFT_FLD_TAG 12000  // tag for shifted field sends and receives
#define SHIFT_NGD_TAG 13031  // tag for shifted NGDs sends and receives
#define P_TAG 20000  //particle tag
#define SHIFT_PTCL_TAG 21000  // tag for shifted particle sends and receives


//--------------------------------------------------------------------
//	Boundary:  This class defines the generic boundary object for
//	some region.  All other boundaries inherit from this pure virtual.

class SPBoundLink
{
  
  Vector3 *lJ;  // the J's we'll need for this boundary
  Vector3 *lE;  // the E's we'll need for this boundary
  Vector3 *lB;  // the B's we'll need for this boundary
  Vector3 *lJdest;  // the J's we'll put for this boundary
  Vector3 *lEdest;  // the E's we'll put for this boundary
  Vector3 *lBdest;  // the B's we'll put for this boundary
  Vector3 *fieldsBufin;  // for packing J,E,B together into 1 large message.
  Vector3 *fieldsBufout;  // for packing J,E,B together into 1 large message.
  
  int LinkType;														
  int lenIn;  // length of the vectors as input
  int lenDest;  // length of the vectors as output
  int DestID;  //tells us which process we're communicating with
  int tag_modifier;  // a unique ID for this link
  int PinArraySize;
  particlePassDat *PinArray;
  
  //
  // For shifting particles
  //
  ShiftedParticlePassData* shftPtcls;	// Array of shifted particles
  size_t numShftPtcls;	// Number of shifted particles
  size_t shftPtclsSize;	// Size of array of shifted particles
  ParticleList shftPtclList;
  Vector3 *shftFldsBufin;  // for packing J,E,B together into 1 large message.
  Vector3 *shftFldsBufout;  // for packing J,E,B together into 1 large message.

  Scalar* lNGD;     // the NGD data we'll need for this boundary
  Scalar* lNGDdest; // the NGD data we'll put for this boundary
  //
  // buffers for the NGD data
  // 
  int numElemNGDsBuf; // size of the following two buffers. 
  Scalar* shftNGDsBufin;
  Scalar* shftNGDsBufout;
  // 
  // flag for sending the NGDs in the MPI send call
  //
  bool shiftedNGDsSent;

  //
  // For shifting fields
  //
  bool shiftedFieldsSent;
  
#ifdef MPI_VERSION
  //  the send statuses/requests
  MPI_Status Esstatus;
  MPI_Status Bsstatus;
  MPI_Status Jsstatus;
  MPI_Request Esrequest;
  MPI_Request Bsrequest;
  MPI_Request Jsrequest;
  
  
  // the receive statuses/requests
  MPI_Status Erstatus;
  MPI_Status Brstatus;
  MPI_Status Jrstatus;
  MPI_Request Errequest;
  MPI_Request Brrequest;
  MPI_Request Jrrequest;
  
  //  particle send/receive requests/statuses.
  MPI_Request Prrequest;
  MPI_Request Psrequest;
  MPI_Status Psstatus;
  MPI_Status Prstatus;
  
  // requests and statuses for shifted particles
  MPI_Request shftPtclRecvRequest;
  MPI_Request shftPtclSendRequest;
  MPI_Status shftPtclRecvStatus;
  MPI_Status shftPtclSendStatus;
  
  // requests and statuses for shifted fields
  MPI_Request shftFldRecvRequest;
  MPI_Request shftFldSendRequest;
  MPI_Status shftFldRecvStatus;
  MPI_Status shftFldSendStatus;

  //
  // requests and statuses for shifted NGDs
  // 
  MPI_Request shftNGDRecvRequest;
  MPI_Request shftNGDSendRequest;
  MPI_Status  shftNGDRecvStatus;
  MPI_Status  shftNGDSendStatus;  
#endif
  
 public:
  
  /**
   * Construct by giving location for incoming fields
   */
  SPBoundLink(Vector3 *lE,Vector3 *lB,Vector3 *lJ,int _lenIn, int _lenDest, 
              Scalar* ptrlNGDbuf, int numElemNGDsbufArg);
  
  /**
   * Destructor: relinquish memory
   */
  virtual ~SPBoundLink();
  
  /**
   * Make link for same process
   */
  void makeLink(Vector3 *_lEdest, Vector3 *_lBdest, Vector3 *_lJdest,
                Vector3 **_lE,Vector3 **_lB,Vector3 **_lJ);
  
  /**
   * Make link for MPI
   */
  void makeLink(int DestID,int tag_modifier);
  
  /**
   * Some fields for initialization??
   */
  void sendInitFields(Vector3 *liC,Vector3 *liL) {
    sendFields(liC,liL,lJ);  // unimportant third parameter must be allocated
  }

  //
  // Usual particle communication
  //
  
  /**
   * Send particles collected by the boundary
   */
  void sendParticles(int n, particlePassDat *passers);
  
  /**
   * Ask for particles from the other end of the link
   */
  void askParticles();
  
  /**
   * Wait for the particles from the boundary
   *
   * @param n_in pointee is see to the number of particles received
   *
   * @return A pointer to the passed particle data
   */
  particlePassDat* waitParticles(int *n_in);
  
  /**
   * send fields to boundary's partner
   */
  void sendFields(Vector3 *_lEsend, Vector3 *_lBsend, Vector3 *_lJsend);
  void askFields();  // ask the other boundary to send the needed data
  void waitFields(); // wait for the needed data
  void waitFields(Vector3 *lEa, Vector3 *lEb, Vector3 *lEc, 
                  Vector3 *lBa, Vector3 *lBb, Vector3 *lBc, 
                  Vector3 *lJa, Vector3 *lJb, Vector3 *lJc); 
  
  //
  // Shifted particle communication
  //
  
  /**
   * Send particles collected by the boundary
   */
  void sendShiftedParticles(vector<Particle*>& pl);
  
  /**
   * Wait for the particles from the boundary
   *
   * @return reference to a list containing the incoming particles.
   * This reference is still owned by this class.  This could give
   * rise to an error if the reference is used after the instance
   * is destroyed.
   */
  ParticleList& recvShiftedParticles(Species** speciesArray);
  
  //
  // Communication of shifted fields
  //
  void sendShiftedFields(Vector3 *_lEsend, Vector3 *_lBsend, Vector3 *lJsend);
  void askShiftedFields();  // ask the other boundary to send the needed data
  void waitShiftedFields(); // wait for the needed data
  
  //
  // Communication of shifted NGDs
  // 
  void sendShiftedNGDs( Scalar* NGDSendBuffer, int numElements ) /* throw(Oops) */;
  void askShiftedNGDs();  // ask the other boundary to send the needed NGD data
  void waitShiftedNGDs(); // wait for the needed NGD data
};

#endif	//	ifndef __SPLINK_H
