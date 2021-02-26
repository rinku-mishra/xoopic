#ifndef	__DIAG2FILE_H
#define	__DIAG2FILE_H

/*
====================================================================

DIAG2FILE.H

Class for managing writing data to files.


0.01	(JK, 2013-07-30) -- initial class
====================================================================
 */

#include "sptlrgn.h"
#include "ostring.h"

// added by JK, 2013-07-26
#define WRITE_DATA_BY_STEP		1
#define WRITE_DATA_BY_TIME		2


// types of data to write
#define DATA_TYPE_SCALAR		1
#define DATA_TYPE_VECTOR3		2
#define DATA_TYPE_SCALAR_2D		3
#define DATA_TYPE_SPECIES		4
#define DATA_TYPE_SPECIES_X_V	5
#define DATA_TYPE_TIME_GRAPH	6
#define DATA_TYPE_SPECIES_TIME_GRAPH	7

#define MAX_NUMBER_SPECIES		128

// parameters for writing data to output files; JK 2013-07-26
struct write_params {
	ostring output_directory;		// directory in which to write files
	int disabled;					// is current diagnostic disabled (exists in input file, but is ignored)
	int write_append;				// append to file or not
	// name of output file(s); can have printf format %d,
	// which is replaced with current computational step
	ostring output_file;
	ostring species_list;			// which species include in write
	int n;							// if defined, write only at "n"-th step
	int n_start, n_step, n_end;		// if defined, write in steps, defined by start, step and end steps
	int n_width;					// width of period (in comp. steps) for doing derivative
	Scalar t;						// if defined, write only at "t" time
	Scalar t_start, t_step, t_end;	// if define, write in times defined by start, step and end
	int write_data_by;				// which condition is used when checking when to write: step or time
};


class Diag2File
{
protected:
	ostring title, VarName;
	bool derivative_data;			// is writing to file derivative (true) or not (false); default is false

	// parameters for writing data to output files
	ostring output_directory;		// directory in which to write files
	int disabled;					// is current diagnostic disabled (exists in input file, but is ignored)
	int write_append;				// append to file or not
	// name of output file(s); can have printf format %d,
	// which is replaced with current computational step
	ostring output_file;
	ostring species_list;			// which species include in write
	int n;							// if defined, write only at "n"-th step
	int n_start, n_step, n_end;	// if defined, write in steps, defined by start, step and end steps
	int n_width;
	Scalar t;						// if defined, write only at "t" time
	Scalar t_start, t_step, t_end;	// if define, write in times defined by start, step and end
	int write_data_by;				// which condition is used when checking when to write: step or time

	// current values for running variables...
	bool header_written;
	int n_current, next_write_n;
	Scalar t_current, next_write_t, sim_dt;

	// variables for storing pointers to data for writing
	int* data_size[2];
	int data_type;
	int vector_component;
	int num_species;
	Scalar **scalar_data;
	Vector3 **vector_data;
	Scalar *x1_array, *x2_array;
	// species data
	Scalar *species_data1[MAX_NUMBER_SPECIES];
	Scalar *species_data2[MAX_NUMBER_SPECIES];
	int* species_data_size[MAX_NUMBER_SPECIES];

	// variables for storing copy of data for computing time derivatives of values
	// this should be used only for grid diagnostics as grid does not changed with time
//	int n_copy_saved;
//	Scalar t_copy_saved;
//	Scalar **scalar_data_copy;
//	Vector3 **vector_data_copy;
//	Scalar value_factor; 			// factor with which all values are multiplied before they are written to file; JK, 2013-08-23

	void setWriteParams(struct write_params* wp);
	bool checkOutputDirectoy(bool create);
	inline bool doAnyWriting();
	void computeNextWritePoint();
	void checkWriteParameters();
	void write_data(FILE *fp);

	/**
	 * Define steps for writing data to output files. Start step, number of steps to skip and end step.
	 */
	inline void set_write_data_by_step(long in_n_start, long in_n_step, long in_n_end ) {
		this->n_start = in_n_start;
		this->n_step = in_n_step;
		this->n_end = in_n_end;
		this->write_data_by = WRITE_DATA_BY_STEP;
	}

	/**
	 * Define times for writing data to output files. Start time, time to skip and end time.
	 */
	inline void set_write_data_by_time(double in_t_start, double in_t_step, double in_t_end ) {
		this->t_start = in_t_start;
		this->t_step = in_t_step;
		this->t_end = in_t_end;
		this->write_data_by = WRITE_DATA_BY_TIME;
	}

public:
	Diag2File(ostring in_var_name, ostring in_title, struct write_params* wp);
	virtual ~Diag2File();
	void init();

	// for checking if writing is required according to given writing parameters
	static bool check_if_write_required(struct write_params* wp);

