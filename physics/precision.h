#ifndef PRECISION_H
#define PRECISION_H

#include <float.h>

/* this header file is used only to define the precision (float or double)
  with which the code is to be built.

  2.50  Bruhwiler  01/19/2000 
*/

#ifdef SCALAR_DOUBLE
  #define Scalar double
  #define Scalar_EPSILON DBL_EPSILON
  #define Scalar_MIN DBL_MIN
  #define Scalar_MAX DBL_MAX
  #define ScalarChar "double"
  #define ScalarInt 8
  #define MPI_Scalar MPI_DOUBLE
#else
  #define Scalar float
  #define Scalar_EPSILON FLT_EPSILON
  #define Scalar_MIN FLT_MIN
  #define Scalar_MAX FLT_MAX
  #define ScalarChar "float"
  #define ScalarInt 4
  #define MPI_Scalar MPI_FLOAT
#endif

#endif  /* PRECISION_H */

