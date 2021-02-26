#ifndef	__NEWDIAG_H
#define	__NEWDIAG_H

/*
====================================================================

NEWDIAG.H

The pure virtual base class that represents a generic diagnostic object.
All diagnostic objects (e.g., point, line) must inherit from this class.


1.01	(PG, KC, 9-21-95) After release. First draft.
1.02	(JohnV 03-08-96) Added initwin(params...) for PC version
1.03	(JK, 2013-07-26); added structure for writing data to files, independent of xGrafix and user intervention
====================================================================
 */

#include "sptlrgn.h"
#include "ostring.h"

extern "C" {
void WinSet2D(const char *PlotType, const char *X_Label, const char *Y_Label, const char *W_Label,
		const char *State, int ulx, int uly, Scalar X_Scale, Scalar Y_Scale,
		int X_Auto_Rescale, int Y_Auto_Rescale, Scalar X_Min,
		Scalar X_Max, Scalar Y_Min, Scalar Y_Max);
}

class GUIConfig;

#define DIAG_WINDOW_INIT_OPEN	"open"
#define DIAG_WINDOW_INIT_CLOSED	"closed"

//--------------------------------------------------------------------
//	Diag:  This class defines the generic boundary object for
//	some region.  All other boundaries inherit from this pure virtual.

class History;
class Diagnostics;
class Diag2File;			// JK, 2013-07-30

class Diag
{
protected:
	SpatialRegion* region;
	Diagnostics *theDiagnostics;
	History* history;
	int j1, k1, j2, k2;
	Scalar A1,A2,B1,B2;
	int nfft, HistMax, Ave, Comb;
	ostring VarName, x1Label, x2Label, x3Label, title;
	int save;
	const char *win_init_stat;		// initial status of diagnostic windows (pointer to correct string)

	// JK, 2013-07-30
	Diag2File* write2file;
	// JK, 2016-02-27
	bool derivativeDiag;

public:
#ifdef HAVE_HDF5
	virtual void dumpDiagH5(dumpHDF5 *dumpObj){}
#endif
	ostring getVarName(){return VarName;}
	Diag(SpatialRegion* SR, int j1, int k1, int j2, int k2, int nfft, int HistMax,
			int Ave, int Comb, ostring VarName, ostring x1Label,
			ostring x2Label, ostring x3Label, ostring title, int save, int open_init, struct write_params wp);
	Diag(SpatialRegion* _region, ostring _VarName, ostring _title, int _save, int _open_init, struct write_params wp);
	virtual ~Diag(){};
	virtual int Restore_2_00(FILE *DMPFile, int j1, int k1, int j2, int k2, ostring title);
	virtual int Restore(FILE *DMPFile,Scalar A1,Scalar A2,Scalar B1,Scalar B2,
			int xl,int yl, const char *title);
#ifdef HAVE_HDF5
	virtual int dumpH5(dumpHDF5 &dumpObj,int number);
#endif
	virtual int Dump(FILE *DMPFile);
	virtual void MaintainDiag(Scalar t, int n) {};				// added "n", JK 2013-07-29
	void setDiagnostics(Diagnostics *_theDiagnostics) { theDiagnostics=_theDiagnostics;};
	virtual void initwin() { initwin(ostring("")); };			// JK, 2016-03-18
	virtual void initwin(ostring prependVarName) {};			// JK, 2016-02-27
	virtual void initwin(GUIConfig* gui, int itemNumber) {}
	/**
	virtual void WinSet2D(const char *PlotType, const char *X_Label, const char *Y_Label, const char *W_Label, 
             const char *State, int ulx, int uly, Scalar X_Scale, Scalar Y_Scale,
             int X_Auto_Rescale, int Y_Auto_Rescale, Scalar X_Min,
             Scalar X_Max, Scalar Y_Min, Scalar Y_Max);
	 */
	int get_j1() { return j1;};
	int get_j2() { return j2;};
	int get_k1() { return k1;};
	int get_k2() { return k2;};

//	virtual void output(FILE *fp) {}
	// JK, 2013-07-31
	Diag2File* getDiag2File() { return write2file; }
	const char* getWinInitState() { return win_init_stat; }
	// return string for open/close status of window for diagnostics
	//char *getInitOpenWindows
	// JK, 2013-08-01
	void defineParameters(ostring _x1label, ostring _x2label, Scalar* _x1_array, Scalar* _x2_array, Vector3** _vals, int* _jm, int* _km, int _component);
	void defineParameters(ostring _x1label, ostring _x2label, Scalar* _x1_array, Scalar* _x2_array, Scalar** _vals, int* _jm, int* _km);
	void defineParameters(ostring _x1label, ostring _x2label, int _num_species);
	void defineParameters(ostring _x1label, ostring _x2label, Scalar* _x1positions, Scalar* _x2positions, Scalar* _x1velocities, Scalar* _x2velocities, Scalar* _x3velocities, int* _num_data, int species_index);
	void defineParameters(ostring _x1label, ostring _x2label, Scalar* _x1_array, Scalar* _x2_array, int* _num_data);

	void addSpeciesParameters(Scalar* _x1_values, Scalar* _x2_values, int* _size, int _species_index);
	// JK, 2016-02-27
	void setDerivativeFlag() { derivativeDiag = true; };
	bool getDerivativeFlag() { return derivativeDiag; };
	bool isDerivative() { return derivativeDiag; };
	virtual void compute(Scalar t, int n) {};
};

#endif	//	ifndef __NEWDIAG_H

