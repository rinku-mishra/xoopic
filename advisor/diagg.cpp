//diagg.cpp

#include "spatialg.h"
#include "diagg.h"
#include "diag2file.h"
#include "gridprob.h"
#include "probergn.h"
#include "laserspotdiag.h"
#include "wavesplitdiag.h"
#//include "predefDiag.h"
#include "derivDiag.h"

#ifdef HAVE_FFT
#include "psdFieldDiag1d.h"
#include "psdFieldDiag2d.h"
#endif // HAVE_FFT 

#include "spbound.h"
#include "diags.h"

//=================== DiagParams Class

int Between2(int i, int lo, int hi)
{return ((lo <= i) && (i <= hi));}

DiagParams::DiagParams(GridParams *_GP,SpatialRegionGroup* srg)
: BoundaryParams(_GP,srg)
{
	name = "Diagnostics";
	// GP = _GP;
	spatialRegionGroup = srg;
	j1.setNameAndDescription("j1","x1 Index for Diagnostic");
	j1.setValue("-1");
	k1.setNameAndDescription("k1","x2 Index for Diagnostic");
	k1.setValue("0");
	j2.setNameAndDescription("j2","x1 Index for Diagnostic");
	j2.setValue("0");
	k2.setNameAndDescription("k2","x2 Index for Diagnostic");
	k2.setValue("0");
	normal.setNameAndDescription("normal", "normal for Diagnostic");
	normal.setValue("1");
	parameterList.add(&j1);
	parameterList.add(&k1);
	parameterList.add(&j2);
	parameterList.add(&k2);
	A1.setNameAndDescription("A1","x1 Location (MKS) of boundary endpoint");
	A1.setValue("0");
	parameterList.add(&A1);
	A2.setNameAndDescription("A2","x2 Location (MKS) of boundary endpoint");
	A2.setValue("0");
	parameterList.add(&A2);
	B1.setNameAndDescription("B1","x1 Location (MKS) of boundary endpoint");
	B1.setValue("0");
	parameterList.add(&B1);
	B2.setNameAndDescription("B2","x2 Location (MKS) of boundary endpoint");
	B2.setValue("0");
	parameterList.add(&B2);

	VarName.setNameAndDescription("VarName",  "Name of Variable to be plotted");
	VarName.setValue("NULL");
	parameterList.add(&VarName);

	polarizationEB.setNameAndDescription("polarizationEB",  "E_component B_component");
	polarizationEB.setValue("EyBz");
	parameterList.add(&polarizationEB);

	psd1dFlag.setNameAndDescription("psd1dFlag",  "Flag for the 1D PSD");
	psd1dFlag.setValue("0"); // default is to turn off this calculation
	parameterList.add(&psd1dFlag);

	windowName.setNameAndDescription("windowName",
			"Name of the window for use in the 1D PSD");
	windowName.setValue("Blackman"); // default is the Blackman window
	parameterList.add(&windowName);

	fieldName.setNameAndDescription("fieldName",  "E or B");
	fieldName.setValue("None"); // no field name is set by default
	parameterList.add(&fieldName);

	fieldComponentLabel.setNameAndDescription("fieldComponentLabel",
			"1, 2, or 3");
	fieldComponentLabel.setValue("0"); // the default is not a valid component label
	parameterList.add(&fieldComponentLabel);

	integral.setNameAndDescription("integral",  "type of integral performed: line, flux, and sum");
	integral.setValue("NULL");
	parameterList.add(&integral);

	x1Label.setNameAndDescription("x1_Label","x1 Label the plot");
	x1Label.setValue("x1");
	parameterList.add(&x1Label);

	x2Label.setNameAndDescription("x2_Label","x2 Label the plot");
	x2Label.setValue("x2");
	parameterList.add(&x2Label);

	x3Label.setNameAndDescription("x3_Label","x3 Label the plot");
	x3Label.setValue("x3");
	parameterList.add(&x3Label);

	title.setNameAndDescription("title","Title of the plot");
	title.setValue("not_named");
	parameterList.add(&title);

	nfft.setNameAndDescription("nfft",  "number of fft must be power of 2");
	nfft.setValue("0");
	parameterList.add(&nfft);

	save.setNameAndDescription("save",  "Flag: 1 saves the diagnostic data in the dumpfile");
	save.setValue("0");
	parameterList.add(&save);

	HistMax.setNameAndDescription("HistMax",  "Maximum Length of History Buffer");
	HistMax.setValue("64");
	parameterList.add(&HistMax);

	Comb.setNameAndDescription("Comb",  "Every \'Comb\' value is left when HistMax is reached");
	Comb.setValue("0");
	parameterList.add(&Comb);

	Ave.setNameAndDescription("Ave",  "Averaged over \'Ave\' when HistMax is reached");
	Ave.setValue("0");
	parameterList.add(&Ave);

	// JK, 2013-07-26; add new parameters for diagnostics
	write_params.output_directory.setNameAndDescription("directory", "directory for output files; default = output");
	write_params.output_directory.setValue("");
	parameterList.add(&write_params.output_directory);

	write_params.output_file.setNameAndDescription("file", "name of output file; can include C printf format char %d for current step");
	write_params.output_file.setValue("");
	parameterList.add(&write_params.output_file);

	write_params.write_append.setNameAndDescription("append", "append data to file (= 1) or write data to empty file each time (= 0)");
	write_params.write_append.setValue(0);
	parameterList.add(&write_params.write_append);

	write_params.n.setNameAndDescription("n", "defines step number at which to write data to file; default is 0 (no writing)");
	write_params.n.setValue(0);
	parameterList.add(&write_params.n);

	write_params.n_start.setNameAndDescription("n_start", "defines start computation step when writes in file starts; default is 0 = start with first step");
	write_params.n_start.setValue(0);
	parameterList.add(&write_params.n_start);

	write_params.n_step.setNameAndDescription("n_step", "defines step for computation step between writes in file; default is 1 = write every step");
	write_params.n_step.setValue(1);
	parameterList.add(&write_params.n_step);

	write_params.n_end.setNameAndDescription("n_end", "defines end computation step when writes in file stops; default is 0 = no last step");
	write_params.n_end.setValue(0);
	parameterList.add(&write_params.n_end);

	write_params.n_width.setNameAndDescription("n_width", "defines width of period in computational steps over which derivative is taken; only used in derivative diagnostics; default=0 -- no derivative is done");
	write_params.n_width.setValue(0);
	parameterList.add(&write_params.n_width);

	write_params.t.setNameAndDescription("t", "defines time at which to write data to file");
	write_params.t.setValue(0);
	parameterList.add(&write_params.t);

	write_params.t_start.setNameAndDescription("t_start", "defines start time when writes in file starts");
	write_params.t_start.setValue(0);
	parameterList.add(&write_params.t_start);

	write_params.t_step.setNameAndDescription("t_step", "defines time difference between writes in file");
	write_params.t_step.setValue(0);
	parameterList.add(&write_params.t_step);

	write_params.t_end.setNameAndDescription("t_end", "defines end time when writes in file stops");
	write_params.t_end.setValue(0);
	parameterList.add(&write_params.t_end);

	// is diagnostics windows open when starting application (=1) or closed (=0)
	open_init.setNameAndDescription("open", "open diagnostics windows initially (=1) or not (=0, default)");
	open_init.setValue(0);
	parameterList.add(&open_init);

/*
 * not implemented yet (august 2013)
 *
	write_params.species_list.setNameAndDescription("species", "list for species to include in diagnostic; default is to include all species");
	write_params.species_list.setValue("");
	parameterList.add(&write_params.species_list);

	write_params.disabled.setNameAndDescription("disabled", "is diagnostic active (= 0, default) or disabled (= 1)");
	write_params.disabled.setValue(0);
	parameterList.add(&write_params.disabled);
*/

};

