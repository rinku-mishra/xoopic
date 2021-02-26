#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "newdiag.h"
#include "diagn.h"
#include "psdFieldDiag1d.h"
#include "boundary.h"
#include "grid.h"
#include "globalvars.h"

#define True 1


using namespace std;

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

PSDFieldDiag1d::PSDFieldDiag1d(SpatialRegion* SR, 
		int j1, int k1, int j2, int k2, int nfft, int HistMax,
		int Ave, int Comb, ostring VarName, ostring x1Label,
		ostring x2Label, ostring x3Label, ostring title, int save,
		ostring _windowName,
		ostring _fieldName, int const _fieldComponentLabel, int open_init, struct write_params write_data) /* throw(Oops) */
		: Diag(SR, j1, k1, j2, k2, nfft, HistMax,
				Ave, Comb, VarName, x1Label,
				x2Label, x3Label, title, 0, open_init, write_data),
				windowName(_windowName), fieldName(_fieldName),
				fieldComponentLabel(_fieldComponentLabel)
{ 
	if ( fieldName != (ostring)"E" && fieldName != (ostring)"B" ) {
		stringstream ss (stringstream::in | stringstream::out);
		ss << "PSDFieldDiag1d::PSDFieldDiag1d: Error: \n"<<
				"fieldName = " << fieldName << " passed to the constructor.\n" <<
				"fieldName must be set equal to E or B in the Diagnostic\n" <<
				"group of the input file that also includes: \n" <<
				"VarName = PSDFieldDiag1d" << endl;

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
		ss << endl << endl << endl
				<< "??????????????????????????????????????????????????????" << endl
				<< "fieldComponentLabel = " << fieldComponentLabel << " passed to " << endl
				<< "PSDFieldDiag1d::PSDFieldDiag1d(..., fieldComponentLabel, ...)" << endl
				<< "fieldComponentLabel must be set equal to 1, 2, or 3 in the Diagnostic"
				<< endl
				<< "group of the input file that also includes: " << endl
				<< "VarName = PSDFieldDiag1d" << endl
				<< "Exiting to the system..." << endl
				<< "??????????????????????????????????????????????????????"
				<< endl << endl << endl;
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
	xsize = region->getJ();
	ysize = region->getK();
	grid = region->get_grid();
	// number of data elements for the real FFT in 1d:
	numElementsRFFT1d = xsize;
	// number of data elements for each 1D PSD calculation:
	num_psd1d = numElementsRFFT1d/2 + 1;

	x1_arrayLength = ysize;
	x1_array = new Scalar[x1_arrayLength];
	for( k = 0; k < x1_arrayLength; k++)
		x1_array[k] = grid->getX()[0][k].e2();

	x2_arrayLength = num_psd1d;
	x2_array = new Scalar[x2_arrayLength];
	Scalar intervalLength = (grid->getX()[1][0].e1()) - (grid->getX()[0][0].e1());
	intervalLength = 2.*M_PI/(xsize*intervalLength);
	x2_array[0] = 0.0;
	for ( j = 1; j < x2_arrayLength; j++ )
		x2_array[j] = x2_array[j-1] + intervalLength;
	//
	// allocate memory to store the PSD values
	//
	psd1d = new Scalar* [ysize];
	for ( j = 0; j < ysize; j++ ) {
		psd1d[j] = new Scalar [num_psd1d];
		for ( k = 0; k < num_psd1d; k++ ) {
			psd1d[j][k] = 0.0;
		}
	}

	//
	// create a plan for a 1d real to complex fftw transform of
	// data with numElementsFFT1DR number of elements.
	//
	try{
		pRFFT1d = new WrapFFTW(1, &numElementsRFFT1d, 1, 1);
	}
	catch(Oops& oops2){
		oops2.prepend("PSDFieldDiag1d::PSDFieldDiag1d: Error: \n");//done
		throw oops2;
	}

	ptrPowerSpectrum1d = new PowerSpectrum1d(numElementsRFFT1d,
			pRFFT1d, (string)windowName, true);
	inData = new Scalar[numElementsRFFT1d];

	//
	// initialize the variables needed for the diagnostics of the chirp
	//
	flagChirp = true;
	if(flagChirp) {
		numDataPointsChirp = region->getJ();
		numTransformPointsChirp = 64;
		numSkipPointsChirp = 16;
		numIntervalsChirp = (numDataPointsChirp - numTransformPointsChirp)/
				numSkipPointsChirp;
		numPSDChirp = numTransformPointsChirp/2 + 1;
		x1LengthChirp = numIntervalsChirp;
		x1Chirp = new Scalar[x1LengthChirp];
		Scalar tmpDelta;
		tmpDelta = 0.5*(grid->getX()[numTransformPointsChirp-1][0].e1() -
				grid->getX()[0][0].e1());
		for ( int i = 0; i < x1LengthChirp; i++ ) {
			x1Chirp[i] = tmpDelta +
					grid->getX()[i*numSkipPointsChirp][0].e1();
		}
		x2LengthChirp = numPSDChirp;
		x2Chirp = new Scalar[x2LengthChirp];
		Scalar samplingFrequencyChirp = 1./
				(2.*(grid->getX()[1][0].e1()-grid->getX()[0][0].e1()));
		Scalar tmpVar = 4.*M_PI*samplingFrequencyChirp/
				static_cast<Scalar>(numTransformPointsChirp);
		for (int i = 0; i < x2LengthChirp; i++ )
			x2Chirp[i] = 2.9979e8*tmpVar*static_cast<Scalar>(i);
		psdChirp = new Scalar*[x1LengthChirp];
		for ( int i = 0; i < x1LengthChirp; i++ ) {
			psdChirp[i] = new Scalar[x2LengthChirp];
			for ( j = 0; j < x2LengthChirp; j++ )
				psdChirp[i][j] = 0.;
		}
		try{
			ptrFFT1dChirp = new WrapFFTW(1, &numTransformPointsChirp, 1, 1);
		}
		catch(Oops& oops2){
			oops2.prepend("PSDFieldDiag1d::PSDFieldDiag1d: Error: \n");//OK
			throw oops2;
		}
		ptrPowerSpectrum1dChirp =
				new PowerSpectrum1d(numTransformPointsChirp,
						ptrFFT1dChirp, (string)windowName, true);
		inDataChirp = new Scalar[numTransformPointsChirp];

		x1LabelChirp="x";
		x2LabelChirp="Angular Frequency";
		subTitleChirp=" Chirp Diagnostic";
	}
}

PSDFieldDiag1d::~PSDFieldDiag1d()
{
	int j;
	if (x1_array)
		delete [] x1_array;
	if (x2_array)
		delete [] x2_array;
	if (pRFFT1d)
		delete pRFFT1d;
	if (ptrPowerSpectrum1d)
		delete ptrPowerSpectrum1d;
	if (inData)
		delete [] inData;
	if ( psd1d  ) {
		for ( j = 0; j < ysize; j++ )
			delete [] psd1d[j];
		delete [] psd1d;
	}
	//
	// delete the memory associated with the diagnostics of the chirp
	//
	if(flagChirp) {
		if(x1Chirp)
			delete [] x1Chirp;
		if(x2Chirp)
			delete [] x2Chirp;
		if(psdChirp) {
			for ( j = 0; j < x1LengthChirp; j++ )
				delete [] psdChirp[j];
			delete [] psdChirp;
		}
		if(ptrFFT1dChirp)
			delete ptrFFT1dChirp;
		if(ptrPowerSpectrum1dChirp)
			delete ptrPowerSpectrum1dChirp;
		if(inDataChirp)
			delete [] inDataChirp;
	}
}

// added "n", JK 2013-07-29
void PSDFieldDiag1d::MaintainDiag(Scalar t, int n)
{
	int n1, n2;
	for ( n1 = 0; n1 < x1_arrayLength; n1++ ) {
		for ( n2 = 0; n2 < numElementsRFFT1d; n2++ ) {
			inData[n2] = fieldHandler[n2+1][n1+1].e(fieldComponentLabel);
		}
		ptrPowerSpectrum1d->calc_psd1d(inData, psd1d[n1]);
	}
	//
	// the following is code for maintaining the chirp laser pulse diagnostics
	//
	if(flagChirp) {
		int indexYHalf = ysize/2 - 1;
		for ( n1 = 0; n1 < numIntervalsChirp; n1++ ) {
			for ( n2 = 0; n2 < numTransformPointsChirp; n2++) {
				inDataChirp[n2] =
						fieldHandler[n1*numSkipPointsChirp+n2][indexYHalf].e(fieldComponentLabel);
			}
			ptrPowerSpectrum1dChirp->calc_psd1d(inDataChirp, psdChirp[n1]);
		}
	}
}

void PSDFieldDiag1d::initwin(ostring prepend)
{
	// JK, 2013-08-27
	defineParameters(x1Label, x2Label, x1_array, x2_array, psd1d, &x1_arrayLength, &x2_arrayLength);
#ifndef NOX
	XGSet3D("linlinlin", x1Label.c_str(), x2Label.c_str(),
			strdup((prepend + fieldLabel + " "  + title).c_str()),
			45,225,"closed",1,1,
			1.0,1.0,1.0,0,0,1,x1_array[0],x1_array[x1_arrayLength-1],
			x2_array[0],x2_array[x2_arrayLength-1],0,1.0);
	XGSurf(x1_array, x2_array, psd1d, &x1_arrayLength, &x2_arrayLength, 1);
#endif // NOX

	if(flagChirp) {
		// JK, 2013-08-27
		defineParameters(x1LabelChirp, x2LabelChirp, x1Chirp, x2Chirp, psdChirp, &x1LengthChirp, &x2LengthChirp);
#ifndef NOX
		XGSet3D("linlinlin", x1LabelChirp.c_str(), x2LabelChirp.c_str(),
				strdup((prepend + fieldLabel + " "  + title + subTitleChirp).c_str()),
				45,225,"closed",1,1,
				1.0,1.0,1.0,0,0,1,x1Chirp[0],x1Chirp[x1LengthChirp-1],
				x2Chirp[0],x2Chirp[x2LengthChirp-1],0,1.0);
		XGSurf(x1Chirp, x2Chirp, psdChirp, &x1LengthChirp, &x2LengthChirp, 1);
#endif // NOX
	}
}


#ifdef HAVE_HDF5
void PSDFieldDiag1d::dumpDiagH5(dumpHDF5 *dumpObj)
{


	string datasetName;
	int rank = 3;
	int size[3] = {x1_arrayLength,x2_arrayLength,1};
	Scalar *data = new Scalar[x1_arrayLength*x2_arrayLength];

	// on grid so don't output x1array, x2array
	ostring nn = fieldComponentLabel;

	datasetName = "/psd1d" + fieldLabel.str();
	for(int j=0;j<x1_arrayLength;j++)
		for(int k=0;k<x2_arrayLength;k++)
			data[j*(x2_arrayLength) + k] = psd1d[j][k];
	dumpObj->writeSimple(datasetName,data,rank,size);

	// output grid points for psd
	datasetName = "/psd1d" + fieldLabel.str() + "x1array";
	rank = 1;
	size[0] = x1_arrayLength;
	dumpObj->writeSimple(datasetName,x1_array,rank,size);

	datasetName = "/psd1d" + fieldLabel.str() + "x2array";
	size[0] = x2_arrayLength;
	dumpObj->writeSimple(datasetName,x2_array,rank,size);



	if(flagChirp) {
		delete[] data;
		rank = 3;
		data = new Scalar[x1LengthChirp*x2LengthChirp];

		size[0] = x1LengthChirp; size[1] = x2LengthChirp;size[2] = 1;
		datasetName = "psdChirp" + fieldLabel.str();
		for(int j=0;j<x1LengthChirp;j++)
			for(int k=0;k<x2LengthChirp;k++)
				data[j*(x2LengthChirp) + k] = psdChirp[j][k];
		dumpObj->writeSimple(datasetName,data,rank,size);

		// output grid points for Chirp
		// This is done because the chirp data is not on the standard grid.
		// h5Merge will only use these special grid points if they are put
		// in a the datasets <diagName>x1array and <diagName>x2array

		datasetName = "/psdChirp" + fieldLabel.str() + "x1array";
		rank = 1;
		size[0] = x1LengthChirp;
		dumpObj->writeSimple(datasetName,x1Chirp,rank,size);

		datasetName = "/psdChirp" + fieldLabel.str() + "x2array";
		size[0] = x2LengthChirp;
		dumpObj->writeSimple(datasetName,x2Chirp,rank,size);

	}

	delete[] data;
	return;
}
#endif //HAVE_HDF5

#endif // HAVE_FFT
