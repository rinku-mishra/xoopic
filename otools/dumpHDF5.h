/*
====================================================================

DUMPHDF5.H

Defines hdf5IO class



====================================================================
*/
#ifndef	__DUMP_HDF5_H
#define	__DUMP_HDF5_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_HDF5
#include <hdf5.h>
#endif

#include <string>
using std::string;
#include <precision.h>

class dumpHDF5{

  public:

  /**
 * Constructor
 *
 * @param baseName the base name for data files.
 * @param seqNumber the sequence number
 
 * @param rank the rank of the processor
 * 
 */
    dumpHDF5();

/**
 * Destructor
 */
    ~dumpHDF5();

    /** dump to dump file
     */
   void  dumpH5(string baseName, int seqNumber, int rank);


   /** Restore from HDF5 file */
   //  void restoreH5(SpatialRegion*);

/** create HDF5 file */
   void createFile();
    

/** read dataset from HDF5 file */
   void readSimple(string dataPath,Scalar* data,int &dataLength);

/** write dataset to HDF5 file */
   void writeSimple(string dataPath,Scalar* data, int rank, int *size);


/** get scalar type HDF5 identifier */
   #ifdef HAVE_HDF5

    hid_t getHDF5ScalarType()
      {
	return h5Scalar;
      }
    #endif

/** get index of dump file */
    //   string getDumpIndex(){return dumpIndexStr;}


/** set MPI rank of dump file 
Adds to baseName and overwrites dumpFileName.
*/
    void setMpiRank(int rank);
     

/** set index of dump file 
*/
    void setDumpIndex(int index);
      

/** set name of dump file */
void setDumpBaseName(string name)
      {
	baseName = name;
	dumpFileName = name + ".h5";
	return;
      }

void setDumpFileName(string name)
      {
      	dumpFileName = name;
	return;
      }


/** get name of dump file */
    string getDumpFileName()
      {
	return dumpFileName;
      }
/** increment name of dump file */  
    void indexDumpFileName();
     
     
/** number of steps between dumps */
    int dumpPeriod;

/** name of dataset to write data  */
    string datasetName;
    

 protected:
#ifdef HAVE_HDF5
    hid_t h5Scalar;
#endif
/** name of dump file*/
    string dumpFileName;

/** base name of dump file*/
    string baseName;

/** current index of dump (often included in the file name) */  
     string dumpIndexStr;

/** current index of MPI process (often included in the file name) */ 
     string mpiRankStr;
};






#endif	//	ifndef __DUMP_HDF5_H


