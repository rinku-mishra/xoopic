//--------------------------------------------------------------------
//
// File:	NGD.cpp
//
// Purpose:	implementations of the NGD (Neutral Gas Density) class
//
// Version:	$Id: ngd.cpp 2296 2006-09-18 20:20:31Z yew $
//
// Copyright 2001, Tech-X Corporation, all rights reserved
//
//--------------------------------------------------------------------

#include "ngd.h"
#include "grid.h"

#ifdef _MSC_VER
#include <fstream>
using std::cout;
using std::cerr;
using std::endl;
#endif

const Scalar NGD::firstRevisionNumberWithNGD = 2.61;

NGD::NGD(Grid* pgrid, const ostring& gType, 
         const ostring& _analyticF, const Scalar& _gasDensity,  
         const Vector2& _p1Grid, const Vector2& _p2Grid, 
         const int& _discardDumpFileNGDDataFlag) 
  /* throw(Oops) */{

// Must create all memory or set pointer to zero at beginning

// Beginning of construction
  ptrLocalGrid = pgrid;
  gasType = gType;

  J = ptrLocalGrid->getJ(); 
  K = ptrLocalGrid->getK();
  NeutralGasDensity = new Scalar* [J];
  int j;
  int k;
  for (j = 0; j<J; j++) { 
    NeutralGasDensity[j] = new Scalar [K]; 
    for(k=0; k<K; k++) NeutralGasDensity[j][k]=0.0;
  }
  maxNGD = 0.0;

  ostring analyticF;
  // the following parser related lines are for the cross-platform compatability
#ifndef UNIX
  analyticF=_analyticF; 
  ostring endLine = "\n";
  analyticF=_analyticF + endLine;
#else
  analyticF=_analyticF + '\n';
#endif

// now set the Neutral Gas Density

  if( analyticF == (ostring)"0.0\n" ) { 

// this is the default value of analyticF
// for the case of uniform gasDensity
// calculated above and used here to
// initialize the NutralGasDensity
    try{
      setNeutralGasDensity( _gasDensity, _p1Grid, _p2Grid);
    }
    catch(Oops& oops){
      oops.prepend("NGD::NGD: Error: \n"); //SpatialRegion::initNGD
      throw oops;
    }

    maxNGD = _gasDensity;

  } else {
    /**
     * parse the function specified by the string contained in
     * analyticF to initialize the NeutralGasDensity and the
     * max value of the NeutralGasDensity.
     */
    try{
      setNeutralGasDensity(analyticF, _p1Grid, _p2Grid);
    }
    catch(Oops& oops){
      oops.prepend("NGD::NGD: Error: \n"); //OK
      throw oops;
    }
    
  }

  /**
   * set the boolean data member discardDumpFileNGDDataFlag 
   * according to the value passed from the input file
   */
  if ( !_discardDumpFileNGDDataFlag )
    discardDumpFileNGDDataFlag = false;
  else 
    discardDumpFileNGDDataFlag = true;
  
  /**
   * set TI_np2c to 0 as its default value
   */
  TI_np2c = 0;

  /**
   * set the excessNumIons pointer to 0 as its
   * default value. 
   */
  excessNumIons = 0;

  isTIOn = 0;

}

NGD::~NGD() {

  int j;
  for (j=0; j<J; j++) delete[] NeutralGasDensity[j];
  delete[] NeutralGasDensity;
  
  /**
   * dealocate if memory for np2c was allocated
   */
  if( TI_np2c ) {
    int j;
    for (j=0; j<J; j++) delete[] TI_np2c[j];
    delete[] TI_np2c;
  }  

// deallocate if memory for excessNumIons was allocated
  if( excessNumIons ) {
    int j;
    for (j=0; j<=J; j++) delete[] excessNumIons[j];
    delete[] excessNumIons;
  }   

}

/** 
 * helper function to check for out of bounds condition in both
 * the j and k indeces. For now the function will print which index
 * is out of bounds and in which function (given by char* place)
 * and exit to the system with status of 1. If the j, k are 
 * inbounds it will return true. dad 04/11/01.
 */
