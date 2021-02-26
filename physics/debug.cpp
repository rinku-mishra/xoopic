
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if defined(_MSC_VER)
#include <iostream>
#else

#endif

#include "debug.h"
#include "misc.h"

#include <cstdlib>
#include <cmath>
using namespace std;

Scalar analytic_epsilon(const Domain* d, const int i, const int j)
{
  return 1.;
}

// 2d analytic solutions u to -del(eps(del(u))=f
Scalar analytic_solution(const Domain* d, const int i, const int j)
{
  Scalar dx1 = d->uniform_dx1();
  Scalar dx2 = d->uniform_dx2();

  Scalar x1 = (Scalar) i*dx1;
  Scalar x2 = (Scalar) j*dx2;
 
  return cos(2*M_PI*x1)*cos(2*M_PI*x2);
  //return exp(-x1)*cos(M_PI*x2);
}

Scalar analytic_rhs(const Domain* d, const int i, const int j)
{
  return 8*(M_PI)*(M_PI)*analytic_solution(d,i,j);
  //return (1+M_PI*M_PI)*analytic_solution(d,i,j);
}

Scalar analytic_derivative(const Domain* d, const int i, const int j)
  /* throw(Oops) */{
  Scalar dx1 = d->uniform_dx1();
  Scalar dx2 = d->uniform_dx2();

  Scalar x1 = (Scalar) i*dx1;
  Scalar x2 = (Scalar) j*dx2;

  Scalar partial_x1 = 2*M_PI*sin(2*M_PI*x1)*cos(2*M_PI*x2);
  Scalar partial_x2 = 2*M_PI*cos(2*M_PI*x1)*sin(2*M_PI*x2);

  if (d->normal(i,j)[0]==1)
	 return partial_x1;
 
  if (d->normal(i,j)[0]==-1)
	 return -partial_x1;

  if (d->normal(i,j)[1]==1)
	 return partial_x2;

  if (d->normal(i,j)[1]==-1)
	 return -partial_x2;

  else{
	  
    stringstream ss (stringstream::in | stringstream::out);
    ss <<"debug::analytic_derivative: Error: \n"<<
      "not a Neumann Point!"<<endl;
    std::string msg;
    ss >> msg;
    Oops oops(msg);
    throw oops;    // exit() nobody calls this.

  }
}

void write_2d_function(const Domain* d, const Vector<Scalar>& x, char* name)
{
  FILE *function;
  function = fopen(name,"w");

  Scalar dx1 = d->uniform_dx1();
  Scalar dx2 = d->uniform_dx2();
  
  for (int j=0; j<=d->nc2(); j++) {
    for (int i=0; i<=d->nc1(); i++) {
		Scalar x1 = (Scalar) i*dx1;
		Scalar x2 = (Scalar) j*dx2;
		if (d->type(i,j)!=EXTERIOR)
		  fprintf(function,"%e %e %e\n",x1,x2,x[d->index(i,j)]);
		else
		  fprintf(function,"%e %e %e\n",x1,x2,0.);
	 }
	 fprintf(function,"\n");
  }

  fclose(function);

}

