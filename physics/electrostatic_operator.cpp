#include "electrostatic_operator.h"
#include <cmath>
#ifndef M_PI
#define M_PI 3.141592654
#endif


#ifdef _MSC_VER
using std::cout;
using std::cerr;
using std::endl;
#endif

// operator discretized on Cartesian grid with uniform mesh widths
void Electrostatic_Operator::set_coefficients()
{
  int J = d->nc2();
  int I = d->nc1();

  Scalar dx1 = d->uniform_dx1();
  Scalar dx2 = d->uniform_dx2();
  Scalar idx1  = 1./dx1;
  Scalar idx2  = 1./dx2;
  Scalar idx12 = 1./(dx1*dx1);
  Scalar idx22 = 1./(dx2*dx2);
  Scalar diag  = 2.*(idx12+idx22);

  // optimal for recantangular boundaries with purely Dirichlet data
  omega = 2./(1.+M_PI*sqrt(2./(idx12+idx22)));

  for (int j=0; j<=J; j++)
	 for (int i=0; i<=I; i++)
		{
		  const Grid_point_type type = d->type(i,j);
		  const int             gp   = d->index(i,j);

		  // symmetric positive definite Laplacian 
		  // second-order accurate centered difference
		  if (type==INTERIOR) 
			 {
				c00 [gp] = diag;
				//c00[gp]=idx12*(eps[i+1][j]+eps[i-1][j])+
				//        idx22*(eps[i][j+1]+eps[i][j-1]);

				c01 [gp] = -idx22;
				//c01[gp]=-idx22*eps[i][j+1];

				c0_1[gp] = -idx22;
				//c0_1[gp]=-idx22*eps[i][j-1];

				c10 [gp] = -idx12;
				//c10[gp]=-idx12*eps[i+1][j];
				
				c_10[gp] = -idx12;
				//c_10[gp]=-idx12*eps[i-1][j];
			 }

		  // normal derivative on boundary
		  // nonsymmetric second-order accurate 3-point forward/backward differences
		  // maintains positive definiteness ??
		  else if (type==NEUMANN)
			 {
				if (d->normal(i,j)[1]==0)
				  {
					 c00[gp] = -1.5*idx1;
					 c10[gp] = 2.*idx1;          
					 c01[gp] = -0.5*idx1;        
				  }
				else if (d->normal(i,j)[0]==0)
				  {
					 c00[gp] = -1.5*idx2;
					 c10[gp] = 2.*idx2;          
					 c01[gp] = -0.5*idx2;        
				  }
				else
				  cout << "invalid boundary normal at ("
						 << i << "," << j << ")" << endl;
			 }
		}
}


Vector<Scalar> Electrostatic_Operator::apply(const Vector<Scalar>& x)
//void Electrostatic_Operator::apply(const Vector<Scalar>& x, Vector<Scalar>& Ax )
{
  Vector<Scalar> Ax(dimension);

  int J = d->nc2();
  int I = d->nc1();
  int periodic1 = d->PERIODIC1();

  int i_plus, i_minus;
  register int j, i;

  for (j=0; j<=J; j++)
	 for (i=0; i<=I; i++)
		{
		  const Grid_point_type type = d->type(i,j);
		  const int             gp   = d->index(i,j);

		  switch (type)
			 {
			 case INTERIOR:
				i_minus = i-1; i_plus = i+1;
				if (periodic1) {
				  if (i==0) {
					 i_minus = I;
				  } else if (i==I) {
					 i_plus = 0;
				  }
				}

				Ax[gp] = c0_1[gp] * x[ d->index(i      ,j-1) ] 
			           +c_10[gp] * x[ d->index(i_minus,j  ) ] 
				        +c00 [gp] * x[ gp ] 
				        +c10 [gp] * x[ d->index(i_plus, j  ) ] 
				        +c01 [gp] * x[ d->index(i,      j+1) ];
				break;
			 case DIRICHLET:
				Ax[gp] = x[gp];
				break;
			 case NEUMANN:
				int n0, n1;
				n0 = d->normal(i,j)[0];
				n1 = d->normal(i,j)[1];
				Ax[gp] = c00[gp] * x[ gp ] 
				        +c10[gp] * x[ d->index(i+  n0, j+  n1) ] 
				        +c01[gp] * x[ d->index(i+2*n0, j+2*n1) ];
				break;
			 default:
				{} // EXTERIOR point
			 }
		}
  
  return Ax;
}

