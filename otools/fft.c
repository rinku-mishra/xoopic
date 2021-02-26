#include <math.h>

#include "fft.h"

#if defined(_MSC_VER)
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#endif

#define SWAP(a, b) tempr=(a); (a)=(b); (b)=tempr

/***************************************************************/
/* Replace `data' by its discrete Fourier transform, if `isign'is
	input as 1, or by its inverse discrete Fourier transform, if 
	`isign' is input as -1. `data' is a complex array of length `nn',
	input as a real array data[1..2*nn]. `nn' MUST be an integer
	power of 2 (this is not checked for!?)  */

void four1(Scalar *data, int nn, int isign)
{
  register int i, j, m, n;
  int  mmax, istep;
  Scalar wtemp, wr, wpr, wpi, wi, theta;
  Scalar tempr, tempi;
  
  n= nn<<1;
  j=1;
  
  for(i=1; i<n; i+=2)
  {
    if(j >i)
    {
      SWAP(data[j], data[i]);
      SWAP(data[j+1], data[i+1]);
    }
    m= n>>1;
    while(m >=2 &&j >m)
    {
      j -=m;
      m >>=1;
    }
    j +=m;
  }
  mmax =2;
  while(n> mmax)
  {
    istep= 2*mmax;
    theta= 6.28318530717959/(isign*mmax);
    wtemp= sin(.5*theta);
    wpr= -2.0*wtemp*wtemp;
    wpi= sin(theta);
    wr= 1.0;
    wi= 0.0;
    for(m=1; m<mmax; m+=2)
    {
      for(i=m; i<n; i+=istep)
      {
	j=i+mmax;
	tempr= wr*data[j]-wi*data[j+1];
	tempi= wr*data[j+1]+wi*data[j];
	data[j]= data[i]- tempr;
	data[j+1]= data[i+1]- tempi;
	data[i] +=tempr;
	data[i+1] +=tempi;
      }
      wr= (wtemp=wr)*wpr - wi*wpi+wr;
      wi= wi*wpr + wtemp*wpi + wi;
    }
    mmax= istep;
  }
}

/***************************************************************/
/* Calculates the Fourier transform of a set of 2n real-valued
	data points. Replaces `data' by the positive frequency half of
	its complex Fourier transform. The real-valued first and last
	components of the complex transform are returnedas elements
	data[1] and data[2] respectively. `n' MUST be a power of 2.
	This routine also calculates the inverse transform of a complex
	data array if it is the transform of real data. (Result in
	this case MUST be divided by `n'.)  */

void realft(Scalar *data, int n, int isign)
{
  register int i, i1, i2, i3, i4, n2p3;
  Scalar c1=0.5, c2, h1r, h1i, h2r, h2i;
  Scalar wr, wi, wpr, wpi, wtemp, theta;
  
  theta= M_PI/(Scalar) n;
  if(isign ==1)
  {
    c2= -0.5;
    four1(data, n, 1);
  }
  else
  {
    c2= 0.5;
    theta= -theta;
  }
  wtemp= sin(0.5*theta);
  wpr= -2.0*wtemp*wtemp;
  wpi= sin(theta);
  wr= 1.0+wpr;
  wi= wpi;
  n2p3= 2*n+3;
  
  for(i=2; i<= n/2; i++)
  {
    i4= 1+(i3=n2p3 -(i2=1 +(i1=i+i-1)));
    h1r= c1*(data[i1] +data[i3]);
    h1i= c1*(data[i2] -data[i4]);
    h2r= -c2*(data[i2] +data[i4]);
    h2i= c2*(data[i1] -data[i3]);
    
    data[i1]= h1r +wr*h2r -wi*h2i;
    data[i2]= h1i +wr*h2i +wi*h2r;
    data[i3]= h1r -wr*h2r +wi*h2i;
    data[i4]= -h1i +wr*h2i +wi*h2r;
    
    wr= (wtemp=wr)*wpr -wi*wpi +wr;
    wi= wi*wpr +wtemp*wpi +wi;
  }
  
  if(isign ==1)
  {
    data[1]= (h1r= data[1]) +data[2];
    data[2]= h1r - data[2];
  }
  else
  {
    data[1]= c1*((h1r=data[1]) +data[2]);
    data[2]= c1*(h1r -data[2]);
    four1(data, n, -1);
  }
}

/***************************************************************/




void sinft(Scalar *y, int n)
{
	int j,m=n/2,n2=n+2;
	Scalar sum,y1,y2;
	Scalar theta,wi=0.0,wr=1.0,wpi,wpr,wtemp;
	void realft();

	theta=3.14159265358979/(Scalar) n;
	wtemp=sin(0.5*theta);
	wpr = -2.0*wtemp*wtemp;
	wpi=sin(theta);
	y[1]=0.0;
	for (j=2;j<=m+1;j++) {
		wr=(wtemp=wr)*wpr-wi*wpi+wr;
		wi=wi*wpr+wtemp*wpi+wi;
		y1=wi*(y[j]+y[n2-j]);
		y2=0.5*(y[j]-y[n2-j]);
		y[j]=y1+y2;
		y[n2-j]=y1-y2;
	}
	realft(y,m,1);
	y[1]*=0.5;
	sum=y[2]=0.0;
	for (j=1;j<=n-1;j+=2) {
		sum += y[j];
		y[j]=y[j+1];
		y[j+1]=sum;
	}
}


