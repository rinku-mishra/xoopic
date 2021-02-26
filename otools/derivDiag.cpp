/*
 ====================================================================

 PREDEFDIAG.CPP

 0.01	(JK, 2013-08-01) Initial class for enabling/writing derived diagnostics.

 ====================================================================
 */
#include "derivDiag.h"
#include "newdiag.h"
#include "diagn.h"
#include "boundary.h"
#include "grid.h"
#include "globalvars.h"
#include "diag2file.h"
#define True 1

extern "C++" void printf(char *);
extern "C" {
#ifdef NOX
#include <xgmini.h>
#else
#include <xgdefs.h>
#endif //NOX
}

//--------------------------------------------------------------------
DerivativeDiagnostic::DerivativeDiagnostic(SpatialRegion* region, int j1, int k1, int j2, int k2,
											 int nfft, int HistMax,
											 int Ave, int Comb, ostring _VarName,
											 ostring diagXLabel,
											 ostring diagYLabel, ostring diagZLabel, ostring diagLabel,
											 int save,
											 ostring _integral, int open_init, struct write_params write_data)
:Diag(region, j1, k1, j2, k2, nfft, HistMax, Ave, Comb, _VarName, diagXLabel,
			diagYLabel, diagZLabel, diagLabel, save, open_init, write_data)
{
	// init structures and all other staff
	this->initDiag(write_data);


}


DerivativeDiagnostic::DerivativeDiagnostic(SpatialRegion* region, ostring _VarName, ostring _title, int save, int open_init, struct write_params write_data)
:Diag(region, _VarName, _title, save, open_init, write_data)
{
	// init structures and all other staff
	this->initDiag(write_data);
}


DerivativeDiagnostic::~DerivativeDiagnostic() {

	for( int j=0 ; j < jm+1 ; j++ ) {
		// de-allocate other direction and initialize it's to 0
		delete [] scalar_data[j];
		delete [] scalar_data_copy[j];
	}

	delete [] scalar_data;
	delete [] scalar_data_copy;

	scalar_source = NULL;
	vector_source = NULL;
}


/**
 * Initialize internal structures according to given parameters.
 */
void DerivativeDiagnostic::initDiag(struct write_params write_data) {
	int j, k;


	local_region = region;

	derivativeDiag = true;
	component = -1;
	value_factor = 1;
	mainVarName = "";
	data_source_type = 0;
	scalar_source = scalar_data = scalar_data_copy = NULL;
	vector_source = NULL;
	value_factor = 1;		// default value is 1 - no change in values

	if( write_data.n_width <= 0 ) {
		// wrong parameter
		printf("DerivativeDiagnostic::constructor for %s: value of n_width is wrong. It should be positive, now it is %d. Using n_width=1!", VarName.c_str(), write_data.n_width);
		write_data.n_width = 1;
	}


	// number of steps before writing that we store values (at t1)
	n_step = MAX(1, write_data.n_step);						// make sure we have positive n_step
	n_width = MAX(1, write_data.n_width);					// make sure we have positive n_width
	n_step = MAX(write_data.n_width, write_data.n_step);	// step should be equal or bigger than n_width, otherwise we would need multiple copies of data...
	n_next_compute = n_width;

	// some values used later...
	jm = region->getJ()+1;
	km = region->getK()+1;

	number_of_species=region->get_nSpecies();

	// get memory for the grid arrays
	x1_array = new Scalar[jm];
	x2_array = new Scalar[km];

	for(j=0 ; j < jm ; j++) {
		x1_array[j]=region->getMKS(j,0).e1();
	}

	for(k=0 ; k < km ; k++) {
		x2_array[k]=region->getMKS(0,k).e2();
	}

	//  get the size of the grid
	x1max = region->getMKS(jm-1,km-1).e1();
	x2max = region->getMKS(jm-1,km-1).e2();
	x1min = region->getMKS(0,0).e1();
	x2min = region->getMKS(0,0).e2();

	//
	// find component for derivation and factor
	// added by JK, 2016-02-26
	//
	int delimStartPos = 0, delimEndPos = VarName.find("/",0);
	// check if there is no delimiter in VarName
	delimEndPos = (delimEndPos == -1) ? VarName.length() : delimEndPos;

	if( VarName.find("eps",0) != -1 ) {
		// OK, multiplying output with EPS0
		setValueOutputFactor( EPS0 );
		delimStartPos = 4;
	}

	ostring tmpComp = VarName.substr(delimEndPos-1,1);

	if( tmpComp == ostring("1") || tmpComp == ostring("2") || tmpComp == ostring("3") ) {
		// we got component defined; store defined component and use one char less for main var name
		delimEndPos --;
		char num = tmpComp[0];
		component = (int) num - '0';
	}

	// main VarName defines "logic" behind diagnostics (scalar, vector data, ....)
	mainVarName = VarName.substr(delimStartPos, delimEndPos-delimStartPos);

	// allocate arrays for scalar data
	// first one direction (container for holding pointers for second dimension)
	scalar_data = new Scalar *[jm];
	scalar_data_copy = new Scalar *[jm];

	for( j=0 ; j < jm ; j++ ) {
		// allocate other direction and initialize it's to 0
		scalar_data[j] = new Scalar [km];
		scalar_data_copy[j] = new Scalar [km];
		memset(scalar_data[j],0,(km)*sizeof(Scalar));
		memset(scalar_data_copy[j],0,(km)*sizeof(Scalar));
	}

	cout << "DerivDiagnostics: " << mainVarName << ", d(steps) = " << n_width << endl;

	if( mainVarName == ostring("dE") ) {
		// OK, E field has source of Vector3 data
		data_source_type = DATA_TYPE_VECTOR3;
		vector_source = region->getENode();
	}

}