bool NGD::check_jk_bounds(const int& j, const int& k, char* place) 
  const /* throw(Oops) */{

  stringstream ss (stringstream::in | stringstream::out);

  if ( j < 0 || j > (J-1) ) {

    ss << "NGD::check_jk_bounds: Error: \n" << 
      "j = " << j << 
	    " is not in [0, " << J-1 << "]." << endl;

    std::string msg;
    ss >> msg;
    Oops oops(msg);
    throw oops;    // exit() NGD::getNGD:
   /*  NGD::getNGD_TI_np2c: 
       NGD::setNGD_TI_np2c: 
       NGD::setNGD_excessNumIons: 
       NGD::getNGD_excessNumIons:  
       NGD::add_to_NGD_excessNumIons:  
      */

  } else if ( k < 0 || k > (K-1) ) {

    ss << "NGD::check_jk_bounds: Error: \n" << 
      "k = " << k << 
	    " is not in [0, " << K-1 << "]." << endl;

    std::string msg;
    ss >> msg;
    Oops oops(msg);
    throw oops;    // exit()  NGD::getNGD:  
   /*   NGD::getNGD_TI_np2c:  
        NGD::setNGD_TI_np2c:  
        NGD::setNGD_excessNumIons:  
        NGD::getNGD_excessNumIons:  
        NGD::add_to_NGD_excessNumIons:  
      */

  }
  return true;
}

Scalar NGD::getNGD(const int& j, const int& k) const /* throw(Oops) */ {
  char* place = (char *)"NGD::getNGD(const int& j, const int& k)";

  try{
    check_jk_bounds(j, k, place); 
  }
  catch(Oops& oops){
    oops.prepend("NGD::getNGD: Error: \n");// lots of places
    throw oops;
  }

  return NeutralGasDensity[j][k];
}

//
// initializes the Neutral Gas Density
//
void NGD::setNeutralGasDensity(const Scalar& uniformGasDensity, 
                               const Vector2& p1Grid, const Vector2& p2Grid) 
  /* throw(Oops) */{
  int j,k;
  Vector2 x;
  Vector2 x1MKS = ptrLocalGrid->getMKS(p1Grid);
  Vector2 x2MKS = ptrLocalGrid->getMKS(p2Grid);
  
  

  if ( uniformGasDensity < 0.0 ) {
    stringstream ss (stringstream::in | stringstream::out);
    ss<< "NGD::setNeutralGasDensity: Error: \n"<<
      "Negative value for the uniform Neutral Gas Density" << 
	    endl << "This should never happen. Check the input file."<<endl;
  
    std::string msg;
    ss>>msg;
    Oops oops(msg);
    throw oops; // exit() "NGD::NGD: 
  }
  
  for (j=0; j < J; j++) {
    for (k=0; k < K; k++) { 
      x = ptrLocalGrid->getMKS(j,k);

      if ( x.e1() < x1MKS.e1() || x.e1() > x2MKS.e1() ||
	   x.e2() < x1MKS.e2() || x.e2() > x2MKS.e2() ) {
	NeutralGasDensity[j][k] = 0.0;
      } else {
	NeutralGasDensity[j][k] = uniformGasDensity;
      }
    }
  } 
}

void NGD::setNeutralGasDensity(const ostring & analyticF, 
                               const Vector2& p1Grid, const Vector2& p2Grid) 
  /* throw(Oops) */{
  int j,k;
  Vector2 x;
  Vector2 x1MKS = ptrLocalGrid->getMKS(p1Grid);
  Vector2 x2MKS = ptrLocalGrid->getMKS(p2Grid);

  stringstream ss (stringstream::in | stringstream::out);
  // maxNGD = 0.0; initial value set in the constructor
  
  for (j=0; j < J; j++) {
    for (k=0; k < K; k++){ 
      x = ptrLocalGrid->getMKS(j,k);
      if ( x.e1() < x1MKS.e1() || x.e1() > x2MKS.e1() ||
	   x.e2() < x1MKS.e2() || x.e2() > x2MKS.e2() )
	NeutralGasDensity[j][k] = 0.0;
      else {
	adv_eval->add_variable("x1",x.e1());
	adv_eval->add_variable("x2",x.e2());
	NeutralGasDensity[j][k] = adv_eval->Evaluate(analyticF.c_str());
	if ( NeutralGasDensity[j][k] < 0.0 ) {
    ss << "NGD::setNeutralGasDensity: Error: \n"<<
	    "Negative value for the Neutral Gas Density " <<
      endl << "while parsing the input function. This should never happen.";
	      
    
    std::string msg;
    ss>>msg;
    Oops oops(msg);
    throw oops;    // exit()  NGD::NGD
	  
	} else if ( NeutralGasDensity[j][k] > maxNGD ) {
	  maxNGD = NeutralGasDensity[j][k];
	}
      }
    }
  }
}

/**
 * get the number of physical particles per computational 
 * particle for a cell on the 2D grid. The cell is specified
 * by the j, k pair of indeces.
 */
