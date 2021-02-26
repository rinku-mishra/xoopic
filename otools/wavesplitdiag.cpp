/*
	 ====================================================================

	 WAVESPLITDIAG.CPP

	 1.01	(KC, 9-21-95) After release. First draft.
	 1.02	(JohnV, 10-27-95) def'ed out xgrafix include for PC version
     1.03	(JK, 2013-08-27) Added writing diagnostics to file

	 ====================================================================
 */

#include "newdiag.h"
#include "diagn.h"
#include "wavesplitdiag.h"
#include "boundary.h"
#include "grid.h"
#include "globalvars.h"
#include "diag2file.h"
#define True 1

extern "C++" void printf(char *);
extern "C" {
#ifdef NOX
#include <xgmini.h>
#else
#include <xgdefs.h>
#endif //NOX
}

//--------------------------------------------------------------------

WaveDirDiag::WaveDirDiag(SpatialRegion* SR, 
		int j1, int k1, int j2, int k2, int nfft, int HistMax,
		int Ave, int Comb, ostring VarName, ostring x1Label,
		ostring x2Label, ostring x3Label, ostring title, int save,
		ostring _polarizationEB, int _psd1dFlag,
		ostring _windowName, int open_init, struct write_params write_data)
:Diag(SR, j1, k1, j2, k2, nfft, HistMax, 
		Ave, Comb, VarName, x1Label,
		x2Label, x3Label, title, 0, open_init, write_data), aj(3./8.), ajp1(3./4.), ajp2(-1./8.),
		psd1dFlag(_psd1dFlag), windowName(_windowName)
{ 
	int j,k;
	xsize = region->getJ();
	ysize = region->getK();
	polarizationEB = _polarizationEB;
	Ldata = new Scalar *[xsize];
	Rdata = new Scalar *[xsize];
	x1_array = new Scalar [xsize];
	x2_array = new Scalar [ysize];
	for(j=0;j<xsize;j++) {
		Ldata[j] = new Scalar [ysize];
		Rdata[j] = new Scalar [ysize];
		x1_array[j]=region->get_grid()->getX()[j][0].e1();
		for(k=0;k<ysize;k++)
			Rdata[j][k]=Ldata[j][k]=0;
	}
	for(k=0;k<ysize;k++)
		x2_array[k]=region->get_grid()->getX()[0][k].e2();

	if ( polarizationEB == (ostring)"EyBz" ) {
		Ecomponent = 2;
		Bcomponent = 3;
		signRdata = 1.;
		signLdata = -1.;
	} else if ( polarizationEB == (ostring)"EzBy" ) {
		Ecomponent = 3;
		Bcomponent = 2;
		signRdata = -1.;
		signLdata = 1.;
	}

#ifdef HAVE_FFT 
	x2_psd1d = 0;
	Rdata_psd1d = 0;
	Ldata_psd1d = 0;
	inData = 0;
	pRFFT1d = 0;
	ptrPowerSpectrum1d = 0;
	if ( psd1dFlag ) {
		// number of data elements for the real FFT in 1d:
		numElementsRFFT1d = xsize;
		// number of data elements for each 1D PSD calculation:
		num_psd1d = numElementsRFFT1d/2 + 1;
		//
		// create a plan for a 1d real to complex fftw transform of
		// data with numElementsFFT1DR number of elements.
		//
		try{
			pRFFT1d = new WrapFFTW(1, &numElementsRFFT1d, 1, 1);
		}
		catch(Oops& oops2){
			oops2.prepend("PSDFieldDiag2d::PSDFieldDiag2d: Error: \n");//OK
			throw oops2;
		}

		///ptrPowerSpectrum1d = new PowerSpectrum1d(numElementsRFFT1d,
		///                                         pRFFT1d, "None", true);
		ptrPowerSpectrum1d = new PowerSpectrum1d(numElementsRFFT1d,
				pRFFT1d, (string)windowName, true);

		K1 = "kx"; // kx axis label
		//
		// allocate memory for the  arrays for storage of the 1D PSDs along x
		// for each value of the y index, and the x-axis array values
		//
		x2_psd1d = new Scalar[num_psd1d];
		//
		// initialize x2_psd1d[] with the first numPSD_1D values of kx
		//
		Scalar intervalLength = x1_array[1]-x1_array[0];
		intervalLength = 2.*M_PI/(xsize*intervalLength);
		x2_psd1d[0] = 0.0;
		for ( j = 1; j < num_psd1d; j++ )
			x2_psd1d[j] = x2_psd1d[j-1] + intervalLength;

		//
		// allocate memory for the psd's
		//
		inData = new Scalar[numElementsRFFT1d];
		Rdata_psd1d = new Scalar* [ysize];
		Ldata_psd1d = new Scalar* [ysize];
		for ( j = 0; j < ysize; j++ ) {
			Rdata_psd1d[j] = new Scalar [num_psd1d];
			Ldata_psd1d[j] = new Scalar [num_psd1d];
			for ( k = 0; k < num_psd1d; k++ ) {
				Rdata_psd1d[j][k] = 0.0;
				Ldata_psd1d[j][k] = 0.0;
			}
		}
	}
#endif // HAVE_FFT
}

