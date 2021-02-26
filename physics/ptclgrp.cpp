/*
====================================================================

File:		ptclgrp.cpp

Purpose:	Implementation of a ParticleGroup.

Version:	$Id: ptclgrp.cpp 2295 2006-09-18 18:05:10Z yew $

Revision History
0.99	(JohnV 01-07-94) Separate from pic.h.
0.991	(JohnV 03-23-94) Streamline includes, remove pic.h.
0.992	(JohnV, KC 05-02-94) Fix Brilloiun beam problem (was neglecting
      centrifugal force).
0.993	(JohnV 09-13-94) Add variable np2c support.
0.994 (PeterM 09-04-95) Added a method to reduce #particles, increase
      the weight of particles.
2.001 (JohnV 11-10) Improve increaseWeight(), handle odd n. 
2.02  (JohnV 12-12-97) Modified increaseParticleWeight() to retain temporal
      order of particles within groups -> handles default.inp properly.
2.03  (KC 1-9)  Added Kinetic Energy diagnostic to advance.
2.04  (Bruhwiler 9/29/99) set bool synchRadiationOn=FALSE in constructor
2.05  (Bruhwiler 11/08/99) now moving window only checks for particles
      leaving the left boundary of the simulation
2.0?  (Cary 23 Jan 99) Noted in a comment that this coding is only for
	right moving windows.
CVS-1.16.2.20 (JohnV 15 Mar 00) Fixed bug in np2cFactor (getting ignored)
2.1   (JohnV 10Jul2003) fixed recomputation of gamma before x update

====================================================================
*/

#include "ptclgrp.h"
#include "particle.h"
#include "fields.h"
#include "dump.h"


using namespace std;

ParticleGroup::ParticleGroup(int _maxN, Species* s, Scalar _np2c0, 
	BOOL vary_np2c)
{
	maxN = _maxN;
	if (vary_np2c)
		qArray = new Scalar[maxN];
	else qArray = 0;	
	np2c0 = _np2c0;
	species = s;
	q_over_m = species->get_q_over_m();
	q = np2c0*species->get_q();
//	speciesSub = species->get_speciesSub();
	x = new Vector2[maxN];
	u = new Vector3[maxN];
	n = 0;

   synchRadiationOn = FALSE;  // by default, synchrotron radiation is off
}

ParticleGroup::~ParticleGroup()
{
	n = maxN = 0;								// deallocate memory
	delete[] x;
	delete[] u;
	if (qArray)
	  delete[] qArray;
}

//--------------------------------------------------------------------
//	This add uses nominal np2c0, q, q_over_m

