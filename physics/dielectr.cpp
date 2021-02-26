/*
   ====================================================================

   dielectr.cpp

   0.90	(PeterM 1-9-95) Creation.
   0.91  (JV/PM Mon Jul  3 ) Used bilinear weighting of Q.
   0.992 (JohnV 08-09-95) Add DielectricRegion
   0.993 (JohnV 09-02-95) Move secondary params into setSecondary().
   0.994 (JohnV 10-04-95) Add secondary emission for DielectricRegion, add
   capability for secondary coeff. > 1.
   1.001	(JohnV 05-07-96) Add toNodes() to handle improved interpolation
   at the boundary.
   1.1   (JohnV 10-09-96) Add reflection = fraction of particles reflected.
   1.11  (JohnV 12-16-96) Add refMaxE = Energy ceiling for reflection.
   1.12  (JohnV 02-07-96) Repaired potential problem for edge detection in 
   DielectricRegion::collect().  Also modified offset for secondaries.
   1.13  (JohnV 06-03-97) Added charge collection for secondaries.
   2.0   (JohnV 08-19-97) Added multiple secondary packages.
   2.1   (JohnV 06-15-98) Added Secondary::threshold.
   2.11  (JohnV 06-25-98) Repaired bug for reflected particles at corners.
   2.2   (JohnV 09-01-98) Moved Secondary to secondary.cpp.
   2.3   (JohnV 04-Mar-2002) Added transparency.

   ====================================================================
   */

#include "dielectr.h"
#include "fields.h"
#include "ptclgrp.h"
#include "misc.h"
#include "psolve.h"
#include "secondary.h"

#ifdef _MSC_VER
using std::cout;
using std::cerr;
using std::endl;
#endif
Dielectric::Dielectric(oopicList <LineSegment> *segments, Scalar _er, 
  int _QuseFlag, Scalar reflection, Scalar refMaxE) 
  : Boundary(segments)
{
  setQuseFlag(_QuseFlag);
  if (QuseFlag){
    nPoints = MAX(abs(j1-j2), abs(k1-k2)) + 1;
    Q = new Scalar[nPoints];
    memset(Q, 0, nPoints*sizeof(Scalar));
  }
  else {
    Q = NULL;
    nPoints = 0;
  }
  epsilon = _er/iEPS0;
  BoundaryType = DIELECTRIC;
  BCType = DIELECTRIC_BOUNDARY;
  if (alongx2()) {
    _delta.set_e1((j1==0)? 1e-6 : normal*1e-6*j1);
    _unitNormal.set_e1(normal);
  }
  else {
    _delta.set_e2((k1==0) ? 1e-6 : normal*1e-6*k1);
    _unitNormal.set_e2(normal);
  }
}

Dielectric::~Dielectric()
{
  if (Q) {
    delete [] Q;
    Q = 0;
  }
  secondaryList.deleteAll();
}

void Dielectric::setPassives()
{
}

//  This function sets the voltage for the use of the Poisson solve

void Dielectric::putCharge_and_Current(Scalar t)
{
   int j=0;
   int k=0;
   int ko=0;
   int jo=0;
   if (QuseFlag)
      {
         Scalar **rho=fields->getrho();
         Scalar **cellvolume = fields->get_grid()->get_halfCellVolumes();
         
         if (alongx2())								//	vertical
            for (k=ko=MIN(k1,k2); k<MAX(k1,k2); k++)
               rho[j1][k]+=Q[k-ko]/cellvolume[j1][k];
         else											//	horizontal
            for (jo=j=MIN(j1,j2); j<MAX(j1,j2); j++)
               rho[j][k1]+=Q[j-jo]/cellvolume[j][k1];
      }
}

void Dielectric::putCharge(void)
{
   int j,k,ko,jo;
   if (QuseFlag){
      Scalar **Charge=fields->getQ();
      
      if (alongx2())								//	vertical
         for (k=ko=MIN(k1,k2); k<MAX(k1,k2); k++)
            Charge[j1][k]+=Q[k-ko];
      else											//	horizontal
         for (jo=j=MIN(j1,j2); j<MAX(j1,j2); j++)
            Charge[j][k1]+=Q[j-jo];
   }
}

void Dielectric::setFields(Fields &f)
{
   Boundary::setFields(f);
}

void Dielectric::addSecondary(Secondary* newSecondary)
{
   if (newSecondary->checkSpecies()) secondaryList.add(newSecondary);
   newSecondary->setBoundaryPtr(this);
}

//--------------------------------------------------------------------
// Dielectric::collect() handles weighting to charge to boundary, and
//	also secondary emission.  Some fraction of particles (reflection)
// can be reflected back, when energy <= refMaxE.

