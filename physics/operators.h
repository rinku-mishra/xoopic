#ifndef OPERATORS_H
#define OPERATORS_H

// this class and its inherited classes are implemented to completely
// encapsulate the discretized operator to support more seamless usage,
// e.g., with the inverter classes.

#include "domain.h"

// five point stencil
class Operators {

protected:

  // stencil coefficients
  Scalar *c0_1, *c_10, *c00, *c10, *c01;

  // dimension of operator
  int dimension;

public:
  
  Operators(const int dim=0)
  {
	 dimension = dim;
	 
	 c0_1 = new Scalar[dimension];
	 c_10 = new Scalar[dimension];
	 c00  = new Scalar[dimension];
	 c10  = new Scalar[dimension];
	 c01  = new Scalar[dimension];
	 
	 for (int i=0; i<dimension; i++)
		c0_1[i] = c_10[i] = c00[i] = c10[i] = c01[i] = 0.;
	 
  }

  virtual ~Operators()
  {
	 delete[] c0_1;
	 delete[] c_10;
	 delete[] c00;
	 delete[] c10;
	 delete[] c01;
  }

  int dim() { return dimension; }

  // set stencil coefficients
  virtual void set_coefficients() = 0;

  // apply operator to vector
  virtual Vector<Scalar> apply(const Vector<Scalar>&) = 0;

  // precondition operator
  virtual void precondition(Vector<Scalar>&, const Vector<Scalar>&) = 0;
  
};

#endif