Scalar NGD::getNGD_TI_np2c(const int& j, const int& k) const 
  /* throw(Oops) */{
  char* place = (char *)"NGD::getNGD_TI_np2c(const int& j, const int& k) const";
  try{
    check_jk_bounds(j, k, place);
  }
  catch(Oops& oops){
    oops.prepend("NGD::getNGD_TI_np2c: Error: \n");//OK
    throw oops;
  }

  Scalar result;
  stringstream ss (stringstream::in | stringstream::out);

  if ( TI_np2c ) 
    result = TI_np2c[j][k];
  else {
    ss<< "NGD::getNGD_TI_np2c: Error: \n" << 
      "Trying to access TI_np2c[" << j << "][" << k << "]\n" <<
      "while TI_np2c is set to 0. " << endl;
    std::string msg;
    ss>>msg;
    Oops oops(msg);
    throw oops;    // exit() CsMCTI::tunnelIonizeCs: 
      /*HeMCTI::tunnelIonizeHe:
      LiMCTI::tunnelIonize: 
      HMCTI::tunnelIonize: 
      MapNGDs::sendNGDsAndShift: 
      SpatialRegionBoundary::sendNGDStripe: 
      */
  }
  return result;
}

/**
 * set the number of physical particles per computational 
 * particle for a cell on the 2D grid. The cell is specified
 * by the j, k pair of indices.
 */
void NGD::setNGD_TI_np2c(const int j, const int k, Scalar value) 
  /* throw(Oops) */{
  stringstream ss (stringstream::in | stringstream::out);
  char* place = (char *)"NGD::setNGD_TI_np2c(const int j, const int k, Scalar value)";
  try{
    check_jk_bounds(j, k, place);  
  }
  catch(Oops& oops){
    oops.prepend("NGD::setNGD_TI_np2c: Error: \n"); //OK
    throw oops;
  }

  if ( TI_np2c ) 
    TI_np2c[j][k] = value;
  else {
    ss <<  "NGD::setNGD_TI_np2c: Error: \n" <<
        "Trying to access TI_np2c[" << j << "][" << k << "]"
         << endl << "while TI_np2c is set to 0. " << endl;
    std::string msg;
    ss>>msg;
    Oops oops(msg);
    throw oops;    // exit() MapNGDs::sendNGDsAndShift:  
    /*SpatialRegionBoundary::sendNGDStripe:
      SpatialRegionBoundary::recvNGDStripe:*/
  } 
}

/**
 * allocate memory for the TI_np2c and initialize it.
 */
void NGD::initNGD_TI_np2c(const Scalar& TI_numMacroParticlesPerCell, 
                          Grid* grid) 
  /* throw(Oops) */{
  if ( !TI_np2c ) {

    int j, k;
    Scalar cellVol;
    Scalar numCellNeutralAtoms;

    TI_np2c = new Scalar* [J];
    for (j = 0; j < J; j++) {
      TI_np2c[j] = new Scalar [K]; 
      for (k=0; k < K; k++) { 
        cellVol =  grid->cellVolume(j, k);
        try{
          numCellNeutralAtoms =  getNGD(j, k) * cellVol;
        }
        catch(Oops& oops2){
          oops2.prepend("NGD::initNGD_TI_np2c: Error: \n");//done
          throw oops2;
        }
        TI_np2c[j][k] = numCellNeutralAtoms/TI_numMacroParticlesPerCell;
      }
    }
  }
}

/**
 * allocate memory for excessNumIons and set its elements to the
 * default values of 0.0
 */

void NGD::initNGD_excessNumIons() {
  if ( !excessNumIons ) {
    int j, k;
    
    excessNumIons = new Scalar* [J+1];
    for (j = 0; j<=J; j++) {
      excessNumIons[j] = new Scalar [K+1];
      for ( k = 0; k <= K; k++ ) 
        excessNumIons[j][k] = 0.0;
    }
  }
}

/**
 * set the excess number of ions for cell (j, k)
 */
void NGD::setNGD_excessNumIons( const int& j, const int& k, 
                                const Scalar& value) 
  /* throw(Oops) */{
  stringstream ss (stringstream::in | stringstream::out);
  char* place = (char *)
    "NGD::setNGD_excessNumIons(const int&, const int&, const Scalar&)";
  try{
    check_jk_bounds(j, k, place); 
  }
  catch(Oops& oops){
    oops.prepend("NGD::setNGD_excessNumIons: Error: \n"); //OK
    throw oops;
  }

  
  if ( excessNumIons ) 
    excessNumIons[j][k] = value;
  else {
    ss << "NGD::setNGD_excessNumIons: Error: \n"<<
      "Trying to access excessNumIons[" << j << "][" << k << "]" 
      << endl << "while excessNumIons is set to 0." << endl;

    std::string msg;
    ss>>msg;
    Oops oops(msg);
    throw oops;    // exit() CsMCTI::tunnelIonizeCs: Error:
    /*MapNGDs::sendNGDsAndShift
HeMCTI::tunnelIonizeHe: Error:
LiMCTI::tunnelIonize: Error: 
HMCTI::tunnelIonize: Error: 
SpatialRegionBoundary::recvNGDStripe*/

  }
}