void	Dielectric::collect(Particle& p, Vector3& dxMKS)
{
   int j=0;
   Scalar w=0.;
   if ((Scalar)frand() <= reflection && p.energy_eV_MKS() <= refMaxE){  // reflect particles
      Vector3 u = p.get_u();
      Vector2 x = p.get_x();
      if (alongx2()){
         u.set_e1(-u.e1()); // reverse u comp. normal to bc
         dxMKS.set_e1(-dxMKS.e1()); // reverse the movement
         if (u.e1() < 0) x.set_e1(x.e1()*.99999);
         else x.set_e1(x.e1()*1.00001);
         if (x.e2()==(int)x.e2()) x.set_e2(x.e2()*1.00001); // move off corners
         else if (x.e2()==(int)x.e2()+1) x.set_e2(x.e2()*0.99999);
      }
      else {
         u.set_e2(-u.e2());
         dxMKS.set_e2(-dxMKS.e2()); // reverse the movement
         if (u.e2() < 0) x.set_e2(x.e2()*.99999);
         else x.set_e2(x.e2()*1.00001);
         if (x.e1()==(int)x.e1()) x.set_e1(x.e1()*1.00001); // move off corners
         else if (x.e1()==(int)x.e1()+1) x.set_e1(x.e1()*0.99999);
      }
      // note that the dt used below should probably be the remaining fraction for the
      // move rather than the full dt.
      Boundary *bPtr = fields->translateAccumulate(x, dxMKS, p.get_q()/fields->get_dt());
      p.set_u(u);
      p.set_x(x);
      if (bPtr) bPtr->collect(p, dxMKS);
      else particleList.add(&p);
      return;
   }
   else if ((Scalar)frand() < transparency) {
     // note that the dt used below should probably be the remaining fraction for the
     // move rather than the full dt.
     Vector2 x = p.get_x();
     Boundary *bPtr = fields->translateAccumulate(x, dxMKS, p.get_q()/fields->get_dt());
     p.set_x(x);
     if (bPtr) bPtr->collect(p, dxMKS);
     else particleList.add(&p);
     return;
   } 
   if (QuseFlag) {
      if (alongx2()){	// vertical
         j = (int) p.get_x().e2();
         w = p.get_x().e2() - j;
         j -= k1;
      }
      else {
         j = (int) p.get_x().e1();
         w = p.get_x().e1() - j;
         j -= j1;
      }
      Q[j] += p.get_q()*(1-w);
      Q[MIN(j+1, nPoints-1)] += p.get_q()*w;
   }
   Boundary::collect(p, dxMKS); // do statistics;
   if (secondaryList.nItems()){
      p.set_x(p.get_x() + _delta); // perturb off edge;
      oopicListIter<Secondary> sIter(secondaryList);
      for (; !sIter.Done(); sIter++){
         int n = sIter()->generateSecondaries(p, particleList);
         if (QuseFlag){
            Scalar q = n*p.get_np2c()*sIter()->get_q();
            Q[j] -= q*(1-w);
            Q[MIN(j+1, nPoints-1)] -= q*w;
         }
      }
   }
   delete &p;
}
#ifdef HAVE_HDF5
int Dielectric::dumpH5(dumpHDF5 &dumpObj,int number)
{
  
  //cerr << "Entered Dielectric::dumpH5.\n";
  hid_t fileId, groupId, datasetId,dataspaceId;
  herr_t status;
  string outFile;
  hsize_t rank; 
  hsize_t     dimsf[1];   
  hsize_t dims;
  hid_t scalarType = dumpObj.getHDF5ScalarType();
  hid_t  attrdataspaceId,attributeId;
#ifdef NEW_H5S_SELECT_HYPERSLAB_IFC
  hsize_t start[2]; /* Start of hyperslab */
#else
  hssize_t start[2]; /* Start of hyperslab */
#endif
  hsize_t count[2];  /* Block count */
  
  //  strstream s;

  stringstream s;


  s << "boundary" << number <<ends;    // ends provides the null terminator (don`t forget that)
  string dname = s.str();
    
//  // Open the hdf5 file with write accessx
  fileId = H5Fopen(dumpObj.getDumpFileName().c_str(), H5F_ACC_RDWR, H5P_DEFAULT);
  dname = "/Boundaries/" + dname;
  groupId = H5Gcreate(fileId,dname.c_str() ,0); 
  
  string datasetName = "dims";  
  
  rank = 1;
  dims = 4;
  attrdataspaceId = H5Screate_simple(rank, &dims, NULL); 
  attributeId = H5Acreate(groupId, datasetName.c_str(), scalarType, attrdataspaceId,
			  H5P_DEFAULT);
// get the data   
  Grid *grid = fields->get_grid();
// Write the physical dimensions of the boundary
  Scalar xw[4];  // in this format:  xs, ys, xf, yf
  Vector2 **X=grid->getX();
  xw[0] = X[j1][k1].e1();
  xw[1] = X[j1][k1].e2();
  xw[2] = X[j2][k2].e1();
  xw[3] = X[j2][k2].e2();
  
  status = H5Awrite(attributeId, scalarType, xw);
  status = H5Aclose(attributeId);
  status = H5Sclose(attrdataspaceId);

// dump boundary type as attribute
   dims = 1;
   attrdataspaceId = H5Screate_simple(1, &dims, NULL);
  attributeId = H5Acreate(groupId, "boundaryType",H5T_NATIVE_INT, attrdataspaceId, H5P_DEFAULT);
  status = H5Awrite(attributeId, H5T_NATIVE_INT, &BoundaryType);
  status = H5Aclose(attributeId);
  status = H5Sclose(attrdataspaceId);

// dump n as attribute
  dims = 1;
  int n = 0;
  attrdataspaceId = H5Screate_simple(1, &dims, NULL);
  attributeId = H5Acreate(groupId, "nQuads",H5T_NATIVE_INT, attrdataspaceId, H5P_DEFAULT);
  status = H5Awrite(attributeId, H5T_NATIVE_INT,&nPoints );
  status = H5Aclose(attributeId);
  status = H5Sclose(attrdataspaceId);
  
//dump positions, charges 


  if(nPoints > 0){

  rank = 2;
  hsize_t     dimsf2[2];
  dimsf2[0] = nPoints;
  dimsf2[1] = 2;

  //  Scalar data[nPoints][2];

  dataspaceId = H5Screate_simple(rank, dimsf2, NULL); 
  datasetName = "positions";  
  datasetId = H5Dcreate(groupId, datasetName.c_str(), scalarType, dataspaceId,
			H5P_DEFAULT);
  hsize_t dim2 = 2;
  hid_t posSlabId = H5Screate_simple(1, &dim2, NULL);


  Scalar x,y;
  for(int i=0; i<nPoints; i++) {
    int ko = MIN(k1,k2);
    int jo = MIN(j1,j2);
    if(alongx2())   // Vertical
      {
	x = X[j1][i+ko].e1();
	y = X[j1][i+ko].e2();
	
      }
    else  // Horizontal
      {
	x = X[i+jo][k1].e1();
	y = X[i+jo][1].e2();
      }
    Scalar pos[2] = {x,y};  
    start[0]  = 0;  count[0]  = 2;
    status = H5Sselect_hyperslab(posSlabId, H5S_SELECT_SET, start, NULL, count, NULL);
    start[0]  = i; start[1]  = 0;
    count[0]  = 1; count[1]  = 2;    
    status = H5Sselect_hyperslab(dataspaceId, H5S_SELECT_SET, start, NULL, count, NULL);
    status = H5Dwrite(datasetId, scalarType, posSlabId, dataspaceId, H5P_DEFAULT, pos);
  }
  
  H5Sclose(posSlabId);
  H5Sclose(dataspaceId);
  H5Dclose(datasetId);
  
  // dump charges
  rank = 1;
  dimsf[0] = nPoints;
  datasetName = "Q";  
  dataspaceId = H5Screate_simple(rank, dimsf, NULL); 
  datasetId = H5Dcreate(groupId, datasetName.c_str(), scalarType, dataspaceId,
			H5P_DEFAULT);
  
  status = H5Dwrite(datasetId, scalarType, H5S_ALL, H5S_ALL,
		    H5P_DEFAULT, Q);
  H5Sclose(dataspaceId);
  H5Dclose(datasetId);
  }


  H5Gclose(groupId);
  H5Fclose(fileId);
  return(1);
  
}
#endif