WaveDirDiag::~WaveDirDiag()
{
	int j;
	for(j=0;j<xsize;j++) {
		delete [] Ldata[j];
		delete [] Rdata[j];
	}
	delete [] Ldata;
	delete [] Rdata;
	delete[] x1_array;
	delete[] x2_array;
	Ldata=Rdata=0;

#ifdef HAVE_FFT   
	if ( x2_psd1d )
		delete [] x2_psd1d;
	if (inData)
		delete [] inData;
	if (Rdata_psd1d) {
		for ( j = 0; j < ysize; j++ )
			delete [] Rdata_psd1d[j];
		delete [] Rdata_psd1d;
	}
	if (Ldata_psd1d) {
		for ( j = 0; j < ysize; j++ )
			delete [] Ldata_psd1d[j];
		delete [] Ldata_psd1d;
	}
	if (pRFFT1d)
		delete pRFFT1d;
	if (ptrPowerSpectrum1d)
		delete ptrPowerSpectrum1d;
#endif // HAVE_FFT
}

// added "n", JK 2013-07-29
void WaveDirDiag::MaintainDiag(Scalar t, int n)
{
	int j,k;
	Grid *grid = region->get_grid();
	Vector3 **E = region->getIntEdl();
	Vector3 **B = region->getIntBdS();
	/*  Vector3 **E = region->getENode();
      Vector3 **B = region->getBNode();  */
	//
	// components of the E and B fields for which to
	// calculate the linear compbinations, alpha has
	// the value given by the Ecomponent label and
	// beta the one given by the Bcomponent.
	//
	Scalar Ealpha;
	Scalar Bbeta;

	double n0 = SPEED_OF_LIGHT;

	for(j=1;j<xsize-3;j++)
		for(k=0;k<ysize;k++) {
			//
			// we use the fact that the moving window requires a uniform
			// grid spacing and we make the same assumption here and use
			// the second order Lagrange interpolation with equal distance
			// between the points.
			//
			Ealpha =
					aj*E[j][k].e(Ecomponent) +
					ajp1*E[j+1][k].e(Ecomponent) +
					ajp2*E[j+2][k].e(Ecomponent);

			Ealpha /= (grid->dl(j,k)).e(Ecomponent);
			Bbeta = B[j][k].e(Bcomponent)/(grid->dS(j,k)).e(Bcomponent);

			Rdata[j][k] = 0.5 *(Ealpha + signRdata * n0 * Bbeta);
			Ldata[j][k] = 0.5 *(Ealpha + signLdata * n0 * Bbeta);
		}
#ifdef HAVE_FFT  
	if ( psd1dFlag ) {
		get_psd1d_Diagnostics(Rdata, Rdata_psd1d);
		get_psd1d_Diagnostics(Ldata, Ldata_psd1d);
	}
#endif // HAVE_FFT
}

void WaveDirDiag::initwin(ostring prepend)
{
	// JK, 2013-08-27
	defineParameters(x1Label, x2Label, x1_array, x2_array, Ldata, &xsize, &ysize);
#ifndef NOX
	XGSet3D("linlinlin",x1Label.c_str(),x2Label.c_str(),
			strdup((prepend + title + ostring("Left")).c_str()),
			45,225,win_init_stat,1,1,
			1.0,1.0,1.0,0,0,1,x1_array[0],x1_array[xsize-1],
			x2_array[0],x2_array[ysize-1],0,1.0);
	XGSurf( x1_array,x2_array,Ldata, &xsize, &ysize, 1 );
#endif // NOX

	// JK, 2013-08-27
	defineParameters(x1Label, x2Label, x1_array, x2_array, Rdata, &xsize, &ysize);
#ifndef NOX
	XGSet3D("linlinlin",x1Label.c_str(),x2Label.c_str(),
			strdup((prepend + title + ostring("Right")).c_str()),
			45,225,win_init_stat,1,1,
			1.0,1.0,1.0,0,0,1,x1_array[0],x1_array[xsize-1],
			x2_array[0],x2_array[ysize-1],0,1.0);
	XGSurf( x1_array,x2_array,Rdata, &xsize, &ysize, 1 );
#endif // NOX

#ifdef HAVE_FFT 
	if ( psd1dFlag ) {
		//
		// note that the x1 array is given by the y values
		// and the x2 array is given by the wave vector values
		// along x
		//
		// JK, 2013-08-27
		// not implemented yet
		// defineParameters(x2Label, K1, x2_array, x2_psd1d, Ldata_psd1d, &ysize, &num_psd1d);
#ifndef NOX
		XGSet3D("linlinlin",x2Label.c_str(),K1,
				strdup((prepend + title + ostring("Left 1d PSD")).c_str()),
				45,225,win_init_stat,1,1,
				1.0,1.0,1.0,0,0,1,x2_array[0],x2_array[ysize-1],
				x2_psd1d[0],x2_psd1d[num_psd1d-1],0,1.0);
		XGSurf( x2_array,x2_psd1d,Ldata_psd1d, &ysize, &num_psd1d, 1 );
#endif // NOX

		// JK, 2013-08-27
		// not implemented yet
		// defineParameters(x2Label, K1, x2_array, x2_array, x2_psd1d, Rdata_psd1d, &ysize, &num_psd1d);
#ifndef NOX
		XGSet3D("linlinlin",x2Label.c_str(),K1,
				strdup((prepend + title + ostring("Right 1d PSD")).c_str()),
				45,225,win_init_stat,1,1,
				1.0,1.0,1.0,0,0,1,x2_array[0],x2_array[ysize-1],
				x2_psd1d[0],x2_psd1d[num_psd1d-1],0,1.0);
		XGSurf( x2_array,x2_psd1d,Rdata_psd1d, &ysize, &num_psd1d, 1 );
#endif // NOX
	}
#endif // HAVE_FFT
}