// symmetric SOR with variable stencil coefficients
void Electrostatic_Operator::precondition(Vector<Scalar> &z, const Vector<Scalar> &r)
{
  int J = d->nc2();
  int I = d->nc1();
  int periodic1 = d->PERIODIC1();
 
  register int j, i;

  // natural order sweep over lower triangular factor
  for (j=0; j<=J; j++) 
	 for (i=0; i<=I; i++)
		if (d->type(i,j)==INTERIOR) 
		  {
			 int gp = d->index(i,j);

			 Scalar tmp = r[gp];
			 if (d->type(i,j-1)==INTERIOR) 
				tmp -= omega*c0_1[gp]*z[d->index(i,j-1)];
			 if (i>0) {
				if (d->type(i-1,j)==INTERIOR) 
				  tmp -= omega*c_10[gp]*z[d->index(i-1,j)];
				if ((i==I)&&(periodic1)) 
				  tmp -= omega*c10[gp]*z[d->index(0,j)];
			 }
			 z[gp] = tmp/c00[gp]; 
		  }
  
  // reverse order sweep over upper triangular factor
  for (j=J; j>=0; j--)
	 for (i=I; i>=0; i--)
		if (d->type(i,j)==INTERIOR) 
		  {
			 int gp = d->index(i,j);

			 Scalar tmp = z[gp];
			 if (d->type(i,j+1)==INTERIOR) 
				tmp -= omega*c01[gp]*z[d->index(i,j+1)];
			 if (i<I) {
				if (d->type(i+1,j)==INTERIOR) 
				  tmp -= omega*c10[gp]*z[d->index(i+1,j)];
				if ((i==0)&&(periodic1)) 
				  tmp -= omega*c_10[gp]*z[d->index(I,j)];
			 }
			 z[gp] = tmp/c00[gp]; 
		  }
		  
}

// symmetric SOR with constant stencil coefficients
void Electrostatic_Operator::precondition_const_coeff(Vector<Scalar> &z, const Vector<Scalar> &r)
{
  int J = d->nc2();
  int I = d->nc1();
  int periodic1 = d->PERIODIC1();
 
//   Scalar dx1;
//   if (periodic1)
//	    dx1 = (Scalar) d->length1()/(I+1);
//   else
// 	 dx1 = (Scalar) d->length1()/I;
//   Scalar dx2 = (Scalar) d->length2()/J;
//   Scalar idx12 = 1./(dx1*dx1);
//   Scalar idx22 = 1./(dx2*dx2);
//   Scalar idiag = 1./(2.*(idx12+idx22));
  
//   Scalar c10_tilde = omega*idx12;
//   Scalar c01_tilde = omega*idx22;

  register int j, i;

  // natural order sweep over lower triangular factor
  for (j=0; j<=J; j++) 
	 for (i=0; i<=I; i++)
		if (d->type(i,j)==INTERIOR) 
		  {
			 int gp = d->index(i,j);

			 Scalar tmp = r[gp];
			 if (d->type(i,j-1)==INTERIOR) {
				//tmp += c01_tilde*z[d->index(i,j-1)];
				tmp -= omega*c0_1[gp]*z[d->index(i,j-1)];
			 }
			 if (i>0) {
				if (d->type(i-1,j)==INTERIOR) {
				  // tmp += c10_tilde*z[d->index(i-1,j)];
				  tmp -= omega*c_10[gp]*z[d->index(i-1,j)];
				}
				if ((i==I)&&(periodic1)) {
				  //tmp += c10_tilde*z[d->index(0,j)];
				  tmp -= omega*c10[gp]*z[d->index(0,j)];
				}
			 }
			 //z[gp] = idiag*tmp;
			 z[gp] = tmp/c00[gp];
		  }
		
  // reverse order sweep over upper triangular factor
  for (j=J; j>=0; j--)
	 for (i=I; i>=0; i--)
		if (d->type(i,j)==INTERIOR) 
		  {
			 int gp = d->index(i,j);

			 Scalar tmp = z[gp];
			 if (d->type(i,j+1)==INTERIOR) {
				//tmp += c01_tilde*z[d->index(i,j+1)];
				tmp -= omega*c01[gp]*z[d->index(i,j+1)];
			 }
			 if (i<I) {
				if (d->type(i+1,j)==INTERIOR) {
				  //tmp += c10_tilde*z[d->index(i+1,j)];
				  tmp -= omega*c10[gp]*z[d->index(i+1,j)];
				}
				if ((i==0)&&(periodic1)) {
				  //tmp += c10_tilde*z[d->index(I,j)];
				  tmp -= omega*c_10[gp]*z[d->index(I,j)];
				}
			 }
			 //z[gp] = idiag*tmp;
			 z[gp] = tmp/c00[gp];
		  }
		  
}