Diag* DiagParams::CreateCounterPart(SpatialRegion* SP)
/* throw(Oops) */{
	region = SP;
	Diag* diagnostic;
	struct write_params write_data;

	// get writing parameters for diagnostics
	convertParamsToWriteData(&write_data);

	if(VarName.getValue() == ostring("LaserSpotSize")) {
		return new LaserSpotDiagnostic(region,
				j1.getValue(),
				k1.getValue(),
				j2.getValue(),
				k2.getValue(),
				nfft.getValue(),
				HistMax.getValue(),
				Ave.getValue(),
				Comb.getValue(),
				VarName.getValue(),
				x1Label.getValue(),
				x2Label.getValue(),
				x3Label.getValue(),
				title.getValue(),
				save.getValue(),
				open_init.getValue(),			// JK, 2013-07-31
				write_data);					// JK, 2013-07-26
	}
	if(VarName.getValue() == ostring("WaveDirDiagnostic")) {
		return new WaveDirDiag(region,
				j1.getValue(),
				k1.getValue(),
				j2.getValue(),
				k2.getValue(),
				nfft.getValue(),
				HistMax.getValue(),
				Ave.getValue(),
				Comb.getValue(),
				VarName.getValue(),
				x1Label.getValue(),
				x2Label.getValue(),
				x3Label.getValue(),
				title.getValue(),
				save.getValue(),
				polarizationEB.getValue(),
				psd1dFlag.getValue(),
				windowName.getValue(),
				open_init.getValue(),			// JK, 2013-07-31
				write_data);					// JK, 2013-07-26
	}   
#ifdef HAVE_FFT    
	if(VarName.getValue() == ostring("PSDFieldDiag1d")) {
		try {
			return new PSDFieldDiag1d(region,
					j1.getValue(),
					k1.getValue(),
					j2.getValue(),
					k2.getValue(),
					nfft.getValue(),
					HistMax.getValue(),
					Ave.getValue(),
					Comb.getValue(),
					VarName.getValue(),
					x1Label.getValue(),
					x2Label.getValue(),
					x3Label.getValue(),
					title.getValue(),
					save.getValue(),
					windowName.getValue(),
					fieldName.getValue(),
					fieldComponentLabel.getValue(),
					open_init.getValue(), write_data);
		}
		catch(Oops& oops){
			oops.prepend("DiagParams::CreateCounterPart: Error:\n"); //SpatialRegionGroup::CreateCounterPart
			throw oops;
		}

	}       
	if(VarName.getValue() == ostring("PSDFieldDiag2d")) {
		try{
			return new PSDFieldDiag2d(region,
					j1.getValue(),
					k1.getValue(),
					j2.getValue(),
					k2.getValue(),
					nfft.getValue(),
					HistMax.getValue(),
					Ave.getValue(),
					Comb.getValue(),
					VarName.getValue(),
					x1Label.getValue(),
					x2Label.getValue(),
					x3Label.getValue(),
					title.getValue(),
					save.getValue(),
					fieldName.getValue(),
					fieldComponentLabel.getValue(),
					open_init.getValue(), write_data);
		}
		catch(Oops& oops){
			oops.prepend("DiagParams::CreateCounterPart: Error:\n"); //SpatialRegionGroup::CreateCounterPart
			throw oops;
		}

	}       
#endif // HAVE_FFT


	if( DerivativeDiagnostic::VarNameDefinesDerivative(VarName.getValue()) ) {
		// derivative diagnostics; JK, 2013-08-06
		return new DerivativeDiagnostic(region, j1.getValue(), k1.getValue(), j2.getValue(), k2.getValue(),
										nfft.getValue(), HistMax.getValue(),Ave.getValue(),
										Comb.getValue(),
										VarName.getValue(), x1Label.getValue(),
										x2Label.getValue(),x3Label.getValue(),
										title.getValue(), save.getValue(), integral.getValue(),
										open_init.getValue(), write_data);

	} else if( VarName.getValue().substr(0,6) == ostring("predef") ) {
		// extract "real" name from variable in input file
		ostring realVarName = VarName.getValue().substr(7);

		return new Diag(region, realVarName, title.getValue(), save.getValue(),
						open_init.getValue(),			// JK, 2013-08-01
						write_data);					// JK, 2013-08-01
	}

	if ((j1.getValue() == j2.getValue()) || (k1.getValue()==k2.getValue())) {
		diagnostic = new GridProbes(region, j1.getValue(), k1.getValue(), j2.getValue(), k2.getValue(),
										nfft.getValue(), HistMax.getValue(),Ave.getValue(),
										Comb.getValue(), VarName.getValue(),
										x1Label.getValue(), x2Label.getValue(),x3Label.getValue(),
										title.getValue(), save.getValue(), integral.getValue(),
										open_init.getValue(),			// JK, 2013-07-31
										write_data);					// JK, 2013-07-26
	} else {
		diagnostic = new ProbeRegion(region, j1.getValue(), k1.getValue(), j2.getValue(), k2.getValue(),
										nfft.getValue(), HistMax.getValue(),Ave.getValue(),
										Comb.getValue(), VarName.getValue(),
										x1Label.getValue(), x2Label.getValue(),x3Label.getValue(),
										title.getValue(),save.getValue(),
										open_init.getValue(),			// JK, 2013-07-31
										write_data);					// JK, 2013-07-26
	}

	return diagnostic;
}

