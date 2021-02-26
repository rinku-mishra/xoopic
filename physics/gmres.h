#ifndef _GMRES_H
#define _GMRES_H

// Y. Saad and M. Schultz, "GMRES: A generalized minimum residual algorithm for
// solving nonsymmetric linear systems", SIAM J. Scientific and Statistical 
// Computing, vol. 7, p. 859-869 (1986)
 
// matrix A may be nonsymmetric; if A is positive definite, then garunteed to 
// converge for "reasonable" subspace dimension 

#include "inverter.h"

class GMRES : public Inverter {

public:

  GMRES(Domain*,Operators*,int=7);
  ~GMRES(); 

  void set_restart(int restart) { m = restart; }
  int get_restart() { return m; }

  void invert(Vector<Scalar>&, const Vector<Scalar>&, Scalar=1e-10, int=200);

private:

  // dimension of Krylov subspace; default is 7
  int m;

  // N-dimensional basis elements for m-dimensional Krylov subspace 
  Vector<Scalar> *basis_element;
  
  // (m+1)x(m)-dimensional Hessenberg matrix 
  Scalar **h;

  // cosine elements in Givens rotation
  Scalar *c;

  // sine elements in Givens rotation
  Scalar *s;

  // Krylov subspace expansion coefficients for solution
  Scalar *alpha;

  // can use beta in place of alpha to save storage
  Scalar *beta;

};

#endif










