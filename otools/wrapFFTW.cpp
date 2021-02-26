/**
 * Implementation of class WrapFFTW member functions. 
 * WrapFFTW class wraps the FFTW library (www.fftw.org)
 * (both in single and double precision) for use with 
 * the data structures in xoopic. 
 * 
 * Version: $Id: wrapFFTW.cpp 2290 2006-07-20 21:23:11Z yew $
 * 
 * Copyright 2001 by Tech-X Corporation. 
 */

#include "wrapFFTW.h"

#ifdef HAVE_FFT 
/**
 * constructor arguments description: 
 * (i)   const int rank  =>   dimensionality of the input array, it
 *                            can be any non-negative integer.
 * (ii)  const int *n    =>   pointer to an integer array of length 
 *                            rank containing the (positive) sizes of 
 *                            each dimension of the array.
 * (iii) const int fftwFlag=> transform flag, if fftwFlag == 0 => use
 *                            general (complex-valued) fftw functions,
 *                            if fftwFlag == 1 => use the 
 *                            real fftw functions. 
 * (iv)  const int t_dir =>   direction of the transfort, i.e. forward (1) or
 *                            backward (-1), note that the fftw plan depends 
 *                            on the direction
 */
WrapFFTW::WrapFFTW( const int t_rank, const int* t_n, 
                    const int fftwFlag, const int t_dir) 
  /* throw(Oops) */{
  rank          = t_rank;
  n             = new int[rank];
  for(int cntr = 0; cntr < rank; cntr++)
    n[cntr] = t_n[cntr];
  transformFlag = fftwFlag;
  direction     = t_dir;
  //
  // initialize all pointer data members to 0 as their default values
  // 
  p1D  = 0;  // 1D complex fftw plan 
  pnD  = 0;  // nD complex fftw plan
  p1Dr = 0;  // 1D real    fftw plan
  pnDr = 0;  // nD real    fftw plan  
  rDataIn  = 0; 
  rDataOut = 0;
  cDataIn  = 0;
  cDataOut = 0; 
  // 
  // create a plan according to the type of fftw to be performed, 
  // i.e. general for transformFlag == 0 and real fftw for
  // transformFlag == 1
  // 
  if ( transformFlag == 0 ) { // general (complex-valued) fftw transforms
    if ( rank == 1 ) { // 1D fft
      if ( direction == 1) { // forward fftw
        p1D = fftw_create_plan(n[0], FFTW_FORWARD, FFTW_MEASURE);
      } else if (direction == -1) { // backward fftw
        p1D = fftw_create_plan(n[0], FFTW_BACKWARD, FFTW_MEASURE);
      } else { // die with unrecognized direction
        try{
          unrecognizedDirectionError();
        }
        catch(Oops& oops){
          oops.prepend("WrapFFTW::WrapFFTW: Error:\n"); //PSDFieldDiag2d::PSDFieldDiag2d PSDFieldDiag1d::PSDFieldDiag1d
          throw oops;
        }
      }
    } else if ( rank > 1 ) { // nD fft
      if ( direction == 1) { // forward fftw
        pnD = fftwnd_create_plan(rank, n, FFTW_FORWARD, FFTW_MEASURE);
      } else if (direction == -1) { // backward fftw
        pnD = fftwnd_create_plan(rank, n, FFTW_BACKWARD, FFTW_MEASURE);
      } else { // die with unrecognized direction
        try{
          unrecognizedDirectionError();
        }
        catch(Oops& oops){
          oops.prepend("WrapFFTW::WrapFFTW: Error:\n"); //OK
          throw oops;
        }
      }
    } else { // die with unrecognized rank
        try{
          unrecognizedRankError();
        }
        catch(Oops& oops){
          oops.prepend("WrapFFTW::WrapFFTW: Error:\n"); //OK
          throw oops;
        }
    }
  } else if ( transformFlag == 1 ) { // real-valued fftw transforms
    if ( rank == 1 ) { // 1D fft
      if ( direction == 1) { // forward fftw
        p1Dr = rfftw_create_plan(n[0], FFTW_FORWARD, FFTW_MEASURE);
      } else if (direction == -1) { // backward fftw
        p1Dr = rfftw_create_plan(n[0], FFTW_BACKWARD, FFTW_MEASURE);
      } else { // die with unrecognized direction
        try{
          unrecognizedDirectionError();
        }
        catch(Oops& oops){
          oops.prepend("WrapFFTW::WrapFFTW: Error:\n"); //OK
          throw oops;
        }
      }
    } else if ( rank > 1 ) { // nD fft
      if ( direction == 1) { // forward fftw
        pnDr = rfftwnd_create_plan(rank, n, FFTW_FORWARD, FFTW_MEASURE);
        //pnDr = rfftw2d_create_plan(n[0], n[1], FFTW_REAL_TO_COMPLEX, FFTW_MEASURE);
      } else if (direction == -1) { // backward fftw
        pnDr = rfftwnd_create_plan(rank, n, FFTW_BACKWARD, FFTW_MEASURE);
      } else { // die with unrecognized direction
        try{
          unrecognizedDirectionError();
        }
        catch(Oops& oops){
          oops.prepend("WrapFFTW::WrapFFTW: Error:\n"); //OK
          throw oops;
        }
      }
    } else { // die with unrecognized rank
      try{
        unrecognizedRankError();
      }
      catch(Oops& oops){
        oops.prepend("WrapFFTW::WrapFFTW: Error:\n"); //OK
        throw oops;
      }

    }
  } else { // die with unrecognized transform flag error
      try{
        unrecognizedTransformFlagError();
      }
      catch(Oops& oops){
        oops.prepend("WrapFFTW::WrapFFTW: Error:\n"); //OK
        throw oops;
      }
  }
}
WrapFFTW::~WrapFFTW() {
  delete [] n;
  //
  // destroy the plans if created
  // 
  if (p1D) 
    fftw_destroy_plan(p1D);     // 1D complex fftw plan 
  if (pnD)
    fftwnd_destroy_plan(pnD);   // nD complex fftw plan  
  if (p1Dr)
    rfftw_destroy_plan(p1Dr);   // 1D real    fftw plan
  if (pnDr)
    rfftwnd_destroy_plan(pnDr); // nD real    fftw plan
  //
  // clean the memory for the data structures if allocated
  // 
  if (rDataIn)
    delete [] rDataIn;
  if (rDataOut) 
    delete [] rDataOut;
  if (cDataIn)
    delete [] cDataIn;
  if (cDataOut) 
    delete [] cDataOut;
}

