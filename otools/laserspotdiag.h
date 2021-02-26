#ifndef	__LASERSPOTSIZE_H
#define	__LASERSPOTSIZE_H

/*
====================================================================

LASERSPOTSIZE.H

Laser spot measuring diagnostic.


.99
====================================================================
 */

#include "sptlrgn.h"
#include "newdiag.h"
#include "diag2file.h"

class GUIConfig;

//--------------------------------------------------------------------
//	Diag:  This class defines the generic boundary object for
//	some region.  All other boundaries inherit from this pure virtual.

class History;
class Diagnostics;

class LaserSpotDiagnostic : public Diag
{
public:
	History *past_measurements_y4Ey2;
	History *past_measurements_y2Ey2;
	History *past_measurements_yEy2;
	History *past_measurements_Ey2;
	History *past_measurements;
	int size;
	Grid *grid;
	LaserSpotDiagnostic(SpatialRegion* SR,
			int j1, int k1, int j2, int k2, int nfft, int HistMax,
			int Ave, int Comb, ostring VarName, ostring x1Label,
			ostring x2Label, ostring x3Label, ostring title, int save, int open_init, struct write_params write_data);


	virtual ~LaserSpotDiagnostic();
	virtual int Restore_2_00(FILE *DMPFile, int j1, int k1, int j2, int k2, ostring title) {return 0;};
	virtual int Restore(FILE *DMPFile,Scalar A1,Scalar A2,Scalar B1,Scalar B2,
			int xl,int yl, char *title){ return 1;} ;
	virtual void MaintainDiag(Scalar t, int n);			// added "n", JK 2013-07-29
	void setDiagnostics(Diagnostics *_theDiagnostics) { theDiagnostics=_theDiagnostics;};
	void initwin() { initwin(ostring("")); };
	void initwin(ostring prepend);
	void initwin(GUIConfig* gui, int itemNumber) {}
	int get_j1() { return j1;};
	int get_j2() { return j2;};
	int get_k1() { return k1;};
	int get_k2() { return k2;};
};

#endif	//	ifndef __LASERSPOTDIAG_H



