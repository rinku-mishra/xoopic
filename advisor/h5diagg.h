//h5diagg.h

#ifndef   __H5DIAGG_H
#define   __H5DIAGG_H

#ifdef UNIX
#include <config.h>
#endif

#include "part1g.h"


//===================DiagParams  Class
// base class for all diagnostics
// abstract

class H5DiagParams : public ParameterGroup
{protected:



public:
  
  StringParameter VarName;
  StringParameter fileName;
  IntParameter dumpPeriod;

 /** 
  * The variables:
  *VarName -- name of diagnostic to be dumped
  *dumpPeriod
  *fileName
  */
 

  H5DiagParams();
  //  since boundaries are all placed on grid, this class needs
  //  to know the dimensions of the grid
  
  virtual ~H5DiagParams() {};
  
};                                       

#endif  //  __H5DIAGG_H

