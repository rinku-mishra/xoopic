#ifndef BaseWindow_H
#define BaseWindow_H
/**
 * A pure abstract class for providing interface to data windowing
 * classes for use in applications of the FFT.
 *
 * dad, started on 08/16/01.
 * 
 * Version: $Id: baseWindow.h 1743 2001-08-30 19:16:31Z kgl $
 * 
 * Copyright 2001 by Tech-X Corporation. 
 */

// problem including cmath with Linux/Alpha using Commpaq CXX.
//  use math.h instead 
//#include <cmath>
#include <math.h>

#include "precision.h"

class BaseWindow {
 public:
  BaseWindow(const int numElements) : N(numElements) {}
  virtual ~BaseWindow() {}
  virtual void applyWindow(Scalar* data) = 0;
  virtual Scalar get_winScaleFactor() const = 0;
 protected:
  const int N; // number of elements
};

#endif // BaseWindow_H