int Dielectric::Dump(FILE * DMPFile)
{
  //#ifdef UNIX
  {
    Scalar xw[4];  // in this format:  xs, ys, xf, yf
    Vector2 **X=fields->get_grid()->getX();
    xw[0] = X[j1][k1].e1();
      xw[1] = X[j1][k1].e2();
      xw[2] = X[j2][k2].e1();
      xw[3] = X[j2][k2].e2();
      XGWrite(xw,ScalarInt,4,DMPFile,ScalarChar);
  }
  // write the BoundaryType
  XGWrite(&BoundaryType, 4, 1, DMPFile, "int");
  
   XGWrite(&nPoints,4,1,DMPFile,"int");
   XGWrite(&nPoints,4,1,DMPFile,"int");  // redundant, but necessary
   XGWrite(&nPoints,4,1,DMPFile,"int");  // redundant, but necessary
   
   Vector2 **X = fields->get_grid()->getX();
   Scalar x,y;
   for(int i=0; i<nPoints; i++) {
     int ko = MIN(k1,k2);
     int jo = MIN(j1,j2);
     if(alongx2())   // Vertical
       {
	 x = X[j1][i+ko].e1();
	 y = X[j1][i+ko].e2();
       }
     else  // Horizontal
         {
	   x = X[i+jo][k1].e1();
	   y = X[i+jo][k1].e2();
         }
     XGWrite(&x,ScalarInt,1,DMPFile,ScalarChar);
     XGWrite(&y,ScalarInt,1,DMPFile,ScalarChar);
     XGWrite(&Q[i], ScalarInt, 1, DMPFile, ScalarChar);
     XGWrite(&Q[i], ScalarInt, 1, DMPFile, ScalarChar);  //redundant, but needed for restore
   }
   //#endif
   return(TRUE);
}


