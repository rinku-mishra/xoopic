/*
====================================================================

GRDPROB.H

e.g., point, line probes that live on the grid


1.01	(KC, 9-21-95) After release. First draft.


====================================================================
*/

#ifndef	__GRDPROBES_H
#define	__GRDPROBES_H
#define MaxDiags 3
#define MaxLen 80

#include <stdio.h>
#include	"ovector.h"
#include "ostring.h"
#include "misc.h"
#include "newdiag.h"
#include "history.h"

//class ostring;

//#ifdef UNIX
// #include "xgio.h"
//#endif



//--------------------------------------------------------------------


class GridProbes : public Diag
{
protected:

	Scalar** VarPtr;  // pointer to the var that will be plotted
	Scalar* dVarPtr; // the d whatever
	Scalar Stuff;
	Scalar* FTVarPtr; // pointer to FT of VarPtr
//	History* history;
	Scalar* x_array;
	Grid* grid;
	int size, Space_fft, Integrate, Chose_Later;
	ostring integral, VarName;
//	int init;
	void Chose_VarPtr();
	
public:
	GridProbes(SpatialRegion* SR, int j1, int k1, int j2, int k2, 
						 int nfft, int HistMax, 
						 int Ave, int Comb, ostring VarName, ostring diagXLabel, 
						 ostring diagYLabel, ostring diagZLabel, ostring diagLabel,
						 int save,
						 ostring integral, int open_init, struct write_params write_data);
	virtual ~GridProbes();
//	virtual int Restore(FILE *DMPFile, int j1, int k1, int j2, int k2, ostring title);
//	virtual int Dump(FILE *DMPFile);
//	virtual void PreChooseVar()=0;
	virtual void MaintainDiag(Scalar t, int n);			// added "n", JK 2013-07-29
	void initwin() { initwin(ostring("")); };
	void initwin(ostring prepend);
	virtual void FFT();
	BOOL	alongx1() {return (k1 == k2) ? TRUE : FALSE;} // boundary lies along x1
	BOOL	alongx2() {return (j1 == j2) ? TRUE : FALSE;}
};

#endif	//	ifndef __GRDPROBES_H
