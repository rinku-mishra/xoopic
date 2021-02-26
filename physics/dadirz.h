//  the DADI object
#ifndef __DADIRZ_H
#define __DADIRZ_H

#include "dadi.h"

class Dadirz : public Dadi
{
  
 protected:

  void adi(Scalar **uadi, Scalar **s, Scalar del_t);

  void init_solve( Grid *grid,Scalar **epsi);

 public:

  int solve(Scalar **u_in, Scalar **s, int itermax,Scalar tol_test);
  virtual int laplace_solve(Scalar **u_in, Scalar **s, int itermax,Scalar tol_test) {
	  return solve(u_in,s,itermax,tol_test);
  }
  Dadirz(Scalar **epsi,Grid *grid) : 
						 Dadi(epsi,grid)
							{
							  init_solve(grid,epsi);
							};

  void set_coefficient(int j, int k, BCTypes type, Grid *grid);
  BCTypes get_coefficient(int j, int k);

  ~Dadirz();
};

#endif //__DADIRZ_H
