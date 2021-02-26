//--------------------------------------------------------------------
//
// File:	spbound.cpp
//
// Purpose:	Implementation of the SpatialRegionBoundary - a boundary
//		between two spatial regions.  Has methods for passing
//		data between regions.
//
// Version:	$Id: spbound.cpp 2230 2004-10-08 22:32:36Z yew $
//
//--------------------------------------------------------------------

/*
====================================================================
Revision History

Cary (23 Jan 00) Added header, implementation of recvParticles, which
	is like emit, except that one does not further weighting of
	the particles.  Did some indentation for readability.
CVS1.27.2.4 (Cary 26 Jan 00) Reorganized to put constructor first.
	Not necessary, I see now, but made sure that j indexing
	was increasing.  Implemented sendStripe and recvStripe.
	Still not checked as we have no way of looking at MPI runs.

(PeterM 1/15/01) Deeper look, allowing Enode/Bnode continuity.
====================================================================
*/

/* These are template defines.  Make it easier not to screw things up.  */

/*The following advance the E field components 1 2 and 3 one timestep.
 "down" means lower index k.  "here" means same index.  "right" means higher index j.
	"left" means lower index j "up" means higher index k*/
#define NEW_E1(iC,iLhere,iLdown,Bhere,Bdown,Ihere) ( (iC).e1() * ( (Bhere).e3()*(iLhere).e3() \
		- (Bdown).e3()*(iLdown).e3() -(Ihere).e1()))


#define NEW_E2(iC,iLhere,iLleft,Bhere,Bleft,Ihere) ( (iC).e2() * ( -(Bhere).e3()*(iLhere).e3() \
	+ (Bleft).e3()*(iLleft).e3() - (Ihere).e2()))

#define NEW_E3(iC,iLhere,iLdown,iLleft,Bhere,Bdown,Bleft,Ihere)  ( (iC).e3() * ( (Bdown).e1()*(iLdown).e1()\
	- (Bhere).e1()*(iLhere).e1()\
	+ (Bhere).e2()*(iLhere).e2()\
	- (Bleft).e2()*(iLleft).e2() - (Ihere).e3()))

/* The following advance the B field components 1 2 and 3 one timestep:  directions are as above. */

#define NEW_B1(Ehere,Eup)        ( (Eup).e3() - (Ehere).e3())
#define NEW_B2(Ehere,Eright)     ( (Ehere).e3() - (Eright).e3())
#define NEW_B3(Ehere,Eright,Eup)  ( (Ehere).e1() - (Eup).e1() + (Eright).e2() - (Ehere).e2())





#include "spbound.h"
#include "fields.h"
#include "mapNGDs.h"


#ifdef _MSC_VER
using std::cout;
using std::cerr;
using std::endl;
#endif

SpatialRegionBoundary::SpatialRegionBoundary(oopicList <LineSegment> *segments) /* throw(Oops) */ :
  Boundary(segments) 
   {

	 if(segments->nItems() > 1) {
     stringstream ss (stringstream::in | stringstream::out);
     ss << "SpatialRegionBoundary::SpatialRegionBoundary: Error: \n"<<
      "Spatial region boundaries CANNOT have multiple segments.\n";

		std::string msg;
    ss >> msg;
    Oops oops(msg);
    throw oops;    // exit() SpatialRegionGroup::constructBoundaryParams:  
    /*"SpatialRegionGroup::makeSRB: 
    SpatialRegionBoundaryParams::CreateCounterPart: */

	 }
	 
	 if(j1!=j2 && k1!=k2) {
     stringstream ss (stringstream::in | stringstream::out);
     ss << "SpatialRegionBoundary::SpatialRegionBoundary: Error: \n"<<
		    "Spatial region boundaries MUST be either horizontal or vertical.\n";
		 
     std::string msg;
     ss >> msg;
     Oops oops(msg);
     throw oops;    // exit() SpatialRegionGroup::constructBoundaryParams:
    /*"SpatialRegionGroup::makeSRB: 
    SpatialRegionBoundaryParams::CreateCounterPart: */

	 }

	 int length = MAX(abs(k1-k2),abs(j1-j2));

	 // memory allocation is 1 larger to allow guard cells for a
	 // simpler field solve.
	 lJa = new Vector3[length+1];  // closest guard layer
	 lJb = new Vector3[length+1];  // next closest guard layer
	 lJc = new Vector3[length+1];  // furthest guard layer
	 lEa = new Vector3[length+1];  // closest guard layer
	 lEb = new Vector3[length+1];  // next closest guard layer
	 lEc = new Vector3[length+1];  // furthest guard layer
	 lBa = new Vector3[length+1];  // closest guard layer
	 lBb = new Vector3[length+1];  // next closest guard layer
	 lBc = new Vector3[length+1];  // furthest guard layer
	 liLa = new Vector3[length+1];  // closest guard layer
	 liLb = new Vector3[length+1];  // next closest guard layer
	 liLc = new Vector3[length+1];  // furthest guard layer

	 /* leave the old variables so it will compile */
	 lJ = new Vector3[length+1];  // soon obsolete
	 lE = new Vector3[length+1];  // soon obsolete
	 lB = new Vector3[length+1];  // soon obsolete
	 liL = new Vector3[length+1]; // soon obsolete

	 // Sending 3 layers for simplicity
	 lJsend = new Vector3[3*length+4];
	 lEsend = new Vector3[3*length+4];
	 lBsend = new Vector3[3*length+4];

         //
         // the following buffers are needed for the communication of
         // the neutral gas density (in NGD objects) data strucures
         // across boundaries; memory for them will be allocated only 
         // if there are NGDs; dad 05/16/01
         // 
         numElementsNGDbuf = 0; 
         lNGDbuf = 0;
         lNGDbufSend = 0;
         
	 //zero the memory

	 memset(lJa,0,(length+1) * sizeof(Vector3));
	 memset(lEa,0,(length+1) * sizeof(Vector3));
	 memset(lBa,0,(length+1) * sizeof(Vector3));
	 memset(liLa,0,(length+1) * sizeof(Vector3));

	 memset(lJb,0,(length+1) * sizeof(Vector3));
	 memset(lEb,0,(length+1) * sizeof(Vector3));
	 memset(lBb,0,(length+1) * sizeof(Vector3));
	 memset(liLb,0,(length+1) * sizeof(Vector3));

	 memset(lJc,0,(length+1) * sizeof(Vector3));
	 memset(lEc,0,(length+1) * sizeof(Vector3));
	 memset(lBc,0,(length+1) * sizeof(Vector3));
	 memset(liLc,0,(length+1) * sizeof(Vector3));

	 memset(lJ,0,(length+1) * sizeof(Vector3));
	 memset(lE,0,(length+1) * sizeof(Vector3));
	 memset(lB,0,(length+1) * sizeof(Vector3));
	 memset(liL,0,(length+1) * sizeof(Vector3));

	 memset(lJsend,0,(3*length+4) * sizeof(Vector3));
	 memset(lEsend,0,(3*length+4) * sizeof(Vector3));
	 memset(lBsend,0,(3*length+4) * sizeof(Vector3));
	 theLink = 0;
	 BCType=SPATIAL_REGION_BOUNDARY;
	 ptclPassArraySize=100;
	 ptclPassArray = new particlePassDat[ptclPassArraySize];
	 number_Particles_passing=0;
	 

}

