/*
 * newdiag.CPP
 *
 * 1.01 (PG, KC, 9-21-95) After release. First draft.	
 * 1.02	(JohnV 03-08-96) Added initwin(params...) for PC version
 * 1.03 (JK, 2013-08-05) Adding methods for saving diagnostics to file
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits>

#include "newdiag.h"
#include "diagn.h"
#include "ostring.h"
#include "diag2file.h"
//#include "predefDiag.h"		// JK, 2013-08-05

extern "C++" void printf(char *);
extern "C" {
#ifdef NOX
#include <xgmini.h>
#else
#include <xgdefs.h>
#endif //NOX
}

//class Diag2File;

//--------------------------------------------------------------------

Diag::Diag(SpatialRegion* _region, int _j1, int _k1, int _j2, int _k2, int _nfft, int _HistMax,
		int _Ave, int _Comb, ostring _VarName, ostring _x1Label,
		ostring _x2Label, ostring _x3Label, ostring _title, int _save, int _open_init, struct write_params wp)
{
	region = _region;
	j1 = MIN(_j1, _j2);
	k1 = MIN(_k1, _k2);
	j2 = MAX(_j1, _j2);
	k2 = MAX(_k1, _k2);
	A1 = region->getX()[j1][k1].e1();
	A2 = region->getX()[j1][k1].e2();
	B1 = region->getX()[j2][k2].e1();
	B2 = region->getX()[j2][k2].e2();

	nfft = _nfft;
	HistMax =  _HistMax;
	Ave = _Ave;
	Comb = _Comb;
	VarName = _VarName;
	x1Label = _x1Label;
	x2Label = _x2Label;
	x3Label = _x3Label;
	title = _title;
	save = _save;

	// JK, 2013-08-23
	theDiagnostics = NULL;
	history = NULL;
	// JK, 2016-02-27
	derivativeDiag = false;

	if( _open_init == 1 ) {
		// open diag window at startup
		win_init_stat = DIAG_WINDOW_INIT_OPEN;

	} else {
		// keep it closed
		win_init_stat = DIAG_WINDOW_INIT_CLOSED;
	}

	if( Diag2File::check_if_write_required(&wp) ) {
		// write to file is required; create object and initialize it
		write2file = new Diag2File(VarName, title, &wp);
	} else {
		// no writing; keep as NULL
		write2file = NULL;
	}
}

/**
 * Diagnostic constructor for "predefined" diagnostics in XOOPIC (E, I, phi, etc.) to enable writing to file.
 * All other (missing) data will be defined when diagnostics is defined (in diagg.cpp).
 * JK, 2013-08-01
 */
Diag::Diag(SpatialRegion* _region, ostring _VarName, ostring _title, int _save, int _open_init, struct write_params wp) {
	region = _region;
	VarName = _VarName;
	title = _title;
	save = _save;

	ostring ending = _VarName.substr(-3);

	if( _open_init == 1 ) {
		// open diag window at startup
		win_init_stat = DIAG_WINDOW_INIT_OPEN;

	} else {
		// keep it closed
		win_init_stat = DIAG_WINDOW_INIT_CLOSED;
	}

	if( Diag2File::check_if_write_required(&wp) ) {
		// write to file is required; create object and initialize it
		write2file = new Diag2File(VarName, title, &wp);
	} else {
		// no writing; keep as NULL
		write2file = NULL;
	}

	// JK, 2013-08-23
	// initialize the rest of parameters
	j1 = k1 = j2 = k2 = 0;
	A1 = A2 = B1 = B2 = 0;
	nfft = HistMax = Ave = Comb = 0;
	x1Label = x2Label = x3Label = "";
	title = "";
	save = 0;
	theDiagnostics = NULL;
	history = NULL;
	// JK, 2016-03-04
	derivativeDiag = false;
}


/**
 * Define values for writing data to file: title, limits, pointer to data, etc.
 * == VECTOR DATA ==
 * JK, 2013-08-01
 */
