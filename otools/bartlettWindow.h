#ifndef BartlettWindow_H
#define BartlettWindow_H
/**
 * A class for the Bartlett windowing of data. 
 *
 * dad, started on 08/17/01.
 * 
 * Version: $Id: bartlettWindow.h 1730 2001-08-24 20:07:49Z dad $
 * 
 * Copyright 2001 by Tech-X Corporation. 
 */
#include "baseWindow.h"

class BartlettWindow : public BaseWindow {
 public:
  BartlettWindow(const int numElements) : BaseWindow(numElements) {
    int i;
    Scalar si;
    Scalar Nov2 = static_cast<Scalar>(N)/2.;
    windowFunction = new Scalar[N];
    winScaleFactor = 0.0;
    for (i = 0; i < N; i++) {
      si = static_cast<Scalar>(i);
      windowFunction[i] = 1. - fabs((si - Nov2)/Nov2);
      winScaleFactor += pow(windowFunction[i], 2);
    }  
    winScaleFactor *= static_cast<Scalar>(N);
  }
  ~BartlettWindow() {
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
  BartlettWindow(const BartlettWindow&);
  BartlettWindow& operator=(const BartlettWindow&);

  Scalar* windowFunction;
  Scalar winScaleFactor;
};

#endif // BartlettWindow_H