// We need to set up a special iC for the Fields:: so it doesn't interfere
// with the special field solve we do for E2 and E3 on the boundary.

void SpatialRegionBoundary::setPassives() {
  int j,k;
  if(j1==j2) // vertical boundary, set iC2 and iC3 to zero
	 {
		for(k=k1;k<=k2;k++) {
		  fields->set_iL1(j1,k,2*fields->getiL()[j1][k].e1());  // fix iL: default is wrong.
		  fields->set_iC2(j1,k,0);
		  fields->set_iC3(j1,k,0);
		}
	 }
  else  // horizontal boundary, set iC1 and iC3 to zero
	 {
		for(j=j1;j<=j2;j++) {
		  fields->set_iL2(j,k1,2*fields->getiL()[j][k1].e2());  // fix iL: default is wrong.
		  fields->set_iC1(j,k1,0);
		  fields->set_iC3(j,k1,0);
		}
	 }
}
		  

//  This is the function which will assemble and send the currents
//  and fields necessary for the other boundary to solve for its
//  fields.  By convention for this boundary, normal points IN

void SpatialRegionBoundary::putCharge_and_Current(Scalar t) {

  Vector3 **intBdS = fields->getIntBdS();
  Vector3 **intEdl = fields->getIntEdl();
  Vector3 **I = fields->getI();
  // Vector3 **iC = fields->getiC();
  // Vector3 **iL = fields->getiL();

// For indexing
  int kl = MIN(k1,k2);
  int kh = MAX(k1,k2);
  int jl = MIN(j1,j2);
  int jh = MAX(j1,j2);
  int length;

// Begin the request
  if(!theLink) return;
  theLink->askFields();
  length = MAX(abs(k2-k1),abs(j2-j1));

// Horizontal boundary data send
  if(k1==k2) for(int j=jl;j<jh;j++) {  

    if(normal==-1) {
// Real region below
      lBsend[j-jl]=Vector3(intBdS[j][k1-1].e1(),0,intBdS[j][k1-1].e3());
      lEsend[j-jl]=intEdl[j][k1-1];
      lJsend[j-jl]=Vector3(I[j][k1].e1(),I[j][k1-1].e2(),I[j][k1].e3());
    }

    else {
// Real region above
      lBsend[j-jl]=Vector3(intBdS[j][k1].e1(), 0, intBdS[j][k1].e3());
      lEsend[j-jl]=Vector3(intEdl[j][k1].e1(), intEdl[j][k1+1].e2(),
		intEdl[j][k1+1].e3());
      lJsend[j-jl]=Vector3(I[j][k1].e1(), I[j][k1].e2(), I[j][k1].e3());
    }

  }

// Vertical boundary data send
  else for(int k=kl;k<=kh;k++) {
	  int kk = k - kl;

// Real region left
    if(normal==-1) {
/*      lBsend[k-kl]=Vector3(0,intBdS[j1-1][k].e2(),intBdS[j1-1][k].e3());
      lEsend[k-kl]=intEdl[j1-1][k];
      lJsend[k-kl]=Vector3(I[j1-1][k].e1(),I[j1][k].e2(),I[j1][k].e3()); */
		 //New way of doing this, send 3 full layers
		 // closest layer first
		 lBsend[kk] = intBdS[j1][k];
		 lBsend[length + 1 + kk] = intBdS[j1-1][k];
		 lBsend[2*length +2 + kk] = intBdS[j1-2][k];

		 lEsend[kk] = intEdl[j1][k];
		 lEsend[length + 1 + kk] = intEdl[j1-1][k];
		 lEsend[2*length +2 + kk] = intEdl[j1-2][k];

		 lJsend[k-kl] = I[j1][k];
		 lJsend[length + 1 + kk] = I[j1-1][k];
		 lJsend[2*length +2 + kk] = I[j1-2][k];
		 
    }

// Real region right
    else {
      /*lBsend[k-kl]=Vector3(0,intBdS[j1][k].e2(),intBdS[j1][k].e3());
      lEsend[k-kl]=Vector3(intEdl[j1][k].e1(), intEdl[j1+1][k].e2(),
		intEdl[j1+1][k].e3());
      lJsend[k-kl]=I[j1][k]; */
		 lBsend[kk] = intBdS[j1][k];
		 lBsend[length + 1 + kk] = intBdS[j1+1][k];
		 lBsend[2*length +2 + kk] = intBdS[j1+2][k];

		 lEsend[kk] = intEdl[j1][k];
		 lEsend[length + 1 + kk] = intEdl[j1+1][k];
		 lEsend[2*length +2 + kk] = intEdl[j1+2][k];

		 lJsend[kk] = I[j1][k];
		 lJsend[length + 1 + kk] = I[j1+1][k];
		 lJsend[2*length +2 + kk] = I[j1+2][k];

    }

  }

/*
#ifdef MPI_VERSION
  cout << "putCharge_and_Current sending fields:" << endl;
  cout << "E1 =";
  for(int kk=0; kk<=k2-k1; kk++) cout << " " << lEsend[kk].e1();
  cout << endl;
  cout << "E2 =";
  for(int kk=0; kk<=k2-k1; kk++) cout << " " << lEsend[kk].e2();
  cout << endl;
  cout << "E3 =";
  for(int kk=0; kk<=k2-k1; kk++) cout << " " << lEsend[kk].e3();
  cout << endl;
  cout << "B1 =";
  for(int kk=0; kk<=k2-k1; kk++) cout << " " << lBsend[kk].e1();
  cout << endl;
  cout << "B2 =";
  for(int kk=0; kk<=k2-k1; kk++) cout << " " << lBsend[kk].e2();
  cout << endl;
  cout << "B3 =";
  for(int kk=0; kk<=k2-k1; kk++) cout << " " << lBsend[kk].e3();
  cout << endl;
#endif
*/

// Send the fields through the link
  theLink->sendFields(lEsend,lBsend,lJsend);
}                  

// This is the function which does the fieldsolve.
// It does it in such a way that toNodes will be wrong,
// and it does it in such a way that serial operation is
// possible.
// By convention for this boundary, normal points IN

void SpatialRegionBoundary::applyFields(Scalar t,Scalar dt) {

  Scalar dt2 = dt/2;
  Vector3 **intBdS = fields->getIntBdS();
  Vector3 **intEdl = fields->getIntEdl();
  Vector3 **I = fields->getI();
  Vector3 **iC = fields->getiC();
  Vector3 **iL = fields->getiL();
 
  int kl = MIN(k1,k2);
  int kh = MAX(k1,k2);
  int jl = MIN(j1,j2);
  int jh = MAX(j1,j2);
  int K = fields->getK();
  int J = fields->getJ();
  if(!theLink) return;
  // old version
  //theLink->waitFields();  //  do a synchronize
  theLink->waitFields(lEa,lEb,lEc,lBa,lBb,lBc,lJa,lJb,lJc);

  // Advance B 1/2 step

  if(dt > 0) {
     SRBadvanceB(t,dt);

     // Advance E 1 step
     SRBadvanceE(t,dt);

  
     // Advance B 1/2 step is now NECESSARY for weighting to the nodes properly.
     
     SRBadvanceB(t,dt);
  }

}