void Diag::defineParameters(ostring _x1label, ostring _x2label, Scalar* _x1_array, Scalar* _x2_array, Vector3** _vals, int* _jm, int* _km, int _component) {

	x1Label = _x1label;		// JK, 2016-02-27
	x2Label = _x2label;		// JK, 2016-02-27

	if( write2file ) {
		// add/define data for writing into file; add text to title
		ostring longTitle = title;

		if( longTitle.length() != 0 ) {
			longTitle += "; ";
		}

		longTitle += "row data - " + VarName + ": index_" + x1Label + " index_" + x2Label + " " + x1Label + " " + x2Label + " ";

		if( _component == -1 ) {
			// all components
			longTitle += VarName + _x1label + " " + VarName + _x2label + " " + VarName + "3 ";
		} else {
			// single component
			longTitle += VarName;
		}

		write2file->addData(VarName, longTitle, _jm, _km, _x1_array, _x2_array, _vals, _component);
	}
}


/**
 * Define values for writing data to file: title, limits, pointer to data, etc.
 * == SCALAR DATA ==
 * JK, 2013-08-01
 */
void Diag::defineParameters(ostring _x1label, ostring _x2label, Scalar* _x1_array, Scalar* _x2_array, Scalar** _vals, int* _jm, int* _km) {

	x1Label = _x1label;		// JK, 2016-02-27
	x2Label = _x2label;		// JK, 2016-02-27

	if( write2file ) {
		// add/define data for writing into file; add text to title
		ostring longTitle = title;

		if( longTitle.length() != 0 ) {
			longTitle += "; ";
		}

		longTitle += "row data - " + VarName + ": index_" + x1Label + " index_" + x2Label + " " + x1Label + " " + x2Label;

		write2file->addData(VarName, longTitle, _jm, _km, _x1_array, _x2_array, _vals);
	}
}


/**
 * Define values for writing data to file: title, limits, pointer to data, etc.
 * == SPECIES POSITIONS AND VELOCITIES ==
 * JK, 2013-08-02
 */
void Diag::defineParameters(ostring _x1label, ostring _x2label, Scalar* _x1positions, Scalar* _x2positions, Scalar* _x1velocities, Scalar* _x2velocities, Scalar* _x3velocities, int* _num_data, int species_index) {

	x1Label = _x1label;		// JK, 2016-02-27
	x2Label = _x2label;		// JK, 2016-02-27

	if( write2file ) {
		// add/define data for writing into file; add text to title
		ostring longTitle = title;

		if( longTitle.length() != 0 ) {
			longTitle += "; ";
		}

		longTitle += "row data - " + VarName + ": species_index particle_index " + x1Label + " " + x2Label;

		write2file->addData(VarName, longTitle, _x1positions, _x2positions, _x1velocities, _x2velocities, _x3velocities, _num_data, species_index);
	}
}


/**
 * Define values for writing data to file: title, limits, pointer to data, etc.
 * == TIME GRAPHS (t, value) ==
 * JK, 2013-08-02
 */
void Diag::defineParameters(ostring _x1label, ostring _x2label, Scalar* _x1_array, Scalar* _x2_array, int* _num_data) {

	x1Label = _x1label;		// JK, 2016-02-27
	x2Label = _x2label;		// JK, 2016-02-27

	if( write2file ) {
		// add/define data for writing into file; add text to title
		ostring longTitle = title;

		if( longTitle.length() != 0 ) {
			longTitle += "; ";
		}

		longTitle += "row data - " + VarName + ": species_index particle_index " + x1Label + " " + x2Label;

		write2file->addData(VarName, longTitle, _x1_array, _x2_array, _num_data);
	}
}


/**
 * Define values for writing data to file: title, limits, pointer to data, etc.
 * JK, 2013-08-01
 */
void Diag::defineParameters(ostring _x1label, ostring _x2label, int _num_species) {

	x1Label = _x1label;		// JK, 2016-02-27
	x2Label = _x2label;		// JK, 2016-02-27

	if( write2file ) {
		// add/define data for writing into file; add text to title
		ostring longTitle = title;

		if( longTitle.length() != 0 ) {
			longTitle += "; ";
		}

		longTitle += "row data - " + VarName + ", ";
		longTitle += _num_species;
		longTitle += " species: species_index, index_inside_species " + x1Label + " " + x2Label;

		write2file->addTitleSpecies(VarName, longTitle, _num_species);
	}
}


/**
 * Add values for new species.
 * JK, 2013-08-01
 */
void Diag::addSpeciesParameters(Scalar* _x1_values, Scalar* _x2_values, int* _size, int _species_index) {
	if( write2file ) write2file->addDataSpecies(_x1_values, _x2_values, _size, _species_index);
}




