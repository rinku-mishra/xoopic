//--------------------------------------------------------------------
//
// File:	numbers.h
//
// Purpose:	Determine the maximum and minimum numbers and epsilon
//		for the choice of Scalar
//
// Version:	$Id: numbers.h 1295 2000-07-24 20:36:48Z cary $
//
//--------------------------------------------------------------------

#if Scalar == double

#define ScalarMin	DBL_MIN
#define ScalarMax	DBL_MAX
#define ScalarEpsilon	DBL_EPSILON

#elif Scalar == float

#define ScalarMin	FLT_MIN
#define ScalarMax	FLT_MAX
#define ScalarEpsilon	FLT_EPSILON

#endif
