#include "gmres.h"
#include <stdio.h>

// Y. Saad and M. Schultz, "GMRES: A generalized minimum residual algorithm for
// solving nonsymmetric linear systems", SIAM J. Scientific and Statistical 
// Computing, vol. 7, p. 859-869 (1986)
 
// matrix A may be nonsymmetric; if A is positive definite, then garunteed to 
// converge for "reasonable" subspace dimension 

GMRES::GMRES(Domain* dom, Operators* op, int restart) 
  : Inverter(dom,op)
{
  // dimension of Krylov subspace; default m = 7
  m = restart;

  // basis elements for Krylov subspace with dimension = m (=m+1?)
  basis_element = new Vector<Scalar>[m+1]/*(A->dim())*/; //BROKEN

  // Hessenberg matrix with dimension = (m+1)x(m) 
  h = new Scalar *[m+1];
  for (int i=0; i<m+1; i++)
    h[i] = new Scalar[m];

  // cosine elements in Givens rotation 
  c = new Scalar[m];

  // sine elements in Givens rotation
  s = new Scalar[m];
 
  // Krylov subspace expansion coefficients for solution
  alpha = new Scalar[m];
 
  // can use beta in place of alpha to save storage
  beta = new Scalar[m+1];
}

GMRES::~GMRES()
{
  for (int i=0; i<=m; i++) 
	 delete[] h[i];
  
  delete[] basis_element;
  delete[] h;
  delete[] c;
  delete[] s;
  delete[] alpha;
  delete[] beta;
}

void GMRES::invert(Vector<Scalar>& x, const Vector<Scalar>& b,
						 Scalar tol, int maxIter)
{
  Scalar tmp;

  // compute normalization factor for convergence test
  Scalar b_norm = (Scalar) b.l2_norm(); // consider using L2 norm instead of l2
  if (b_norm<=macheps)
    b_norm=1.;

  // begin main iteration loop 
  int numIter;
  int restart=0;
  for (numIter=1; numIter<maxIter; numIter++) {
 
    // generate initial basis element
	 basis_element[0] = b - A->apply(x);

	 // solution found if residual = 0
	 if (basis_element[0].l2_norm() < tol*b_norm) {

#ifdef DEBUG_INVERTER
		printf("no iterations...\n");
		printf("Inverter=GMRES(%d) numIter=%d beta[%d]=%e b_norm=%e \n",
				 m,numIter-1,restart,beta[restart],b_norm);
#endif

		return;
	 }
	 
    // precondition
	 A->precondition(basis_element[0],basis_element[0]);
	 
	 beta[0] = basis_element[0].l2_norm();

	 // Check placement -- should it go after normalization in next step?
	 //	 if (fabs(beta[0]) < tol *b_norm)
	 //      break;

    // normalize initial basis element
	 basis_element[0] = (1./beta[0])*basis_element[0];

    restart = m;
	 int j;
    for (j=0; j<restart; j++) {                                                       
      // generate new basis element
		basis_element[j+1] = A->apply(basis_element[j]);
      
      // precondition
		A->precondition(basis_element[j+1],basis_element[j+1]);

		// begin modified Gram-Schmidt
		// consider using Householder reflectors for high condition number systems
				
		// orthogonalize basis elements 
		int i;
		for (i=0; i<=j; i++) {
		  
		  // calculate orthogonal projections -- these form Hessenberg matrix elements 
		  h[i][j] = basis_element[j+1].euclidean_inner_product(basis_element[i]);
		  
		  basis_element[j+1] += -h[i][j]*basis_element[i]; 
		}

		// end modified Gram-Schmidt

		h[j+1][j] = basis_element[j+1].l2_norm();

      // If h[j+1][j] < tol then solution found since h[j+1][j]=0 
      // => s[j]=0 => beta[j+1]=0 and beta[j+1]=norm(residual[j+1])
		// Check placement -- should go after normalization in next step?
      if (fabs(h[j+1][j]) < tol  *b_norm   ) {

#ifdef DEBUG_INVERTER
		  printf("fabs(h[%d][%d])=%e\n",j+1,j,h[j+1][j]);
#endif

		  // printf("| h[j+1][j] | < tol * b_norm\n");
		  restart = j+1;
		  break;
      }

      // normalize new basis element
		basis_element[j+1] = (1./h[j+1][j])*basis_element[j+1];

		// transform new column of Hessenberg matrix via Givens rotation 
		for ( i=0; i<=j-1; i++) {  //BROKEN
		  tmp = h[i][j];
		  h[i][j] = c[i]*tmp +s[i]*h[i+1][j];
		  h[i+1][j] = -s[i]*tmp +c[i]*h[i+1][j];
      }

      // construct new Givens rotation 
      tmp = sqrt (h[j][j]*h[j][j] +h[j+1][j]*h[j+1][j]);
      c[j] = h[j][j]/tmp;
      s[j] = h[j+1][j]/tmp;
      
      // operate with new Givens rotation
      h[j][j] = tmp; 
      h[j+1][j] = 0.;
      tmp = beta[j];
      beta[j] = c[j]*tmp;
      beta[j+1] = -s[j]*tmp;

      // if beta[j+1]<tol then solution found since beta[j+1]=norm(residual[j+1])
      if (fabs(beta[j+1]) < tol*b_norm   ) {

#ifdef DEBUG_INVERTER
		  printf("subspace reduction...\n");
		  //		  printf("     fabs(beta[%d])=%e\n",j+1,beta[j+1]);
#endif

		  restart = j+1;                                                    
		  break;
      }
    
    }
	 
    // back solve for the alpha's 
    for (j=restart-1; j>=0; j--) {
      tmp = 0.;
      for (int i=j+1; i<=restart-1; i++)
		  tmp += h[j][i]*alpha[i];
      alpha[j] = (beta[j]-tmp)/h[j][j];
    }
	 
	 // construct new solution as linear combination of basis elements with alpha weights
	 for (int i=0; i<restart; i++)
		x += alpha[i]*basis_element[i];

    if (fabs(beta[restart]) < tol*b_norm  ) 
      break;

	 
  } // end of main iteration loop 

#ifdef DEBUG_INVERTER

//   check if basis elements form orthonormal set 
//   for (int i=0; i<restart; i++)
// 	 for (int j=i; j<restart; j++) {
// 		Scalar prod = basis_element[i].euclidean_inner_product(basis_element[j]);
// 		printf("Inner product (w[%d],w[%d])=%e\n",i,j,prod);
// 	 }
//   printf("\n");

  printf("Inverter=GMRES(%d) numIter=%d beta[%d]=%e b_norm=%e \n",
			m,numIter,restart,beta[restart],b_norm);

#endif
	
}