#ifdef HAVE_HDF5
int Diag::dumpH5(dumpHDF5 &dumpObj,int number)
{

	Vector2 **X = region->getX();
	//  Scalar x,y;
	//  int xl,yl;

	hid_t fileId, groupId, datasetId,dataspaceId; //subgroupId ;
	herr_t status;
	hsize_t rank;
	hsize_t     dimsf[1];
	hsize_t     dims;
	//  int         attrData;
	hid_t  attrdataspaceId,attributeId;
	Scalar       data[4];
	hid_t scalarType = dumpObj.getHDF5ScalarType();
	string datasetName;

	stringstream s;

	s <<"Diag" << number <<ends;
	string groupName = s.str();

	fileId = H5Fopen(dumpObj.getDumpFileName().c_str(), H5F_ACC_RDWR, H5P_DEFAULT);
	groupId = H5Gcreate(fileId,groupName.c_str(),0);

	rank = 1;
	dimsf[0] = 4;
	dataspaceId = H5Screate_simple(rank, dimsf, NULL);
	datasetName = "dims";
	datasetId = H5Dcreate(groupId, datasetName.c_str(), scalarType, dataspaceId,
			H5P_DEFAULT);

	data[0] = X[j1][k1].e1();
	data[1] = X[j1][k1].e2();
	data[2] = X[j2][k2].e1();
	data[3] = X[j2][k2].e2();

	status = H5Dwrite(datasetId, scalarType, H5S_ALL, H5S_ALL,
			H5P_DEFAULT, data);

	// dump save flag, history title as attribute
	// name of attribute is title of history, diagnostic
	dims = 1;
	attrdataspaceId = H5Screate_simple(1, &dims, NULL);
	attributeId = H5Acreate(datasetId, title.c_str(),H5T_NATIVE_INT, attrdataspaceId, H5P_DEFAULT);
	status = H5Awrite(attributeId,H5T_NATIVE_INT , &save);
	status = H5Aclose(attributeId);
	status = H5Sclose(attrdataspaceId);


	H5Sclose(dataspaceId);
	H5Dclose(datasetId);


	H5Gclose(groupId);
	H5Fclose(fileId);

	if (save)
		cerr << "saving diagnostic.\n";
	//    history->dump(DMPFile);



	return(1);

}
#endif

int Diag::Dump(FILE* DMPFile)
{
	Vector2 **X = region->getX();
	Scalar x,y;
	int xl,yl;  
	x = X[j1][k1].e1();
	y = X[j1][k1].e2();

	XGWrite(&x, ScalarInt, 1, DMPFile, ScalarChar);
	XGWrite(&y, ScalarInt, 1, DMPFile, ScalarChar);
	x = X[j2][k2].e1();
	y = X[j2][k2].e2();

	XGWrite(&x, ScalarInt, 1, DMPFile, ScalarChar);
	XGWrite(&y, ScalarInt, 1, DMPFile, ScalarChar);
	xl = abs(j2-j1);
	yl = abs(k2-k1);
	XGWrite(&xl,4,1,DMPFile,"int");
	XGWrite(&yl,4,1,DMPFile,"int");

	int titleLength = title.length();
	// Write the length of the title
	XGWrite(&titleLength,4,1,DMPFile,"int");
	// Write the title
	char buf[512];
	sprintf(buf, title.c_str());
	XGWrite(buf,1,titleLength+1,DMPFile,"char");

	// Write if there is a history or not 
	XGWrite(&save,4,1,DMPFile,"int");
	if (save)
		history->dump(DMPFile);
	return(TRUE);
}

int Diag::Restore_2_00(FILE* DMPFile, int j1test, int k1test, int j2test, int k2test, ostring titleTest)
{
	if ((j1 == j1test)&&(k1 == k1test)&&(j2 == j2test)&&(k2 == k2test)&&(titleTest == title))
	{
		//Diags is the same get your information from fill
		if (save)
			history->restore_2_00(DMPFile);
		return(TRUE);
	}
	else
		return(FALSE);
}


int Diag::Restore(FILE* DMPFile,Scalar _A1,Scalar _A2,Scalar _B1,Scalar _B2,
		int xl,int yl, const char *_title)
{
	if(!strcmp(title,_title)) { // this is the same diagnostic
		history->restore(DMPFile,_A1,_A2,_B1,_B2,xl,yl,A1,A2,B1,B2,j1,k1,j2,k2);
		return TRUE;
	}
	return(FALSE);
}