/*void DiagParams::toGrid(Grid *G) {

  // j1 == -1 is the flag telling us to use the MKS

  if(j1.getValue()==-1) {
	 j1.setValue((int) (0.5 +G->getGridCoords(Vector2(A1.getValue(),A2.getValue())).e1()));
	 k1.setValue((int) (0.5 +G->getGridCoords(Vector2(A1.getValue(),A2.getValue())).e2()));
	 j2.setValue((int) (0.5 +G->getGridCoords(Vector2(B1.getValue(),B2.getValue())).e1()));
	 k2.setValue((int) (0.5 +G->getGridCoords(Vector2(B1.getValue(),B2.getValue())).e2()));
  } 


}
 */

/*ostring DiagParams::AlignmentConstraint()
{char buffer[200];
 if (((j1.getValue() == j2.getValue()) &&
		(k1.getValue() != k2.getValue())) ||
	  ((j1.getValue() != j2.getValue()) &&
		(k1.getValue() == k2.getValue())))
  sprintf(buffer, "%s","");
 else
  sprintf(buffer, "boundary (%d, %d)- (%d, %d) is not aligned with axis",
		j1.getValue(), k1.getValue(), j2.getValue(), k2.getValue());
 return(ostring(buffer));}

ostring DiagParams::OnGridConstraint()
{char buffer[200];
 if  (Between2(j1.getValue(), 0, GP->getJ()) && Between2(k1.getValue(), 0, GP->getK()) &&
		Between2(j2.getValue(), 0, GP->getJ()) && Between2(k2.getValue(), 0, GP->getK()))
  sprintf(buffer, "%s","");
 else
  sprintf(buffer, "boundary (%d, %d)- (%d, %d) is not contained on grid",
		j1.getValue(), k1.getValue(), j2.getValue(), k2.getValue());
 return(ostring(buffer));}

BOOL DiagParams::HasLength()
{
	if ((j1.getValue() == j2.getValue()) && (k1.getValue() == k2.getValue()))
		return 0;
	else
		return 1;
}

 */