//  This function sends the iL's the paired boundary will need to
//  complete the field solve.
void SpatialRegionBoundary::send_liL() {
  int j,k;
  Vector3 **iL = fields->getiL();
  if(k1==k2) {
	 for(j=j1;j<j2;j++) {
		if(normal==1)  //send the iL's from above
		  lBsend[j-j1] = iL[j][k1+1];
		else //send the iL's from below
		  lBsend[j-j1] = iL[j][k1-1];
	 }
  }
  else {
	 for(k=k1;k<k2;k++) {
		if(normal==1)  //send the iL's from right
		  lBsend[k-k1] = iL[j1+1][k];
		else //send the iL's from left
		  lBsend[k-k1] = iL[j1-1][k];
	 }
  }
  //  Actually send the data over.
  theLink->askFields();  //initialize the transfer
  theLink->sendFields(lEsend,lBsend,lJsend); // do our send
  theLink->waitFields();  //synchronize
}

//  This function initializes the passives across the SRB's.
void SpatialRegionBoundary::initialize_passives() {
  int j,k;
  Vector3 **iL = fields->getiL();
  if(k1==k2) {
	 for(j=j1;j<=j2;j++) {
		if(normal==1)  //send the iL's from below
		  lBsend[j-j1] = iL[j][k1+1];
		else //send the iL's from above
		  lBsend[j-j1] = iL[j][k1-1];
	 }
  }
  else {
	 for(k=k1;k<=k2;k++) {
		if(normal==1)  //send the iL's from right
		  lBsend[k-k1] = iL[j1+1][k];
		else //send the iL's from left
		  lBsend[k-k1] = iL[j1-1][k];
	 }
  }
  //  Actually send the data over.
  theLink->askFields();  //initialize the transfer
  theLink->sendFields(lEsend,lBsend,lJsend); // do our send
  theLink->waitFields();  //synchronize
  get_liL();
}

//  This function sends the iL's the paired boundary will need to
//  complete the field solve.
void SpatialRegionBoundary::get_liL() {
  int j,k;
    if(k1==k2) {
	 for(j=j1;j<=j2;j++) {
		if(normal==1) {
		  liL[j-j1] = lB[j-j1];
		  lB[j-j1]=0;
		}
		else {
		  liL[j-j1] = lB[j-j1];
		  lB[j-j1]=0;
		}
	 }
  }
  else {
	 for(k=k1;k<k2+1;k++) {
		if(normal==1) { 
		  liL[k-k1] = lB[k-k1];
		  lB[k-k1]=0;
		}
		else {
		  liL[k-k1] = lB[k-k1];
		  lB[k-k1]=0;
		}
	 }
  }
}

void SpatialRegionBoundary::toNodes()
{
  oopicListIter <LineSegment> lsI(*segments);
  for(lsI.restart();!lsI.Done();lsI++) {
     int j1,j2,k1,k2,normal,*points;  //local copies of above
     j1=(int)lsI()->A.e1();
     k1=(int)lsI()->A.e2();
     j2=(int)lsI()->B.e1();
     k2=(int)lsI()->B.e2();
     normal = lsI()->normal;
     points=lsI()->points; 
	 
     int j, k,K;
     Scalar w1m=0.5, w1p=0.5;
     Vector3** ENode = fields->getENode();
     Vector3** BNode = fields->getBNode();
     Vector3** intEdl = fields->getIntEdl();
     Vector3** intBdS = fields->getIntBdS();
     Grid* grid = fields->get_grid();
     K = grid->getK();
     BNode[j1][0] = 0;  // unbounded edges

     if(j1==j2 && k1==k2) return;  // don't do toNodes on one-point boundaries.
     if (j1==j2)            // vertical boundary
	{
	   for (j=j1, k=k1; k<=k2; k++) {
	      int kk = k - k1;
	      if(normal == 1) {
		 // ASSUME:  Uniform mesh

		 Scalar w2m = grid->dl2(j,k) / (grid->dl2(j,k) + grid->dl2(j,MAX(0,k-1)));
		 Scalar w2p = 1-w2m;
		 Scalar w1m = 0.5;  // uniform mesh assumption
		 Scalar w1p = 0.5;  // uniform mesh assumption

		 ENode[j][k].set_e1(0.5* (intEdl[j][k].e1() + lEb[kk].e1())/grid->dl1(j+1,k));

		 if(k>0 && k<K) {
		    ENode[j][k].set_e2(w2p*intEdl[j][k].e2()/grid->dl2(j, k) 
				       + w2m*intEdl[j][k-1].e2()/grid->dl2(j, k-1));
		    BNode[j][k].set_e1(intBdS[j][k].e1()*w2p/grid->dS1(j, k)
				       + intBdS[j][k-1].e1()*w2m/grid->dS1(j, k-1));

		    BNode[j][k].set_e3(intBdS[j][k].e3()*w1p*w2p/grid->dS3(j+1, k) // uniform mesh
				       + lBb[kk].e3()*w1m*w2p/grid->dS3(j+1, k)  //uniform mesh
				       + intBdS[j][k-1].e3()*w1p*w2m/grid->dS3(j+1, k-1) // uniform mesh
				       + lBb[kk-1].e3()*w1m*w2m/grid->dS3(j+1, k-1)); //uniform mesh
		 }

		 if(k > 0 || !grid->axis()) {
		    BNode[j][k].set_e2(intBdS[j][k].e2()*(w1p/grid->dS2(j+1, k))  // uniform mesh 
				       + lBb[kk].e2()*w1m/grid->dS2(j+1, k));  //uniform mesh
		    ENode[j][k].set_e3(intEdl[j][k].e3()/grid->dl3(j+1,k)); // uniform mesh
		 }		       
		 if(k==K) {
		    ENode[j][k].set_e2(intEdl[j][k-1].e2()/grid->dl2(j+1, k-1));
		    BNode[j][k].set_e1(2*(intBdS[j][k-1].e1()/grid->dS1(j+1,k-1))
				       -BNode[j][k-1].e1());
		    BNode[j][k].set_e3(2* ( intBdS[j][k-1].e3()*w1p/grid->dS3(j+1,k-1) +
					   lBb[kk-1].e3()*w1m/grid->dS3(j+1,k-1))-BNode[j][k-1].e3()); //uniform mesh
		 }


	      }
	      else {  //normal == -1 , real region on left
		 // ASSUME:  Uniform mesh

		 Scalar w2m = grid->dl2(j,k) / (grid->dl2(j,k) + grid->dl2(j,MAX(0,k-1)));
		 Scalar w2p = 1-w2m;
		 Scalar w1m = 0.5;  // uniform mesh assumption
		 Scalar w1p = 0.5;  // uniform mesh assumption

		 ENode[j][k].set_e1(0.5* (intEdl[j-1][k].e1() + lEa[kk].e1())/grid->dl1(j-1,k));

		 if(k>0 && k<K) {
		    ENode[j][k].set_e2(w2p*intEdl[j][k].e2()/grid->dl2(j, k) 
				       + w2m*intEdl[j][k-1].e2()/grid->dl2(j, k-1));
		    BNode[j][k].set_e1(intBdS[j][k].e1()*w2p/grid->dS1(j, k)
				       + intBdS[j][k-1].e1()*w2m/grid->dS1(j, k-1));

		    BNode[j][k].set_e3(lBa[kk].e3()*w1p*w2p/grid->dS3(j-1, k) // uniform mesh
				       + intBdS[j-1][k].e3()*w1m*w2p/grid->dS3(j-1, k)  //uniform mesh
				       + lBa[kk-1].e3()*w1p*w2m/grid->dS3(j-1, k-1) // uniform mesh
				       + intBdS[j-1][k-1].e3()*w1m*w2m/grid->dS3(j-1, k-1)); //uniform mesh
		 }


		 if(k > 0 || !grid->axis()) {
		    BNode[j][k].set_e2(intBdS[j-1][k].e2()*(w1p/grid->dS2(j-1, k))  // uniform mesh 
				       + lBa[kk].e2()*w1m/grid->dS2(j-1, k));  //uniform mesh
		    ENode[j][k].set_e3(intEdl[j][k].e3()/grid->dl3(j-1,k)); // uniform mesh
		 }		       

		 if(k==K) {
		    ENode[j][k].set_e2(intEdl[j][k-1].e2()/grid->dl2(j-1, k-1));
		    BNode[j][k].set_e1(2*(intBdS[j][k-1].e1()/grid->dS1(j-1,k-1))
				       -BNode[j][k-1].e1());
		    BNode[j][k].set_e3(2* ( intBdS[j-1][k-1].e3()*w1p/grid->dS3(j-1,k-1) +
					   lBa[kk-1].e3()*w1m/grid->dS3(j-1,k-1))-BNode[j][k-1].e3()); //uniform mesh
		 }
		 

	      }
	   }

	   // This needs to be AFTER the for loop because it depends on BNode higher up than the for
	   // loop gets to!!!
	   if(normal==1) {
	      k = k1;
	      int kk = k - k1;
	      if(k==0) {
		 BNode[j][k].set_e1(2*(intBdS[j][k].e1()/grid->dS1(j+1,k))
				 -BNode[j][k+1].e1());
		 if(grid->axis()) {
		    ENode[j][k].set_e2(0);  // none of these fields may be nonzero on axis.
		    BNode[j][k].set_e2(0);
		    BNode[j][k].set_e3(0);

		 } else {
		    ENode[j][k].set_e2(intEdl[j][k].e2()/grid->dl2(j+1,k));  // uniform mesh
		    // Refer to boundary.cpp for this one.
		    BNode[j][k].set_e3(2*(intBdS[j][k].e3()*w1p/grid->dS3(j+1,k)
					  +lBb[kk].e3()*w1m/grid->dS3(j+1,k))-BNode[j][1].e3()); // uniform mesh
		 }
	      }
	   }
	   else {
	      k = k1;
	      int kk = k - k1;
	      if(k==0) {
		 BNode[j][k].set_e1(2*(intBdS[j][k].e1()/grid->dS1(j-1,k))
				    -BNode[j][k+1].e1());
		 if(grid->axis()) {
		    ENode[j][k].set_e2(0);  // none of these fields may be nonzero on axis.
		    BNode[j][k].set_e2(0);
		    BNode[j][k].set_e3(0);
		 } else {
		    ENode[j][k].set_e2(intEdl[j][k].e2()/grid->dl2(j-1, k));
		    BNode[j][k].set_e3(2*(intBdS[j-1][k].e3()*w1p/grid->dS3(j-1,k)
					  +lBa[kk].e3()*w1m/grid->dS3(j-1,k))-BNode[j][1].e3()); // uniform mesh
		 }
	      }
	   }
	}
     else if (k1==k2)                
	{
	   // NIY
	}
  }

}

  