/**
 * get the excess number of ions for cell (j, k)
 */
Scalar NGD::getNGD_excessNumIons( const int& j, const int& k) 
  const /* throw(Oops) */{
  stringstream ss (stringstream::in | stringstream::out);
  char* place = (char *)
    "NGD::getNGD_excessNumIons( const int& j, const int& k)";
  try{
    check_jk_bounds(j, k, place);  
  }
  catch(Oops& oops){
    oops.prepend("NGD::getNGD_excessNumIons: Error: \n"); //OK
    throw oops;
  }

  Scalar result;
  if ( excessNumIons ) 
    result = excessNumIons[j][k];
  else {
    ss   <<"NGD::getNGD_excessNumIons: Error: \n" 
      "Trying to access excessNumIons[" << j << "][" << k << "] " 
      << endl << "while excessNumIons is set to 0." << endl;
    std::string msg;
    ss>>msg;
    Oops oops(msg);
    throw oops;    // exit()CsMCTI::tunnelIonizeCs: Error:
    /*MapNGDs::sendNGDsAndShift
HeMCTI::tunnelIonizeHe: Error:
LiMCTI::tunnelIonize: Error: 
HMCTI::tunnelIonize: Error: 
SpatialRegionBoundary::sendNGDStripe*/
  }
  return result;
}

/** 
 * add to the excess number of ions for cell (j, k)
 */
void NGD::add_to_NGD_excessNumIons( const int& j, const int& k, 
                                    const Scalar& value) 
  /* throw(Oops) */{
  stringstream ss (stringstream::in | stringstream::out);
  char* place = (char *)
    "NGD::add_to_NGD_excessNumIons(const int&, const int, const Scalar&)";
  try{
    check_jk_bounds(j, k, place); 
  }
  catch(Oops& oops){
    oops.prepend("NGD::add_to_NGD_excessNumIons: Error: \n"); //OK
    throw oops;
  }
  if ( excessNumIons ) 
    excessNumIons[j][k] += value; 
  else {
    ss << "NGD::add_to_NGD_excessNumIons: Errors: \n"<<
      "Trying to access excessNumIons[" << j << "][" << k << "] " 
      << endl << "while excessNumIons is set to 0." << endl;
    std::string msg;
    ss>>msg;
    Oops oops(msg);
    throw oops;    // exit()// exit()CsMCTI::tunnelIonizeCs: Error:
    /*HeMCTI::tunnelIonizeHe: Error:
    LiMCTI::tunnelIonize: Error: 
    HMCTI::tunnelIonize: Error: */
  }
   
}

/**
 * Dump the data structures for the neutral gas density
 */

