#ifndef BlackmanWindow_H
#define BlackmanWindow_H
/**
 * A class for the Blackman windowing of data. 
 *
 * dad, started on 08/16/01.
 * 
 * Version: $Id: blackmanWindow.h 1920 2002-06-18 22:34:58Z kgl $
 * 
 * Copyright 2001 by Tech-X Corporation. 
 */
#include "baseWindow.h"
#ifdef _MSC_VER
#include <misc.h>
#endif

class BlackmanWindow : public BaseWindow {
 public:
  BlackmanWindow(const int numElements) : BaseWindow(numElements) {
    int i;
    Scalar si;
    Scalar Nov2 = static_cast<Scalar>(N)/2.;
    windowFunction = new Scalar[N];
    winScaleFactor = 0.0;
    for (i = 0; i < N; i++) {
      si = static_cast<Scalar>(i);
      windowFunction[i] = 
        0.42 - 0.5*cos(M_PI*si/Nov2) + 0.08*cos(2.*M_PI*si/Nov2);
      winScaleFactor += pow(windowFunction[i], 2);
      if (fabs(windowFunction[i]) < 1.e-15)
        windowFunction[i] = 0.0;
    }  
    winScaleFactor *= static_cast<Scalar>(N);
  }
  ~BlackmanWindow() {
    if (windowFunction)
      delete [] windowFunction;
  }
  void applyWindow(Scalar* data) {
    int i;
    for (i = 0; i < N; i++) 
      data[i] *= windowFunction[i];
  }
  Scalar get_winScaleFactor() const { return winScaleFactor; }
 private:
  BlackmanWindow(const BlackmanWindow&);
  BlackmanWindow& operator=(const BlackmanWindow&);

  Scalar* windowFunction;
  Scalar winScaleFactor;
};

#endif // BlackmanWindow_H
