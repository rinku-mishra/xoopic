//diags.h
#ifndef DIAGS_H
#define DIAGS_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "part1g.h"
#include "spatialg.h"

class DiagnosticParams : public ParameterGroup
{
protected:
 GridParams* GP;
 IntParameter j1;
 IntParameter k1;

public:

	  DiagnosticParams();

	  DiagnosticParams(GridParams* _GP);
	  //  since diagnostics are all placed on grid, this class needs
	  //  to know the dimensions of the grid

	  virtual ~DiagnosticParams() {};

	  void setGrid(GridParams* _GP)
		{GP = _GP;}

	  virtual void checkRules();
	  // checks all rules associated with the class

	  virtual void describe() {};

      virtual void writeOutputFile(std::ofstream &fout) { }
	  //  writes to file

	// needed by geometry editor
};

#endif // DIAGS_H
