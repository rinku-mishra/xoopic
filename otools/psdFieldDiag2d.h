#ifndef	__PSDFIELDDIAG2D_H
#define	__PSDFIELDDIAG2D_H

/**
 * The PSDFieldDiag2d class is responsible for the 2d 
 * power spectral density calculation of a filed component for
 * the E or B fields. 
 *
 * dad, started on 08/16/01.
 * 
 * Version: $Id: psdFieldDiag2d.h 2212 2004-09-08 15:31:09Z yew $
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


class GUIConfig;

//--------------------------------------------------------------------
//	Diag:  This class defines the generic boundary object for
//	some region.  All other boundaries inherit from this pure virtual.

class History;
class Diagnostics;

class PSDFieldDiag2d : public Diag
{
private:
	Grid *grid;

	int numElementsRFFT[2]; // number of data elements for the real FFT in 2d
	int num_psd2d[2]; // number of data elements for each 1D PSD calculation
	Scalar psd2dNormalizationFactor;

	ostring fieldName; // "E" or "B"
	int const fieldComponentLabel; // for which component of the field
	Vector3** fieldHandler;

	ostring fieldLabel;
	Scalar** psd2d; // values of the 2d psd

	Scalar** reFFTField; // real part of the FFT Field transform
	Scalar** imFFTField;

	Scalar *x1_array;
	int x1_arrayLength;
	Scalar *x2_array;
	int x2_arrayLength;

	BaseFFT* ptrFFT; // pointer to an object for FFT transforms of real data

public:
	PSDFieldDiag2d(SpatialRegion* SR,
			int j1, int k1, int j2, int k2, int nfft, int HistMax,
			int Ave, int Comb, ostring VarName, ostring x1Label,
			ostring x2Label, ostring x3Label, ostring title, int save,
			ostring _fieldName, int const _fieldComponentLabel, int open_init, struct write_params write_data) /* throw(Oops) */;

	virtual ~PSDFieldDiag2d();

#ifdef HAVE_HDF5
	void dumpDiagH5(dumpHDF5 *dumpObj);
#endif
	virtual int Restore_2_00(FILE *DMPFile, int j1, int k1,
			int j2, int k2, ostring title) {return 0;};
	virtual int Restore(FILE *DMPFile,Scalar A1,Scalar A2,Scalar B1,Scalar B2,
			int xl,int yl, char *title){ return 1;} ;
	virtual void MaintainDiag(Scalar t, int n) /* throw(Oops) */;					// added "n", JK 2013-07-29
	void setDiagnostics(Diagnostics *_theDiagnostics) { theDiagnostics=_theDiagnostics;};
	void initwin() { initwin(ostring("")); };
	virtual void initwin(ostring prepend);
	virtual void initwin(GUIConfig* gui, int itemNumber) {}
	int get_j1() { return j1;};
	int get_j2() { return j2;};
	int get_k1() { return k1;};
	int get_k2() { return k2;};
};
#endif // HAVE_FFT
#endif //	ifndef __PSDFIELDDIAG2D_H



