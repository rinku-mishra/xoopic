
#include "newdiag.h"
#include "diagn.h"
#include "psdFieldDiag2d.h"
#include "boundary.h"
#include "grid.h"
#include "globalvars.h"
#define True 1
//#include <cstdio>

extern "C++" void printf(char *);
extern "C" {
#ifdef NOX
#include <xgmini.h>
#else
#include <xgdefs.h>
#endif //NOX
}

#ifdef HAVE_FFT 
//--------------------------------------------------------------------

PSDFieldDiag2d::PSDFieldDiag2d(SpatialRegion* SR, 
		int j1, int k1, int j2, int k2, int nfft, int HistMax,
		int Ave, int Comb, ostring VarName, ostring x1Label,
		ostring x2Label, ostring x3Label, ostring title,
		int save, ostring _fieldName,
		int const _fieldComponentLabel, int open_init, struct write_params write_data) /* throw(Oops) */
		: Diag(SR, j1, k1, j2, k2, nfft, HistMax,
				Ave, Comb, VarName, x1Label,
				x2Label, x3Label, title, 0, open_init, write_data),
				fieldName(_fieldName),
				fieldComponentLabel(_fieldComponentLabel)
{ 
	if ( fieldName != (ostring)"E" && fieldName != (ostring)"B" ) {
		stringstream ss (stringstream::in | stringstream::out);
		ss <<"PSDFieldDiag2d::PSDFieldDiag2d: Error: \n"<<
				"fieldName = " << fieldName << " passed in. \n" <<
				"fieldName must be set equal to E or B in the Diagnostic\n" <<
				"group of the input file that also includes: \n" <<
				"VarName = PSDFieldDiag2d" << endl;

		std::string msg;
		ss >> msg;
		Oops oops(msg);
		throw oops;    // exit() DiagParams::CreateCounterPart:

	}
	if ( fieldName == (ostring)"E" )
		fieldHandler = region->getENode();
	if ( fieldName == (ostring)"B" )
		fieldHandler = region->getBNode();

	if ( fieldComponentLabel != 1 &&
			fieldComponentLabel != 2 &&
			fieldComponentLabel != 3 ) {
		stringstream ss (stringstream::in | stringstream::out);
		ss << "PSDFieldDiag2d::PSDFieldDiag2d: Error: \n"<<
				"fieldComponentLabel = " << fieldComponentLabel << " passed in. \n" <<
				"fieldComponentLabel must be set equal to 1, 2, or 3 in the Diagnostic\n" <<
				"group of the input file that also includes: \n" <<
				"VarName = PSDFieldDiag2d" << endl;

		std::string msg;
		ss >> msg;
		Oops oops(msg);
		throw oops;    // exit() DiagParams::CreateCounterPart:

	}

	if ( fieldComponentLabel == 1 )
		fieldLabel = fieldName+(ostring)"x";
	else if ( fieldComponentLabel == 2 )
		fieldLabel = fieldName+(ostring)"y";
	else
		fieldLabel = fieldName+(ostring)"z";

	int j,k;

	// number of data elements for the real FFT in 2d:
	numElementsRFFT[0] = region->getJ();
	numElementsRFFT[1] = region->getK();
	psd2dNormalizationFactor = 1./((static_cast<Scalar>(numElementsRFFT[0]))*
			(static_cast<Scalar>(numElementsRFFT[1])));

	grid = region->get_grid();
	//
	// create a plan for a 2d real to complex fftw transform
	//
	try{
		ptrFFT = new WrapFFTW(2, numElementsRFFT, 1, 1);
	}
	catch(Oops& oops2){
		oops2.prepend("PSDFieldDiag2d::PSDFieldDiag2d: Error: \n");//done
		throw oops2;
	}
	// number of data elements for the PSD calculation:
	num_psd2d[0] = numElementsRFFT[0];
	num_psd2d[1] = numElementsRFFT[1]/2 + 1;

	x1_arrayLength = num_psd2d[0];
	x1_array = new Scalar[x1_arrayLength];
	Scalar intervalLength = (grid->getX()[1][0].e1()) - (grid->getX()[0][0].e1());
	intervalLength = 2.*M_PI/(numElementsRFFT[0]*intervalLength);
	x1_array[0] = 0.0;
	for( k = 0; k < x1_arrayLength; k++)
		x1_array[k] = x1_array[k-1] + intervalLength;

	x2_arrayLength = num_psd2d[1];
	x2_array = new Scalar[x2_arrayLength];
	intervalLength = (grid->getX()[0][1].e2()) - (grid->getX()[0][0].e2());
	intervalLength = 2.*M_PI/(numElementsRFFT[1]*intervalLength);
	x2_array[0] = 0.0;
	for ( j = 1; j < x2_arrayLength; j++ )
		x2_array[j] = x2_array[j-1] + intervalLength;
	//
	// allocate memory to store the PSD values
	//
	psd2d = new Scalar* [num_psd2d[0]];
	for ( j = 0; j < num_psd2d[0]; j++ ) {
		psd2d[j] = new Scalar [num_psd2d[1]];
		for ( k = 0; k < num_psd2d[1]; k++ ) {
			psd2d[j][k] = 0.0;
		}
	}

	reFFTField = new Scalar* [numElementsRFFT[0]];
	for ( j = 0; j < numElementsRFFT[0]; j++ ) {
		reFFTField[j] = new Scalar [numElementsRFFT[1]];
		for ( k = 0; k < numElementsRFFT[1]; k++ )
			reFFTField[j][k] = 0.0;
	}
	imFFTField = new Scalar* [numElementsRFFT[0]];
	for ( j = 0; j < numElementsRFFT[0]; j++ ) {
		imFFTField[j] = new Scalar [numElementsRFFT[1]];
		for ( k = 0; k < numElementsRFFT[1]; k++ )
			imFFTField[j][k] = 0.0;
	}
}