#ifdef HAVE_FFT 
void WaveDirDiag::get_psd1d_Diagnostics(Scalar** in, Scalar** out) {
	int n1, n2;
	for ( n1 = 0; n1 < ysize; n1++ ) {
		for ( n2 = 0; n2 < numElementsRFFT1d; n2++ ) {
			inData[n2] = in[n2][n1];
		}
		ptrPowerSpectrum1d->calc_psd1d(inData, out[n1]);
	}
}
#endif // HAVE_FFT

#ifdef HAVE_HDF5
void WaveDirDiag::dumpDiagH5(dumpHDF5 *dumpObj)
{

	//   hid_t   datasetId;dataspaceId;
	//   herr_t status;
	string datasetName;
	string polEB = polarizationEB.str();
	int rank = 3;
	int size[3] = {xsize,ysize,1};
	Scalar *data = new Scalar[xsize*ysize];

	// on grid so don't output x1array, x2array

	datasetName = "waveEnergyLeft" +  polEB;
	for(int j=0;j<xsize;j++)
		for(int k=0;k<ysize;k++)
			data[j*(ysize) + k] = Ldata[j][k];
	dumpObj->writeSimple(datasetName,data,rank,size);


	datasetName = "waveEnergyRight" +  polEB;
	for(int j=0;j<xsize;j++)
		for(int k=0;k<ysize;k++)
			data[j*(ysize) + k] = Rdata[j][k];
	dumpObj->writeSimple(datasetName,data,rank,size);

	// output grid points
	rank = 1;
	size[0] = xsize;
	dumpObj->writeSimple(("/waveEnergyRight" +  polEB + "x1array"),x1_array,rank,size);
	dumpObj->writeSimple(("/waveEnergyLeft" +  polEB + "x1array"),x1_array,rank,size);

	size[0] = ysize;
	dumpObj->writeSimple(("/waveEnergyRight" +  polEB + "x2array"),x2_array,rank,size);
	dumpObj->writeSimple(("/waveEnergyLeft" +  polEB + "x2array"),x2_array,rank,size);


	// dump PSD data
#ifdef HAVE_FFT 

	if ( psd1dFlag ) {
		delete[] data;
		Scalar *data = new Scalar[ysize*num_psd1d];
		size[0] = ysize; size[1] = num_psd1d; size[2] = 1;
		rank=3;
		datasetName = "/waveEnergyLeft"  +  polEB + "psd1d";
		for(int j=0;j<ysize;j++)
			for(int k=0;k<num_psd1d;k++)
				data[j*(num_psd1d) + k] = Ldata_psd1d[j][k];
		dumpObj->writeSimple(datasetName,data,rank,size);

		datasetName = "/waveEnergyRight" +  polEB+ "psd1d";
		for(int j=0;j<ysize;j++)
			for(int k=0;k<num_psd1d;k++)
				data[j*(num_psd1d) + k] = Rdata_psd1d[j][k];
		dumpObj->writeSimple(datasetName,data,rank,size);

		// output grid points
		rank = 1;
		size[0] = ysize;
		dumpObj->writeSimple(("/waveEnergyLeft" +  polEB+ "psd1dx1array"),x2_array,rank,size);
		dumpObj->writeSimple(("/waveEnergyRight"+  polEB+ "psd1dx1array"),x2_array,rank,size);

		size[0] = num_psd1d;
		dumpObj->writeSimple(("/waveEnergyLeft" +  polEB+ "psd1dx2array"),x2_psd1d,rank,size);
		dumpObj->writeSimple(("/waveEnergyRight" +  polEB+ "psd1dx2array"),x2_psd1d,rank,size);

	}
#endif //HAVE_FFT 
	delete[] data;
	return;
}
#endif
