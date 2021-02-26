//gridg.h

#ifndef   __GRIDG_H
#define   __GRIDG_H

#include "part1g.h"

//=================== GridParams Class
// class for specifying the computational grid

class GridParams :public ParameterGroup
{IntParameter J;
 IntParameter K;
 ScalarParameter x1s;
 ScalarParameter x1f;
 ScalarParameter n1;
 ScalarParameter x2s;
 ScalarParameter x2f;
 ScalarParameter n2;
 StringParameter dx1;
 StringParameter dx2;
 IntParameter PeriodicFlagX1;
 IntParameter PeriodicFlagX2;
 IntParameter Geometry;  //  which geometry, RZ, XY, or even R-Theta

// storage used by GUI
protected:
	Vector2** X;

public:
	  GridParams();

	  ~GridParams();

 int getJ() {return J.getValue();}
 int getK() {return K.getValue();}
 
 int getPeriodicFlagX1() {return PeriodicFlagX1.getValue();}
 int getPeriodicFlagX2() {return PeriodicFlagX2.getValue();}

	  Scalar getX1s() {return x1s.getValue();}
	  Scalar getX1f() {return x1f.getValue();}
	  Scalar getX2s() {return x2s.getValue();}
	  Scalar getX2f() {return x2f.getValue();}

	  Scalar mapping_function(Scalar x, Scalar x1, Scalar x2, Scalar n);

	  Grid* CreateCounterPart();
#ifdef MPI_VERSION
     Grid* CreateCounterPart(const ostring &MPIpartition);
#endif /*MPI_VERSION */
public:
     /**
      * Due to problems with conversion from double to float and back to double
      * in the process of initializing the cell vertices when OOPIC is run in 
      * parallel, I'm changing the signature of the 
      * Vector2** createCellVertices(); member function to make sure that the
      * same deltaX is used in each region. The new signature has the beginning
      * of the region's x1 coordinates and the deltaX1. The latter
      * will be calculated in the same way on all processes. All arguments 
      * become of type Scalar as well for consistent handling of the float
      * and double types.
      * dad, Fri May  3 2002.
      */
      /*
       * Removed all hard-coded floats from the code, so conversion from
       * double to float to double should no longer happen. RT, 2003/12/09
       */
	Vector2** createCellVertices(Scalar x1min, Scalar deltaX1);
	Vector2** getCellVertices();
	void deleteCellVertices();
};



#endif  //  __GRIDG_H