SpatialRegionBoundary::~SpatialRegionBoundary() {
  delete [] lJ;
  delete [] lE;
  delete [] lB;
  lJ=lE=lB=0;
  //
  // delete the memory for the NGD data buffers only if
  // it was allocated
  // 
  if ( lNGDbuf )
    delete [] lNGDbuf; 
  if ( lNGDbufSend )
    delete [] lNGDbufSend;
}

//
//  This is the function which packages and sends particles over 
// to the paired SRB.
//
	 
void SpatialRegionBoundary::passParticles() {
  theLink->sendParticles(number_Particles_passing,ptclPassArray);
}

void SpatialRegionBoundary::collect(Particle &p, Vector3& dxMKS) {

// check and see if we've exceeded the passing array memory
  while(number_Particles_passing+1 > ptclPassArraySize) {
    particlePassDat *array = new particlePassDat[2*ptclPassArraySize];
    memcpy(array, ptclPassArray,
	number_Particles_passing * sizeof(particlePassDat));
    delete[] ptclPassArray;
    ptclPassArray = array;
    ptclPassArraySize*=2;
  }

// Add this particle to the array for passing
  ptclPassArray[number_Particles_passing].u = p.get_u();
  if ( p.isVariableWeight() )
       ptclPassArray[number_Particles_passing].vary_np2c = 1.;
  else ptclPassArray[number_Particles_passing].vary_np2c = 0.;
  ptclPassArray[number_Particles_passing].np2c = p.get_np2c();
  ptclPassArray[number_Particles_passing].speciesID = (Scalar) p.get_speciesID();
  ptclPassArray[number_Particles_passing].dxMKS = dxMKS;
  ptclPassArray[number_Particles_passing].x =
     (j1==j2) ? p.get_x().e2() : p.get_x().e1();

// Diagnostics
/*
#ifdef MPI_VERSION
  if ( number_Particles_passing == 0) {
    extern int MPI_RANK;
    cout << "MPI rank " << MPI_RANK << " collected a particle with\n";
    cout << "\n";
  }
#endif
*/

  delete &p;
  number_Particles_passing++;

}

// Emit the particles coming across the boundary.  In this case
// the particles come in and must be weighted in the incoming region.
// This method assumes that calling routine/class will take
// care of emptying the returned ParticleList.  Thus, multiple
// calls to recvParticles accumulate particles.

ParticleList &SpatialRegionBoundary::emit(Scalar t, Scalar dt, 
	Species *species) {

  int i,n_in;
  particlePassDat *inParticles = theLink->waitParticles(&n_in);
  Boundary *bPtr;

  number_Particles_passing=0;  // our send has completed here.
#ifdef DEBUG
  if (n_in > 0)
      printf("\nSRB:emit:  emitting %d particles",n_in);
#endif

  for(i=0;i<n_in;i++) {

// Get the particle's coordinate
    Vector2 x( (j1==j2)? j1* (1 +normal * 1e-6) +
	normal * 1e-20:inParticles[i].x, 
	(j1==j2) ? inParticles[i].x : k1*(1+normal*1e-6)+normal * 1e-20);

	 // figure out if it is a variable-weight particle
	 BOOL _vary_np2c = FALSE;
	 if(inParticles[i].vary_np2c > 0.) _vary_np2c = TRUE;


    Particle * p = new Particle(x,inParticles[i].u, 
	speciesArray[(int)(inParticles[i].speciesID+0.1)],
	inParticles[i].np2c, _vary_np2c);

// finish the push
    x = p->get_x();
    if((bPtr = fields->translateAccumulate(x, inParticles[i].dxMKS, 
		p->get_q()/fields->get_dt()))) {
      p->set_x(x);
// Send this particle to boundary
      bPtr->collect(*p,inParticles[i].dxMKS);

    }
    else {
      p->set_x(x);
      particleList.add(p);
    }

  }

  return particleList;
}

/**
 * Collect the particle into the shiftedParticles array.
 * This is a handoff, SpatialRegionBoundary is subsequently
 * responsible for deletion of the particle.
 *
 * @param p the particle to be collected
 */
void SpatialRegionBoundary::collectShiftedParticle(Particle* p){
  shiftedParticles.push_back(p);
}

