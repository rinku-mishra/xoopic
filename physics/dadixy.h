//  the DADI object
#ifndef __DADIXY_H
#define __DADIXY_H
#include "dadi.h"
class Dadixy : public Dadi
{
  
 protected:

  void adi(Scalar **uadi, Scalar **s, Scalar del_t);

  void init_solve(Grid *grid,Scalar **epsi);

 public:

  int solve(Scalar **u_in, Scalar **s, int itermax,Scalar tol_test);
  virtual int laplace_solve(Scalar **u_in, Scalar **s, int itermax,Scalar tol_test) {
	 return  solve(u_in,s,itermax,tol_test);
  }
  Dadixy(Scalar **epsi,Grid *grid):
			  Dadi(epsi,grid) 
				 {
					init_solve(grid,epsi);
				 }

  void set_coefficient(int j, int k, BCTypes type, Grid *grid);
  BCTypes get_coefficient(int j, int k);
		 
  ~Dadixy();

};
#endif //__DADIXY_H