/**
 * Implement the wrap to the fftw using as input data structures the
 * xoopic Vector3 type. This is a specialization for 2D real forward fft
 * for now. doFFT(...) can be overloaded for other data and
 * fft types.
 */
void WrapFFTW::doFFT(Vector3** reIn, int const component, 
                     Scalar** reOut, Scalar** imOut) 
/* throw(Oops) */{
  if ( transformFlag == 1 && rank == 2 && direction == 1 ) { 
    //
    // allocate memory only for rDataIn and cDataOut (these are sufficient)
    // 
    if ( !rDataIn ) {
      rDataIn = new fftw_real[n[0]*n[1]];
    }
    if ( !cDataOut ) 
      cDataOut = new fftw_complex[n[0]*(n[1]/2 + 1)];

    //
    // map the input data to rDataIn
    // 
    map2DForwardRFFTW(reIn, component);
    // 
    // do the 2D rfftw
    // 
    rfftwnd_one_real_to_complex(pnDr, rDataIn, cDataOut);
    // 
    // map the output data to reOut and imOut
    //
    map2DBackwardRFFTW(reOut, imOut);
  } else {
      stringstream ss (stringstream::in | stringstream::out);
      ss << "WrapFFTW::doFFT: Error: \n"<<
        "Only transformFlag = 1, rank = 2, and direction = 1\n"<< 
        "is implemented for now.\n" << 
        "Extend class WrapFFTW for your own needs.\n" <<endl;

      std::string msg;
      ss >> msg;
      Oops oops(msg);
      throw oops;    // exit() PSDFieldDiag2d::MaintainDiag:  

  }
}
/**
 * helper function to map the space variation of a component of 
 * a Vector3** to the format of the input data for use with the 
 * real fftw 2d. 
 */
void WrapFFTW::map2DForwardRFFTW(Vector3** reIn, const int& component) {
  int c = 0;
  int j, k;
  for ( j = 0; j < n[0]; j++ ) 
   for ( k = 0; k < n[1]; k++ ) 
     rDataIn[c++]   = reIn[j+1][k+1].e(component);
}
/** 
 * helper function to map the 1D data structure used by the
 * 2D real fftw to the corresponding arrays 
 * for the Re and Im parts of the FFT.
 */
void WrapFFTW::map2DBackwardRFFTW(Scalar** reOut, Scalar** imOut) {
  int N1 = n[0];
  int N2 = n[1];
  int j, k, index;
  int N2over2 = N2/2;
  int N2over2plus1 = N2over2+1;
  for ( j = 0; j < N1; j++ ) {
    for ( k = 0; k < N2; k++ ) {
      if ( k > N2over2 ) {
        // 
        // use symmetry and periodicity to assign in this case
        // 
        if ( j == 0 ) 
          index = N2-k;
        else 
          index = (N1-j)*N2over2plus1+(N2-k);
        reOut[j][k] = cDataOut[index].re;
        imOut[j][k] = -cDataOut[index].im;
      } else {                     
        index = j*N2over2plus1+k;  
        reOut[j][k] = cDataOut[index].re;
        imOut[j][k] = cDataOut[index].im;
      }
    }
  }   
}
#endif // HAVE_FFT
