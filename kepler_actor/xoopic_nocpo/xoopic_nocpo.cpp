#include "UALClasses.h"
#include <stdio.h>
#include <string>
#include <stdlib.h>


void xoopic_nocpo(char *inWorkDir, char *inXOOPICDirectory, char *inScriptName, int &outOutput) {
  std::string inWD(inWorkDir);
  std::string inXOOPIC(inXOOPICDirectory);
  std::string inScript(inScriptName);
  std::string comm;
  
  // compose the command
  comm = "cd " + inWD + " ; " + inXOOPIC;

  if( inXOOPIC.substr(inXOOPIC.size()-1) != "/" ) {
    // there is no slash at the end of PyPD1 directory; add one to command
    comm += "/";
  }

  comm += "xoopic_kepler.sh " + inScript;
  
  // call command and run the script
  outOutput = system(comm.c_str());
  
  if( outOutput > 255 ) {
    outOutput = outOutput % 255;
  }
}
