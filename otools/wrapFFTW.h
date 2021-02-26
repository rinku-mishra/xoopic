#ifndef wrapFFTW_H
#define wrapFFTW_H

/**
 * The WrapFFTW class wraps the FFTW library (www.fftw.org)
 * (both in single and double precision) for use with 
 * the data structures in xoopic. 
 * 
 * Version: $Id: wrapFFTW.h 2291 2006-07-20 21:26:07Z yew $
 * 
 * Copyright 2001 by Tech-X Corporation. 
 */
#include <oops.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_FFT 

#include "baseFFT.h"

#ifdef SCALAR_DOUBLE
#ifdef HAVE_DFFTW // double precision FFTW
#include <fftw.h>	// by default we use double precision with shorter names
#include <rfftw.h>
#endif
#else
#ifdef HAVE_SFFTW // single precision FFTW
#include <sfftw.h>
#include <srfftw.h>
#endif
#endif

class WrapFFTW : public BaseFFT {
 public:
  /**
   * constructor arguments description: 
   * (i)   const int t_rank  => dimensionality of the input array, it
   *                            can be any non-negative integer.
   * (ii)  const int *t_n    => pointer to an integer array of length 
   *                            rank containing the (positive) sizes of 
   *                            each dimension of the array.
   * (iii) const int fftwFlag=> transform flag, if fftwFlag == 0 => use
   *                            general (complex-valued) fftw functions,
   *                            if fftwFlag == 1 => use the 
   *                            real fftw functions. 
   * (iv)  const int t_dir   => direction of the transform, i.e. forward 
   *                            transform it is equal to 1 or for 
   *                            backward, equal to -1, 
   *                            note that the fftw plan depends 
   *                            on the direction
   */
  WrapFFTW(const int t_rank, const int* t_n, const int fftwFlag, 
           const int t_dir) /* throw(Oops) */;
  ~WrapFFTW();
  /**
   * the function to call to do the fft on real data
   */
  void doFFT(Vector3** reIn, int const component, 
             Scalar** reOut, Scalar** imOut) /* throw(Oops) */;
  /**
   * r1d_fft(Scalar* in, Scalar* out): a function to perform
   * fft on a real valued 1d function. Scalar* in; is the input data
   * and Scalar* out is for the output data. The order of the 
   * output elements in the forward direction, i.e. real -> complex
   * is defined in the documentation of the fftw package at
   * http://www.fftw.org/doc/
   */ 
  void r1d_fft(Scalar* in, Scalar* out) { rfftw_one(p1Dr, in, out); } 
 private:
  // do not allow copy and assignment
  WrapFFTW(const WrapFFTW&);
  WrapFFTW& operator=(const WrapFFTW&);

  // data members; different input and output data structures 
  // will be used for now although some of the fftw functions
  // support fft in-place.
  int rank; 
  int* n; 
  int transformFlag;
  int direction; 
  /**
   * pointer for the fftw plans:
   */
  fftw_plan p1D;    // 1D complex fftw plan 
  fftwnd_plan pnD;  // nD complex fftw plan
  rfftw_plan p1Dr;  // 1D real    fftw plan
  rfftwnd_plan pnDr;// nD real    fftw plan
  /**
   * pointer to data structure for real-valued fftw transforms
   */ 
  fftw_real* rDataIn;
  fftw_real* rDataOut;
  /** 
   * pointer to data structure for general (complex-valued)
   * fftw transforms
   */
  fftw_complex* cDataIn;
  fftw_complex* cDataOut;
  /**
   * helper function for unrecognized direction error
   */
  void unrecognizedDirectionError() /* throw(Oops) */ {
    stringstream ss (stringstream::in | stringstream::out);
    ss <<"WrapFFTW::unrecognizedDirectionError: Error: \n"<<
      "Unrecognized FFT direction = " << direction 
         << " in class Wrap_FFTW." << endl
         << "FFT direction must be 1 for forward and -1 for backward FFT"<<endl;

    std::string msg;
    ss >> msg;
    Oops oops(msg);
    throw oops;    // exit() constructor calls

  }
  /**
   * helper function for unrecognized rank error
   */
  void unrecognizedRankError() /* throw(Oops) */{
    stringstream ss (stringstream::in | stringstream::out);
    ss << "wrapFFTW::unrecognizedRankError: Error: \n"<<
      "Unrecognized FFT rank = " << rank 
         << " in class Wrap_FFTW." << endl
         << "FFT rank must be a non negative integer.";
    std::string msg;
    ss >> msg;
    Oops oops(msg);
    throw oops;    // exit() constructor
  }
  /**
   * helper function for unrecognized transform flag error
   */
  void unrecognizedTransformFlagError() /* throw(Oops) */{
    stringstream ss (stringstream::in | stringstream::out);
    ss << "wrapFFTW::unrecognizedTransformFlagError: Error"<<
      "Unrecognized transform flag = " << transformFlag 
         << " in class Wrap_FFTW." << endl
         << "transform flag must be 0 for general (complex-valued) FFT"
         << endl << "and 1 for real-valued FFT."<<endl;

    std::string msg;
    ss >> msg;
    Oops oops(msg);
    throw oops;    // exit()constructor
  }
  /**
   * helper function to map the space variation of a component of 
   * a Vector3** to the format of the input data for use with the 
   * real fftw 2d. 
   */
  void map2DForwardRFFTW(Vector3** reIn, const int& component);
  /** 
   * helper function to map the 1D data structure used by the
   * 2D real fftw to the corresponding arrays 
   * for the Re and Im parts of the FFT.
   */
  void map2DBackwardRFFTW(Scalar** reOut, Scalar** imOut);
};

#endif // HAVE_FFT
#endif // wrap_FFTW