#ifdef HAVE_HDF5
int Dielectric::restoreH5(dumpHDF5 &restoreObj,int bType,string whichBoundary,int nQuads)
{
  //cerr << "entering Dielectric::restoreH5().\n";
//  int dummy;
  int i;
  int jl,kl;
  Scalar bDims[4];
  hid_t fileId, groupId, datasetId,dataspaceId;
  herr_t status;
  string outFile;
//  hsize_t rank; 
//  hsize_t     dimsf[1];   
//  hsize_t dims;
  hid_t scalarType = restoreObj.getHDF5ScalarType();
  hid_t attrId;// attrspaceId,
#ifdef NEW_H5S_SELECT_HYPERSLAB_IFC
  hsize_t offset[2]; /* Offset of hyperslab */
#else
  hssize_t offset[2]; /* Offset of hyperslab */
#endif
  hsize_t count[2];  /* Block count */
 
  fileId = H5Fopen(restoreObj.getDumpFileName().c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
  groupId = H5Gopen(fileId, whichBoundary.c_str());

 
  
  attrId = H5Aopen_name(groupId, "dims" );
  status = H5Aread(attrId, scalarType, bDims);
  H5Aclose(attrId);
  
  if(BoundaryType != bType){
    H5Gclose(groupId);
    H5Fclose(fileId);
    return FALSE;
  }
  if(!onMe(bDims[0],bDims[1],bDims[2],bDims[3])) {
    H5Gclose(groupId);
    H5Fclose(fileId);
    return FALSE;
  }
  



//    // OK, this dielectric must be us.
  

  datasetId = H5Dopen(groupId,"positions");
  dataspaceId = H5Dget_space(datasetId );
  int dataRank = H5Sget_simple_extent_ndims(dataspaceId);
  hsize_t *sizes = new hsize_t[dataRank];
  int res = H5Sget_simple_extent_dims(dataspaceId, sizes, NULL);
  int N = sizes[0];
  
  
  hsize_t dim2 = sizes[1]; //should be 2
  hid_t posSlabId =  H5Screate_simple(1, &dim2, NULL);


  hid_t QsetId = H5Dopen(groupId,"Q");
  hid_t QspaceId = H5Dget_space(QsetId );



  for(i=0;i<nQuads;i++) {	
    Scalar x,y,lQ; //dummy;
    Vector2 GridLoc;

    Scalar pos[2];   
    offset[0]  = 0;  count[0]  = 2;
    status = H5Sselect_hyperslab(posSlabId, H5S_SELECT_SET, offset, NULL, count, NULL);
    offset[0]  = i; offset[1]  = 0;;
    count[0]  = 1; count[1]  = 2;    
    status = H5Sselect_hyperslab(dataspaceId, H5S_SELECT_SET, offset, NULL, count, NULL);
    status = H5Dread(datasetId, scalarType, posSlabId, dataspaceId, H5P_DEFAULT, pos);
	      
    x = pos[0];
    y = pos[1];
      
    offset[0]  = 0;  count[0]  = 1;
    status = H5Sselect_hyperslab(posSlabId, H5S_SELECT_SET, offset, NULL, count, NULL);
    offset[0]  = i; offset[1]  = 0;
    count[0]  = 1; count[1]  = 1;     
    status = H5Sselect_hyperslab(QspaceId, H5S_SELECT_SET, offset, NULL, count, NULL);
    status = H5Dread(QsetId, scalarType, posSlabId, QspaceId, H5P_DEFAULT, &lQ);
    

// Find the j/k value of the nearest
// point on this boundary
      GridLoc = fields->get_grid()->getNearestGridCoords(Vector2(x,y));
      jl = (int) (GridLoc.e1() + 0.5);
      kl = (int) (GridLoc.e2() + 0.5);

 // find the point on this boundary this GridLoc corresponds to
      if (alongx2())	// vertical
         {
            int kmax = abs(k2-k1) +1;
            int ki = kl - MIN(k1,k2);
            if(ki <= kmax && kl >= 0)
               Q[ki]= lQ;

         }
      else   // horizontal
         {
            int jmax = abs(j2-j1) + 1;
            int ji = jl - MIN(j1,j2);
            if(ji <= jmax && ji >=0)
               Q[ji]= lQ;
         }
   }	


  H5Sclose(dataspaceId);
  H5Dclose(datasetId);
  H5Sclose(QspaceId);
  H5Dclose(QsetId);
  H5Sclose(posSlabId);
  
  H5Gclose(groupId);
  H5Fclose(fileId);

  delete [] sizes;  
  return(1);
}
#endif


int Dielectric::Restore(FILE *DMPFile, int _BoundaryType,
                        Scalar _A1,Scalar _A2, Scalar _B1, Scalar _B2,int nQuads)
     
{
   int dummy;
   int i;
   int jl,kl;
   //#ifdef UNIX
   // Two checks to see if this data is loadable by this boundary.
   if(BoundaryType != _BoundaryType) return FALSE;
   if(!onMe(_A1,_A2,_B1,_B2)) return FALSE;
   
   // OK, this dielectric must be us.
   

   // next two don't matter
   XGRead(&dummy,4,1,DMPFile,"int");
   XGRead(&dummy,4,1,DMPFile,"int");
   

   for(i=0;i<nQuads;i++) {	
      Scalar x,y,lQ,dummy;
      Vector2 GridLoc;
      XGRead(&x,ScalarInt,1,DMPFile,ScalarChar);
      XGRead(&y,ScalarInt,1,DMPFile,ScalarChar);
      XGRead(&lQ,ScalarInt,1,DMPFile,ScalarChar);
      XGRead(&dummy,ScalarInt,1,DMPFile,ScalarChar);
      // Find the j/k value of the nearest
      // point on this boundary
      GridLoc = fields->get_grid()->getNearestGridCoords(Vector2(x,y));
      jl = (int) (GridLoc.e1() + 0.5);
      kl = (int) (GridLoc.e2() + 0.5);

      // find the point on this boundary this GridLoc corresponds to
      if (alongx2())	// vertical
         {
            int kmax = abs(k2-k1) +1;
            int ki = kl - MIN(k1,k2);
            if(ki <= kmax && kl >= 0)
               Q[ki]= lQ;

         }
      else   // horizontal
         {
            int jmax = abs(j2-j1) + 1;
            int ji = jl - MIN(j1,j2);
            if(ji <= jmax && ji >=0)
               Q[ji]= lQ;
         }
   }	
   //#endif
   return(TRUE);
}


int Dielectric::Restore_2_00(FILE * DMPFile, int j1test,
                             int k1test, int j2test, int k2test)
{
   if ((j1 == j1test)&&(k1 == k1test)&&(j2 == j2test)&&(k2 == k2test))

      {
         //#ifdef UNIX
         for(int i=0; i<nPoints; i++)
            XGRead(&Q[i], ScalarInt, 1, DMPFile, ScalarChar);
         //#endif
         return(TRUE);

      }
   else
      return(FALSE);
}

//===============================================================
// DielectricRegion.  Must construct Q with different array size than
//	Dielectric, so pass QuseFlag=0 to prevent allocation in Dielectric()

DielectricRegion::DielectricRegion(oopicList <LineSegment> *segments, 
                                   Scalar er, int _QuseFlag, Scalar reflection, Scalar refMaxE) 
: Dielectric(segments, er, 0)
{

   if(segments->nItems() > 1) {
      cout << "Warning, Dielectric Regions can only have 1 segment.\n";
      cout << "Check your input file.\n";
   }

   QuseFlag = _QuseFlag;					// must set here since we send 0 to Dielectric()
   if (QuseFlag)
      {
         nPoints = 2*(j2 - j1 + k2 - k1);
         Q = new Scalar[nPoints*2];
         memset(Q, 0, nPoints*sizeof(Scalar));
      }
   BoundaryType = DIELECTRIC_REGION;
   BCType = DIELECTRIC_BOUNDARY;
}

DielectricRegion::~DielectricRegion()
{
}

void DielectricRegion::setPassives()
{
}

#define DELTA 1e-6

void DielectricRegion::collect(Particle& p, Vector3& dxMKS)
{
   int j=0;
   EDGE _edge;
   Scalar xIntercept=0.;
   // Compute the edge which collected:
   if (fabs(p.get_x().e1() - j1) < DELTA)	//	left edge
      {
         xIntercept = p.get_x().e2();
         _edge = LEFT;
         _delta.set_e1(j1 ? -DELTA*j1 : -DELTA);
         _delta.set_e2(0);
         _unitNormal.set_e1(-1);
         _unitNormal.set_e2(0);
      }
   else if (fabs(p.get_x().e1() - j2) < DELTA)	// right edge
      {
         xIntercept = p.get_x().e2();
         _edge = RIGHT;
         _delta.set_e1(DELTA*j2);
         _delta.set_e2(0);
         _unitNormal.set_e1(1);
         _unitNormal.set_e2(0);
      }
   else if (fabs(p.get_x().e2() - k1) < DELTA)	// bottom edge
      {
         xIntercept = p.get_x().e1();
         _edge = BOTTOM;
         _delta.set_e1(0);
         _delta.set_e2(k1 ? -DELTA*k1 : DELTA);
         _unitNormal.set_e1(0);
         _unitNormal.set_e2(-1);
      }
   else if (fabs(p.get_x().e2() - k2) < DELTA)	// top edge
      {
         xIntercept = p.get_x().e1();
         _edge = TOP;
         _delta.set_e1(0);
         _delta.set_e2(DELTA*k2);
         _unitNormal.set_e1(0);
         _unitNormal.set_e2(1);
      }
   else {
      cout << "DielectricRegion::collect failed to find an edge" << endl;
      printf("delta=(%g,%g)   x=(%g,%g)   s=%s   j=%d, k=%d\n", 
	_delta.e1(), _delta.e2(), p.get_x().e1(), p.get_x().e2(), 
	p.get_speciesPtr()->get_name().c_str(), j1, k1);
// JRC: if no edge, cannot continue.
      return;
   }
   if (QuseFlag)
      {
         j = (int) xIntercept;
         addQ(_edge, j, xIntercept - j, p.get_q());
      }
   Boundary::collect(p, dxMKS); // for stats
   if (secondaryList.nItems()){
      p.set_x(p.get_x() + _delta); // perturb off edge;
      oopicListIter<Secondary> sIter(secondaryList);
      for (; !sIter.Done(); sIter++){
         int n = sIter()->generateSecondaries(p, particleList);
         if (QuseFlag && n>0){
            Scalar q = -n*p.get_np2c()*sIter()->get_q();
            addQ(_edge, j, xIntercept - j, q);
         }
      }
   }
   delete &p;
}

//-------------------------------------------------------------------
// Set the permittivity inside the dielectric region
void DielectricRegion::setEpsilon()
{
   //return;
   for (int j = j1; j < j2; j++)
      for (int k = k1; k < k2; k++)
         fields->set_epsi(epsilon, j, k);
}

//-------------------------------------------------------------------
// Add q to the surface charge array; must be mapped from perimeter
// to linear array.
void DielectricRegion::addQ(EDGE edge, int i, Scalar w, Scalar q)
{
   int index1=0;
   int index2=0;
   switch (edge)
      {
       case BOTTOM:
          index1 = i - j1;
          index2 = index1 + 1;
          break;
        case RIGHT:
           index1 = i - k1 + j2 - j1;
          index2 = index1 + 1;
          break;
        case TOP:
           index1 = 2*j2 - j1 + k2 - k1 - i;
          index2 = index1 - 1;
          break;
        case LEFT:
           index1 = 2*(j2 - j1) + 2*k2 - k1 - i;
          index2 = index1 - 1;
          index1 %= nPoints;				// wrap around to 0
          break;             
       }
   Q[index1] += q*(1 - w);
   Q[index2] += q*w;
}

int DielectricRegion::getIndex(int j, int k)
{
   if (k == k1) return j - j1;    				//	bottom
   else if (j == j2)	return k - k1 + j2 - j1;		//	right
   else if (k == k2) return 2*j2 - j1 + k2 - k1 - j;	//	top
   else if (j == j1) return 2*(j2 - j1 + k2) - k1 - k;	//	left
   else exit (-1);							// points must lie on surface
   return j-j1;  //  goofy default case.
}

void DielectricRegion::putCharge_and_Current(Scalar t)
{
   if (!QuseFlag) return;
   Scalar **rho=fields->getrho();
   Scalar **cellvolume = fields->get_grid()->get_halfCellVolumes();
   for (int i=0; i < nPoints; i++)
      {
         int j, k;
         if (i < j2 - j1) {j = j1 + i; k = k1;}
         else if (i < j2 - j1 + k2 - k1) {j = j2; k = i - j2 + j1 + k1;}
         else if (i < 2*(j2 - j1) + k2 - k1) {j = 2*j2 - i - j1 + k2 - k1; k = k2;}
         else {j = j1; k = 2*k2 - k1 - i + 2*(j2 - j1);}
         rho[j][k] += Q[i]/cellvolume[j][k];
      }
}

void DielectricRegion::putCharge(void)
{
   if (!QuseFlag) return;
   Scalar **Charge=fields->getQ();
   for (int i=0; i < nPoints; i++)
      {
         int j, k;
         if (i < j2 - j1) {j = j1 + i; k = k1;}
         else if (i < j2 - j1 + k2 - k1) {j = j2; k = i - j2 + j1 + k1;}
         else if (i < 2*(j2 - j1) + k2 - k1) {j = 2*j2 - i - j1 + k2 - k1; k = k2;}
         else {j = j1; k = 2*k2 - k1 - i + 2*(j2 - j1);}
         Charge[j][k] += Q[i];
      }
}

//---------------------------------------------------------------------
//	setBoundaryMask sets the boundary masks in Grid for determing boundaries.
// Boundary::setBoundaryMask() is overloaded to deal with volumetric 
// boundaries.

void DielectricRegion::setBoundaryMask(Grid &grid)
{
   int j, k=k2;
   // setBoundaryMask might need to be replaced with SetNodeBoundary so
   // particles can live in a dielectric. 
   // ie. M. Turner changing epsilon speed up.
   /*
      for (j=j1; j<j2; j++)
      for (int k=k1; k<=k2; k++)
      grid.setBoundaryMask1(j, k, this);

      for (j=j1; j<=j2; j++)
      for (int k=k1; k<k2; k++)
      grid.setBoundaryMask2(j, k, this);
      */
   for (j=j1; j<j2; j++)
      for (k=k1; k<k2; k++){
         grid.setCellMask(j, k, this);
         grid.setBoundaryMask1(j, k, this, 0);
         grid.setBoundaryMask2(j, k, this, 0);
      }

   for (j=j1; j<j2; j++)
      grid.setBoundaryMask1(j, k, this, 0);
   
   for (k=k1; k<k2; k++)
      grid.setBoundaryMask2(j, k, this, 0);

}

//---------------------------------------------------------------------
//	Weight the fields to nodal locations along the edges of the dielectric
//	region.  Must be handled slightly differently than Boundary::toNodes()
//	just to account for four edges.  Assume only normal fields need be
//	updated.

void DielectricRegion::toNodes()
{
   Vector3** intEdl = fields->getIntEdl();
   Vector3** ENode = fields->getENode();
   Grid* grid = fields->get_grid();
   for (int j = j1; j <= j2; j++)		// along x1
      {
         if (k1 > 1) ENode[j][k1].set_e2(intEdl[j][k1-1].e2()/grid->dl2(j, k1-1));
         if (k2 < grid->getK())	ENode[j][k2].set_e2(intEdl[j][k2].e2()/grid->dl2(j, k2));
         /*		if (k1 > 0)	ENode[j][k1].set_e2((epsilon*intEdl[j][k1].e2()/grid->dl2(j, k1) 
			- Q[getIndex(j, k1)]/grid->dS2(j,k1))/fields->get_epsi(j, k1-1));
                        if (k2 < grid->getK())	ENode[j][k2].set_e2((Q[getIndex(j, k2)]/grid->dS2(j,k1)
			- epsilon*intEdl[j][k2-1].e2()/grid->dl2(j, k2-1))/fields->get_epsi(j, k2));
                        */	}        
   for (int k = k1; k <= k2; k++)		// along x2
      {
         if (j1 > 0) ENode[j1][k].set_e1(intEdl[j1-1][k].e1()/grid->dl1(j1-1, k));
         if (j2 < grid->getJ()) ENode[j2][k].set_e1(intEdl[j2][k].e1()/grid->dl1(j2, k));
         /*		if (j1 > 0) ENode[j1][k].set_e1((epsilon*intEdl[j1][k].e1()/grid->dl1(j1, k)
			- Q[getIndex(j1, k)]/grid->dS1(j1, k))/fields->get_epsi(j1-1, k));
                        if (j2 < grid->getJ()) ENode[j2][k].set_e1((Q[getIndex(j2, k)]/grid->dS1(j2, k)
			- epsilon*intEdl[j2-1][k].e1()/grid->dl1(j2-1, k))/fields->get_epsi(j2, k));
                        */	}
}
#ifdef HAVE_HDF5
int DielectricRegion::dumpH5(dumpHDF5 &dumpObj,int number)
{

  //cerr << "Entered DielectricRegion::Dump--HDF5.\n";
  hid_t fileId, groupId, datasetId,dataspaceId;
  hid_t  attrdataspaceId,attributeId;
  herr_t status;
  string outFile;
  hsize_t rank; 
  hsize_t     dimsf[1];   
  hsize_t dims;
  hid_t scalarType = dumpObj.getHDF5ScalarType();
#ifdef NEW_H5S_SELECT_HYPERSLAB_IFC
  hsize_t start[2]; /* Start of hyperslab */
#else
  hssize_t start[2]; /* Start of hyperslab */
#endif
  hsize_t count[2];  /* Block count */

  //  strstream s;

  stringstream s;

  s << "boundary" << number <<ends; 
  string dname = s.str();
 
//  // Open the hdf5 file with write accessx
  fileId = H5Fopen(dumpObj.getDumpFileName().c_str(), H5F_ACC_RDWR, H5P_DEFAULT);
   dname = "/Boundaries/" + dname;
   groupId = H5Gcreate(fileId,dname.c_str() ,0); 
   string datasetName = "dims";  
  rank = 1;
  dims = 4;
  attrdataspaceId = H5Screate_simple(rank, &dims, NULL); 
  attributeId = H5Acreate(groupId, datasetName.c_str(), scalarType, attrdataspaceId,
		      H5P_DEFAULT);
// get the data   
  Grid *grid = fields->get_grid();
// Write the physical dimensions of the boundary
  
  Scalar xw[4];  // in this format:  xs, ys, xf, yf
      Vector2 **X=fields->get_grid()->getX();
      xw[0] = X[j1][k1].e1();
      xw[1] = X[j1][k1].e2();
      xw[2] = X[j2][k2].e1();
      xw[3] = X[j2][k2].e2();
  
  status = H5Awrite(attributeId, scalarType, xw);
  status = H5Aclose(attributeId);
  status = H5Sclose(attrdataspaceId);

// dump boundary type as attribute
  dims = 1;
  attrdataspaceId = H5Screate_simple(1, &dims, NULL);
  attributeId = H5Acreate(groupId, "boundaryType",H5T_NATIVE_INT, attrdataspaceId, H5P_DEFAULT);
  status = H5Awrite(attributeId, H5T_NATIVE_INT, &BoundaryType);
  status = H5Aclose(attributeId);
  status = H5Sclose(attrdataspaceId);

// dump boundary type as attribute
  dims = 1;
  attrdataspaceId = H5Screate_simple(1, &dims, NULL);
  attributeId = H5Acreate(groupId, "nQuads",H5T_NATIVE_INT, attrdataspaceId, H5P_DEFAULT);
  status = H5Awrite(attributeId, H5T_NATIVE_INT, &nPoints);
  status = H5Aclose(attributeId);
  status = H5Sclose(attrdataspaceId);






// dump positions

 rank = 2;
  hsize_t     dimsf2[2];
  //  Scalar data[nPoints][2];
  dimsf2[0] = nPoints;
  dimsf2[1] = 2;

  dataspaceId = H5Screate_simple(rank, dimsf2, NULL); 
  datasetName = "positions";  
  datasetId = H5Dcreate(groupId, datasetName.c_str(), scalarType, dataspaceId,
			H5P_DEFAULT);
  hsize_t dim2 = 2;
  hid_t posSlabId = H5Screate_simple(1, &dim2, NULL);

  Scalar x,y;
 for(int i=0; i<nPoints; i++) {
      int j, k;
      
      if (i < j2 - j1) {j = j1 + i; k = k1;}
      else if (i < j2 - j1 + k2 - k1) {j = j2; k = i - j2 + j1 + k1;}
      else if (i < 2*(j2 - j1) + k2 - k1) {j = 2*j2 - i - j1 + k2 - k1; k = k2;}
      else {j = j1; k = 2*k2 - k1 - i + 2*(j2 - j1);}
      x = X[j][k].e1();
      y = X[j][k].e2();
      
      Scalar pos[2] = {x,y};  
      start[0]  = 0;  count[0]  = 2;
      status = H5Sselect_hyperslab(posSlabId, H5S_SELECT_SET, start, NULL, count, NULL);
      start[0]  = i; start[1]  = 0;
      count[0]  = 1; count[1]  = 2;    
      status = H5Sselect_hyperslab(dataspaceId, H5S_SELECT_SET, start, NULL, count, NULL);
      status = H5Dwrite(datasetId, scalarType, posSlabId, dataspaceId, H5P_DEFAULT, pos);
      
    }
 H5Sclose(posSlabId);
H5Sclose(dataspaceId);
 H5Dclose(datasetId);


  rank = 1;
  dimsf[0] = nPoints;
  datasetName = "Q";  
  dataspaceId = H5Screate_simple(rank, dimsf, NULL); 
  datasetId = H5Dcreate(groupId, datasetName.c_str(), scalarType, dataspaceId,
			H5P_DEFAULT);
  
  status = H5Dwrite(datasetId, scalarType, H5S_ALL, H5S_ALL,
		    H5P_DEFAULT, Q);

  H5Sclose(dataspaceId);
  H5Dclose(datasetId);

  H5Gclose(groupId);
  H5Fclose(fileId);
  return(1);

}
#endif

int DielectricRegion::Dump(FILE * DMPFile)
{
   //#ifdef UNIX
   {
      Scalar xw[4];  // in this format:  xs, ys, xf, yf
      Vector2 **X=fields->get_grid()->getX();
      xw[0] = X[j1][k1].e1();
      xw[1] = X[j1][k1].e2();
      xw[2] = X[j2][k2].e1();
      xw[3] = X[j2][k2].e2();
      XGWrite(xw,ScalarInt,4,DMPFile,ScalarChar);
   }
   // write the BoundaryType
   XGWrite(&BoundaryType, 4, 1, DMPFile, "int");

   Scalar x,y;
   XGWrite(&nPoints,4,1,DMPFile,"int");
   XGWrite(&nPoints,4,1,DMPFile,"int");  // redundant, but necessary
   XGWrite(&nPoints,4,1,DMPFile,"int");  // redundant, but necessary

   Vector2 **X = fields->get_grid()->getX();

   for(int i=0; i<nPoints; i++) {
      int j, k;
      
      if (i < j2 - j1) {j = j1 + i; k = k1;}
      else if (i < j2 - j1 + k2 - k1) {j = j2; k = i - j2 + j1 + k1;}
      else if (i < 2*(j2 - j1) + k2 - k1) {j = 2*j2 - i - j1 + k2 - k1; k = k2;}
      else {j = j1; k = 2*k2 - k1 - i + 2*(j2 - j1);}
      x = X[j][k].e1();
      y = X[j][k].e2();
      XGWrite(&x,ScalarInt,1,DMPFile,ScalarChar);
      XGWrite(&y,ScalarInt,1,DMPFile,ScalarChar);
      XGWrite(&Q[i], ScalarInt, 1, DMPFile, ScalarChar);
      XGWrite(&Q[i], ScalarInt, 1, DMPFile, ScalarChar);   //redundant, but needed for restore
   }
   //#endif
   return(TRUE);
}


#ifdef HAVE_HDF5
int DielectricRegion::restoreH5(dumpHDF5 &restoreObj,int bType,string whichBoundary,int nQuads)
{
  //cerr << "entering DielectricRegion::restoreH5().\n";
//  int dummy;
  int i;
  int jl,kl;
  Scalar bDims[4];
  hid_t fileId, groupId, datasetId,dataspaceId;
  herr_t status;
  string outFile;
//  hsize_t rank; 
//  hsize_t     dimsf[1];   
//  hsize_t dims;
  hid_t scalarType = restoreObj.getHDF5ScalarType();
//  hid_t  attrspaceId,
  hid_t attrId;
#ifdef NEW_H5S_SELECT_HYPERSLAB_IFC
  hsize_t offset[2]; /* Offset of hyperslab */
#else
  hssize_t offset[2]; /* Offset of hyperslab */
#endif
  hsize_t count[2];  /* Block count */


  fileId = H5Fopen(restoreObj.getDumpFileName().c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
  groupId = H5Gopen(fileId, whichBoundary.c_str());
    


  attrId = H5Aopen_name(groupId, "dims" );
  status = H5Aread(attrId, scalarType, bDims);
  H5Aclose(attrId);
      
  if(BoundaryType != bType){
    H5Gclose(groupId);
    H5Fclose(fileId);
    return FALSE;
  }
  if(!onMe(bDims[0],bDims[1],bDims[2],bDims[3])) {
    H5Gclose(groupId);
    H5Fclose(fileId);
    return FALSE;
  }
    // OK, this dielectric must be us.
  
// Open the dataset for reading.  Determine shape and set field size.





 datasetId = H5Dopen(groupId,"positions");
  dataspaceId = H5Dget_space(datasetId );
  int dataRank = H5Sget_simple_extent_ndims(dataspaceId);
  hsize_t *sizes = new hsize_t[dataRank];
  int res = H5Sget_simple_extent_dims(dataspaceId, sizes, NULL);
  int N = sizes[0];
  
  
  hsize_t dim2 = sizes[1]; //should be 2
  hid_t posSlabId =  H5Screate_simple(1, &dim2, NULL);


  hid_t QsetId = H5Dopen(groupId,"Q");
  hid_t QspaceId = H5Dget_space(QsetId );

  for(i=0;i<nQuads;i++) {	
    Scalar x,y,lQ;//dummy;
    Vector2 GridLoc;
    

 Scalar pos[2];   
    offset[0]  = 0;  count[0]  = 2;
    status = H5Sselect_hyperslab(posSlabId, H5S_SELECT_SET, offset, NULL, count, NULL);
    offset[0]  = i; offset[1]  = 0;;
    count[0]  = 1; count[1]  = 2;    
    status = H5Sselect_hyperslab(dataspaceId, H5S_SELECT_SET, offset, NULL, count, NULL);
    status = H5Dread(datasetId, scalarType, posSlabId, dataspaceId, H5P_DEFAULT, pos);
	      
    x = pos[0];
    y = pos[1];
      
    offset[0]  = 0;  count[0]  = 1;
    status = H5Sselect_hyperslab(posSlabId, H5S_SELECT_SET, offset, NULL, count, NULL);
    offset[0]  = i; offset[1]  = 0;
    count[0]  = 1; count[1]  = 1;     
    status = H5Sselect_hyperslab(QspaceId, H5S_SELECT_SET, offset, NULL, count, NULL);
    status = H5Dread(QsetId, scalarType, posSlabId, QspaceId, H5P_DEFAULT, &lQ);
            
    // Find the j/k value of the nearest
    // point on this boundary
    GridLoc = fields->get_grid()->getNearestGridCoords(Vector2(x,y));
    jl = (int) (GridLoc.e1() + 0.5);
    kl = (int) (GridLoc.e2() + 0.5);
    
    int kmax = MAX(k1,k2);
    int jmax = MAX(j1,j2);
    int jmin = MIN(j1,j2);
    int kmin = MIN(k1,k2);
    Q[getIndex(jl,kl)]+= lQ;
  }	
   
  H5Sclose(dataspaceId);
  H5Dclose(datasetId);
  H5Sclose(QspaceId);
  H5Dclose(QsetId);
  H5Sclose(posSlabId);
  
  
  H5Gclose(groupId);
  H5Fclose(fileId);

  delete [] sizes;  
  return(TRUE);
}
#endif

int DielectricRegion::Restore(FILE *DMPFile, int _BoundaryType,
                              Scalar _A1,Scalar _A2, Scalar _B1, Scalar _B2,int nQuads)
{
   int dummy;
   int i;
   int jl,kl;
   //#ifdef UNIX
   // Two checks to see if this data is loadable by this boundary.
   if(BoundaryType != _BoundaryType) return FALSE;
   if(!onMe(_A1,_A2,_B1,_B2)) return FALSE;
   
   // OK, this dielectric must be us.
   
   // next two don't matter
   XGRead(&dummy,4,1,DMPFile,"int");
   XGRead(&dummy,4,1,DMPFile,"int");
   

   for(i=0;i<nQuads;i++) {	
      Scalar x,y,lQ,dummy;
      Vector2 GridLoc;
      XGRead(&x,ScalarInt,1,DMPFile,ScalarChar);
      XGRead(&y,ScalarInt,1,DMPFile,ScalarChar);
      XGRead(&lQ,ScalarInt,1,DMPFile,ScalarChar);
      XGRead(&dummy,ScalarInt,1,DMPFile,ScalarChar);
      // Find the j/k value of the nearest
      // point on this boundary
      GridLoc = fields->get_grid()->getNearestGridCoords(Vector2(x,y));
      jl = (int) (GridLoc.e1() + 0.5);
      kl = (int) (GridLoc.e2() + 0.5);

      int kmax = MAX(k1,k2);
      int jmax = MAX(j1,j2);
      int jmin = MIN(j1,j2);
      int kmin = MIN(k1,k2);
      Q[getIndex(jl,kl)]+= lQ;

      //find which edge it is on and addQ it
      /*		if(jl == jmin) {
			if(kl <=kmax && kl >=kmin) {
                        //addQ(LEFT,jl,kl,lQ);

			}
                        }

                        if(jl==jmax) {
			if(kl <=kmax && kl >=kmin) {
                        addQ(RIGHT,jl,kl,lQ);
                        }
                        }

                        if(kl == kmin) {
			if(jl <=jmax && jl >=jmin) {
                        addQ(BOTTOM,jl,kl,lQ);
			}
                        }
                        if(kl == kmax) {
			if(jl <=jmax && jl >=jmin) {
                        addQ(TOP,jl,kl,lQ);
			}
                        }

                        */
   }	
   //#endif
   return(TRUE);


}

int DielectricRegion::Restore_2_00(FILE * DMPFile, int j1test, 
                                   int k1test, int j2test, int k2test)
{
   if ((j1 == j1test)&&(k1 == k1test)&&(j2 == j2test)&&(k2 == k2test))
      {
         //#ifdef UNIX
         for(int i=0; i<nPoints; i++)
            XGRead(&Q[i], ScalarInt, 1, DMPFile, ScalarChar);
         //#endif
         return(TRUE);
      }
   else
      return(FALSE);
}

DielectricTriangle::
DielectricTriangle(oopicList <LineSegment> *segments,
                   Scalar _er, int _QuseFlag) :Dielectric(segments,_er,0)
{
   QuseFlag=_QuseFlag;
   if(segments->nItems() > 1) {
      cout << "Warning, Dielectric Triangles can only have 1 segment.\n";
      cout << "Check your input file.\n";
   }

   /*	if(j1>j2) {  //swap the two points.
        int jt,kt;
        jt=j2;j2=j1;j1=jt;
        kt=k2;k2=k1;k1=kt;
	}
        */
   if(QuseFlag)
      {
         fprintf(stderr,"DielectricTriangles don't support QuseFlag\n");
         QuseFlag = 0;
      }
   epsilon = _er/iEPS0;

   BoundaryType = normal*(DIELECTRIC_TRIANGLE_UP);
   BCType = DIELECTRIC_BOUNDARY;
}

void DielectricTriangle::setPassives()
{
   /*
      int j,k,jl,kl,jh,kh;
      int J=fields->getJ();
      int K=fields->getK();
      int *points = segments->headData()->points;
      // If the normal is positive, fill the triangle upwards
      kh=MAX(k1,k2);
      if(normal > 0) {
      for(j=0;j<4*abs(j2-j1)+4;j+=2) {
      kl = points[j+1];
      jl = points[j];
      for(k=kl;k<=kh;k++)
      {
      if (jl==0 || jl==J || k==0 || k==K)
      fields->getPoissonSolve()->set_coefficient(jl,k,NEUMANN,fields->get_grid());
      else
      fields->getPoissonSolve()->set_coefficient(jl,k,FREESPACE,fields->get_grid());
      }
      }
      }
      else {
      kl = MIN(k1,k2);
      for(j=0;j<4*abs(j2-j1)+4;j+=2) {
      kh = points[j+1];
      jl = points[j];
      for(k=kl;k<=kh;k++)
      {
      if (jl==0 || jl==J || k==0 || k==K)
      fields->getPoissonSolve()->set_coefficient(jl,k,NEUMANN,fields->get_grid());
      else
      fields->getPoissonSolve()->set_coefficient(jl,k,FREESPACE,fields->get_grid());
      }
      }
      }
      */
}

void DielectricTriangle::collect(Particle &p, Vector3& dxMKS)
{
   delete &p;
}

//set the permittivity inside the triangle

void DielectricTriangle::setEpsilon()
{
   int j,k,jl,kl,kh;

   int *points = segments->headData()->points;
   // If the normal is positive, fill the triangle upwards
   kh=MAX(k1,k2);
   if(normal > 0) {
      for(j=0;j<4*abs(j2-j1)+4;j+=2) {
         kl = points[j+1];
         jl = points[j];
         for(k=kl;k<=kh;k++)
            fields->set_epsi(epsilon,jl,k);
      }
   }
   else {
      kl = MIN(k1,k2);
      for(j=0;j<4*abs(j2-j1)+4;j+=2) {
         kh = points[j+1];
         jl = points[j];
         for(k=kl;k<=kh;k++)
            fields->set_epsi(epsilon,jl,k);
      }
   }
}	

void DielectricTriangle::setBoundaryMask(Grid &grid) {

   Boundary::setBoundaryMask(grid);  //this does a lot of the work we need to do.

   //for the other two boundaries of the triangle, we need to figure out where to
   // put them.
   int jm,km,jh,kh,jl,kl;
   kh = MAX(k1,k2);
   jh = MAX(j1,j2);
   jl = MIN(j1,j2);
   kl = MIN(k1,k2);
   
   if(normal > 0) {
      km = kh;
      jm = ( k1 < k2) ? jl:jh;
   }
   else {
      km=kl;
      jm = (k1 < k2)? jh:jl;
   }

   //Horizontal boundary
   for(int j=jl;j<jh;j++) grid.setBoundaryMask1(j,km,this,0);
   
   //Vertical boundary
   for(int k=kl;k<kh;k++) grid.setBoundaryMask2(jm,k,this,0);
}




int DielectricTriangle::Dump(FILE * DMPFile)
{
   return(TRUE);
}
