
#include "dump.h"
#include "dumpHDF5.h"

#include "globalvars.h"
#include "plsmadev.h"
#include "sptlrgn.h"
void restoreHDF5(SpatialRegion*,const char* filename); 

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <string>

using namespace std;

#ifdef MPI_VERSION
#include <mpi.h>
extern int MPI_RANK;
#endif


#if defined(_MSC_VER)
#ifdef OOPICPRO
extern "C" char theDumpFile[180];
#else
extern "C" char* const volatile theDumpFile;
#endif

#ifdef TXQ_GUI
extern "C" int dumpWithHDF5;
extern "C" int restoreFromHDF5;
extern "C" char theRestoreFile[180];
#endif // TXQ_GUI

#else // MSC_VER

#ifdef OOPICPRO
extern char theDumpFile[180];
#else
extern char* const volatile theDumpFile;
#endif

#ifdef TXQ_GUI
extern int dumpWithHDF5;
extern int restoreFromHDF5;
extern char theRestoreFile[180];
#endif // TXQ_GUI

#endif //MSC_VER

// if using xoopic, define HDF5 flags here
#ifndef TXQ_GUI
int restoreFromHDF5 = 0;
int dumpWithHDF5 = 0;
char theRestoreFile[180];
#endif


extern PlasmaDevice *theDevice;
extern oopicList<SpatialRegion> *theRegions;
extern "C" {
void Dump(const char *);
void Quit(void);
}



/***************************************************************/
/* Dumping the current state of the system into a binary file. */
/* Note: the binary file is written in the format used by IBM  */
/* and DEC compatible machines (low bytes followed by high     */
/* bytes). For this Dump() and Restore() call the functions    */
/* sun_read() and sun_write().                                 */

char Revision[]={'2','.','6','1','\0'};
Scalar dumpFileRevisionNumber = -1.; // to be set by Restore(...)

// Doing a re-org of dump.
// calling plasma device and cascading from there.

void Dump(const char *the_dump_file_name)
{

#ifdef HAVE_HDF5

	// check for dump file extension,choose dump format accordingly
	if(!dumpWithHDF5) {
		string fileName = the_dump_file_name;
		size_t posExt = fileName.find(".h5");
		//  cerr << "fileName : " << fileName<< " posExt : " << posExt;
		if(posExt !=  string::npos)
			dumpWithHDF5 = 1;
	}
	else{
		string fileName = the_dump_file_name;
		size_t posExt = fileName.find(".dmp");
		//  cerr << "fileName : " << fileName<< " posExt : " << posExt;
		if(posExt !=  string::npos)
			dumpWithHDF5 = 0;
	}

#else

	dumpWithHDF5 = 0;

#endif

#ifdef HAVE_HDF5 

	if (dumpWithHDF5) {
		int seqNumber = 0;
		int mpiRank = 1;
		dumpHDF5 dumpObj;
		string fileName = the_dump_file_name;

#ifdef MPI_VERSION

		stringstream s1;

		s1 << "_p" << MPI_RANK <<ends;
		size_t posExt = fileName.find(".h5");
		fileName.insert(posExt,s1.str() );

#endif // MPI_VERSION

		dumpObj.dumpH5(fileName, seqNumber, mpiRank);
		return;
	}

#endif // HAVE_HDF5

	FILE *DMPFile;
	char theDumpFileName[256];

#ifdef MPI_VERSION

	sprintf(theDumpFileName,"%s.%d",the_dump_file_name,MPI_RANK);
	extern int MPI_RANK;
	std::cout << "Dumping in file name: " << theDumpFileName << endl
			<< "from MPI rank " << MPI_RANK << endl;
	std::cout.flush();

#else  // if not MPI_VERSION

	sprintf(theDumpFileName,"%s",the_dump_file_name);

#endif // MPI_VERSION

	if ((DMPFile = fopen(theDumpFileName, "w+b")) == NULL)
	{
		printf("filename = %s\n", theDumpFile);
		puts("Dump: open failed error#1");
		return;
	}

	XGWrite(Revision, 1, 4, DMPFile, "char");

	// Cascade from Plasma Device

	theDevice->Dump(DMPFile);

	fclose(DMPFile);

}


/****************************************************************/