BOOL ParticleGroup::addMKS(Vector2 _x, Vector3 _v)
{
	if (n < maxN)
	{
		x[n] = _x;
		Vector3	beta = iSPEED_OF_LIGHT*_v;
		u[n] = _v/sqrt(1 - beta*beta);	//	u = gamma*v
		if (qArray) qArray[n] = q;
		n++;
		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------------
//	Add a group of particles.  Not well tested yet.

BOOL ParticleGroup::add(ParticleGroup& p)
{
	if (p.q_over_m == q_over_m && p.np2c0 == np2c0)
		while (--p.n >= 0)
		{
		  if (p.qArray){
			 if (!add(x[p.n], u[p.n], p.get_np2c(p.n)))
				break;
		  }
		  else if (p.get_q(p.n) == q){
			 if (!add(x[p.n], u[p.n]))	//	did add fail?
				break;						//	bail out
		  }
		}
	p.n++;										//	restore proper value
	if (p.n) return FALSE;
	else return TRUE;     					//	caller responsible for destructor?
}

//-------------------------------------------------------------------
// duplicates particles in group N times, dividing the particle weight appropriately

/*
ParticleGroup**     ParticleGroup::duplicateParticles(int N)
{
  int i, j, ntemp;
  BOOL varweight;
  Vector2 xN;
  Vector3 uN;
  Scalar np2cfactor;
  Scalar Nfactor;
  ParticleGroup **pg;
  ParticleGroup *p;

  if(N < 1) { return NULL; }
  Nfactor = 1.0/((Scalar)(N+1));
  ntemp = n;
  varweight = isVariableWeight();

  //  fprintf(stderr, 
  //	  "\nduplicateParticles %d times, Nfactor=%g\n", N, Nfactor);
  //  cerr << "\nVariable weight: " << varweight << ", np2c0 =" << np2c0;

  pg = new ParticleGroup*[N];

  if(ntemp > 0)
    {
      if(varweight)
	{
	  np2cfactor = np2c0;
	}
      else
	{
	  np2cfactor = np2c0*Nfactor;
	}
    }
  else
    {
      return NULL;
    }

  //  fprintf(stderr, "\nnp2cfactor=%g", np2cfactor);

  for(i=0; i < N; i++)
    {
      pg[i] = new ParticleGroup(maxN, species, np2cfactor, isVariableWeight());
    }
  
  for(j=0; j < ntemp; j++)
    {
      xN = x[j];
      uN = u[j];
      np2cfactor = get_np2c(j)*Nfactor;
      if(varweight) 
	{ 
	  qArray[j] = q*np2cfactor/np2c0;
	}
      
      for(i=0; i < N; i++)
	{
	  p = pg[i];

	  if(varweight)
	    {
	      if(!p->add(xN, uN, np2cfactor)) 
		{ 
		  fprintf(stderr, "\nduplicateParticles: error adding particles");
		  return NULL; 
		}
	    }
	  else
	    {
	      if(!p->add(xN, uN, -1.)) 
		{ 
		  fprintf(stderr, "\nduplicateParticles: error adding particles");
		  return NULL; 
		}
	    }	  
	}
    }
  if(!varweight)
    {
      np2c0 *= Nfactor;
      q = np2c0*species->get_q();
      //      fprintf(stderr, "\nSetting base particle weight to %g\n", np2c0);
    }
  return pg;
}
*/

ParticleGroup**     ParticleGroup::duplicateParticles(int N)
{
  int i, j, ntemp;
  BOOL varweight;
  Vector2 xN;
  Vector3 uN;
  Scalar np2cfactor;
  Scalar Nfactor;
  ParticleGroup **pg;
  ParticleGroup *p;

  if(N < 1) { return NULL; }
  Nfactor = 1.0/((Scalar)(N+1));
  ntemp = n;
  varweight = isVariableWeight();

  //  fprintf(stderr, 
  //	  "\nduplicateParticles %d times, Nfactor=%g\n", N, Nfactor);
  //  cerr << "\nVariable weight: " << varweight << ", np2c0 =" << np2c0;

  pg = new ParticleGroup*[N];

  if(ntemp > 0)
    {
      np2cfactor = np2c0;
    }
  else
    {
      return NULL;
    }

  //  fprintf(stderr, "\nnp2cfactor=%g", np2cfactor);

  for(i=0; i < N; i++)
    {
      pg[i] = new ParticleGroup(maxN, species, np2cfactor, isVariableWeight());
    }
  
  for(j=0; j < ntemp; j++)
    {
      xN = x[j];
      uN = u[j];
      np2cfactor = get_np2c(j);
      if(varweight) 
	{ 
	  qArray[j] = q*np2cfactor/np2c0;
	}
      else
	{
	  np2cfactor = -1.;
	}
      
      for(i=0; i < N; i++)
	{
	  p = pg[i];

	  if(!p->add(xN, uN, -1.)) 
	    { 
	      fprintf(stderr, "\nduplicateParticles: error adding particles");
	      return NULL; 
	    }
	}
    }

  return pg;
}

//--------------------------------------------------------------------
//	Add a new particle to this group, possibly of variable weight.
//	Note that uNew is gamma*v, and xNew is in grid coords.

BOOL ParticleGroup::add(const Vector2& xNew, const Vector3& uNew, Scalar np2cNew)
{
	if (n < maxN)
	{
		if (np2cNew > 0) {
			if (qArray)	{
				qArray[n] = q*np2cNew/np2c0;
			} else {
				return FALSE;
			}
		}

		x[n] = xNew;
		u[n] = uNew;
		n++;
		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------------
//	Add a Particle to this group.

BOOL ParticleGroup::add(Particle* p)
{
//	if (p->get_q_over_m() == q_over_m)
	if (p->get_speciesID() == get_speciesID() && n < maxN) {
		if (qArray && p->isVariableWeight()) {
			//		if (qArray)
			return add(p->get_x(), p->get_u(), p->get_np2c());
		} else if (p->get_np2c() == np2c0) {
			return add(p->get_x(), p->get_u());
		}
	}

	return FALSE;
}

//
// we need to be able to remove particles from the particle group (in the
// tunneling ionization of multi electron atoms). Note the implementation
// of this function: a particle with index particleIndex is deleted and
// the last particle in the group is placed at its index.    
// Returns TRUE if the particle group becomes empty (at deletion of 
// the last particle in the group), otherwise it resturns FALSE. dad 06/29/01.
//
BOOL ParticleGroup::remove(int particleIndex) {
  //
  // is particleIndex in the allowable range
  // 
  if ( particleIndex < 0 || particleIndex > (n-1) ) {
    cerr << endl << endl << endl
         << "An out of range particleIndex = " << particleIndex << endl
         << "passed to BOOL ParticleGroup::remove(int particleIndex)." << endl
         << "This index must be in [0, " << n-1 << "]" << endl
         << "Exiting to the system..." << endl << endl << endl;
  }
  
  if ( n == 1 ) {
    // 
    // we are deleting the only macroparticle in the group
    // => return false to prompt the deletion of the group
    // 
    n--;
    return TRUE;
  } else if ( particleIndex == (n-1) ) {
    //
    // deleting the last macroparticle in a group with more
    // than one macroparticles => need only to decrement the
    // number of macroparticles in the group
    //  
    n--;
    return FALSE;
  } else {
    // 
    // deleting any other particle but the last one
    // 
    x[particleIndex] = x[n-1];
    u[particleIndex] = u[n-1];
    if ( qArray ) 
      qArray[particleIndex] = qArray[n-1];
    n--;
    return FALSE;
  } 
}

//--------------------------------------------------------------------
//	Advance particle equations of motion one timestep.  This includes
//	weighting fields to the particle, solving the equation of motion,
//	and accumulating the particle current on the grid.  Boris mover
//	with u = gamma*v, v in MKS units; see B&L for variables.

void ParticleGroup::advance(Fields& fields, Scalar dt)
{
	Scalar	f = 0.5*dt*q_over_m;
	Scalar	q_dt = q/dt;
	Vector3	uPrime;
	Vector3	a;
	Vector3	t;
	Vector3	s;
	Vector2*	x = ParticleGroup::x;
	Vector3*	u = ParticleGroup::u;
	Vector3	dxMKS;
	Boundary*	bPtr;
	Grid&	grid = *fields.get_grid();
	Scalar igamma;
	Scalar localEnergy = 0;

	for (int i=0; i<n; i++, x++, u++)
	{
		if (qArray) q_dt = qArray[i]/dt;	// variable weighted particles
		a = f*fields.E(*x);					// half of the change of velocity due to accelerations
//
		*u += a;							// 1. half: change the velocity because of the acceleration
		igamma = 1/ParticleGroup::gamma(*u);
		t = (f*igamma)*fields.B(*x);
		uPrime = *u + u->cross(t);
		s = 2*t/(1 + t*t);
		*u += uPrime.cross(s);				// rotation
		*u += a; 							// 2. half: change the velocity because of the acceleration
		igamma = 1/ParticleGroup::gamma(*u); // must recompute gamma
		dxMKS = grid.differentialMove(fields.getMKS(*x), *u*igamma, *u, dt);
		if ((bPtr = fields.translateAccumulate(*x, dxMKS, q_dt)))
		{
			//	send this particle to boundary
			bPtr->collect(*(new Particle(*x, *u, species, get_np2c(i),
				(BOOL)(qArray!=0))), dxMKS);
			//	Move last particle into this slot and advance it.
			n--;
			if (i == n) break;				//	last particle in array?
			*x = ParticleGroup::x[n];		//	move last particle into open slot
			*u = ParticleGroup::u[n];
			if (qArray) qArray[i] = qArray[n];
			i--;							//	Set index to do i again.
			x--;							//	Setup to redo the array element
			u--;							// it gets inc in for loop
		}
		else{
			if (qArray)
				localEnergy += qArray[i]*(1/igamma-1);
			else
				localEnergy += 1/igamma-1;
		}
#ifdef DEBUG
		if(strncmp(HOSTTYPE, "alpha", 5) != 0)
		  {
		    if(((*x).e1()==grid.getJ() || (*x).e1()==0 || (*x).e2()==grid.getK()
			|| (*x).e2()==0)&&!bPtr) {
		      printf("Particle escaped.\n");
		      int *crash = 0;	
		      *crash = 10;
		    }
		  }
#endif //DEBUG
	}
	if (qArray)
		species->addKineticEnergy(SPEED_OF_LIGHT_SQ*localEnergy/q_over_m);
	else
		species->addKineticEnergy(SPEED_OF_LIGHT_SQ*localEnergy*get_m());
}

// this function combs the particles, doubling weights
// and killing every second particle. Note that the particles must
// remain time-ordered so that multiple packings do not leave gaps.

void ParticleGroup::increaseParticleWeight()
{
	int new_n = (int) (n*0.5 + frand()); // keep odd particles 50% time
	Scalar ratio = 2; // = ((Scalar) n)/new_n;
	if (qArray) qArray[0] *= ratio;
	for (int i = 1; i < new_n; i++)
	{
		x[i]=x[2*i];
		u[i]=u[2*i];
		if(qArray) qArray[i] = qArray[2*i]*ratio;
	}
	n = new_n;	// halve the num of particles
	np2c0 *= ratio;	// double the weights
	q *= ratio;		// double the charge
}

void ParticleGroup::fill(int index)
{
	n--;
	if (index == n) return;				//	last particle in array?
	x[index] = x[n];		//	move last particle into open slot
	u[index] = u[n];
	if (qArray) qArray[index] = qArray[n];
}
#ifdef HAVE_HDF5
int ParticleGroup::dumpH5(dumpHDF5 &dumpObj, Grid *grid,string groupName) 
{

  //cerr << "entering ptclgrp::dumpH5(dumpObj)\n";
 
  int vary_np2c = 0;
  int speciesID = get_speciesID();
//  Scalar temp;
  Vector2 xMKS;
//  char theDumpFileName[256];
  // char *fdname ={"pdmp"};

  hid_t fileId, groupId, datasetId,dataspaceId;
  herr_t status;
  hsize_t rank; 
  hsize_t     dimsf[2];   
  hsize_t     dims;
//  int         attrData;
  hid_t  attrdataspaceId,attributeId;
#ifdef NEW_H5S_SELECT_HYPERSLAB_IFC
  hsize_t start[2]; /* Start of hyperslab */
#else
  hssize_t start[2]; /* Start of hyperslab */
#endif
  hsize_t count[2];  /* Block count */

  Scalar (*qData) = new Scalar[n];
  hid_t scalarType = dumpObj.getHDF5ScalarType();
 
  if (qArray)	vary_np2c = 1;
 
 // Open the hdf5 file with write access
  fileId = H5Fopen(dumpObj.getDumpFileName().c_str(), H5F_ACC_RDWR, H5P_DEFAULT);
  groupId = H5Gcreate(fileId,groupName.c_str(),0);

 
  // dump speciesID as attribute
   dims = 1;
   attrdataspaceId = H5Screate_simple(1, &dims, NULL);
   attributeId = H5Acreate(groupId, "speciesID",H5T_NATIVE_INT, attrdataspaceId, H5P_DEFAULT);
   status = H5Awrite(attributeId,H5T_NATIVE_INT , &speciesID);
   status = H5Aclose(attributeId);
   status = H5Sclose(attrdataspaceId);
   
 // dump vary_np2c as attribute
   dims = 1;
   attrdataspaceId = H5Screate_simple(1, &dims, NULL);
   attributeId = H5Acreate(groupId, "vary_np2c",H5T_NATIVE_INT, attrdataspaceId, H5P_DEFAULT);
   status = H5Awrite(attributeId,H5T_NATIVE_INT , &vary_np2c);
   status = H5Aclose(attributeId);
   status = H5Sclose(attrdataspaceId);

 // dump np2c as attribute
   dims = 1;
   attrdataspaceId = H5Screate_simple(1, &dims, NULL);
   attributeId = H5Acreate(groupId, "np2c",scalarType, attrdataspaceId, H5P_DEFAULT);
   status = H5Awrite(attributeId, scalarType, &np2c0);
   status = H5Aclose(attributeId);
   status = H5Sclose(attrdataspaceId);
 
   rank = 2;
   dimsf[0] = n;
   dimsf[1] = 5;
   dataspaceId = H5Screate_simple(rank, dimsf, NULL); 
   string datasetName = "ptcls";  
   datasetId = H5Dcreate(groupId, datasetName.c_str(), scalarType, dataspaceId,
			H5P_DEFAULT);


   hsize_t dim5 = 5;
   hid_t posSlabId = H5Screate_simple(1, &dim5, NULL);

   for(int i=0;i<n;i++){		
     
     xMKS = grid->getMKS(x[i]);

     Scalar pos[5] = {xMKS.e1(),xMKS.e2(),u[i].e1(),u[i].e2(),u[i].e3()};  
     start[0]  = 0;  count[0]  = 5;
     status = H5Sselect_hyperslab(posSlabId, H5S_SELECT_SET, start, NULL, count, NULL);
     start[0]  = i; start[1]  = 0;
     count[0]  = 1; count[1]  = 5;     
     status = H5Sselect_hyperslab(dataspaceId, H5S_SELECT_SET, start, NULL, count, NULL);
     status = H5Dwrite(datasetId, scalarType, posSlabId, dataspaceId, H5P_DEFAULT, pos);
     
   }


   H5Sclose(posSlabId);
   H5Sclose(dataspaceId);
   H5Dclose(datasetId);


if (qArray)
  {
// write charge   
    rank = 1;
    dims = n;
    dataspaceId = H5Screate_simple(rank, &dims, NULL); 
    string datasetName = "Q";  
    datasetId = H5Dcreate(groupId, datasetName.c_str(), scalarType, dataspaceId,
			  H5P_DEFAULT);

    status = H5Dwrite(datasetId, scalarType, H5S_ALL, H5S_ALL,
		      H5P_DEFAULT, qArray); 

    H5Sclose(dataspaceId);
    H5Dclose(datasetId);
  }
 
 H5Gclose(groupId);
 H5Fclose(fileId);
 return (1);
  
}
#endif

int ParticleGroup::Dump(FILE *DMPFile,Grid *grid) 
{
	int vary_np2c = 0;
	int speciesID = get_speciesID();
//	Scalar temp;
	Vector2 xMKS;
//        char theDumpFileName[256];
        // char *fdname ={"pdmp"};
/*
//reg
        #ifdef MPI_VERSION
	   extern int MPI_RANK;
           sprintf(theDumpFileName,"%s.%d",fdname,MPI_RANK);
	#else
           sprintf(theDumpFileName,"%s",fdname);
        #endif
//
	ofstream dataPart(theDumpFileName, ios::app);
*/

	XGWrite(&speciesID, 4, 1, DMPFile, "int");
	if (qArray)	vary_np2c = 1;
	XGWrite(&vary_np2c, 4, 1, DMPFile, "int");
	XGWrite(&np2c0, ScalarInt, 1, DMPFile, ScalarChar);
	XGWrite(&n, 4, 1, DMPFile, "int");

	if (qArray)
	   {
		for(int i=0;i<n;i++){
			XGWrite(&qArray[i], ScalarInt, 1, DMPFile, ScalarChar);
			xMKS = grid->getMKS(x[i]);
			XGVector2Write(xMKS, DMPFile);
			XGVector3Write(u[i], DMPFile);

		       
/*
			dataPart << xMKS.e1()<<"  " << xMKS.e2() 
				 <<"   "<< u[i].e1() << endl;
*/
		}
	   }
	else{
		for(int i=0;i<n;i++){
			xMKS = grid->getMKS(x[i]);
			XGVector2Write(xMKS, DMPFile);
			XGVector3Write(u[i], DMPFile);
			//			cout << xMKS.e1() << " "<<xMKS.e2()<<endl;

/*
			dataPart << xMKS.e1()<<"  " << xMKS.e2() 
				 <<"   "<< u[i].e1() << endl;
*/

		}
	}
//	dataPart.close();

	return (1);
} 


#ifdef HAVE_HDF5
int ParticleGroup::restoreH5(dumpHDF5 &restoreObj, Grid *grid, Scalar np2cFactor,string whichGroup)
{
  // cerr << "entering ParticleGroup::restoreH5()\n";

  Scalar xmin, xmax,ymin,ymax;
  Vector2 xin;
  Vector3 uin;
  Scalar qin;
  int i;
  int incount;
  int realcount=0;
  hid_t fileId, groupId, datasetId,dataspaceId;
  herr_t status;
  string outFile;
//  hsize_t rank; 
//  hsize_t     dimsf[1];   
//  hsize_t dims;
  hid_t scalarType = restoreObj.getHDF5ScalarType();
//  hid_t  attrspaceId,attrId;
#ifdef NEW_H5S_SELECT_HYPERSLAB_IFC
  hsize_t offset[2]; /* Offset of hyperslab */
#else
  hssize_t offset[2]; /* Offset of hyperslab */
#endif
  hsize_t count[2];  /* Block count */


  fileId = H5Fopen(restoreObj.getDumpFileName().c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
  groupId = H5Gopen(fileId, whichGroup.c_str());

  xmin = grid->getX()[0][0].e1();
  ymin = grid->getX()[0][0].e2();
  xmax = grid->getX()[grid->getJ()][grid->getK()].e1();
  ymax = grid->getX()[grid->getJ()][grid->getK()].e2();

  // read Q values
 
  // read position values
  datasetId = H5Dopen(groupId,"ptcls");
  dataspaceId = H5Dget_space(datasetId );
  int dataRank = H5Sget_simple_extent_ndims(dataspaceId);
  hsize_t *sizes = new hsize_t[dataRank];
  int res = H5Sget_simple_extent_dims(dataspaceId, sizes, NULL);
  incount = sizes[0];
  hsize_t dim = sizes[1];  // should be 5
  hid_t posSlabId =  H5Screate_simple(1, &dim, NULL);
  
  Scalar *ptclData = new Scalar[dim];


  if(qArray){
    hid_t QsetId = H5Dopen(groupId,"Q");
    hid_t QspaceId = H5Dget_space(QsetId );


    for(i=0;i<incount;i++){
      offset[0]  = 0;  count[0]  = dim;
      status = H5Sselect_hyperslab(posSlabId, H5S_SELECT_SET, offset, NULL, count, NULL);
      offset[0]  = i; offset[1]  = 0;
      count[0]  = 1; count[1] = dim;    
      status = H5Sselect_hyperslab(dataspaceId, H5S_SELECT_SET, offset, NULL, count, NULL);
      status = H5Dread(datasetId, scalarType, posSlabId, dataspaceId, H5P_DEFAULT, ptclData);
      
      xin.set_e1(ptclData[0]);
      xin.set_e2(ptclData[1]);
      
      //  cout << ptclData[2] <<" " <<ptclData[3]<<" "<<ptclData[4]<<"\n";
      
      
      uin.set_e1(ptclData[2]);
      uin.set_e2(ptclData[3]);
      uin.set_e3(ptclData[4]);
      
      offset[0]  = 0;  count[0]  = 1;
      status = H5Sselect_hyperslab(posSlabId, H5S_SELECT_SET, offset, NULL, count, NULL);
      offset[0]  = i; offset[1]  = 0;
      count[0]  = 1; count[1] = 1;    
      status = H5Sselect_hyperslab(QspaceId, H5S_SELECT_SET, offset, NULL, count, NULL);
      status = H5Dread(QsetId, scalarType, posSlabId, QspaceId, H5P_DEFAULT, &qin);
      
      // check to see if inbounds
      if(xin.e1() > xmin && xin.e1() < xmax &&
	 xin.e2() > ymin && xin.e2() < ymax) {
	qArray[realcount]=qin*np2cFactor;
	x[realcount] = grid->getGridCoords(xin);
	u[realcount] = uin;
	realcount++;
      }
    }
    
    status =    H5Sclose(QspaceId);
    status =    H5Dclose(QsetId);
    
  }
  else {
    
    //      cout <<"particles at restore:\n";
    //   cerr << "incount"<<incount<<"\n";
    
    for(i=0;i<incount;i++){
      
      offset[0]  = 0;  count[0]  = dim;
      status = H5Sselect_hyperslab(posSlabId, H5S_SELECT_SET, offset, NULL, count, NULL);
      offset[0]  = i; offset[1]  = 0;
      count[0]  = 1; count[1] = dim;    
      status = H5Sselect_hyperslab(dataspaceId, H5S_SELECT_SET, offset, NULL, count, NULL);
      status = H5Dread(datasetId, scalarType, posSlabId, dataspaceId, H5P_DEFAULT, ptclData);
      
      xin.set_e1(ptclData[0]);
      xin.set_e2(ptclData[1]);
      
      uin.set_e1(ptclData[2]);
      uin.set_e2(ptclData[3]);
      uin.set_e3(ptclData[4]);
      
      // check to see if inbounds
      if(xin.e1() > xmin && xin.e1() < xmax &&
	 xin.e2() > ymin && xin.e2() < ymax) {
	x[realcount] = grid->getGridCoords(xin);
	u[realcount] = uin;
	realcount++;
      }
    }
    
  }
  n = realcount;
 
  status =    H5Sclose(dataspaceId);
  status =    H5Dclose(datasetId);
 
  status = H5Sclose(posSlabId);
  H5Gclose(groupId);
  H5Fclose(fileId);

  delete [] sizes;
  return (1);
}
#endif

int ParticleGroup::Restore(FILE *DMPFile,Grid *grid, Scalar np2cFactor) 
{
	Scalar xmin, xmax,ymin,ymax;
	int i;
	int incount;
	int realcount=0;
	Vector2 xin;
	Vector3 uin;
	Scalar qin;

	xmin = grid->getX()[0][0].e1();
	ymin = grid->getX()[0][0].e2();
	xmax = grid->getX()[grid->getJ()][grid->getK()].e1();
	ymax = grid->getX()[grid->getJ()][grid->getK()].e2();

	
	XGRead(&incount, 4, 1, DMPFile, "int");

	if (qArray)
	  for(i=0;i<incount;i++){
			XGRead(&qin, ScalarInt, 1, DMPFile, ScalarChar);
			XGVector2Read(xin, DMPFile);
			XGVector3Read(uin, DMPFile);

			// check to see if inbounds
			if(xin.e1() > xmin && xin.e1() < xmax &&
				xin.e2() > ymin && xin.e2() < ymax) {
				qArray[realcount]=qin*np2cFactor;
				x[realcount] = grid->getGridCoords(xin);
				u[realcount] = uin;
				realcount++;
			}
		}
	else
		for(i=0;i<incount;i++){
			XGVector2Read(xin, DMPFile);
			XGVector3Read(uin, DMPFile);

			// check to see if inbounds
			if(xin.e1() > xmin && xin.e1() < xmax &&
				xin.e2() > ymin && xin.e2() < ymax) {
				x[realcount] = grid->getGridCoords(xin);
				u[realcount] = uin;
				realcount++;
			}
		}

	n = realcount;
	return(1);
}

int ParticleGroup::Restore_2_00(FILE *DMPFile) 
{

	XGRead(&n, 4, 1, DMPFile, "int");
	if (qArray)
		for(int i=0;i<n;i++){
			XGRead(&qArray[i], ScalarInt, 1, DMPFile, ScalarChar);
			XGVector2Read(x[i], DMPFile);
			XGVector3Read(u[i], DMPFile);
		}
	else
		for(int i=0;i<n;i++){
			XGVector2Read(x[i], DMPFile);
			XGVector3Read(u[i], DMPFile);
		}

	return(1);

}


void ParticleGroup::shiftParticles(Fields& fields, Vector2 shift) {

  Vector2*	x = ParticleGroup::x;
  Vector3*	u = ParticleGroup::u;
  Grid *grid = fields.get_grid();
  Boundary *bPtr;
  Scalar smallTest = 1.0e-06;
  Scalar x_e1_tmp;

  for(int i=0;i<n; i++,x++,u++) {

// Shift the particle
    *x += shift;

// Start with no boundary
    bPtr = 0;
    bool removeParticle = false;

// Take care of particles near the boundary.
// Unfortunately, this kind of nonsense is required to prevent
//    particles from being caught "on" the edge of a boundary.
// First, consider particles that should be left behind.
    x_e1_tmp = x->e1();
    if( ( x_e1_tmp >= 0. )  &&  ( x_e1_tmp < smallTest ) ) {
      x->set_e1(smallTest);
		}
// Next, consider particles that need to be shifted.
    else if( ( x_e1_tmp <= 0. )  &&  ( x_e1_tmp > -smallTest ) ) {
      x->set_e1(-smallTest);
		}

// Now do the shift, assuming a rightward moving window.
    if( (shift.e1() == -1.) && ( x->e1() < 0. ) ) {
// Peter's original code:
//      if( ( bPtr=grid->GetBC2()[(int)(*x).e1()][(int)(*x).e2()] ) == 0 )
//                bPtr = grid->GetBC1()[(int)(*x).e1()][(int)(*x).e2()];
// relied on the fact that int of a negative number gives zero.
      bPtr = grid->GetBC2()[0][(int)(*x).e2()];
      if(!bPtr) {
        static bool printed = false;
        if(!printed) cout << "No boundary in shiftParticles!" << endl;
        printed = true;
      }
      removeParticle = true;
    }

// Give the particle to the boundary
    if(bPtr) bPtr->collectShiftedParticle( new Particle(*x, *u, species, 
		get_np2c(i), (BOOL)(qArray!=0) ) );
    if(removeParticle){

// Decrement number of particles
      n--;
      if (i == n) break;          // last particle in array?
      *x = ParticleGroup::x[n];   // move last particle into open slot
      *u = ParticleGroup::u[n];
      if (qArray) qArray[i] = qArray[n];

// Decrement counters as new particle moved here
      i--;
      x--;
      u--;

    }

  }

}
