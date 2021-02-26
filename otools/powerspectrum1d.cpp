#include "powerspectrum1d.h"
#include "blackmanWindow.h"
#include "hammingWindow.h"
#include "bartlettWindow.h"
#include "hannWindow.h"
#include "welchWindow.h"

#ifdef HAVE_FFT 
   
PowerSpectrum1d::PowerSpectrum1d(const int _numElements, BaseFFT* _pRFFT1d, 
                                 string _windowName, 
                                 bool _isDCcomponentSubtracted)
  : numElements(_numElements), pRFFT1d(_pRFFT1d), windowName(_windowName), 
  isDCcomponentSubtracted(_isDCcomponentSubtracted) 
{
  out = new Scalar[numElements];
  ptrBaseWindow = 0;   
  if ( windowName == "Blackman") {
    ///cout << "Using Blackman Window" << endl;
    ptrBaseWindow = new BlackmanWindow(numElements); 
    winScaleFactor = ptrBaseWindow->get_winScaleFactor();
  } else if ( windowName == "Welch" ) {
    ///cout << "Using Welch Window" << endl;
    ptrBaseWindow = new WelchWindow(numElements); 
    winScaleFactor = ptrBaseWindow->get_winScaleFactor();
  } else if ( windowName == "Bartlett" ) {
    ///cout << "Using Bartlett Window" << endl;
    ptrBaseWindow = new BartlettWindow(numElements); 
    winScaleFactor = ptrBaseWindow->get_winScaleFactor();
  } else if ( windowName == "Hamming" ) {
    ///cout << "Using Hamming Window" << endl;
    ptrBaseWindow = new HammingWindow(numElements); 
    winScaleFactor = ptrBaseWindow->get_winScaleFactor();
  } else if ( windowName == "Hann" ) {
    ///cout << "Using Hann Window" << endl;
    ptrBaseWindow = new HannWindow(numElements); 
    winScaleFactor = ptrBaseWindow->get_winScaleFactor();
  } else {
    // default to no window
    ///cout << "Calsulating the 1d psd without windowing the data!" << endl;
    winScaleFactor = pow(static_cast<double>(numElements),2);
  } 
}

PowerSpectrum1d::~PowerSpectrum1d()
{
  delete [] out;
  if (ptrBaseWindow)
    delete ptrBaseWindow;
}

/** 
 * a helper function to calculate the power spectral density of 
 * a 1D real valued function. Note that the number of elements
 * in the input array is given by data member: "numElements"
 * and the number of elements of the power spectral density 
 * array PSD by: "numElements/2+1". 
 */
void PowerSpectrum1d::calc_psd1d(Scalar* in, Scalar* PSD) {
  int k;
  //
  // window the data if requested
  //
  if (ptrBaseWindow) 
    ptrBaseWindow->applyWindow(in);
  //
  // remove the DC component if isDCcomponentSubtracted is true
  // 
  if (isDCcomponentSubtracted) {
    double DCcomponent = 0.0;
    for ( k = 0; k < numElements; k++ ) {
      DCcomponent += in[k];
    }
    DCcomponent /= static_cast<double>(numElements);
    for ( k = 0; k < numElements; k++ ) 
      in[k] -= DCcomponent;  
  }
  //
  // do the real to complex 1d fft
  // 
  pRFFT1d->r1d_fft(in, out);

  //
  // calculate the power spectral density
  //
  PSD[0] = out[0]*out[0]/winScaleFactor;
   
  for (k = 1; k < (numElements+1)/2; k++) {
    PSD[k]=(out[k]*out[k] + 
            out[numElements-k]*out[numElements-k])/winScaleFactor;
  } 

  //
  // set the PSD at the Nyquist frequency for even numElements
  // 
  if (numElements % 2 == 0) {
    PSD[numElements/2] = out[numElements/2]*
      out[numElements/2]/winScaleFactor;
  }
} 

#endif // HAVE_FFT