/**
 * Pass the shifted particles over the boundary
 *
 * Because we are responsible for deleting the particles,
 * we must delete after sending.
 */
void SpatialRegionBoundary::passShiftedParticles(){

// Send the particles to the link
/*
#ifdef MPI_VERSION
  extern int MPI_RANK;
  cerr << "MPI_RANK of " << MPI_RANK << ": sending " << 
	shiftedParticles.size() << " particles.\n";
#endif
*/

  if(theLink) theLink->sendShiftedParticles(shiftedParticles);

// Clear out the particles
  for(int i=0; (unsigned)i<shiftedParticles.size(); i++) delete shiftedParticles[i];
  shiftedParticles.clear();

}

/**
 * Receive the shifted particles from the boundary
 */
ParticleList& SpatialRegionBoundary::recvShiftedParticles(){

#ifdef MPI_VERSION
  extern int MPI_RANK;
#endif

// Must have a link
  assert(theLink);

// Return the particles from the link
  // cout << "MPI_RANK = " << MPI_RANK << 
	// ": receiving particles in recvShiftedParticles." << endl;
  ParticleList& pl = theLink->recvShiftedParticles(speciesArray);
  // cout << "MPI_RANK = " << MPI_RANK << 
	// ": particles received in recvShiftedParticles." << endl;

/*
#ifdef MPI_VERSION
  extern int MPI_RANK;
  cerr << "MPI_RANK of " << MPI_RANK << ": received " << 
	pl.nItems() << " particles.\n";
#endif
*/

// Shift the particles over to the right
  oopicListIter<Particle> piter(pl);
  Particle* pp;
  Vector2 x;
  bool prtd = false;
  for(piter.restart(); !piter.Done(); piter++){
    pp = piter.current();
    // if(!prtd){
      // cout << "Before shift, first particle received has x1 = " << 
	// pp->get_x().e1() << endl;
    // }
    x = pp->get_x();
    x.set_e1(x.e1() + j2);
    pp->set_x(x);

    // if(!prtd){
      // cout << "After shift, first particle received has x1 = " << 
	// pp->get_x().e1() << endl;
    // }
    prtd = true;
  }

  return pl;

}

// Ask the link to get the stripe of fields
//
void SpatialRegionBoundary::askStripe(){
  assert(theLink);
  theLink->askShiftedFields();
}


// This is like putCharge_and_Current, except that it sends over
// a pure stripe as is needed for shifting fields
//
void SpatialRegionBoundary::sendStripe(int dir){

#ifdef  MPI_VERSION
  extern int MPI_RANK;
#endif

  Vector3 **intBdS = fields->getIntBdS();
  Vector3 **intEdl = fields->getIntEdl();
  Vector3 **I = fields->getI();

  switch(dir){

// Fields sending to left (right moving window)
    case 1:{
      assert(j1 == j2);
// On KC's authority, one always has k1 <= k2
      for(int k=k1; k<=k2; k++) {
	lEsend[k-k1] = Vector3( intEdl[j1][k].e1(), intEdl[j1+1][k].e2(),
		intEdl[j1+1][k].e3() );
	lBsend[k-k1] = Vector3( intBdS[j1+1][k].e1(), intBdS[j1][k].e2(),
		intBdS[j1][k].e3() );
	lJsend[k-k1] = Vector3( I[j1][k].e1(), I[j1+1][k].e2(),
		I[j1+1][k].e3() );
      }
    }
    break;

// Fields sending to right (left moving window)
    case 2:{
      assert(j1 == j2);
// On KC's authority, one always has k1 <= k2
      for(int k=k1; k<=k2; k++) {
        lEsend[k-k1] = intEdl[j1-1][k];
        lBsend[k-k1] = intBdS[j1-1][k];
        lJsend[k-k1] = I[j1-1][k];
      }
    }
    break;

// Fields sending up (to smaller k, downward moving window)
    case 3:{
      assert(k1 == k2);
// On KC's authority, one always has k1 <= k2
      for(int j=j1; j<=j2; j++) {
	lEsend[j-j1] = Vector3( intEdl[j][k1+1].e1(), intEdl[j][k1].e2(),
		intEdl[j][k1+1].e3() );
	lBsend[j-j1] = Vector3( intBdS[j][k1].e1(), intBdS[j][k1+1].e2(),
		intBdS[j][k1].e3() );
	lJsend[j-j1] = Vector3( I[j][k1+1].e1(), I[j][k1].e2(),
		I[j][k1+1].e3() );
      }
    }
    break;

// Fields sending down (to greater k, upward moving window)
    case 4:{
      assert(k1 == k2);
// On KC's authority, one always has k1 <= k2
      for(int j=j1; j<=j2; j++) {
        lEsend[j-j1] = intEdl[j][k1-1];
        lBsend[j-j1] = intBdS[j][k1-1];
        lJsend[j-j1] = I[j][k1-1];
      }
    }
    break;

  }

/*
#ifdef MPI_VERSION
  cout << "Sending stripe:" << endl;
  cout << "E1 =";
  for(int kk=0; kk<=k2-k1; kk++) cout << " " << lEsend[kk].e1();
  cout << endl;
  cout << "E2 =";
  for(int kk=0; kk<=k2-k1; kk++) cout << " " << lEsend[kk].e2();
  cout << endl;
  cout << "E3 =";
  for(int kk=0; kk<=k2-k1; kk++) cout << " " << lEsend[kk].e3();
  cout << endl;
  cout << "B1 =";
  for(int kk=0; kk<=k2-k1; kk++) cout << " " << lBsend[kk].e1();
  cout << endl;
  cout << "B2 =";
  for(int kk=0; kk<=k2-k1; kk++) cout << " " << lBsend[kk].e2();
  cout << endl;
  cout << "B3 =";
  for(int kk=0; kk<=k2-k1; kk++) cout << " " << lBsend[kk].e3();
  cout << endl;
#endif
*/

// Send the fields through the link
/*
#ifdef  MPI_VERSION
  cout << "MPI_RANK = " << MPI_RANK << 
	", SpatialRegionBoundary::sendStripe sending fields. j1,j2,k1,k2 = " <<
	j1 << "," << j2 << "," << k1 << "," << k2 << endl;
#endif
*/

  theLink->sendShiftedFields(lEsend,lBsend,lJsend);

/*
#ifdef  MPI_VERSION
  cout << "MPI_RANK = " << MPI_RANK << 
	", SpatialRegionBoundary::sendStripe sent fields." << endl;
#endif
*/

}