#ifdef HAVE_HDF5
int NGD::dumpH5(dumpHDF5 &dumpObj,int number)
{
  Vector2 **X = ptrLocalGrid->getX();
//  Scalar x,y;
  //cerr << "entered NGD::dumpH5.\n";


  hid_t fileId, groupId, datasetId,dataspaceId; //subgroupId ;
  herr_t status;
  hsize_t rank; 
  hsize_t     dimsf[3];   
  hsize_t     dims;
//  int         attrData;
  hid_t  attrdataspaceId,attributeId;
  hid_t scalarType = dumpObj.getHDF5ScalarType();
  string datasetName;

  //  strstream s;
  stringstream s;

  s <<"NGD" << number <<ends;    
  string groupName = s.str();

  fileId = H5Fopen(dumpObj.getDumpFileName().c_str(), H5F_ACC_RDWR, H5P_DEFAULT);
  groupId = H5Gcreate(fileId,groupName.c_str(),0);

  rank = 3;
  dimsf[0] = J;
  dimsf[1] = K;
  dimsf[2] = 3;
  dataspaceId = H5Screate_simple(rank, dimsf, NULL); 
  datasetName = "NGD";  
  datasetId = H5Dcreate(groupId, datasetName.c_str(), scalarType, dataspaceId,
		      H5P_DEFAULT);


//    Scalar**  posData;
//    posData = (Scalar**) X;
//    int compNum = 3;
 
  


  //  rank = 3;
//    dimsf[0] = J;
//    dimsf[1] = K;
//    dimsf[2] = 2;
//    dataspaceId = H5Screate_simple(rank, dimsf, NULL); 
//    datasetName = "positions";  
//    datasetId = H5Dcreate(groupId, datasetName.c_str(), scalarType, dataspaceId,
//  		      H5P_DEFAULT);
 
  Scalar *data = new Scalar[J*K*3];
 
  //  ofstream out;
  //  out.open("NGDdump.txt");


  for (int j=0; j<J;j++)
    for (int k=0; k<K;k++) {     
      
      hsize_t dim3[] = {3};
      hsize_t rank = 1;
//      hsize_t coord3[3][3];
      
      
      //       if(j*(K+1) +k  < 10){
	//  out << X[j][k].e1()<<" ";
//  	out << X[j][k].e2()<<" ";
//  	out << NeutralGasDensity[j][k]<<"\n";
	
	// }
      
      
      data[K*3*j + k*3] =  X[j][k].e1();
      data[K*3*j + k*3 + 1] =  X[j][k].e2();
      data[K*3*j + k*3 + 2] =  NeutralGasDensity[j][k];
    }
 
   status = H5Dwrite(datasetId, scalarType, H5S_ALL, H5S_ALL,
		    H5P_DEFAULT, data);
  

//     out.close();
 
  H5Sclose(dataspaceId);
  H5Dclose(datasetId);
  delete[] data;
  
// dump TIon as attribute
   dims = 1;
   attrdataspaceId = H5Screate_simple(1, &dims, NULL);
   attributeId = H5Acreate(groupId, "isTIOn",H5T_NATIVE_INT, attrdataspaceId, H5P_DEFAULT);
   status = H5Awrite(attributeId,H5T_NATIVE_INT , &isTIOn);
   status = H5Aclose(attributeId);
   status = H5Sclose(attrdataspaceId);



  // dump TIon data
   if ( isTIOn ) {
     rank = 2;
     dimsf[0] = J;
     dimsf[1] = K;
     
     Scalar *TIdata = new Scalar[J*K];

       for (int j=0; j<J;j++)
	 for (int k=0; k<K;k++) {    
	   TIdata[j*K + k]  = TI_np2c[j][k];
	 }

     dataspaceId = H5Screate_simple(rank, dimsf, NULL); 
     datasetName = "TI_np2c";  
     datasetId = H5Dcreate(groupId, datasetName.c_str(), scalarType, dataspaceId,
			   H5P_DEFAULT);
     status = H5Dwrite(datasetId, scalarType, H5S_ALL, H5S_ALL,
		       H5P_DEFAULT, TIdata);
     H5Sclose(dataspaceId);
     H5Dclose(datasetId);
     
     for (int j=0; j<J;j++)
	 for (int k=0; k<K;k++) {    
	   TIdata[j*K + k]  = excessNumIons[j][k];
	 }

     dataspaceId = H5Screate_simple(rank, dimsf, NULL); 
     datasetName = "excessNumIons";  
     datasetId = H5Dcreate(groupId, datasetName.c_str(), scalarType, dataspaceId,
			   H5P_DEFAULT);
     status = H5Dwrite(datasetId, scalarType, H5S_ALL, H5S_ALL,
		     H5P_DEFAULT, TIdata);
     
     H5Sclose(dataspaceId);
     H5Dclose(datasetId);
     delete[] TIdata;
    
//       out.open("TIdump.txt");
     for (int j=0; j<J;j++)
    for (int k=0; k<K;k++) {   
   //     out << TI_np2c[j][k]<<" ";
//        out << excessNumIons[j][k]<<" \n";
    }
     
 //      out.close();
  }

 

//   if ( isTIOn ) {
//     for (int j=0; j<J;j++)
//       for (int k=0; k<K;k++) {    

//         hsize_t dim2[] = {2};
//         hsize_t rank = 1;
//         hsize_t coord[1][2];
//         hid_t tempSpace2 = H5Screate_simple(rank, dim2, NULL);
       
	  
//  	      Scalar vals[1] = { TI_np2c[j][k]};
//  	      coord[0][0] = j; coord3[0][1] = k; 
//  	      status = H5Sselect_elements(dataspaceId, H5S_SELECT_SET, 1, 
//  					  (const hssize_t **)coord);
//  	      status = H5Dwrite(datasetId, scalarType, tempSpace2, dataspaceId, H5P_DEFAULT, vals);
	      
//  	      vals[0] = excessNumIons[j][k];
//  	      status = H5Sselect_elements(EIspaceId, H5S_SELECT_SET, 1, 
//  					  (const hssize_t **)coord);
//  	      status = H5Dwrite(EIsetId, scalarType, tempSpace2, EIspaceId, H5P_DEFAULT, &excessNumIons[j][k]);
//  	      status = H5Sclose(tempSpace2);
//       }
 
 
//   status = H5Dwrite(datasetId, scalarType, H5S_ALL, H5S_ALL,
//  		   H5P_DEFAULT, data2);
 
//   }

  H5Gclose(groupId);
 H5Fclose(fileId);
 
  return(1);
}
#endif


