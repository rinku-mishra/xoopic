//diagg.h

#ifndef   __DIAGG_H
#define   __DIAGG_H

#ifdef UNIX
#include <config.h>
#endif

#include "gridg.h"
#include "newdiag.h"

class SpatialRegionGroup;

//===================DiagParams  Class
// base class for all diagnostics
// abstract

// parameters for writing data to output files; JK 2013-07-26
struct input_file_write_params {
	StringParameter output_directory;		// directory in which to write files
	IntParameter disabled;					// is current diagnostic disabled (exists in input file, but is ignored)
	IntParameter write_append;				// append to file or not
	// name of output file(s); can have printf format %d,
	// which is replaced with current computational step
	StringParameter output_file;
	StringParameter species_list;			// which species include in write
	IntParameter n;							// if defined, write only at "n"-th step
	IntParameter n_start, n_step, n_end;	// if defined, write in steps, defined by start, step and end steps
	IntParameter n_width;					// width of period for doing derivatives
	ScalarParameter t;						// if defined, write only at time "t"
	ScalarParameter t_start, t_step, t_end;	// if define, write in times defined by start, step and end
	IntParameter write_data_by;				// which condition is used when checking when to write: step or time
};

class DiagParams : public BoundaryParams
{protected:
	GridParams* GP;
	SpatialRegionGroup* spatialRegionGroup;
	Diag* dPtr;
	SpatialRegion* region;

	// options/vars for writing data to output files; JK 2013-07-26
	struct input_file_write_params write_params;

public:
	IntParameter nfft;
	IntParameter save;
	IntParameter HistMax;
	IntParameter Ave;
	IntParameter Comb;
	StringParameter VarName;
	StringParameter integral;
	StringParameter x1Label;
	StringParameter x2Label;
	StringParameter x3Label;
	StringParameter title;
	IntParameter open_init;		// JK, 2013-07-31: is diagnostic open initially (=1) or closed (=0, default)

	/**
	 * The variables:
	 * StringParameter polarizationEB;
	 * IntParameter psd1dFlag;
	 * StringParameter windowName;
	 * are associated with the extention of the the
	 * WaveDirDiag class for handling different linear
	 * combinations of the E and B components together
	 * with the corresponding power spectral densities.
	 * psd1dFlag is for the turning on/off the 1d
	 * power spectral density calculation of the
	 * corresponding linear combinations of E and B
	 * specified by the "polarizationEB" parameter.
	 * psd1dFlag == 0 turns off this calculation.
	 * windowName == "Blackman" is the default for windowing
	 *                data in the 1d power spectrum calculation.
	 * dad, Mon Aug 13 10:49:47 MDT 2001
	 */
	StringParameter polarizationEB;
	IntParameter psd1dFlag;
	StringParameter windowName;

	/**
	 * Variables associated with the PSDFieldDiag:
	 */
	StringParameter fieldName; // E or B
	IntParameter fieldComponentLabel; // 1, 2, or 3

	BOOL HasLength();

	DiagParams(GridParams *_GP,SpatialRegionGroup* srg);
	//  since boundaries are all placed on grid, this class needs
	//  to know the dimensions of the grid

	virtual ~DiagParams() {};

	//ostring AlignmentConstraint();
	// ensures that boundary is aligned with grid

	//  ostring OnGridConstraint();
	// ensures that boundary is on grid

	virtual void checkRules();
	// checks all rules associated with the class

	virtual Diag* CreateCounterPart(SpatialRegion* region) /* throw(Oops) */;

	// void toGrid(Grid *g);

	// query function for GUI
#ifndef UNIX
	// Segment getSegment();
	// void setSegment(Segment& s);
#endif

	// JK, 2013-07-26
	void convertParamsToWriteData(struct write_params *wd);

};                                       

#endif  //  __DIAGG_H

