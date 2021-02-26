/**
 * A base class to wrap FFT libraries for use with the 
 * data structures in xoopic. The derived classes will
 * implenet the wrapping for each FFT library that may
 * be of interest to use. 
 * 
 * Version: $Id: baseFFT.h 1730 2001-08-24 20:07:49Z dad $
 * 
 * Copyright 2001 by Tech-X Corporation. 
 */

#ifndef baseFFT_H
#define baseFFT_H

#include "ovector.h"
#include "precision.h"

class BaseFFT {
 public:
  /**
   * The constructor of the derived class will set the neccessary
   * data structures for use with the corresponding library, the
   * dimensionallity of the transform, and what type of transform
   * to use, e.g. the general complex fft functions or the fft
   * functions for real functions. 
   */
  virtual ~BaseFFT() {}
  /**
   * The following are overloaded functions that will be called to do
   * the actual FFT depending on input and type of transform needed. 
   * Expand this list for your own needs. 
   */
  /**
   * a function to perform 2D ffts. The input consists of a pointer to 
   * a Vector3 array (for the real data input), an integer to specify 
   * which component of 
   * the Vector3 to fft, two pointers to arrays of Vector3's for the real 
   * and imaginary parts of the output.
   */
  virtual void doFFT(Vector3** reIn, int const component, 
                     Scalar** reOut, Scalar** imOut) = 0;
  /**
   * r1d_fft(Scalar* in, Scalar* out): a function to perform
   * fft on a real valued 1d function. Scalar* in; is the input data
   * and Scalar* out is for the output data. The number of elements
   * in these arrays is set in the constructors of the derived 
   * classes. 
   */ 
  virtual void r1d_fft(Scalar* in, Scalar* out) = 0;
};

#endif // baseFFT_H
