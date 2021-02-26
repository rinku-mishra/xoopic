#ifndef	__VARLOAD_H
#define	__VARLOAD_H

/*
====================================================================

varload.h

Loads particles with a weighting that increases linearly with
radius.  This class only makes sense for z-r corrdinates; this
should be enforced by the expert advisor.

2.01 (Bruhwiler 10-28-99) Initial implementation.
====================================================================
 */

#include "load.h"

#include <oops.h>


class VarWeightLoad : public Load
{
	Scalar	rFraction;						//	stores next r/rMax to be loaded
	Scalar	w0;                   //	r/rMax = w0 + w1 * random
	Scalar	w1;

	Scalar getRandom_rFraction() {
		return w0 + w1 * frand();
	}

	Scalar	getReverse_rFraction(int seedValue, int arg2) {
		return w0 + w1 * revers(seedValue, arg2);
	}

	// np2c at outer radius
	Scalar	np2cMax() {
		return np2c;
	}

public:
	VarWeightLoad( SpatialRegion* SP, Scalar Density, Maxwellian* max,
			Scalar zMinMKS, Scalar zMaxMKS, Scalar rMinMKS,
			Scalar rMaxMKS, int LoadMethodFlag, Scalar np2c,
			const ostring &analyticF, const ostring &DensityFile,
			const ostring &name_in);
	virtual ~VarWeightLoad();
	virtual int load_it()  /* throw(Oops) */;
};

#endif  // ifndef __VARLOAD_H
