#ifndef	__PSDFIELDDIAG1D_H
#define	__PSDFIELDDIAG1D_H

/**
 * The PSD1DFieldDiag class is responsible for the 1d 
 * power spectral density calculation of a filed component for
 * the E or B fields. 
 *
 * dad, started on 08/16/01.
 * 
 * Version: $Id: psdFieldDiag1d.h 2198 2004-09-01 21:24:31Z yew $
 * 
 * Copyright 2001 by Tech-X Corporation. 
 */

#include "sptlrgn.h"
#include "newdiag.h"
#include "diag2file.h"

#include <oops.h>


#ifdef UNIX
#include <config.h>
#endif

#ifdef HAVE_FFT 
#include "wrapFFTW.h"
#include "powerspectrum1d.h"


class GUIConfig;

//--------------------------------------------------------------------
//	Diag:  This class defines the generic boundary object for
//	some region.  All other boundaries inherit from this pure virtual.

class History;
class Diagnostics;

class PSDFieldDiag1d : public Diag
{
private:
	string windowName;
	int xsize,ysize;
	Grid *grid;
	//
	// data members related to the calculation of the 1D PSDs along x
	// for each value of the y index, and axes array values
	//
	int numElementsRFFT1d; // number of data elements for the real FFT in 1d
	int num_psd1d; // number of data elements for each 1D PSD calculation
	ostring fieldName; // "E" or "B"
	int const fieldComponentLabel; // for which component of the field
	Vector3** fieldHandler;

	ostring fieldLabel;
	Scalar** psd1d; // values of the 1d psd

	Scalar* inData; // working array

	Scalar *x1_array;
	int x1_arrayLength;
	Scalar *x2_array;
	int x2_arrayLength;

	BaseFFT* pRFFT1d; // for 1D FFT transforms of real data
	PowerSpectrum1d* ptrPowerSpectrum1d;

	/**
	 * variables associated with the calculation of a spectrogram
	 * for diagnostics of chirped signals (for chirp in laser pulses)
	 * all these variables end with "Chirp"
	 */
	bool flagChirp; // true for calculating the spectrogram for chirp diagnostics
	int numDataPointsChirp; // number of points in the data
	int numTransformPointsChirp; // a segment of the data to be FFTed
	int numSkipPointsChirp; // number of points to skip before reading
	// the next numTransformPointsChirp
	int numIntervalsChirp;  // number of intervals of numTransformPointsChipr
	// to be considered
	int x1LengthChirp; // number of data points for the plotting of
	// the spectrogram along "time" axis
	Scalar* x1Chirp;
	int x2LengthChirp;
	Scalar* x2Chirp;
	int numPSDChirp; // number of points for the spectrogram, "frequencies"
	Scalar** psdChirp; // values of the PSD
	BaseFFT* ptrFFT1dChirp; // for 1D FFT transforms of real data
	PowerSpectrum1d* ptrPowerSpectrum1dChirp;
	Scalar* inDataChirp;
	ostring x1LabelChirp;
	ostring x2LabelChirp;
	ostring subTitleChirp;

public:
	PSDFieldDiag1d(SpatialRegion* SR,
			int j1, int k1, int j2, int k2, int nfft, int HistMax,
			int Ave, int Comb, ostring VarName, ostring x1Label,
			ostring x2Label, ostring x3Label, ostring title, int save,
			ostring _windowName,
			ostring _fieldName, int const _fieldComponentLabel, int open_init, struct write_params write_data) /* throw(Oops) */;

	virtual ~PSDFieldDiag1d();

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
	void initwin(ostring prepend);
	virtual void initwin(GUIConfig* gui, int itemNumber) {}
	int get_j1() { return j1;};
	int get_j2() { return j2;};
	int get_k1() { return k1;};
	int get_k2() { return k2;};
};
#endif // HAVE_FFT
#endif //	ifndef __PSDFIELDDIAG1D_H