void SpatialRegionBoundary::recvStripe(int dir){

  Vector3 **intBdS = fields->getIntBdS();
  Vector3 **intEdl = fields->getIntEdl();
  Vector3 **I = fields->getI();

  assert(theLink);
#ifdef  MPI_VERSION
  extern int MPI_RANK;
#endif

/*
#ifdef  MPI_VERSION
  cout << "MPI_RANK = " << MPI_RANK << 
	", SpatialRegionBoundary::recvStripe awaiting fields. j1,j2,k1,k2 = " <<
	j1 << "," << j2 << "," << k1 << "," << k2 << endl;
#endif
*/

  theLink->waitShiftedFields();  //  do a synchronize

/*
#ifdef  MPI_VERSION
  cout << "MPI_RANK = " << MPI_RANK << 
	", SpatialRegionBoundary::recvStripe received fields." << endl;
#endif
*/

/*
#ifdef MPI_VERSION
  cout << "Received stripe:" << endl;
  cout << "E1 =";
  for(int kk=0; kk<=k2-k1; kk++) cout << " " << lE[kk].e1();
  cout << endl;
  cout << "E2 =";
  for(int kk=0; kk<=k2-k1; kk++) cout << " " << lE[kk].e2();
  cout << endl;
  cout << "E3 =";
  for(int kk=0; kk<=k2-k1; kk++) cout << " " << lE[kk].e3();
  cout << endl;
  cout << "B1 =";
  for(int kk=0; kk<=k2-k1; kk++) cout << " " << lB[kk].e1();
  cout << endl;
  cout << "B2 =";
  for(int kk=0; kk<=k2-k1; kk++) cout << " " << lB[kk].e2();
  cout << endl;
  cout << "B3 =";
  for(int kk=0; kk<=k2-k1; kk++) cout << " " << lB[kk].e3();
  cout << endl;
#endif
*/

  switch(dir){

// Fields coming from right (right moving window)
    case 1:{
      assert(j1 == j2);
// On KC's authority, one always has k1 <= k2
      for(int k=k1; k<=k2; k++) {
        intEdl[j1-1][k].set_e1(lE[k-k1].e1());
        intEdl[j1][k].set_e2(lE[k-k1].e2());
        intEdl[j1][k].set_e3(lE[k-k1].e3());
        intBdS[j1][k].set_e1(lB[k-k1].e1());
        intBdS[j1-1][k].set_e2(lB[k-k1].e2());
        intBdS[j1-1][k].set_e3(lB[k-k1].e3());
        I[j1-1][k].set_e1(lJ[k-k1].e1());
        I[j1][k].set_e2(lJ[k-k1].e2());
        I[j1][k].set_e3(lJ[k-k1].e3());
      }
    }
    break;

// Fields coming from left (left moving window)
    case 2:{
      assert(j1 == j2);
// On KC's authority, one always has k1 <= k2
      for(int k=k1; k<=k2; k++) {
        intEdl[j1][k] = lE[k-k1];
        intBdS[j1][k] = lB[k-k1];
        I[j1][k] = lJ[k-k1];
      }
    }
    break;

// Fields coming from bottom (from greater k, downward moving window)
    case 3:{
      assert(k1 == k2);
// On KC's authority, one always has k1 <= k2
      for(int j=j1; j<=j2; j++) {
        intEdl[j][k1].set_e1(lE[j-j1].e1());
        intEdl[j][k1-1].set_e2(lE[j-j1].e2());
        intEdl[j][k1].set_e3(lE[j-j1].e3());
        intBdS[j][k1-1].set_e1(lB[j-j1].e1());
        intBdS[j][k1].set_e2(lB[j-j1].e2());
        intBdS[j][k1-1].set_e3(lB[j-j1].e3());
        I[j][k1].set_e1(lJ[j-j1].e1());
        I[j][k1-1].set_e2(lJ[j-j1].e2());
        I[j][k1].set_e3(lJ[j-j1].e3());
      }
    }
    break;

// Fields coming from top (from smaller k, upward moving window)
    case 4:{
      assert(k1 == k2);
// On KC's authority, one always has k1 <= k2
      for(int j=j1; j<=j2; j++) {
        intEdl[j][k1] = lE[j-j1];
        intBdS[j][k1] = lB[j-j1];
        I[j][k1] = lJ[j-j1];
      }
    }
    break;

  }

}

void SpatialRegionBoundary::SRBadvanceB(Scalar t, Scalar dt) {
   Scalar dt2 = dt/2;
   Vector3 **intBdS = fields->getIntBdS();
   Vector3 **intEdl = fields->getIntEdl();
   Vector3 **I = fields->getI();
   Vector3 **iC = fields->getiC();
   Vector3 **iL = fields->getiL();

   int kl = MIN(k1,k2);
   int kh = MAX(k1,k2);
   int jl = MIN(j1,j2);
   int jh = MAX(j1,j2);
   int K = fields->getK();
   int J = fields->getJ();

   //  Advance B 1/2 step
   if(k1==k2) {  //horizontal boundary 
      for(int j=jl;j<jh;j++) {
	 //The regular field solve handles B2[j][k]
	 if(normal == 1) { //  the virtual region is below us
	    lB[j-jl] -= dt2 * Vector3 ( intEdl[j][k1].e3() - lE[j-jl].e3(),
				       0,  //handled by regular field solve
				       lE[j-jl].e1() - intEdl[j][k1].e1() +
				       lE[j-jl+1].e2() - lE[j-jl].e2());
	 }
	 else {    // the virtual region is above us
	    lB[j-jl] -= dt2 * Vector3 ( -intEdl[j][k1].e3() + lE[j-jl].e3(),
				       0,
				       -lE[j-jl].e1() + intEdl[j][k1].e1() +
				       lE[j-jl+1].e2() - lE[j-jl].e2());
	 }
      }
   }
   else {  //vertical boundary
      for(int k=kl;k<kh;k++) {
	 int kk = k-kl;
	 // write the local E fields to lEa.  The other side is not "authoritative"
	 // and if we don't, we get an instability!!
	 lEa[kk].set_e2(intEdl[j1][k].e2());
	 lEa[kk].set_e3(intEdl[j1][k].e3());
	 //The regular field solve handles B1[j][k]
	 if(normal == 1) {  // lB handles the virtual fields on the left
	    /* new way of doing this:  use the LOCAL E's where possible. */
	    lBa[kk] = intBdS[j1][k];  // These are the authoratitive fields

	    lBb[kk] -= dt2 * Vector3( NEW_B1( lEb[kk], lEb[kk+1]),
				     NEW_B2( lEb[kk], intEdl[j1][k]),
				     NEW_B3( lEb[kk], intEdl[j1][k], lEb[kk+1]));
	    
	    lBc[kk] -= dt2 * Vector3( NEW_B1( lEc[kk], lEc[kk+1]),
				     NEW_B2( lEc[kk], lEb[kk]),
				     NEW_B3( lEc[kk], lEb[kk],lEc[kk+1]));
	 }
	 else {  //lB handles tp *intEdl[4]@4he virtual fields on the right
	    /* new way of doing this:  we use the OTHER side's version of E3, E2 to make sure we're consistent */
	    lBa[kk] -= dt2 * Vector3 ( NEW_B1( lEa[kk], lEa[kk+1]), //These have correct  E3, E2, not E1   
				      NEW_B2( lEa[kk], lEb[kk]),  
				      NEW_B3( lEa[kk], lEb[kk], lEa[kk+1]));

	    lBb[kk] -= dt2 * Vector3 ( NEW_B1( lEb[kk],lEb[kk+1]),
				      NEW_B2( lEb[kk], lEc[kk]),
				      NEW_B3( lEb[kk],lEc[kk],lEb[kk+1]));

	    lBc[kk] -= dt2 * Vector3 ( NEW_B1( lEc[kk], lEc[kk+1]),
				      0, // cannot calculate:  insufficient info
				      0); // cannot calculate:  insufficient info
	    // no update of intBds necessary, lB's are all we need, lB1 done properly by reg.
	    // field solve.
	 }
      }
   }
}


