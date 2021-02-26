#ifndef DOMAIN_H
#define DOMAIN_H

// Defines grid parameters including setting of grid point types,
// indices, boundary normals for Neumann data, etc.

// Handles arbitrarily oriented boundary normals

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iomanip>
#include <iostream>

using namespace std;

#include "complex.h"
#include "N_vector.h"
#include "precision.h"

#include <oops.h>

class Grid;
typedef enum {INTERIOR, DIRICHLET, NEUMANN, EXTERIOR} Grid_point_type;

class Domain {

#ifdef UNIX
  friend ostream& operator<<(ostream&,const Domain&);
#endif
private:

  // Pointer to grid object from OOPIC
  Grid *grid;

  // Type of grid point
  Grid_point_type **grid_point_type;

  // Specifies each grid point w/ single integer to enable using 1d array
  // Useful for Fortran interfacing and more seamless switching between 2d and 3d.
  int **grid_point_index; 

  // Number of INTERIOR, DIRICHLET, and NEUMANN grid points
  int number_grid_points;
  
  // Grid indices
  int i,j;

  // Normal vectors on domain boundary: (n,grad(u))
  Vector<int> **boundary_normal;

  // horizontal and vertical lengths of encapsulating system
  Scalar length_1, length_2;

  // Number of cells
  int number_cells_1, number_cells_2;

  // True if system is periodic in horizontal dimension
  // Decrement nc1 by 1 if true
  bool PERIODIC_1;

  // uniform grid mesh widths 
  Scalar dx1, dx2;

public:

  Domain(Grid*,Scalar=1.,Scalar=1.,int=1,int=1,bool=false) /* throw(Oops) */;
  ~Domain();

  void set_grid_point(const Grid_point_type=EXTERIOR, 
							 const Vector<int> &v = Vector<int>(0,0));

  inline Grid_point_type type(int i,int j) const { return grid_point_type[i][j]; }
  inline int index(int i,int j)            const { return grid_point_index[i][j]; }
  inline Vector<int> normal(int i, int j)  const { return boundary_normal[i][j]; }
  inline int num()                         const { return number_grid_points; }
  inline int nc1()                         const { return number_cells_1; }
  inline int nc2()                         const { return number_cells_2; }
  inline Scalar length1()                  const { return length_1; }
  inline Scalar length2()                  const { return length_2; }
  inline Scalar uniform_dx1()              const { return dx1; }
  inline Scalar uniform_dx2()              const { return dx2; }
  inline bool PERIODIC1()                  const { return PERIODIC_1; }

  void set_up_domain();
  void set_up_domain_oopic() /* throw(Oops) */;
 
};

#endif





