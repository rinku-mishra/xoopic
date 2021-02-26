//h5diagg.cpp

#include "h5diagg.h"

     
//=================== DiagParams Class


H5DiagParams::H5DiagParams()
              : ParameterGroup()
{
  name = "H5Diagnostics";
  
  
  VarName.setNameAndDescription("VarName",  "Name of Variable to be plotted");
  VarName.setValue("NULL");
  parameterList.add(&VarName);
  
  dumpPeriod.setNameAndDescription("dumpPeriod",  "Period of diagnostic dump.");
  dumpPeriod.setValue("0");  // value of zero => dump at same times as dump file
  parameterList.add(&dumpPeriod);
  
  fileName.setNameAndDescription("fileName",  "Name of diagnostic HDF5 dump file.");
  fileName.setValue("Diagnostics");  // will later set this to be <inputfilebase>.h5
  parameterList.add(&fileName);
  
};