int NGD::Dump(FILE *DMPFile) {
  Vector2 **X = ptrLocalGrid->getX();
  Scalar x,y;
  XGWrite(&J,4,1,DMPFile,"int");
  XGWrite(&K,4,1,DMPFile,"int");   

  //
  // if isTIOn is non zero, we need to dump TI_np2c and excessNumIons
  //
  XGWrite(&isTIOn,4,1,DMPFile,"int");

  Vector2 Xloc;

#ifdef MPI_VERSION
      extern int MPI_RANK;
      cout << "Dumping NDGs " 
           << "from MPI rank " << MPI_RANK << endl
           << "J = " << J << endl
           << "K = " << K << endl;
      cout.flush();
#endif
  for (int j=0; j<J;j++)
    for (int k=0; k<K;k++) {     
      x = X[j][k].e1();
      y = X[j][k].e2();
      Xloc = ptrLocalGrid->getNearestGridCoords(Vector2(x,y));
      XGWrite(&x,ScalarInt,1,DMPFile,ScalarChar);
      XGWrite(&y,ScalarInt,1,DMPFile,ScalarChar);
      x = NeutralGasDensity[j][k];
      XGWrite(&x,ScalarInt,1,DMPFile,ScalarChar);
      if ( isTIOn ) {
        x = TI_np2c[j][k];
        XGWrite(&x,ScalarInt,1,DMPFile,ScalarChar);
        x = excessNumIons[j][k];
        XGWrite(&x,ScalarInt,1,DMPFile,ScalarChar);
      }
    }
  return (1);
}



/*
 * Restore the data structures from an HDF5 file
 */
