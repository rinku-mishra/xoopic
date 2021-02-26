/*
 * diag2file.CPP
 *
 * 0.0.1 (JK, 2013-07-30) - initial class for writing diagnostics to files at arbitrary time/computational steps
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits>

#include "diag2file.h"
#include "ostring.h"

//--------------------------------------------------------------------

Diag2File::Diag2File(ostring in_var_name, ostring in_title, struct write_params* wp) {
	// JK, 2013-07-29
	init();
	setWriteParams(wp);
	checkWriteParameters();

	if( write_data_by == WRITE_DATA_BY_STEP ) {
		next_write_n = n_start;
	} else if( write_data_by == WRITE_DATA_BY_TIME ) {
		next_write_t = t_start;
	}
}


Diag2File::~Diag2File()
{
}

/**
 * Check if writing to file is required according to given writing parameters.
 */
bool Diag2File::check_if_write_required(struct write_params* wp) {
	// writing is requested if output directory and file are defined
	return (wp->output_directory != (ostring)"" && wp->output_file != (ostring)"" );
}



/*
 * Initialize writing parameters to initial values before assigning new values.
 */
void Diag2File::init() {
	output_directory = "";
	disabled = false;
	output_file = "";
	species_list = "";
	write_append = false;
	n = n_start = n_step = n_end = 0;
	t = t_start = t_step = t_end = 0.;

	write_data_by = WRITE_DATA_BY_STEP;
	n_current = next_write_n = 0;
	t_current = next_write_t = 0.;
	header_written = false;
	// define initial values
	n_step = 1;		// write every step
	n_width = 0;		// no derivative

	// initialize pointers and variables for defining data for writing
	data_size[0] = data_size[1] = 0;
	vector_component = num_species = 0;
	x1_array = x2_array = NULL;

	// data for derivative diagnostics
	derivative_data = false;
	//value_factor = 1;		// default value is 1
}

/**
 * Define writing parameters.
 */
void Diag2File::setWriteParams(struct write_params* wp){
	output_directory = wp->output_directory;
	disabled = wp->disabled;
	output_file = wp->output_file;
	species_list = wp->species_list;
	write_append = wp->write_append;
	n = wp->n;
	n_start = wp->n_start;
	n_step = wp->n_step;
	n_end = wp->n_end;
	n_width = MIN(wp->n_width, wp->n_step);
	t = wp->t;
	t_start = wp->t_start;
	t_step = wp->t_step;
	t_end = wp->t_end;
	write_data_by = wp->write_data_by;
}



/*
 * Check if we have to write anything and call method to do actual writing to file.
 */
void Diag2File::write(Scalar t, int n) {
	struct stat buf ;
	FILE *fp;
	ostring fname, mode;
	ostring outname;


//	printf("Diag::write; t=%g, n=%d; dir=%s, file=%s\n", t, n, output_directory.c_str(), output_file.c_str());

	// define current simulation time and computational step
	t_current = t;
	n_current = n;

	if( !doAnyWriting() ) {
		// nothing to do; return
		return;
	}

	// compute next write point for current diagnostics
	computeNextWritePoint();

	if( !checkOutputDirectoy( true ) ) {
		// error checking output directory
		return;
	}

	// create file name with output directory
	outname = output_directory;

	if( output_directory.substr(output_directory.length()-1,1) != (ostring)"/" ) {
		// add slash
		outname += "/";
	}

	if( output_file.find( "%" ) ) {
		// filename has printf format characters; include current file number
		char tmpfn[128];
		// sprintf( tmpfn, file.c_str(), write_file_index );
		// use current simulation step in filename
		sprintf( tmpfn, output_file.c_str(), n_current );
		outname += tmpfn;
	} else {
		// use given filename
		outname += output_file;
	}

	if( stat( outname.c_str(), &buf) == 0 && !(write_append && header_written) ) {
		// delete target file if one exists
		if( S_ISREG(buf.st_mode) ) {
			// regular file; delete it
			unlink(outname.c_str());
		} else {
			// target filename exists; display error, return
			printf( "Output filename '%s' already exists and is not a file. Writing skipped in this step.\n", outname.c_str() );
			return;
		}
	}

	// get file open mode
	mode = get_file_mode();

	if( (fp = fopen( outname.c_str(), mode.c_str()) ) != NULL ) {
		// file prepared and open for writing -> now we can finally write data to file...
		if( !write_append || (write_append && !header_written) ) {
			// write header into file
			output_header(fp);
			header_written = true;
		}

		// do actual writing to file
		write_data(fp);
		// close file after writing
		fclose( fp );

	} else {
		// after all tests we still have troubles opening file
		fprintf(stderr, "Diagnostic::write_data_to_file : file '%s' could not be opened\n", outname.c_str() );
	}

}


