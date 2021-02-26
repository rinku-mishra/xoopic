#ifndef _CG_H
#define _CG_H

#include "inverter.h"

class Conjugate_Gradient : public Inverter {

public:

  Conjugate_Gradient(Domain*,Operators*);
  ~Conjugate_Gradient() {}

  virtual void invert(Vector<Scalar>&, const Vector<Scalar>&, 
							 Scalar=1e-6, int=200);

private:

  // residual 
  Vector<Scalar> r;
  
  // search direction 
  Vector<Scalar> p;
  
  // matrix-vector product Ap 
  Vector<Scalar> q;
  
  // solution to BB^Tz=r where B is approximate lower triangular factor of matrix 
  // note: Symmetric, positive definite matrices such as A admit Cholesky 
  // decomposition A = LL^T where matrix L is nonsingular lower triangular 
  Vector<Scalar> z;

};

#endif