void cosft(Scalar *y, int n, int isign)
{
	int j,m,n2;
	Scalar enf0,even,odd,sum,sume,sumo,y1,y2;
	Scalar theta,wi=0.0,wr=1.0,wpi,wpr,wtemp;
	void realft();

	theta=3.14159265358979/(Scalar)n;
	wtemp=sin(0.5*theta);
	wpr = -2.0*wtemp*wtemp;
	wpi=sin(theta);
	sum=y[1];
	m=n >> 1;
	n2=n+2;
	for (j=2;j<=m;j++) {
		wr=(wtemp=wr)*wpr-wi*wpi+wr;
		wi=wi*wpr+wtemp*wpi+wi;
		y1=0.5*(y[j]+y[n2-j]);
		y2=(y[j]-y[n2-j]);
		y[j]=y1-wi*y2;
		y[n2-j]=y1+wi*y2;
		sum += wr*y2;
	}
	realft(y,m,1);
	y[2]=sum;
	for (j=4;j<=n;j+=2) {
		sum += y[j];
		y[j]=sum;
	}
	if (isign == -1) {
		even=y[1];
		odd=y[2];
		for (j=3;j<=n-1;j+=2) {
			even += y[j];
			odd += y[j+1];
		}
		enf0=2.0*(even-odd);
		sumo=y[1]-enf0;
		sume=(2.0*odd/n)-sumo;
		y[1]=0.5*enf0;
		y[2] -= sume;
		for (j=3;j<=n-1;j+=2) {
			y[j] -= sumo;
			y[j+1] -= sume;
		}
	}
}


void twofft(Scalar *data1, Scalar *data2, Scalar *fft1, Scalar *fft2, int n)
{
	int nn3,nn2,jj,j;
	Scalar rep,rem,aip,aim;
	void four1();

	nn3=1+(nn2=2+n+n);
	for (j=1,jj=2;j<=n;j++,jj+=2) {
		fft1[jj-1]=data1[j];
		fft1[jj]=data2[j];
	}
	four1(fft1,n,1);
	fft2[1]=fft1[2];
	fft1[2]=fft2[2]=0.0;
	for (j=3;j<=n+1;j+=2) {
		rep=0.5*(fft1[j]+fft1[nn2-j]);
		rem=0.5*(fft1[j]-fft1[nn2-j]);
		aip=0.5*(fft1[j+1]+fft1[nn3-j]);
		aim=0.5*(fft1[j+1]-fft1[nn3-j]);
		fft1[j]=rep;
		fft1[j+1]=aim;
		fft1[nn2-j]=rep;
		fft1[nn3-j] = -aim;
		fft2[j]=aip;
		fft2[j+1] = -rem;
		fft2[nn2-j]=aip;
		fft2[nn3-j]=rem;
	}
}



 
void fourn(Scalar *data, int *nn, int ndim, int isign)
{
	int i1,i2,i3,i2rev,i3rev,ip1,ip2,ip3,ifp1,ifp2;
	int ibit,idim,k1,k2,n,nprev,nrem,ntot;
	Scalar tempi,tempr;
	Scalar theta,wi,wpi,wpr,wr,wtemp;

	ntot=1;
	for (idim=1;idim<=ndim;idim++)
		ntot *= nn[idim];
	nprev=1;
	for (idim=ndim;idim>=1;idim--) {
		n=nn[idim];
		nrem=ntot/(n*nprev);
		ip1=nprev << 1;
		ip2=ip1*n;
		ip3=ip2*nrem;
		i2rev=1;
		for (i2=1;i2<=ip2;i2+=ip1) {
			if (i2 < i2rev) {
				for (i1=i2;i1<=i2+ip1-2;i1+=2) {
					for (i3=i1;i3<=ip3;i3+=ip2) {
						i3rev=i2rev+i3-i2;
						SWAP(data[i3],data[i3rev]);
						SWAP(data[i3+1],data[i3rev+1]);
					}
				}
			}
			ibit=ip2 >> 1;
			while (ibit >= ip1 && i2rev > ibit) {
				i2rev -= ibit;
				ibit >>= 1;
			}
			i2rev += ibit;
		}
		ifp1=ip1;
		while (ifp1 < ip2) {
			ifp2=ifp1 << 1;
			theta=isign*6.28318530717959/(ifp2/ip1);
			wtemp=sin(0.5*theta);
			wpr = -2.0*wtemp*wtemp;
			wpi=sin(theta);
			wr=1.0;
			wi=0.0;
			for (i3=1;i3<=ifp1;i3+=ip1) {
				for (i1=i3;i1<=i3+ip1-2;i1+=2) {
					for (i2=i1;i2<=ip3;i2+=ifp2) {
						k1=i2;
						k2=k1+ifp1;
						tempr=wr*data[k2]-wi*data[k2+1];
						tempi=wr*data[k2+1]+wi*data[k2];
						data[k2]=data[k1]-tempr;
						data[k2+1]=data[k1+1]-tempi;
						data[k1] += tempr;
						data[k1+1] += tempi;
					}
				}
				wr=(wtemp=wr)*wpr-wi*wpi+wr;
				wi=wi*wpr+wtemp*wpi+wi;
			}
			ifp1=ifp2;
		}
		nprev *= n;
	}
}

/*  Window function:  
The following function is basically a triangular window
for transforming data before giving it to an FFT.  It
reduces the amount of bleeding into adjacent bins in
the FFT diagram.

The one used here transforms the dataset by a triangle-function.

*/

void triangle_window(Scalar *data,int N) {
  int i;

  for(i=0;i<N;i++) 
	 data[i]*= 1 - fabs( ( i - 0.5 * ( N-1))/ (0.5 * (N+1)));
}
  
