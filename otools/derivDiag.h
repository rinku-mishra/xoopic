/*
====================================================================

DERIVDIAG.H

Class for managing derived diagnostic (values are derivatives of some value; all is computed when writing to file).
Class stores a copy of data from previous time step to compute derivative.

0.01	(JK, 2013-08-05) Initial class

====================================================================
*/

#ifndef	__DERIVDIAG_H
#define	__DERIVDIAG_H

#include <stdio.h>
#include "ovector.h"
#include "ostring.h"
#include "misc.h"
#include "newdiag.h"
#include "history.h"

//#define DEFINED_DERIVED_DIAGNOSTICS			8


//const char* _derivDiagList[DEFINED_DERIVED_DIAGNOSTICS] =
//		{ "dE/dt", "dE1/dt","dE2/dt","dE3/dt",
//		  "eps_dE/dt", "eps_dE1/dt","eps_dE2/dt","eps_dE3/dt" };

//--------------------------------------------------------------------



class DerivativeDiagnostic : public Diag
{
protected:
	Scalar *x1_array, *x2_array;
	Scalar **scalar_data, **scalar_source, **scalar_data_copy;
	Vector3 **vector_source;
	Scalar x1max, x2max, x1min, x2min;
	ostring mainVarName;
	int data_source_type;			// type of source data we are displaying (e.g. scalar, vector3)
	int component;					// direction of derivative; 1-3, -1 means total derivative
	int jm;							// size of scalar data
	int km;
	int n_width;					// width of derivative period in computational steps
	int n_step;						// step between writes to file; used to decide if making copy or not
	int n_copy_saved;				// simulation step when data was saved into "copy"
	int n_next_compute;				// simulation step when next computing will be done
	Scalar t_copy_saved;			// simulation time when data was saved into "copy"
	Scalar value_factor;			// multiplication factor with which all values are multiplied before they are displayed to screen/written to file; JK, 2016-02-24
	//	int init;
	void Chose_VarPtr();

	SpatialRegion* local_region;

public:
	DerivativeDiagnostic(SpatialRegion* region, int j1, int k1, int j2, int k2,
						 int nfft, int HistMax, int Ave, int Comb, ostring _VarName,
						 ostring diagXLabel, ostring diagYLabel, ostring diagZLabel, ostring diagLabel,
						 int save, ostring _integral, int open_init, struct write_params write_data);
	DerivativeDiagnostic(SpatialRegion* SR, ostring VarName, ostring _title, int save, int open_init, struct write_params write_data);
	virtual ~DerivativeDiagnostic();
	void initDiag(struct write_params write_data);	// init structures according to given values
	virtual void MaintainDiag(Scalar t, int n);			// added "n", JK 2013-07-29
	void initwin() { initwin(ostring("")); };
	void initwin(ostring prependVarName);
	virtual void FFT();
//	void initDerivDiagNames();
//	static const char* getDefinedDiagName(int pos);
//	static int getDiagNamePosition(const char *name);
//	static int definedDiagListLen() { return DEFINED_DERIVED_DIAGNOSTICS; }
	void makeCopyOfData(Scalar t, int n);				// JK, 2016-02-24
	inline void setValueOutputFactor( double in_value ) { value_factor = in_value; };		// JK, 2016-02-24
	inline double getValueOutputFactor() { return value_factor; };							// JK, 2016-02-24
	void compute(Scalar t, int n);
	// Check if given variable defines derivative diagnostics or not; it checks if the varname includes "/dt"
	static bool VarNameDefinesDerivative( ostring varName ) { return (varName.contains(ostring("/dt")) == 1); };
};

#endif	//	ifndef __DERIVDIAG_H