void SpatialRegionBoundary::SRBadvanceE(Scalar t, Scalar dt) {
   Scalar dt2 = dt/2;
   Vector3 **intBdS = fields->getIntBdS();
   Vector3 **intEdl = fields->getIntEdl();
   Vector3 **I = fields->getI();
   Vector3 **iC = fields->getiC();
   Vector3 **iL = fields->getiL();

   int kl = MIN(k1,k2);
   int kh = MAX(k1,k2);
   int jl = MIN(j1,j2);
   int jh = MAX(j1,j2);
   int K = fields->getK();
   int J = fields->getJ();

   if(k1==k2) {  //horizontal boundary 
      for(int j=jl;j<jh;j++) {
	 
	 if(normal==1) {  //real region is below
	    //first update the current
	    I[j][k1]+=Vector3(lJ[j-jl].e1(),0,lJ[j-jl].e3());
	    

	    //update the E fields
	    intEdl[j][k1]+=dt * iC[j][k1-1].jvMult(Vector3(
							   ((j<J)?lB[j-jl].e3() * liL[j-jl].e3() - intBdS[j][k1-1].e3() * iL[j][k1-1].e3():0),
							   0,  //This field isn't my problem here, fields takes care of it
							   +intBdS[j][k1-1].e1() * iL[j][k1-1].e1() - lB[j-jl].e1() * liL[j-jl].e1()
							   +((j<J)?intBdS[j][k1].e2() * iL[j][k1].e2():0) - ((j>0)?intBdS[j-1][k1].e2()*iL[j-1][k1].e2():0))
						   -Vector3(I[j][k1].e1(),0,I[j][k1].e3()));

	    // need to update this component for to-nodes.
	    //		  lE[k-kl]+= dt * iC[j1+1][k].jvMult(Vector3(lB[k-kl].e3()*iL[j1+1][k].e3()
	    //						-((k>0)?lB[k-kl-1].e3()*iL[j1+1][k-1].e3():0) - lJ[k-kl].e1(),0,0));
	    
	 }
	 else {  // real region is above.
	    //first update the current
	    I[j][k1]+=Vector3(lJ[j-jl].e1(),0,lJ[j-jl].e3());

	    //update the E fields
	    intEdl[j][k1]+=dt * iC[j][k1-1].jvMult(Vector3(
							   -((j<J)?lB[j-jl].e3() * liL[j-jl].e3() + intBdS[j][k1].e3() * iL[j][k1].e3():0),
							   0,  //This field isn't my problem here, fields takes care of it
							   -intBdS[j][k1-1].e1() * iL[j][k1-1].e1() + lB[j-jl].e1() * liL[j-jl].e1()
							   +((j<J)?intBdS[j][k1].e2() * iL[j][k1].e2():0) - ((j>0)?intBdS[j-1][k1].e2()*iL[j-1][k1].e2():0))
						   -Vector3(I[j][k1].e1(),0,I[j][k1].e3()));

	    
	    
	 }
      }
      
   }
   else {  //vertical boundary
      for(int k=kl;k<=kh;k++) {
	 int kk = k - kl;
	 // First, update the current
	 I[j1][k]+=Vector3(0,lJa[kk].e2(),lJa[kk].e3());
	 lJa[kk] = Vector3(lJa[kk].e1(),I[j1][k].e2(),I[j1][k].e3());  // make sure we got the right value in lJa too

	 if(normal==1) { //real region is to the right.
	    // ASSUME:  UNIFORM MESH (assume iL)
	    // ASSUME:  UNIFORM DIELECTRIC CONSTANT (assume iC)
	    // ASSUME:  NO BOUNDARY CONDITION ON OPPOSITE SIDE  (assume iC)
	    // To remove these assumptions, iC and iL must be passed and recalculated.

	    lEa[kk] += dt * Vector3(0,  // Reg. field solve calculates this.
				    NEW_E2( iC[j1+1][k], iL[j1+1][k], iL[j1+1][k], intBdS[j1][k], lBb[kk], I[j1][k]),
				    NEW_E3( iC[j1+1][k], iL[j1+1][k], iL[j1+1][MAX(k-1,0)], iL[j1+1][k], intBdS[j1][k], ((kk>0)? intBdS[j1][k-1]: Vector3(0)),
					   lBb[kk], I[j1][k]));

	    lEb[kk] += dt * Vector3( NEW_E1( iC[j1+1][k], iL[j1+1][k], iL[j1+1][MAX(k-1,0)], lBb[kk], ((kk>0)? lBb[kk-1]: Vector3(0)),
					    lJb[kk]),
				    NEW_E2( iC[j1+1][k], iL[j1+1][k], iL[j1+1][k], lBb[kk], lBc[kk], lJb[kk]),
				    NEW_E3( iC[j1+1][k], iL[j1+1][k], iL[j1+1][MAX(k-1,0)], iL[j1+1][k], lBb[kk], ((kk>0)? lBb[kk-1]: Vector3(0)),
					   lBc[kk], lJb[kk]));

	    lEc[kk] += dt * Vector3( NEW_E1( iC[j1+1][k], iL[j1+1][k], iL[j1+1][MAX(k-1,0)], lBc[kk], ((kk>0)? lBc[kk-1]: Vector3(0)),
					    lJc[kk]),
				    0,  //  Cannot calculateNEW_E2( iC[j1+1][k], iL[j1+1][k], iL[j1+1][k], lBb[kk], lBc[kk], lJb[kk]),
				    0);// cannot calculate NEW_E3( iC[j1+1][k], iL[j1+1][k], iL[j1+1][k], lBb[kk], ((kk>0)? lBb[kk-1]: Vector3(0)),
	    //	 lBc[kk], lJb[kk]));
	 }
	 else {  // real region to the left.
	    // ASSUME:  UNIFORM MESH (assume iL)
	    // ASSUME:  UNIFORM DIELECTRIC CONSTANT (assume iC)
	    // ASSUME:  NO BOUNDARY CONDITION ON OPPOSITE SIDE  (assume iC)
	    // To remove these assumptions, iC and iL must be passed and recalculated.
	    lEa[kk] += dt * Vector3( NEW_E1( iC[j1-1][k], iL[j1-1][k], iL[j1-1][MAX(k-1,0)], lBa[kk], ((kk>0)? lBa[kk-1]: Vector3(0)),
					    lJa[kk]),
				    NEW_E2( iC[j1-1][k], iL[j1-1][k], iL[j1-1][k], lBa[kk], intBdS[j1-1][k], I[j1][k]),
				    NEW_E3( iC[j1-1][k], iL[j1-1][k],iL[j1-1][MAX(k-1,0)], iL[j1-1][k], lBa[kk], ((kk>0)? lBa[kk-1]: Vector3(0)),
					   intBdS[j1-1][k], I[j1][k]));

	    lEb[kk] += dt * Vector3( NEW_E1( iC[j1-1][k], iL[j1-1][k], iL[j1-1][MAX(k-1,0)], lBb[kk], ((kk>0)? lBb[kk-1]: Vector3(0)),
					    lJb[kk]),
				    NEW_E2( iC[j1-1][k], iL[j1-1][k], iL[j1-1][k], lBb[kk], lBa[kk], lJb[kk]),
				    NEW_E3( iC[j1-1][k], iL[j1-1][k],iL[j1-1][MAX(k-1,0)], iL[j1-1][k], lBb[kk], ((kk>0)? lBb[kk-1]: Vector3(0)),
					   lBa[kk], lJb[kk]));

	    lEc[kk] += dt * Vector3( NEW_E1( iC[j1-1][k], iL[j1-1][k], iL[j1-1][MAX(k-1,0)], lBc[kk], ((kk>0)? lBc[kk-1]: Vector3(0)),
					    lJc[kk]),
				    0,  //  Cannot calculateNEW_E2( iC[j1+1][k], iL[j1+1][k], iL[j1+1][k], lBb[kk], lBc[kk], lJb[kk]),
				    0);// cannot calculate NEW_E3( iC[j1+1][k], iL[j1+1][k], iL[j1+1][k], lBb[kk], ((kk>0)? lBb[kk-1]: Vector3(0)),
	    //	 lBc[kk], lJb[kk]));


	    
	 }
	 // write back the updated E fields
	 intEdl[j1][k] = Vector3(intEdl[j1][k].e1(),lEa[kk].e2(),lEa[kk].e3());
      }
   }
}

