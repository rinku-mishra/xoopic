#ifndef COMPLEX_H
#define COMPLEX_H

#ifdef _MSC_VER
#include <iomanip>
#include <iostream>
using std::ends;
#else

using namespace std;
#endif

#include <cmath>

#include <oops.h>

template<class Real> class Complex {

#ifdef UNIX
  friend ostream &operator<<(ostream& output,const Complex& z)
  {
	 output << "(" << z.re << "," << z.im << ")";
	 return output;
  };
#endif

  friend inline Complex operator+(Real x, const Complex& z)
  { return Complex(x+z.re,z.im); };

  friend inline Complex operator-(Real x, const Complex& z)
  { return Complex(x-z.re,z.im); };

  friend inline Complex operator*(Real x, const Complex& z)
  { return Complex(x*z.re,x*z.im); };

  friend Real csqrt(const Complex &z) 
    /* throw(Oops) */{
	 if (z.im_part()==0)
		return sqrt(z.re_part());
	 else {
     stringstream ss (stringstream::in | stringstream::out);
     ss <<"Complex::csqrt: Error: \n" << 
		  "UNDEFINED: attempting sqrt of complex data type with nonzero imaginary part"
			  << endl;

		 std::string msg;
     ss >> msg;
     Oops oops(msg);
     throw oops;    // exit() not called

	 }
  }

  friend Real fabs(const Complex &z)
  { return sqrt(z.re_part()*z.re_part()+z.im_part()*z.im_part()); }

public:

  inline Complex(Real _re=0,Real _im=0)
  { set_complex(_re,_im); };

  inline Complex(const Complex& z)
  { set_complex(z.re,z.im); };

  inline ~Complex() {};
  
  inline void set_complex(Real _re,Real _im)
  { re=_re; im=_im; };

  inline Real re_part() const
  { return re; };

  inline Real im_part() const
  { return im; };

  inline Complex conjugate() const
  { return Complex(re,-im); };

  inline int operator==(const Complex& z)
  { return ((re==z.re)&&(im==z.im)); };

  inline const Complex &operator=(const Complex &z)
  { re = z.re; im = z.im;
  return *this; }

  inline const Complex &operator=(const Real x)
  { re = x; im = 0;
  return *this; };

  inline const Complex& operator+=(const Complex &z)
  { re += z.re; im += z.im;
  return *this; };

  inline const Complex& operator-=(const Complex &z)
  { re -= z.re; im -= z.im;
  return *this; };
  
  inline Complex operator+(const Complex &z)
  { return Complex(re+z.re,im+z.im); };

  inline Complex operator+(const Real x)
  { return Complex(re+x,im); };

  inline Complex operator-(const Complex &z)
  { return Complex(re-z.re,re-z.im); };

  inline Complex operator-(const Real x)
  { return Complex(re-x,im); };

  inline Complex operator-()
  { return Complex(-re,-im); };

  inline Complex operator*(const Complex &z)
  { return Complex(re*z.re-im*z.im,re*z.im+im*z.re); } ;

  inline Complex operator*(const Real x)
  { return Complex(re*x,im*x); };

private:

  Real re;
  Real im;

};

typedef Complex<float> fComplex;
typedef Complex<double> dComplex;

#endif