void DiagParams::checkRules()
{
	//ruleMessages.removeAll();
	ostring result = AlignmentConstraint();
	if (strlen(result.c_str()) > 0)
		ruleMessages.add(new ostring(result));
	result = OnGridConstraint();
	if (strlen(result.c_str()) > 0)
		ruleMessages.add(new ostring(result));
	// Loop thru rules
	oopicListIter<RuleBase> nR(RuleList);
	oopicListIter<BaseParameter> nP(parameterList);
	for (nR.restart(); !nR.Done(); nR++)
		// Loop thru parameters and set parameter values required by rule
	{for (nP.restart(); !nP.Done(); nP++)
		nR.current()->setRuleVariable(nP.current()->getName(),
				nP.current()->getValueAsDouble());
	// Check the rule
	ostring result = nR.current()->checkRule();
	if (strlen(result.c_str()) > 0)
		ruleMessages.add(new ostring(result));}
}

// query function for GUI
// #ifndef UNIX
/**
Segment DiagParams::getSegment()
{
	Segment result(Point(j1.getValue(), k1.getValue()), 
		Point(j2.getValue(), k2.getValue()));
	result.setNormal(normal.getValue());
	return result; 
}
void DiagParams::setSegment(Segment& s)
{
	char buff[200];
	itoa(s.getj1(), buff, 10);
	j1.setValue(buff);
	itoa(s.getk1(), buff, 10);
	k1.setValue(buff);
	itoa(s.getj2(), buff, 10);
	j2.setValue(buff);
	itoa(s.getk2(), buff, 10);
	k2.setValue(buff);

//heuristics for setting the normal -- ntg 2-23-95
	normal.setValue("1");
	if (((j1.getValue() == j2.getValue()) &&
		 (j1.getValue() > GP->getJ()/2)) ||
		(((k1.getValue() == k2.getValue()) &&
		 (k1.getValue() > GP->getK()/2))))
		normal.setValue("-1");

}
 */
// #endif // ifndef UNIX

/*
 * Convert structure from write_parameters to write_data -- last one is used in diagnostics.
 * JK, 2013-07-26
 */
void DiagParams::convertParamsToWriteData(struct write_params *wd) {

	wd->output_directory = write_params.output_directory.getValue();
	wd->disabled = write_params.disabled.getValue();
	wd->write_append = write_params.write_append.getValue();
	wd->output_file = write_params.output_file.getValue();
	wd->species_list = write_params.species_list.getValue();
	wd->n = write_params.n.getValue();
	wd->n_start = write_params.n_start.getValue();
	wd->n_step = write_params.n_step.getValue();
	wd->n_end = write_params.n_end.getValue();
	wd->n_width = write_params.n_width.getValue();
	wd->t = write_params.t.getValue();
	wd->t_start = write_params.t_start.getValue();
	wd->t_step = write_params.t_step.getValue();
	wd->t_end = write_params.t_end.getValue();
	wd->write_data_by = write_params.write_data_by.getValue();

}