	virtual void output_header(FILE *fp) {
		fprintf( fp, "# xoopic; %s\n", get_title().c_str() );
	}

	// set and get methods for parameters for writing data to files
	// common set/get methods
	ostring getVarName() { return VarName; }
	ostring get_title() { return title; }
	ostring get_file_mode() { return (write_append == 1 ? "a" : "w"); };
	inline int getNextWriteStep() { return next_write_n; };									// JK, 2013-08-05
	inline int getDataType() { return data_type; };											// JK, 2013-08-05
	inline void setDerivativeDataFlag( bool flag ) { derivative_data = flag; };				// JK, 2013-08-05
	inline bool getDerivativeDataFlag() { return derivative_data; };						// JK, 2013-08-05

	void makeCopyOfData(Scalar t, int n);		// JK, 2013-08-05
	void write(Scalar t, int n);
	virtual void output_data(FILE *fp) {};

	/*==========================================================================
	 * Methods to add pointers to data that is written to file with "write_data".
	 *
	 *==========================================================================*/
	void addData(ostring in_var_name, ostring in_title, int* size, Scalar** data) {
		this->VarName = in_var_name;
		this->title = in_title;
		this->scalar_data = data;
		this->data_size[0] = size;
		this->data_type = DATA_TYPE_SCALAR;
	}

	void addData(ostring in_var_name, ostring in_title, int* _jm, int* _km, Scalar* _x1_array, Scalar* _x2_array, Vector3** _vals, int _component) {
		this->VarName = in_var_name;
		this->title = in_title;
		this->vector_data = _vals;
		this->x1_array = _x1_array;
		this->x2_array = _x2_array;
		this->data_size[0] = _jm;
		this->data_size[1] = _km;
		this->data_type = DATA_TYPE_VECTOR3;

		if( _component >= -1 && _component <= 2 ) {
			// coordinate index is OK
			this->vector_component = _component;
		}
	}

	void addData(ostring in_var_name, ostring in_title, int* _jm, int* _km, Scalar* _x1_array, Scalar* _x2_array, Scalar** _vals) {
		this->VarName = in_var_name;
		this->title = in_title;
		this->scalar_data = _vals;
		this->x1_array = _x1_array;
		this->x2_array = _x2_array;
		this->data_size[0] = _jm;
		this->data_size[1] = _km;
		this->data_type = DATA_TYPE_SCALAR_2D;
	}

	// JK, 2013-08-02
	void addData(ostring in_var_name, ostring in_title, Scalar* _x1positions, Scalar* _x2positions, Scalar* _x1velocities, Scalar* _x2velocities, Scalar* _x3velocities, int* _num_data, int species_index) {
		this->VarName = in_var_name;
		this->title = in_title;
		this->species_data1[0] = _x1positions;
		this->species_data1[1] = _x2positions;
		this->species_data2[0] = _x1velocities;
		this->species_data2[1] = _x2velocities;
		this->species_data2[2] = _x3velocities;
		this->species_data_size[0] = _num_data;
		this->num_species = species_index;				// holds species index NOT number of species!! JK, 2013-08-20
		this->data_type = DATA_TYPE_SPECIES_X_V;
	}

	// JK, 2013-08-02
	void addData(ostring in_var_name, ostring in_title, Scalar* _x1_array, Scalar* _x2_array, int* _num_data) {
		this->VarName = in_var_name;
		this->title = in_title;
		this->x1_array = _x1_array;
		this->x2_array = _x2_array;
		this->data_size[0] = _num_data;
		this->data_type = DATA_TYPE_TIME_GRAPH;
	}

	void addTitleSpecies(ostring in_var_name, ostring in_title, int _num_species) {
		this->VarName = in_var_name;
		this->title = in_title;
		this->num_species = _num_species;

		for( int i=0 ; i < _num_species ; i++ ) {
			// initialize species data
			this->species_data1[i] = NULL;
			this->species_data2[i] = NULL;
			this->species_data_size[i] = NULL;
		}

		ostring varEnd = in_var_name.substr(in_var_name.length()-3, 3);

		if( varEnd == (ostring)"(t)" ) {
			// values that are time dependent and for each species
			this->data_type = DATA_TYPE_SPECIES_TIME_GRAPH;

		} else {
			// "usual" data for species
			this->data_type = DATA_TYPE_SPECIES;
		}
	}

	void addDataSpecies(Scalar* _x1_values, Scalar* _x2_values, int* _size, int _species_index) {
		if( _species_index < this->num_species ) {
			this->species_data1[_species_index] = _x1_values;
			this->species_data2[_species_index] = _x2_values;
			this->species_data_size[_species_index] = _size;
		}
	};
};

#endif	//	ifndef __DIAG2FILE_H

