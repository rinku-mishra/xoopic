/*
 * DUMPHDF5.CPP
 *
 *  Implementation of dumpHDF5 class
 *  0.99 (KC, 11-9-97)  Replacing often used codes with funcitons
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "dumpHDF5.h"

#include <string>


#include "sptlrgn.h"
#include "globalvars.h"
#include <stdio.h>
#include "plsmadev.h"

#if defined(_MSC_VER)
#ifdef OOPICPRO
extern "C" char theDumpFile[180];
#else
extern "C" char* const volatile theDumpFile;
#endif
extern "C" char theRestoreFile[180];
#else
#ifdef OOPICPRO
extern char theDumpFile[180];
#else
extern char* const volatile theDumpFile;
#endif
extern char theRestoreFile[180];
#endif


extern PlasmaDevice *theDevice;
extern oopicList<SpatialRegion> *theRegions;
using namespace std;


dumpHDF5::dumpHDF5()
{
#ifdef HAVE_HDF5
#ifdef SCALAR_DOUBLE
  h5Scalar = H5T_NATIVE_DOUBLE;
#else
  h5Scalar = H5T_NATIVE_FLOAT;
#endif 
#endif //HAVE_HDF5

  string temp = ".h5";
  dumpFileName = theDumpFile;
  //  dumpFileName = theDumpFile + temp;
  dumpPeriod = 0;
  datasetName = "Data";
  mpiRankStr = "";
  dumpIndexStr = "";
  return;
}

dumpHDF5::~dumpHDF5(){}

#ifdef HAVE_HDF5

 void dumpHDF5::createFile()
{
  hid_t fileId;
  fileId = H5Fcreate(dumpFileName.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);  
  H5Fclose(fileId);
  return;
}

 void dumpHDF5::setDumpIndex(int index)
{
  stringstream s;

  s <<index; // <<ends; // Commented out by Busby, 12/'05
  dumpIndexStr = s.str();
  dumpFileName = baseName + "_i" + dumpIndexStr;
  if(mpiRankStr.size() > 0)
    dumpFileName = dumpFileName + "_p" + mpiRankStr;
  dumpFileName += ".h5";
  return;
}

void dumpHDF5::setMpiRank(int rank)
{
  //  strstream s;

  stringstream s;

  s <<rank; // <<ends; // Commented out by Busby, 12/'05
  mpiRankStr = s.str();

  dumpFileName = baseName; 
  if(dumpIndexStr.size() > 0)
    dumpFileName = dumpFileName + "_i" + dumpIndexStr;
  dumpFileName  = dumpFileName + "_p" + mpiRankStr + ".h5";
  return;
}

void dumpHDF5::dumpH5(string baseName, int seqNumber, int rank)
{
  //  strstream s;

  stringstream s;

  dumpFileName = baseName;
  hid_t fileId; 
  fileId = H5Fcreate(dumpFileName.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);  
 
   theDevice->dumpH5(*this);
  H5Fclose(fileId);
  return;
}

void dumpHDF5::readSimple(string dataPath,Scalar *data,int &dataLength)
{
// data should be an unused pointer
  hid_t fileId,datasetId,dataspaceId;
  herr_t status;
  fileId = H5Fopen(dumpFileName.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
  datasetId = H5Dopen(fileId,dataPath.c_str());
  dataspaceId = H5Dget_space(datasetId );
  int dataRank = H5Sget_simple_extent_ndims(dataspaceId);
  hsize_t *sizes = new hsize_t[dataRank];
  int res = H5Sget_simple_extent_dims(dataspaceId, sizes, NULL);

  dataLength = 1;
  for(int i=0;i<dataRank;i++)
  {
    dataLength = dataLength * sizes[i];
  }
  data = new Scalar[dataLength];
  status = H5Dread(datasetId, h5Scalar,H5S_ALL, dataspaceId, H5P_DEFAULT, data);
  status =    H5Sclose(dataspaceId);
  status =    H5Dclose(datasetId);
  delete [] sizes;
  return;
}

void dumpHDF5::writeSimple(string dataPath,Scalar *data,int rank,int* size)
{
// data is 1-D array of Scalars.
// rank, size can describe a 2-D array of any dimension, but number of elements
// should equal the number in 'data'

 hid_t fileId; // groupId, 
 hid_t datasetId,dataspaceId;
 herr_t status;
 fileId= H5Fopen(dumpFileName.c_str(), H5F_ACC_RDWR, H5P_DEFAULT);
// set uip dimensions of dataspace
 hsize_t *dim = new hsize_t[rank];
 for(int i=0;i<rank;i++)
   dim[i] = size[i];
 
 dataspaceId = H5Screate_simple(rank, dim, NULL);
 datasetId = H5Dcreate(fileId, dataPath.c_str(), h5Scalar, dataspaceId, H5P_DEFAULT);

 if(datasetId < 0){
   status = H5Sclose(dataspaceId);
   //  status = H5Dclose(datasetId);
   status = H5Fclose(fileId);
   return;
 }
//write data to file
 status = H5Dwrite(datasetId, h5Scalar, H5S_ALL, dataspaceId, H5P_DEFAULT, data);
 status = H5Sclose(dataspaceId);
 status = H5Dclose(datasetId);
 status = H5Fclose(fileId);
 delete[] dim; 
 return;
}



void restoreHDF5(SpatialRegion * region,const char* FileName)
  /* throw(Oops) */{
  //cerr << "\nentered restoreHDF5\n";
  int seq = 0;
  int mpiRank = 0;
  dumpHDF5 restoreObj;
  string name  = FileName;
  restoreObj.setDumpFileName(name);
  try{
    region->restoreH5(restoreObj);
  }
  catch(Oops& oops2){
    oops2.prepend("dumpHDF5::restoreHDF5: Error: \n");//done
    throw oops2;
  }
  return;
}

#endif
