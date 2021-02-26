#ifndef	__PROBEREGION_H
#define	__PROBEREGION_H

/*
====================================================================

probergn.h

region probes that live on the grid


1.01	(KC, 9-21-95) After release. First draft.

====================================================================
*/

#include <stdio.h>
#include "ovector.h"
#include "ostring.h"
#include "misc.h"
#include "newdiag.h"
#include "history.h"


class GUIConfig;

//--------------------------------------------------------------------

class ProbeRegion : public Diag
{
protected:
	Vector3*** EVarPtr, ***IVarPtr, **AVarPtr;  // pointer to the var that will be plotted
	Scalar*** VarPtr;
//	History* history;
	Scalar* x_array, *y_array;
	int sizeK, sizeJ;
	void Chose_VarPtr();
//	int init;

public:
	ProbeRegion(SpatialRegion* region, int j1, int k1, int j2, int k2, int nfft, int HistMax,
				 int Ave, int Comb, ostring VarName, ostring diagXLabel,
				 ostring diagYLabel, ostring diagZLabel, ostring diagLabel, int save, int open_init, struct write_params write_data);
	virtual ~ProbeRegion();
//	virtual int Restore(FILE *DMPFile, int j1, int k1, int j2, int k2);
//	virtual int Dump(FILE *DMPFile);
	virtual void MaintainDiag(Scalar t, int n);
	void initwin() { initwin(ostring("")); };
	void initwin(ostring prepend);
	virtual void initwin(GUIConfig* theGui, int itemNumber);
	BOOL	alongx1() {return (k1 == k2) ? TRUE : FALSE;} // boundary lies along x1
	BOOL	alongx2() {return (j1 == j2) ? TRUE : FALSE;}
};

#endif	//	ifndef __PROBEREGION_H