/**
 * Ask for a stripe of NGDs for this boundary
 */
void SpatialRegionBoundary::askNGDStripe() {
  assert(theLink);
  theLink->askShiftedNGDs();
}
/**
 * Send a stripe of the data structures related to 
 * the Neutral Gas Density  for this boundary
 *
 * Again, only the right moving window is implemented
 * for now: case 1 in the switch statement. 
 *
 * @param dir the direction integer
 */
void SpatialRegionBoundary::sendNGDStripe(int dir) 
  /* throw(Oops) */{
 
#ifdef  MPI_VERSION
  extern int MPI_RANK;
#endif

  switch(dir){

    // NGDs sending to left (right moving window)
    case 1:{
      assert(j1 == j2);
      // On KC's authority, one always has k1 <= k2
      // note that for the NGDs we loop to k<k2; dad 05/23/01.
      int cntr = 0;
      NGD* ptrNGD;
      oopicListIter<NGD> NGDIter(*(ptrMapNGDs->getPtrNGDList()));
      for ( NGDIter.restart(); !NGDIter.Done(); NGDIter++ ) {
        ptrNGD = NGDIter.current(); 
        for(int k=k1; k<k2; k++) {
          try{
            lNGDbufSend[cntr++] = ptrNGD->getNGD(j1, k);
            if ( ptrNGD->getisTIOn() ) {
              lNGDbufSend[cntr++] = ptrNGD->getNGD_excessNumIons(j1, k);
              lNGDbufSend[cntr++] = ptrNGD->getNGD_TI_np2c(j1, k);
            }
          }
          catch(Oops& oops){
            oops.prepend("SpatialRegionBoundary::sendNGDStripe: Error: \n"); //OK
            throw oops;
          }
        }
      }
      /*
        #ifdef  MPI_VERSION
        cout << "MPI_RANK = " << MPI_RANK << 
        ", SpatialRegionBoundary::sendStripe sending NGDs." << endl;
        #endif
      */
      try{
        theLink->sendShiftedNGDs( lNGDbufSend, cntr );
      }
      catch(Oops& oops){
        oops.prepend("SpatialRegionBoundary::sendNGDStripe: Error:\n");  //OK
        throw oops;
      }


      /*
        #ifdef  MPI_VERSION
        cout << "MPI_RANK = " << MPI_RANK << 
	", SpatialRegionBoundary::sendStripe sent NGDs." << endl;
        #endif
      */
    }
    break;
  default:{
    stringstream ss (stringstream::in | stringstream::out);
    ss << "SpatialRegionBoundary::sendNGDStripe: Error: \n"<<
        "Only right moving window (dir = 1) " << endl
           << "is implemented for now! You have requested dir = " << dir << endl;
         
      std::string msg;
      ss >> msg;
      Oops oops(msg);
      throw oops;    // exit()MapNGDs::sendNGDsAndShift

    }
    break;
  }
}
/**
 * Receive a stripe of NGDs from this boundary.
 */
void SpatialRegionBoundary::recvNGDStripe(int dir) 
  /* throw(Oops) */{

  assert(theLink);
#ifdef  MPI_VERSION
  extern int MPI_RANK;
#endif
  
  theLink->waitShiftedNGDs();  //  do a synchronize

  switch(dir){

    //
    // NGDs coming from right (right moving window)
    // Only this case is impleneted for now. 
    // dad 05/24/01.
    //
    case 1:{
      assert(j1 == j2);
// On KC's authority, one always has k1 <= k2
      int cntr = 0;
      int j = j1 - 1;
      NGD* ptrNGD;
      oopicListIter<NGD> NGDIter(*(ptrMapNGDs->getPtrNGDList()));
      for ( NGDIter.restart(); !NGDIter.Done(); NGDIter++ ) {
        ptrNGD = NGDIter.current(); 
        for(int k = k1; k < k2; k++) {
          ptrNGD->set(j, k, lNGDbuf[cntr++]);
          if ( ptrNGD->getisTIOn() ) {
            try{
              ptrNGD->setNGD_excessNumIons(j, k, lNGDbuf[cntr++]);
              ptrNGD->setNGD_TI_np2c(j, k, lNGDbuf[cntr++]);
            }
            catch(Oops& oops){
              oops.prepend("SpatialRegionBoundary::recvNGDStripe: Error: \n"); //OK
              throw oops;
            }
          }
        }
      }
    }
    break;
    //
    // the default case is to print an error message and exit
    //
  default:{
    stringstream ss (stringstream::in | stringstream::out);
    ss <<"SpatialRegionBoundary::recvNGDStripe: Error: \n"<<
        "Only right moving window (dir = 1)\n" <<
        "is implemented for now! You have requested dir = " << dir << endl;
      std::string msg;
      ss >> msg;
      Oops oops(msg);
      throw oops;    // exit() MapNGDs::sendNGDsAndShift: 

    }
    break;
  }
}

/**
 * A helper function to allocate memory for the 
 * data structures needed for the send/recv of
 * the NGD stuff. 
 */
void SpatialRegionBoundary::allocMemNGDbuffers() {
  if ( ptrMapNGDs ) {
    int numNGDs = ptrMapNGDs->getNumNGDs();
    if ( numNGDs ) {
#ifdef MPI_VERSION
      extern int MPI_RANK;
      cout << "MPI rank " << MPI_RANK 
           << " in void SpatialRegionBoundary::allocMemNGDbuffers()" 
           << endl;
#endif
      cout << "Allocating memory for NGD buffers..." << endl;
      int length = MAX(abs(k1-k2),abs(j1-j2));
      //
      // currently the number of elements in the NGD buffers
      // is determined by the length times the number of NGDs
      // instantiated times at most 3 (there are currently three data 
      // sets of size length that have to be communicated; 
      // these are the NGDs, the np2c's, and the excess number
      // of ions, see the NGD class). dad 01/18/02. 
      // 
      numElementsNGDbuf = 0; 
      NGD* ptrNGD;
      oopicListIter<NGD> NGDIter(*(ptrMapNGDs->getPtrNGDList()));
      for ( NGDIter.restart(); !NGDIter.Done(); NGDIter++ ) {
        ptrNGD = NGDIter.current(); 
        if ( ptrNGD->getisTIOn() )
          numElementsNGDbuf += 3*length;
        else
          numElementsNGDbuf += length;
      }
      lNGDbuf = new Scalar[numElementsNGDbuf];
      lNGDbufSend = new Scalar[numElementsNGDbuf];    
      //
      // zero the memory
      // 
      memset(lNGDbuf,     0, numElementsNGDbuf*sizeof(Scalar));
      memset(lNGDbufSend, 0, numElementsNGDbuf*sizeof(Scalar));
    }
  }
} 