#ifdef HAVE_HDF5
int NGD::restoreH5(dumpHDF5 &restoreObj,string whichNGD) 
  /* throw(Oops) */{
 //   cerr << "entering NGD::restoreH5.\n";
//  #ifdef MPI_VERSION
//    extern int MPI_RANK;
//    cerr << " ,MPI_RANK "<<MPI_RANK<<"   "<<restoreObj.getDumpFileName()<<"\n";
//    cerr << "NGD number "<<whichNGD<<"\n";
//  #endif

  stringstream ss (stringstream::in | stringstream::out);

  Scalar x, y;
  Vector2 Xloc;
  Scalar tmpNeutralGasDensity;
//  Scalar tmpTI_np2c;
//  Scalar tmpexcessNumIons;
  int Jl, Kl;  // dimensions of the data in the dump file
  int jl, kl;  // indices over the data in the dump file
  int ji, ki;  // tmp indices
  hid_t fileId, groupId; // datasetId,dataspaceId;
  hid_t NGDsetId,NGDspaceId;//pssetId; //posspaceId;
  herr_t status;
  string outFile;
//  hsize_t rank; 
//  hsize_t     dimsf[1];   
//  hsize_t dims;
  hid_t scalarType = restoreObj.getHDF5ScalarType();
  hid_t  attrId; //attrspaceId,
//  hssize_t offset[3]; /* Offset of hyperslab */
//  hsize_t count[3];  /* Block count */
// Initialize to stop compiler complaints
  hid_t TIsetId=0, TIspaceId=0;
  hid_t EIsetId=0, EIspaceId=0;
  Scalar *TIdata=0, *EIdata=0;


  fileId = H5Fopen(restoreObj.getDumpFileName().c_str(), H5F_ACC_RDONLY, 
	H5P_DEFAULT);
  groupId = H5Gopen(fileId, whichNGD.c_str());


// read isTIOn attribute

  attrId = H5Aopen_name(groupId, "isTIOn");
  status = H5Aread(attrId, H5T_NATIVE_INT , &isTIOn);
  status = H5Aclose(attrId);

  if ( isTIOn ) {
    if ( TI_np2c == 0 || excessNumIons == 0 ) {
      ss << "NGD::restoreH5: Errors: \n"<<
        "TI_np2c and excessNumIons data present in the dump file" << endl
           << "but memory is not allocated for the TI_np2c and " << endl
           << "excessNumIons data structures." << endl;
           
      std::string msg ;
      ss>>msg;
      Oops oops(msg);
      throw oops;    // exit()SpatialRegion::restoreH5
    }
  }

 

  NGDsetId = H5Dopen(groupId,"NGD" );
  NGDspaceId = H5Dget_space(NGDsetId );
  int dataRank = H5Sget_simple_extent_ndims(NGDspaceId);
  hsize_t *sizes = new hsize_t[dataRank];
  int res = H5Sget_simple_extent_dims(NGDspaceId, sizes, NULL);

  Jl = sizes[0]; // dimensions of data
  Kl = sizes[1];
  hsize_t dim = sizes[2]; //should be 3
  // hid_t ngdSlabId =  H5Screate_simple(1, &dim, NULL);
      
  Scalar *NGDbuf = new Scalar[(Jl)*(Kl)*3];
  status = H5Dread(NGDsetId, scalarType,H5S_ALL, NGDspaceId, H5P_DEFAULT,NGDbuf);

  delete [] sizes;
  
 //   possetId = H5Dopen(groupId,"positions" );
//    posspaceId = H5Dget_space(possetId );
//    Scalar *posbuf = new Scalar[(Jl)*(Kl)*2];
//    status = H5Dread(possetId, scalarType,H5S_ALL, posspaceId, H5P_DEFAULT,posbuf);

 
  if ( isTIOn ) { 
    TIsetId = H5Dopen(groupId, "TI_np2c");
    TIspaceId = H5Dget_space(TIsetId);
    TIdata = new Scalar[(Jl)*(Kl)];
    status = H5Dread(TIsetId, scalarType,H5S_ALL, TIspaceId, H5P_DEFAULT, TIdata);

    EIsetId = H5Dopen(groupId, "excessNumIons");
    EIspaceId = H5Dget_space(EIsetId);
    EIdata = new Scalar[(Jl)*(Kl)];
    status = H5Dread(EIsetId, scalarType,H5S_ALL, EIspaceId, H5P_DEFAULT, EIdata);

  }



 // ofstream out;
 //  out.open("NGDrestore.txt");

  int compNum = 3;
  int maxNum = Jl*Kl;
 //   for(int i=0;i<maxNum*compNum;i++)
//      { //    out.close();
//      //   out << NGDbuf[i] <<" ";
//        if((i+1)%compNum ==0 )
//  	//out << "\n";
//      }
  //  out.close();
  // out.open("TIrestore.txt");

//    for(int i=0;i<maxNum;i++)
//      {  
//        //   out << TIdata[i] <<" ";
//        //  out << EIdata[i] <<"\n";
//      }
//    //  out.close();

  //out.open("NGDafterrestore.txt");
  int errs = 0; 
  for (jl=0; jl<Jl;jl++)
    for (kl=0; kl<Kl;kl++) {
      // 
// read in the coordinates
      // 

     //   Scalar ngd[3];   
//        offset[0]  = 0;  count[0]  = 3;
//        status = H5Sselect_hyperslab(ngdSlabId, H5S_SELECT_SET, offset, NULL, count, NULL);
//        offset[0]  = jl; offset[1]  = kl;offset[2] = 0;
//        count[0]  = 1; count[1]  = 1; count[2] = 3;    
//        status = H5Sselect_hyperslab(dataspaceId, H5S_SELECT_SET, offset, NULL, count, NULL);
//        status = H5Dread(datasetId, scalarType, ngdSlabId, dataspaceId, H5P_DEFAULT, ngd);
      
      x = NGDbuf[jl*Kl*3 + kl*3];
      y = NGDbuf[jl*Kl*3 + kl*3 + 1];
      tmpNeutralGasDensity = NGDbuf[jl*Kl*3 + kl*3 + 2];
      
    


// 
// read the rest of the data associated with this data location
      // 

    //    if ( isTIOn ) {

//  	offset[0]  = 0;  count[0]  = 1;
//  	status = H5Sselect_hyperslab(ngdSlabId, H5S_SELECT_SET, offset, NULL, count, NULL);
//  	offset[0]  = jl; offset[1]  = kl;offset[2] = 0;
//  	count[0]  = 1; count[1]  = 1; count[2] = 1;    
//  	status = H5Sselect_hyperslab(TIspaceId, H5S_SELECT_SET, offset, NULL, count, NULL);
//  	status = H5Dread(TIsetId, scalarType, ngdSlabId, TIspaceId, H5P_DEFAULT, &tmpTI_np2c);
	
//  	status = H5Sselect_hyperslab(EIspaceId, H5S_SELECT_SET, offset, NULL, count, NULL);
//  	status = H5Dread(EIsetId, scalarType, ngdSlabId, EIspaceId, H5P_DEFAULT, &tmpexcessNumIons);
	
//        }
      //
// find the grid locations here
      //
      Xloc = ptrLocalGrid->getNearestGridCoords(Vector2(x,y));
      //  out << x <<" ";
      //  out << y <<" ";

      if(Xloc.e1() >= 0) { // valid location
        ji = (int)(Xloc.e1());
        ki = (int)(Xloc.e2());
        if ( ji < Jl ) {
          NeutralGasDensity[ji][ki] = tmpNeutralGasDensity;
	  //  out << NeutralGasDensity[ji][ki]<<" \n"; 
          if ( isTIOn ) {
            TI_np2c[ji][ki] = TIdata[jl*Kl + kl];
            excessNumIons[ji][ki] = EIdata[jl*Kl + kl];
          }
        }
      }
    }

  //  out.close();
  //  status = H5Sclose(ngdSlabId);

 if ( isTIOn ) {
   //  cerr << "\n\n\n TI IS On !!!!\n\n";
   
   status = H5Sclose(EIspaceId);
   status = H5Dclose(EIsetId);
   status = H5Sclose(TIspaceId);
   status = H5Dclose(TIsetId);
   delete[] EIdata;
   delete[] TIdata;
 }

 
 // out.open("TIafterrestore.txt");


//   for (jl=0; jl<Jl;jl++)
//      for (kl=0; kl<Kl;kl++) {
//        //   out <<TI_np2c[ji][ki] <<" ";
//        //  out <<excessNumIons[ji][ki] <<" \n";
      
//      }

 //  out.close();



  status = H5Sclose(NGDspaceId);
  status = H5Dclose(NGDsetId);
  status = H5Gclose(groupId);
  status = H5Fclose(fileId);

  delete[] NGDbuf;


  return(1);
}
#endif