void Restore(SpatialRegion *region)
/* throw(Oops) */{
	char Rev[5];
	char FileName[256];
	char serialFileName[256];
	FILE *DMPFile=NULL;
	int i=0;
	string restoreFile;
	// All of the files of the form
	// theDumpFile
	// theDumpFile.1
	// theDumpFile.2
	// ..
	// theDumpFile.n
	// have to be checked.

	// if not OOPIC Pro, set restore file to be same as dump file

#ifndef TXQ_GUI
	strcpy(theRestoreFile,theDumpFile);
#endif

	// check for dump file extension,choose dump format accordingly

#ifdef HAVE_HDF5

	if(!restoreFromHDF5) {
		string fileName = theRestoreFile;
		size_t posExt = fileName.find(".h5");
		if(posExt !=  string::npos)
			restoreFromHDF5 = 1;
	}
	else{
		string fileName = theRestoreFile;
		size_t posExt = fileName.find(".dmp");
		if(posExt !=  string::npos)
			restoreFromHDF5 = 0;
	}
#else

	restoreFromHDF5 = 0;

#endif

	sprintf(FileName,"%s",theRestoreFile);

	for (int iCopy=0; iCopy<256; ++iCopy) {
		serialFileName[iCopy] = FileName[iCopy];
	}
	//   cerr << "dump file before opening: " << FileName <<"\n";
	DMPFile = fopen(FileName, "r+b");

	do {
		if(DMPFile==NULL && i==0)  {
			if(restoreFromHDF5){  // insert mpi rank into HDF5 file
				restoreFile = FileName;
				size_t posExt = restoreFile.find(".h5");
				restoreFile.insert(posExt,"_p0");
				strcpy(FileName,restoreFile.c_str());
			}else{
				sprintf(FileName,"%s.%d",serialFileName,i);
			}

			//	 cerr << "MPI dump file before opening: " << FileName <<"\n";
			DMPFile = fopen(FileName,"r+b");
			if(DMPFile==NULL) {
				printf("Can't read dump file.");
				printf("Returning from dump.cpp");

				std::cout << "Can't read dump file."   << endl;
				std::cout << "Returning from dump.cpp" << endl;
				return;
			}

		}
		if(i>0) {
			if(restoreFromHDF5){  // insert mpi rank into HDF5 file
				restoreFile = serialFileName;
				size_t posExt = restoreFile.find(".h5");

				//	  strstream s1;

				stringstream s1;

				s1 << "_p" << i <<ends;
				restoreFile.insert(posExt,s1.str() );
				strcpy(FileName,restoreFile.c_str());
			}else{
				sprintf(FileName,"%s.%d",serialFileName,i);
			}
			DMPFile = fopen(FileName,"r+b");
			if(DMPFile == NULL) return;
		}


		if(restoreFromHDF5){

#ifdef HAVE_HDF5
			fclose(DMPFile);
			try{
				restoreHDF5(region,FileName);
			}
			catch(Oops& oops3){
				oops3.prepend("dump::Restore: Error: \n");
				throw oops3;
			}

#endif

		}else{

			XGRead(Rev, 1, 4, DMPFile, "char");
			// restore version 2.00 dump files.
			if(strncmp(Rev,"2.00",4)==0) {
				try{
					region->Restore_2_00(DMPFile);
				}
				catch(Oops& oops){
					oops.prepend("dump::Restore: Error:\n");
					throw oops;
				}
				return;
			}
			Rev[4]='\0';
			dumpFileRevisionNumber = 2.61;
			//      Scalar dumpFileRevisionNumber = atof(Rev);
			std::cout << "The current revision number of the code is: "
					<< Revision << endl
					<< "Started reading dump file with revision number: "
					<< dumpFileRevisionNumber << endl;

			std::cout << "Started reading dump file." << endl;

			region->Restore(DMPFile);
			std::cout << "Finished reading dump file." << endl;
			fclose(DMPFile);
		}
		i++;
	} while(DMPFile != NULL);  // may as well be a while(1)--exits before if DMPFile==NULL
}

/* restore the previous verson of the dump file. */

void Restore_2_00(SpatialRegion *region)
/* throw(Oops) */{

	char Rev[5];
	static FILE *DMPFile=NULL;
	int i;

	if(DMPFile==NULL) {
		if ((DMPFile = fopen(theRestoreFile, "r+b")) == NULL)
		{
			puts("Dump: open failed error #2");
			printf("filename %s", theRestoreFile);
			return;
		}
	}
	XGRead(Rev, 1, 4, DMPFile, "char");
	for (i=0; i<4; i++)
		if (Rev[i]!=Revision[i]){
			stringstream ss (stringstream::in | stringstream::out);
			ss<<"Dump::Restore_2_00: Error: \n"<<
					"Incompatible dump file version\n";
			//putchar(7);

			std::string msg;
			ss >> msg;
			Oops oops(msg);
			throw oops;    // exit()  not called

		}
	std::cout << "started reading dump file." << endl;
	try{
		region->Restore_2_00(DMPFile);
	}

	catch(Oops& oops){
		oops.prepend("Dump::Restore_2_00: Error:\n"); //not called
		throw oops;
	}
	std::cout << "finished reading dump file." << endl;

	fclose(DMPFile);
}

/****************************************************************/

void Quit(void)
{
	if (theDevice) {
		delete theDevice;
		theDevice = 0;
	}

#ifdef MPI_VERSION
	MPI_Finalize();
#endif
}

void XGVector2Write(Vector2& vec, FILE *DMPFile)
{
	Scalar temp;

	for (int e=1; e<=2;e++){
		temp = vec.e(e);
		XGWrite(&temp, ScalarInt, 1, DMPFile, ScalarChar);
	}
}

void XGVector3Write(Vector3& vec, FILE *DMPFile)
{
	Scalar temp;

	for (int e=1; e<=3;e++){
		temp = vec.e(e);
		XGWrite(&temp, ScalarInt, 1, DMPFile, ScalarChar);
	}
}

void XGVector2Read(Vector2& vec, FILE *DMPFile)
{
	Scalar temp;

	for (int e=1; e<=2;e++){
		XGRead(&temp, ScalarInt, 1, DMPFile, ScalarChar);
		vec.set(e,temp);
	}
}

void XGVector3Read(Vector3& vec, FILE *DMPFile)
{
	Scalar temp;

	for (int e=1; e<=3;e++){
		XGRead(&temp, ScalarInt, 1, DMPFile, ScalarChar);
		vec.set(e,temp);
	}
}