/**
 * Do the diagnostic maintenance.
 */
void DerivativeDiagnostic::MaintainDiag(Scalar t, int n) {

	if( n == n_next_compute ) {
		// we have to compute derivatives
		compute(t, n);
		n_next_compute += n_width;
	}

	if( n == n_next_compute - n_width ) {
		// we have to save current values of source fields to be able to compute derivative in n_width steps
		makeCopyOfData(t, n);
	}
}


/**
 * Make a copy of current data to be used for computing derivative next time we have to write data to file.
 * JK 2013-08-05
 */
void DerivativeDiagnostic::makeCopyOfData(Scalar t, int n) {

	// save time and comp. step
	t_copy_saved = t;
	n_copy_saved = n;

	switch( data_source_type ) {
	case DATA_TYPE_SCALAR:
		// source is vector field
		for( int j=0 ; j < jm ; j++ ){
			// copy all data to copy array (to be used later)
			for( int k=0 ; k < km ; k++ ){
				this->scalar_data_copy[j][k] = this->scalar_source[j][k];
			}
		}
		break;

	case DATA_TYPE_VECTOR3:
		// source is vector field; use define component of a field (if defined), otherwise use total value of value
		if( component == -1 ) {
			// compute vector size and store it into scalar_copy
			for( int j=0 ; j < jm ; j++ ){
				// copy all data to copy array (to be used later)
				for( int k=0 ; k < km ; k++ ){
					this->scalar_data_copy[j][k] = (this->vector_source[j][k]).magnitude();
				}
			}

		} else {
			for( int j=0 ; j < jm ; j++ ){
				// copy all data to copy array (to be used later)
				for( int k=0 ; k < km ; k++ ){
					this->scalar_data_copy[j][k] = (this->vector_source[j][k]).e(component);
				}
			}
		}
		break;

	}

}


/**
 * Compute current values for derivative.
 * JK, 2016-02-27
 */
void DerivativeDiagnostic::compute(Scalar t, int n) {

	switch( data_source_type ) {
	case DATA_TYPE_SCALAR:
		// display derivative of scalar field
		if( scalar_data_copy != NULL ) {
			// we have a copy of data so we can compute derivative
			for( int j=0 ; j < jm ; j++ ){
				for( int k=0 ; k < km ; k++ ){
					this->scalar_data[j][k] = value_factor * (this->scalar_source[j][k] - this->scalar_data_copy[j][k]) / (t - t_copy_saved);
				}
			}
		}
		break;

	case DATA_TYPE_VECTOR3:
		// display derivative of vector field
		if( scalar_data_copy != NULL ) {
			// we have a copy of data so we can compute derivative
			if( this->component == -1 ) {
				// display all components
				for( int j=0 ; j < jm ; j++ ){
					// copy all data to copy array (to be used later)
					for( int k=0 ; k < km ; k++ ){
						// subtract values at current point and divide it by time elapsed from copy to current time
						this->scalar_data[j][k] = value_factor * (this->vector_source[j][k].magnitude() - this->scalar_data_copy[j][k]) / (t - t_copy_saved);
					}
				}

			} else {
				// compute derivative for given component
				for( int j=0 ; j < jm ; j++ ){
					// copy all data to copy array (to be used later)
					for( int k=0 ; k < km ; k++ ){
						// subtract values at current point and divide it by time elapsed from copy to current time
						this->scalar_data[j][k] = value_factor * (this->vector_source[j][k].e(this->component) - this->scalar_data_copy[j][k]) / (t - t_copy_saved);
					}
				}
			}
		}
		break;

	}


}


/**
 * Initialize diagnostics data display in Xwindow.
 * JK, 2016-02-27
 */
void DerivativeDiagnostic::initwin(ostring prependVarName) {
	char *buf = new char[80];
	ostring prepend1 = "";


	if( VarName.find("eps",0) == true ) {
		// one of the eps_ diagnostics in one of direction
		prepend1 = "eps_";
	}

	if( component == -1 ) {
		// total derivative (dE/dt or eps_dE/dt)
		sprintf(buf, "%s%sdE/dt", prependVarName.c_str(), prepend1.c_str());

	} else {
		// one of directional derivatines
		sprintf(buf, "%s%sdE%d/dt", prependVarName.c_str(), prepend1.c_str(), component);
	}

	// define label for coordinates according to region geometry
	switch(region->get_grid()->query_geometry()) {
		case ZRGEOM:
			x1Label = prepend1 + "z";
			x2Label = prepend1 + "r";
			x3Label = prepend1 + "phi";
			break;

		case ZXGEOM:
			x1Label = prepend1 + "x";
			x2Label = prepend1 + "y";
			x3Label = prepend1 + "z";
			break;
	};

	x1Label = prepend1 + x1Label;
	x2Label = prepend1 + x2Label;
	x3Label = prepend1 + x3Label;

	// define values to plot
	defineParameters(x1Label, x2Label, x1_array, x2_array, this->scalar_data, &jm, &km);

#ifndef NOX
	XGSet3D("linlinlin", x1Label.c_str(), x2Label.c_str(), strdup(buf), 45.0, 225.0, win_init_stat, 1, 1,
			1.0, 1.0, 1.0, 0, 0, 1, x1min, x1max, x2min, x2max, 0.0, 1.0);
	XGSurf(x1_array, x2_array, this->scalar_data, &jm, &km, 3 );
#endif
}

void DerivativeDiagnostic::FFT() {
}
