#ifndef __fft_h
#define __fft_h

/*//====================================================================*/
/*

fft.h

0.9	(JohnV 01-11-96) Added for definitions.

*/
/*//====================================================================*/

#include "precision.h"

#ifdef __cplusplus
extern "C" {
#endif
void four1(Scalar *data, int nn, int isign);
void realft(Scalar *data, int n, int isign);
void sinft(Scalar *y, int n);
void cosft(Scalar *y, int n, int isign);
void twofft(Scalar *data1, Scalar *data2, Scalar *fft1, Scalar *fft2, int n);
void fourn(Scalar *data, int *nn, int ndim, int isign);
#ifdef __cplusplus
}
#endif

#endif
