//  the DADI object, xy geometry, periodic in x1, hardwired.
#ifndef __DADIXYP_H
#define __DADIXYP_H
#include "dadi.h"

#include <oops.h>


class DadixyPer : public Dadi
{
  
 protected:

  void adi(Scalar **uadi, Scalar **s, Scalar del_t);

  void init_solve(Grid *grid,Scalar **epsi)/* throw(Oops) */;

 public:

  int solve(Scalar **u_in, Scalar **s, int itermax,Scalar tol_test);
  int laplace_solve(Scalar **u_in, Scalar **s, int itermax,Scalar tol_test) {
	  return solve(u_in,s,itermax,tol_test);
  }

  DadixyPer(Scalar **epsi,Grid *grid) /* throw(Oops) */:
			  Dadi(epsi,grid) 
				{
          try{ 
				    init_solve(grid,epsi);
          }
          catch(Oops& oops2){
            oops2.prepend("DadixyPer::DadixyPer: Error: \n");//done
            throw oops2;
          }
				}

  void set_coefficient(int j, int k, BCTypes type, Grid *grid) /* throw(Oops) */;
  BCTypes get_coefficient(int j, int k);
		 
  ~DadixyPer();

};
#endif //__DADIXYP_H
