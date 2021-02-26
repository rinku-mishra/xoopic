#ifndef	__WAVESPLITDIAG_H
#define	__WAVESPLITDIAG_H

/*
====================================================================

wavesplitdiag.H

Laser spot measuring diagnostic.


.99
====================================================================
 */

#include "sptlrgn.h"
#include "newdiag.h"

#ifdef UNIX
#include <config.h>
#endif

#ifdef HAVE_FFT 
#include "wrapFFTW.h"
#include "powerspectrum1d.h"
#endif 


class GUIConfig;

//--------------------------------------------------------------------
//	Diag:  This class defines the generic boundary object for
//	some region.  All other boundaries inherit from this pure virtual.

class History;
class Diagnostics;

class WaveDirDiag : public Diag
{
private:
	ostring polarizationEB;
	int Ecomponent;
	int Bcomponent;
	Scalar signRdata;
	Scalar signLdata;

	// coefficients for the quadratic Lagrange interpolation:
	Scalar const aj;
	Scalar const ajp1;
	Scalar const ajp2;

	int const psd1dFlag;
	ostring windowName;

#ifdef HAVE_FFT 
	//
	// data members related to the calculation of the 1D PSDs along x
	// for each value of the y index, and axes array values
	//
	int numElementsRFFT1d; // number of data elements for the real FFT in 1d
	int num_psd1d; // number of data elements for each 1D PSD calculation

	ostring K1; // kx axis label

	Scalar **Rdata_psd1d;
	Scalar **Ldata_psd1d;
	Scalar* inData; // working array
	//
	// we need only values for the wave vectors, the space positions
	// are already available via the x1_array and x2_array arrays:
	//
	Scalar* x2_psd1d;

	BaseFFT* pRFFT1d; // for 1D FFT transforms of real data
	PowerSpectrum1d* ptrPowerSpectrum1d;

	//
	// a helper function to manage the calculations of the
	// the 1d psd diagnostics for all values of the y index
	//
	void get_psd1d_Diagnostics(Scalar** in, Scalar** out);

#endif // HAVE_FFT

public:
	Scalar **Rdata;
	Scalar **Ldata;
	Scalar *x1_array;
	Scalar *x2_array;
	int xsize,ysize;
	Grid *grid;
	WaveDirDiag(SpatialRegion* SR,
			int j1, int k1, int j2, int k2, int nfft, int HistMax,
			int Ave, int Comb, ostring VarName, ostring x1Label,
			ostring x2Label, ostring x3Label, ostring title, int save,
			ostring _polarizationEB, int _psd1dFlag, ostring _windowName, int open_init, struct write_params write_data);


	virtual ~WaveDirDiag();
#ifdef HAVE_HDF5
	void dumpDiagH5(dumpHDF5 *dumpObj);
#endif
	virtual int Restore_2_00(FILE *DMPFile, int j1, int k1,
			int j2, int k2, ostring title) {return 0;};
	virtual int Restore(FILE *DMPFile,Scalar A1,Scalar A2,Scalar B1,Scalar B2,
			int xl,int yl, char *title){ return 1;} ;
	virtual void MaintainDiag(Scalar t, int n);			// added "n", JK 2013-07-29
	void setDiagnostics(Diagnostics *_theDiagnostics) { theDiagnostics=_theDiagnostics;};
	void initwin() { initwin(ostring("")); };
	virtual void initwin(ostring prepend);
	virtual void initwin(GUIConfig* gui, int itemNumber) {}
	int get_j1() { return j1;};
	int get_j2() { return j2;};
	int get_k1() { return k1;};
	int get_k2() { return k2;};
};

#endif	//	ifndef __WAVESPLITDIAG_H



