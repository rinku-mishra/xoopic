#include "cg.h"
#include <stdio.h>

// conjugate gradient method 
// matrix A must be symmetric and positive definite 

Conjugate_Gradient::Conjugate_Gradient(Domain* dom, Operators* op) 
  : Inverter(dom,op)
{
  int N = A->dim();

  // residual 
  r.redimensionalize(N);
  
  // search direction 
  p.redimensionalize(N);
  
  // matrix-vector product Ap
  q.redimensionalize(N);
  
  // solution to BB^Tz=r where B is approximate lower triangular factor of matrix 
  // note: Symmetric, positive definite matrices such as A admit Cholesky 
  //  decomposition A = LL^T where matrix L is nonsingular lower triangular 
  z.redimensionalize(N);
}

void Conjugate_Gradient::invert(Vector<Scalar>& x, const Vector<Scalar>& b,
										  Scalar tol, int maxIter)
{
  // compute normalization factor for convergence test
  Scalar b_norm = (Scalar) b.l2_norm(); // consider using L2 norm instead
  if (b_norm<=macheps)
    b_norm=1.;

  // number of CG iterations
  int numIter = 0;
   
  // compute initial residual
  r = b-A->apply(x); 

  // solution found if residual = 0
  if (r.l2_norm() < tol*b_norm) {

#ifdef DEBUG_INVERTER
	 printf("Inverter=CG numIter=%d r.l2_norm()=%e b_norm=%e \n",
			  numIter,r.l2_norm(),b_norm);
#endif

	 return;
  }

  // z=r; // turn off preconditioner 
  A->precondition(z,r); // turn on preconditioner
   
  Scalar rz = r.euclidean_inner_product(z);
  
  // set initial search direction equal to initial z
  p = z;
  
  // compute initial q
  q = A->apply(z); 

  // begin main iteration loop
  for (numIter=1; numIter<=maxIter; numIter++) {

    // update solution and compute residual
    Scalar alpha = rz/p.euclidean_inner_product(q); 
	 x +=  alpha*p; 
	 r += -alpha*q; 
	
	 // z=r; // turn off preconditioner 
	 A->precondition(z,r); // turn on preconditioner
	     
    Scalar rz_prev = rz;
    rz = r.euclidean_inner_product(z);
    
    // update search direction 
    Scalar beta = rz/rz_prev;
	 q = A->apply(z)+beta*q;
	 p = z+beta*p; 
	 
    if (r.l2_norm() < tol *b_norm) 
		break;
	
  } // end main iteration loop

#ifdef DEBUG_INVERTER
  printf("Inverter=CG numIter=%d r.l2_norm()=%e b_norm=%e \n",
			numIter,r.l2_norm(),b_norm);
#endif
  
}