/**
  * Restore the data structures for the neutral gas density
  */
int NGD::Restore(FILE *DMPFile) {

  if ( dumpFileRevisionNumber < firstRevisionNumberWithNGD ) {
    cout << "The revision number of the dump file: "
         << dumpFileRevisionNumber << " is smaller than" << endl
         << "the first revision number: " << firstRevisionNumberWithNGD 
         << " of the code with the neutral gas density objects." << endl
         << "Returning from: int NGD::Restore(FILE *DMPFile) {...}" 
         << endl;
    return 1;
  }

  Scalar x, y;
  Vector2 Xloc;
  Scalar tmpNeutralGasDensity;
  Scalar tmpTI_np2c;
  Scalar tmpexcessNumIons;
  int Jl, Kl;  // dimensions of the data in the dump file
  int jl, kl;  // indices over the data in the dump file
  int ji, ki;  // tmp indices
  int tmpisTIOn;

  if ( discardDumpFileNGDDataFlag ) {
    cout << "Value of discardDumpFileNGDDataFlag = " 
         << discardDumpFileNGDDataFlag << "." << endl
         << "The NGD data from the dump file will be read and discarded."
         << endl;
    XGRead(&Jl,4,1,DMPFile,"int");
    XGRead(&Kl,4,1,DMPFile,"int");
    XGRead(&tmpisTIOn,4,1,DMPFile,"int");

    for (jl=0; jl<Jl;jl++)
      for (kl=0; kl<Kl;kl++) {
        // 
        // read in the coordinates
        // 
        XGRead(&x,ScalarInt,1,DMPFile,ScalarChar);
        XGRead(&y,ScalarInt,1,DMPFile,ScalarChar);
        // 
        // read the rest of the data associated with this data location
        // 
        XGRead(&tmpNeutralGasDensity,ScalarInt,1,DMPFile,ScalarChar);
        if ( tmpisTIOn ) {
          XGRead(&tmpTI_np2c,ScalarInt,1,DMPFile,ScalarChar);
          XGRead(&tmpexcessNumIons,ScalarInt,1,DMPFile,ScalarChar);
        }
      }
  } else {
    cout << "Value of discardDumpFileNGDDataFlag = " 
         << discardDumpFileNGDDataFlag << "." << endl
         << "The NGD data from the dump file will be read and used to"
         << endl << "initialize the NGD objects." << endl;

    XGRead(&Jl,4,1,DMPFile,"int");
    XGRead(&Kl,4,1,DMPFile,"int");    
    XGRead(&tmpisTIOn,4,1,DMPFile,"int");
    //
    // restoring the NGD data
    // 
    for (jl=0; jl<Jl;jl++)
      for (kl=0; kl<Kl;kl++) {
        // 
        // read in the coordinates
        // 
        XGRead(&x,ScalarInt,1,DMPFile,ScalarChar);
        XGRead(&y,ScalarInt,1,DMPFile,ScalarChar);
        // 
        // read the rest of the data associated with this data location
        // 
        XGRead(&tmpNeutralGasDensity,ScalarInt,1,DMPFile,ScalarChar);
        if ( tmpisTIOn ) {
          XGRead(&tmpTI_np2c,ScalarInt,1,DMPFile,ScalarChar);
          XGRead(&tmpexcessNumIons,ScalarInt,1,DMPFile,ScalarChar);
        }
        //
        // find the grid locations here
        //
        Xloc = ptrLocalGrid->getNearestGridCoords(Vector2(x,y));
        
        if(Xloc.e1() >= 0) { // valid location
          ji = (int)(Xloc.e1());
          ki = (int)(Xloc.e2());
          if ( ji < Jl ) {
            NeutralGasDensity[ji][ki] = tmpNeutralGasDensity;
            if ( isTIOn && tmpisTIOn ) {
              TI_np2c[ji][ki] = tmpTI_np2c;
              excessNumIons[ji][ki] = tmpexcessNumIons;
            }
          }
        }
      }
  }
  return 0;
}
