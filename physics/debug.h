#ifndef _DEBUG_H
#define _DEBUG_H

#include "domain.h"
#include <oops.h>

Scalar analytic_epsilon(const Domain* d, const int i, const int j);
Scalar analytic_solution(const Domain* d, const int i, const int j);
Scalar analytic_rhs(const Domain* d, const int i, const int j);
Scalar analytic_derivative(const Domain* d, const int i, const int j) /* throw(Oops) */;

void write_2d_function(const Domain* d, const Vector<Scalar>& x, char* name);

#endif
