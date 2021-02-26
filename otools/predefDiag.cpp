/*
	 ====================================================================

	 PREDEFDIAG.CPP

	 0.01	(JK, 2013-08-01) Initial class for enabling/writing predefined diagnostics.

	 ====================================================================
	 */

#include "newdiag.h"
#include "diagn.h"
#include "predefDiag.h"
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

PredefDiagnostic::PredefDiagnostic(SpatialRegion* region, ostring _VarName, ostring _title,
											 int save, int open_init, struct write_params write_data)
:Diag(region, _VarName, _title, save, open_init, write_data)
{
		
}
PredefDiagnostic::~PredefDiagnostic()
{
}

void PredefDiagnostic::MaintainDiag(Scalar t, int n)
{
}

void PredefDiagnostic::initwin()
{
}

void PredefDiagnostic::FFT()
{
}