PSDFieldDiag2d::~PSDFieldDiag2d()
{
	int j;
	if (x1_array)
		delete [] x1_array;
	if (x2_array)
		delete [] x2_array;
	if (ptrFFT)
		delete ptrFFT;
	if ( psd2d  ) {
		for ( j = 0; j < num_psd2d[0]; j++ )
			delete [] psd2d[j];
		delete [] psd2d;
	}
	if ( reFFTField  ) {
		for ( j = 0; j < numElementsRFFT[0]; j++ )
			delete [] reFFTField[j];
		delete [] reFFTField;
	}
	if ( imFFTField  ) {
		for ( j = 0; j < numElementsRFFT[0]; j++ )
			delete [] imFFTField[j];
		delete [] imFFTField;
	}
}

// added "n", JK 2013-07-29
void PSDFieldDiag2d::MaintainDiag(Scalar t, int n)
/* throw(Oops) */{
	int j, k;
	Scalar Re, Im;

	try {
		ptrFFT->doFFT(fieldHandler, fieldComponentLabel, reFFTField, imFFTField);
	}
	catch(Oops& oops){
		oops.prepend("PSDFieldDiag2d::MaintainDiag: Error:\n");
		throw oops;
	}

	//
	// calcluate the power spectral density (2d), no windowing
	//
	for ( j = 0; j < num_psd2d[0]; j++ ) {
		for ( k = 0; k < num_psd2d[1]; k++ ) {
			Re = reFFTField[j][k];
			Im = imFFTField[j][k];
			psd2d[j][k] = psd2dNormalizationFactor*(Re*Re + Im*Im);
		}
	}
}

void PSDFieldDiag2d::initwin(ostring prepend)
{
	// JK, 2013-08-27
	defineParameters(x1Label, x2Label, x1_array, x2_array, psd2d, &x1_arrayLength, &x2_arrayLength);
#ifndef NOX
	XGSet3D("linlinlin", x1Label.c_str(), x2Label.c_str(),
			strdup((prepend + fieldLabel +" "+ title).c_str()),
			45,225,"closed",1,1,
			1.0,1.0,1.0,0,0,1,x1_array[0],x1_array[x1_arrayLength-1],
			x2_array[0],x2_array[x2_arrayLength-1],0,1.0);
	XGSurf(x1_array, x2_array, psd2d, &x1_arrayLength, &x2_arrayLength, 1);
#endif // NOX
}


#ifdef HAVE_HDF5
void PSDFieldDiag2d::dumpDiagH5(dumpHDF5 *dumpObj)
{

	string datasetName;
	int rank = 3;
	int size[3] = {x1_arrayLength,x2_arrayLength,1};
	Scalar *data = new Scalar[x1_arrayLength*x2_arrayLength];

	// on grid so don't output x1array, x2array
	ostring nn = fieldComponentLabel;

	datasetName = "/psd2d" + fieldLabel.str();
	for(int j=0;j<x1_arrayLength;j++)
		for(int k=0;k<x2_arrayLength;k++)
			data[j*(x2_arrayLength) + k] = psd2d[j][k];
	dumpObj->writeSimple(datasetName,data,rank,size);

	// output grid points for psd
	datasetName = "/psd2d" + fieldLabel.str() + "x1array";
	rank = 1;
	size[0] = x1_arrayLength;
	dumpObj->writeSimple(datasetName,x1_array,rank,size);

	datasetName = "/psd2d" + fieldLabel.str() + "x2array";
	size[0] = x2_arrayLength;
	dumpObj->writeSimple(datasetName,x2_array,rank,size);


	delete[] data;
	return;
}
#endif //HAVE_HDF5

#endif // HAVE_FFT

