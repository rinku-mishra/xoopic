/*
====================================================================

PREDEFDIAG.H

Class for managing predefined diagnostic (diagnostic that is already implemented in xooppic) for writing data to files
and to open diagnostic during program startup.

0.01	(JK, 2013-07-31) Initial class

====================================================================
*/

#ifndef	__PREDEFDIAG_H
#define	__PREDEFDIAG_H
#define MaxDiags 3
#define MaxLen 80

#include <stdio.h>
#include "ovector.h"
#include "ostring.h"
#include "misc.h"
#include "newdiag.h"
#include "history.h"


//--------------------------------------------------------------------


class PredefDiagnostic : public Diag
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
	PredefDiagnostic(SpatialRegion* SR, ostring VarName, ostring _title,
						 int save, int open_init, struct write_params write_data);
	virtual ~PredefDiagnostic();
	virtual void MaintainDiag(Scalar t, int n);			// added "n", JK 2013-07-29
	virtual void initwin();
	virtual void FFT();
	BOOL	alongx1() {return (k1 == k2) ? TRUE : FALSE;} // boundary lies along x1
	BOOL	alongx2() {return (j1 == j2) ? TRUE : FALSE;}
};

#endif	//	ifndef __PREDEFDIAG_H
