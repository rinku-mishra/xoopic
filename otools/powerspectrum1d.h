#ifndef PowerSpectrum1d_H
#define PowerSpectrum1d_H
/**
 * The PowerSpectrum1d class handles for now the calculation of 
 * of the power spectrum of a 1d real function 
 * via the FFT. The calculation uses the FFTW library for
 * the FFT and expects that the output format of the FFT
 * will be stored in a 1d array following the convention
 * in the FFTW lib (www.fftw.org). This is a commonly accepted and
 * used convention. The data can be windowed with one of the following
 * window functions: "Blackman", "Hamming", "Welch", "Hann", "Bartlett".
 * Passing "None" as the value of the "String windowName" variable of
 * the constructor prevents the data windowing. 
 *
 * Passing isDCcomponentSubtracted = true leads to subtracting of the
 * the DC component after the data is windowed (in case windowing is
 * used). 
 *
 * dad, started on 08/16/01.
 * 
 * Version: $Id: powerspectrum1d.h 1739 2001-08-29 21:29:08Z dad $
 * 
 * Copyright 2001 by Tech-X Corporation. 
 */

#ifdef UNIX
#include <config.h>
#endif

#ifdef HAVE_FFT 

#include <string>
#include "baseFFT.h"
#include "baseWindow.h"
#include "precision.h"

class PowerSpectrum1d {
 public:
  PowerSpectrum1d(const int _numElements, BaseFFT* _pRFFT1d, 
                  string _windowName, bool _isDCcomponentSubtracted);
  ~PowerSpectrum1d();
  void calc_psd1d(Scalar* in, Scalar* PSD);
 private:
  // do not allow copy and assignment
  PowerSpectrum1d(const PowerSpectrum1d&);
  PowerSpectrum1d& operator=(const PowerSpectrum1d&);

  const int numElements;
  BaseFFT* pRFFT1d;
  string windowName;
  BaseWindow* ptrBaseWindow;
  bool isDCcomponentSubtracted;
  Scalar* out; // pointer to an array to store the result of the FFT
  Scalar winScaleFactor;
};

#endif // HAVE_FFT

#endif // PowerSpectrum1d_H