/*
 * Check if output directory exist and if so requested, create it.
 */
bool Diag2File::checkOutputDirectoy(bool create) {
	bool allOK = false;
	struct stat buf ;


	if( stat( output_directory.c_str(), &buf ) == 0 ) {
		// output directory exists; check if it is really a directory
		if( S_ISDIR(buf.st_mode) ) {
			// OK is directory
			allOK = true;
		} else {
			// exists but not a directory
			printf("'%s' exists, but is not a directory. No files will be created.\n", output_directory.c_str() );
		}

	} else {
		// no, directory does not exist
		if( create ) {
			// create directory
			if( mkdir(output_directory.c_str(), 0777 ) == 0 ) {
				// all OK
				printf( "Output directory '%s' created.\n", output_directory.c_str() );
				allOK = true;
			} else {
				// error creating directory
				printf( "Error creating output directory '%s'.\n", output_directory.c_str() );
				allOK = false;
			}
		} else {
			printf( "Output directory '%s' does not exist. No files will be created.\n", output_directory.c_str() );
			allOK = false;
		}
	}

	return allOK;
}


/**
 * Check and save some data - if conditions are OK.
 * JK, 2013-07-08
 */
inline bool Diag2File::doAnyWriting(){

	switch( write_data_by ) {
	case WRITE_DATA_BY_STEP:
		// check step
		return (n_current == next_write_n);

	case WRITE_DATA_BY_TIME:
		// check time
		// return true if difference between given time and next write time is
		// less that half of simulation time step
		return ( t_current >= next_write_t ) ;

	}

	return false;
}


/**
 * Compute next step/time to do writing.
 */
void Diag2File::computeNextWritePoint() {
	int n_max = n_end;
	double t_max = t_end;


	if( n_end == 0 ) {
		// end not defined; use one less than max
		n_max = std::numeric_limits<int>::max()-1;
	}

	if( t_end == 0 ) {
		// end not defined; use one less than max
		t_max = std::numeric_limits<Scalar>::max()-0.1;
	}

	switch( write_data_by ) {
	case WRITE_DATA_BY_STEP:
		// check step
		if( n_step > 0 && next_write_n < n_max ) {
			// we have step defined; compute next step for writing with the use of step
			next_write_n += n_step;
			//cout << "n_max = " << n_max << " next_write = " << next_write_n << endl;

		} else {
			// we wrote all files; nothing to do any more;
			// set next write point as far away as possible
			next_write_n = std::numeric_limits<int>::max() - 1;
		}

		break;

	case WRITE_DATA_BY_TIME:
		// check time
		if( t_step > 0 && next_write_t < t_max  ) {
			// compute next step for writing with the use of time step
			next_write_t += t_step;
			//cout << "t_max = " << t_max << " next_write = " << next_write_t << endl;

		} else {
			// we wrote all files; nothing to do any more;
			// set next write point as far away as possible
			next_write_t = std::numeric_limits<Scalar>::max() - 0.1;
		}

		break;
	}
}


/**
 * Check parameters for writing data to file(s).
 */
void Diag2File::checkWriteParameters() {

	if( this->n > 0 ) {
		// single point (comp. step defined);
		set_write_data_by_step( this->n, 0, 0 );
		return;
	}

	if( this->t > 0 ) {
		// single point (time defined);
		set_write_data_by_time( this->t, 0, 0 );
		return;
	}

	if( this->n_step > 0 ) {
		// number of computational steps between files defined
		if( this->n_start > this->n_end && this->n_end > 0 ) {
			// end is less than start with ending step defined; swap values
			set_write_data_by_step(this->n_end, this->n_step, this->n_start);
		} else {
			// start and end defined OK
			set_write_data_by_step(this->n_start, this->n_step, this->n_end);
		}

		return;
	}

	if( this->t_step > 0 ) {
		// number of comp. steps between files defined
		if( this->t_start > this->t_end && this->t_end > 0) {
			// end is less than start with ending time defined; swap values
			set_write_data_by_time(this->t_end, this->t_step, this->t_start);
		} else {
			// start and end defined OK
			set_write_data_by_time(this->t_start, this->t_step, this->t_end);
		}

		return;
	}
}


