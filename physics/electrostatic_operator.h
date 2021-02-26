#ifndef ELECTROSTATIC_OPERATOR_H
#define ELECTROSTATIC_OPERATOR_H

#include "operators.h"

// operator from Poisson's equation: del(eps(del(phi))=-rho
class Electrostatic_Operator : public Operators {

private:

  Domain* d;

  // dielectric material
  Scalar **eps;

  // relaxation parameter for symmetric SOR
  Scalar omega;

public:
  
  Electrostatic_Operator(Domain* dom, Scalar** epsilon) : Operators(dom->num())
  {
	 d = dom;
	 eps = epsilon;

	 set_coefficients();
  }

  ~Electrostatic_Operator() {}

  virtual void set_coefficients();

  virtual Vector<Scalar> apply(const Vector<Scalar>&);

  // symmetric SOR with variable stencil coefficients
  virtual void precondition(Vector<Scalar>&, const Vector<Scalar>&);

  // symmetric SOR with constant stencil coefficients
  void precondition_const_coeff(Vector<Scalar>&, const Vector<Scalar>&);
  
};

#endif