/**
 * Do actual writing of data to given file handle.
 * JK, 2013-07-31
 */
void Diag2File::write_data(FILE *fp) {
	Scalar value;
	Vector3 vector;
	Vector3 *pv, *pv_copy;


	// printf(" -- write: %s, type = %d, n=%d, t=%g\n", VarName.c_str(), this->data_type, n_current, t_current);

	switch(this->data_type) {
	case DATA_TYPE_SCALAR:
		// vector of data
		if( this->write_append == 1 ) {
			// when appending, write time as first value and all other values into single line
			fprintf(fp, "%g", t_current);

			for( int i=0 ; i < *this->data_size[0] ; i++ ){
				fprintf(fp, " %g", *this->scalar_data[i]);
			}

			fprintf(fp, "\n");

		} else {
			// write index as first data and value as second
			for( int i=0 ; i < *this->data_size[0] ; i++ ){
				fprintf(fp, "%d %g\n", i, *this->scalar_data[i]);
			}
		}

		break;

	case DATA_TYPE_VECTOR3:
		// display one component of vector3 field; if coordinate == -1, display all components in single row
		if( this->vector_component == -1 ) {
			// display all components
			for( int i=0 ; i < *this->data_size[0] ; i++ ){
				pv = this->vector_data[i];

				for( int j=0 ; j < *this->data_size[1] ; j++ ){
					fprintf(fp, "%d %d %g %g %g %g %g\n", i, j, this->x1_array[i], this->x2_array[j], pv[j].e1(), pv[j].e2(), pv[j].e3());

				}
			}
		} else {
			// display value from single component
			for( int i=0 ; i < *this->data_size[0] ; i++ ){
				pv = this->vector_data[i];

				for( int j=0 ; j < *this->data_size[1] ; j++ ){
					fprintf(fp, "%d %d %g %g %g\n", i, j, this->x1_array[i], this->x2_array[j], pv[j].e(vector_component+1));
				}
			}
		}

		break;

	case DATA_TYPE_SCALAR_2D:
		// display scalar field
		for( int i=0 ; i < *this->data_size[0] ; i++ ){
			for( int j=0 ; j < *this->data_size[1] ; j++ ){
				fprintf(fp, "%d %d %g %g %g\n", i, j, this->x1_array[i], this->x2_array[j], this->scalar_data[i][j]);
			}
		}

		break;

	case DATA_TYPE_SPECIES:
		// display values for species (x, vx) || (t, x) || (t, vx)...
		for( int i=0 ; i < this->num_species ; i++ ){
			// display all species
			for( int j=0 ; j < *this->species_data_size[i] ; j++ ){
				// display data for current species
				fprintf(fp, "%d %d %g %g\n", i, j, this->species_data1[i][j], this->species_data2[i][j]);
			}
		}

		break;

	case DATA_TYPE_SPECIES_X_V:
		// display values for species: positions and velocities
		// "num_species" holds species index NOT number of species
		for( int i=0 ; i < *this->species_data_size[0] ; i++ ){
			fprintf(fp, "%d %d %g %g %g %g %g\n", this->num_species, i, this->species_data1[0][i], this->species_data1[1][i],
					this->species_data2[0][i], this->species_data2[1][i], this->species_data2[2][i]);
		}

		break;

	case DATA_TYPE_TIME_GRAPH:
		// display values for time graphs: time vs given values
		for( int i=0 ; i < *this->data_size[0] ; i++ ){
			fprintf(fp, "%d %g %g\n", i, this->x1_array[i], this->x2_array[i]);
		}

		break;

	case DATA_TYPE_SPECIES_TIME_GRAPH:
		// write species data that is time dependent
		if( this->write_append == 1 ) {
			// when appending, write time as first value and all other values into single line
			int numData;
			// for data, always write only last value at the moment of writing
			fprintf(fp, "%g", t_current);

			for( int i=0 ; i < this->num_species ; i++ ){
				// display last value for all species
				numData = *this->species_data_size[i];
				fprintf(fp, " %g", this->species_data2[i][numData-1]);
			}

			fprintf(fp, "\n");

		} else {
			// write index as first data and value as second
			for( int i=0 ; i < this->num_species ; i++ ){
				// display all species
				for( int j=0 ; j < *this->species_data_size[i] ; j++ ){
					// display data for current species
					fprintf(fp, "%d %d %g %g\n", i, j, this->species_data1[i][j], this->species_data2[i][j]);
				}
			}
		}

		break;
	}
}
