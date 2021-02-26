/*  This is a class which maintains histories of quantities,
    for diagnostic purposes.  */
#include "ovector.h"
#include "history.h"
// #include "xgio.h"
void History::dump(FILE *DMPFile){
	XGWrite(&hist_num, 4, 1, DMPFile, "int");
	XGWrite(&hist_hi, 4, 1, DMPFile, "int");
	XGWrite(&alloc_size, 4, 1, DMPFile, "int");
	for(int i=0; i<hist_num; i++)
		XGWrite(&time_array[i], ScalarInt, 1, DMPFile, ScalarChar);
}
void History::restore_2_00(FILE *DMPFile){
	XGRead(&hist_num, 4, 1, DMPFile, "int");
	XGRead(&hist_hi, 4, 1, DMPFile, "int");
	XGRead(&alloc_size, 4, 1, DMPFile, "int");
	for(int i=0; i<hist_num; i++)
		XGRead(&time_array[i], ScalarInt, 1, DMPFile, ScalarChar);
}

void History::restore(FILE *DMPFile,Scalar _A1,Scalar _A2,Scalar _B1,Scalar _B2,int xl, int yl,Scalar A1,Scalar A2, Scalar B1,Scalar B2, int j1, int k1, int j2, int k2){
	XGRead(&hist_num, 4, 1, DMPFile, "int");
	XGRead(&hist_hi, 4, 1, DMPFile, "int");
	XGRead(&alloc_size, 4, 1, DMPFile, "int");
	for(int i=0; i<hist_num; i++)
		XGRead(&time_array[i], ScalarInt, 1, DMPFile, ScalarChar);
}

Scalar_History::Scalar_History(int _alloc_size, int _comb):History(_alloc_size) {
	comb = _comb;
	n_comb = 0;
	take_state = 0;
	hist_hi = -1;
	hist_num = 0;
	step = 1;
	if(comb < 2) comb = 2;
	data = new Scalar[alloc_size+1];
	memset(data,0,(alloc_size+1) * sizeof(Scalar));
}


void Scalar_History::add(Scalar value, Scalar time) {
	if(take_state) {
		take_state--;
		return;
	}
	take_state = step-1;  // reset the counter
	hist_hi++;
	hist_num = hist_hi+1;
	if(hist_hi >= alloc_size) comb_arrays();
	data[hist_hi] = value;
	time_array[hist_hi] = time;
}


void Scalar_History::comb_arrays(void) {
  int i;
  int j;
  
  hist_hi/=comb;
  hist_num = hist_hi+1;

  n_comb++;
	
  // discard elements by taking every "comb"th element

  for(i=0,j=0;i<hist_hi;i++,j+=comb) {
    data[i]=data[j];
    time_array[i]=time_array[j];
  }
  step*=comb;  // increase the step.
}

void Scalar_History::dump(FILE *DMPFile){
	History::dump(DMPFile);
	XGWrite(&hist_num, 4, 1, DMPFile, "int");
	XGWrite(&comb, 4, 1, DMPFile, "int");
	XGWrite(&n_comb, 4, 1, DMPFile, "int");
	XGWrite(&take_state, 4, 1, DMPFile, "int");
	XGWrite(&step, 4, 1, DMPFile, "int");

// the following 7 are redundant, but we need a total of 12 ints
	XGWrite(&step, 4, 1, DMPFile, "int"); //1
	XGWrite(&step, 4, 1, DMPFile, "int");
	XGWrite(&step, 4, 1, DMPFile, "int"); //3
	XGWrite(&step, 4, 1, DMPFile, "int");
	XGWrite(&step, 4, 1, DMPFile, "int"); //5
	XGWrite(&step, 4, 1, DMPFile, "int");
	XGWrite(&step, 4, 1, DMPFile, "int"); //7

	for(int i=0; i<hist_num; i++)
		XGWrite(&data[i], ScalarInt, 1, DMPFile, ScalarChar);
}
	
void Scalar_History::restore(FILE *DMPFile,Scalar _A1,
									  Scalar _A2,Scalar _B1,Scalar _B2,int xl, int yl,Scalar A1,Scalar A2, Scalar B1,Scalar B2, int j1, int k1, int j2, int k2){
	History::restore(DMPFile,_A1,_A2,_B1,_B2,xl, yl, A1, A2,B1,B2,j1,  k1,j2, k2);
	int dummy[11];
	int i;
	XGRead(&hist_num,4,1,DMPFile,"int");
	XGRead(&comb, 4, 1, DMPFile, "int");
	XGRead(&n_comb, 4, 1, DMPFile, "int");
	XGRead(&take_state, 4, 1, DMPFile, "int");
	XGRead(&step, 4, 1, DMPFile, "int");
	// read in the 7 unused ints
	XGRead(dummy,4,7,DMPFile,"int");
	for(i=0;i<hist_num;i++) 
		XGRead(&data[i],ScalarInt,1,DMPFile,ScalarChar);

}	

void Scalar_History::restore_2_00(FILE *DMPFile){
	History::restore_2_00(DMPFile);
	XGRead(&comb, 4, 1, DMPFile, "int");
	XGRead(&n_comb, 4, 1, DMPFile, "int");
	XGRead(&take_state, 4, 1, DMPFile, "int");
	XGRead(&step, 4, 1, DMPFile, "int");
	for(int i=0; i<hist_num; i++)
		XGRead(&data[i], ScalarInt, 1, DMPFile, ScalarChar);
}	
/*************************************************************************/

Scalar_Local_History::Scalar_Local_History(int _alloc_size,int _left_shift):
History(_alloc_size) {

	left_shift = _left_shift;
	alloc_size=_alloc_size;
	hist_hi = -1;
	hist_num = 0;
	if(left_shift > alloc_size) left_shift = alloc_size;
	if(left_shift < 1) left_shift = 1;
	data = new Scalar[alloc_size + 1];
	memset(data,0,(alloc_size+1)*sizeof(Scalar));
}

void Scalar_Local_History::add(Scalar value, Scalar time)
{
	hist_hi++;
	hist_num = hist_hi+1;
	if(hist_hi>=alloc_size) shift_arrays();
	data[hist_hi] = value;
	time_array[hist_hi] = time;
}


void Scalar_Local_History::shift_arrays()
{
	int i,j;

	// unfortunately, we can't just do this cyclically because
	// of limitations of our graphics library

	for(i=left_shift,j=0;i<hist_hi;i++,j++) {
		data[j]=data[i];
		time_array[j]=time_array[i];
	}
	hist_hi-=left_shift;
	hist_num = hist_hi+1;
}
void Scalar_Local_History::dump(FILE *DMPFile){
	History::dump(DMPFile);
	XGWrite(&hist_num, 4, 1, DMPFile, "int");
	XGWrite(&left_shift, 4, 1, DMPFile, "int");

	// the following 10 ints are redundant, but necessary
	XGWrite(&left_shift, 4, 1, DMPFile, "int"); //1
	XGWrite(&left_shift, 4, 1, DMPFile, "int"); //2 
	XGWrite(&left_shift, 4, 1, DMPFile, "int"); //3
	XGWrite(&left_shift, 4, 1, DMPFile, "int"); //4 
	XGWrite(&left_shift, 4, 1, DMPFile, "int"); //5
	XGWrite(&left_shift, 4, 1, DMPFile, "int"); //6
	XGWrite(&left_shift, 4, 1, DMPFile, "int"); //7
	XGWrite(&left_shift, 4, 1, DMPFile, "int"); //8
	XGWrite(&left_shift, 4, 1, DMPFile, "int"); //9
	XGWrite(&left_shift, 4, 1, DMPFile, "int"); //10
	
	for(int i=0; i<hist_num; i++)
		XGWrite(&data[i], ScalarInt, 1, DMPFile, ScalarChar);
}
	
void Scalar_Local_History::restore_2_00(FILE *DMPFile){
	History::restore_2_00(DMPFile);
	XGRead(&left_shift, 4, 1, DMPFile, "int");
	for(int i=0; i<hist_num; i++)
		XGRead(&data[i], ScalarInt, 1, DMPFile, ScalarChar);
}	

void Scalar_Local_History::restore(FILE *DMPFile,Scalar _A1,
									  Scalar _A2,Scalar _B1,Scalar _B2,int xl, int yl,Scalar A1,Scalar A2, Scalar B1,Scalar B2, int j1, int k1, int j2, int k2){
	History::restore(DMPFile,_A1, _A2, _B1,_B2, xl,  yl, A1, A2,  B1, B2, j1,  k1,  j2,  k2);
	int dummy[11];
	XGRead(&hist_num, 4, 1, DMPFile,"int");
	XGRead(&left_shift, 4, 1, DMPFile, "int");
	XGRead(dummy, 4, 10, DMPFile, "int");
	for(int i=0; i<hist_num; i++)
		XGRead(&data[i], ScalarInt, 1, DMPFile, ScalarChar);
}	
/**************************************************************************/

Scalar_Ave_Local_History::Scalar_Ave_Local_History(int _alloc_size,int _ave, int _left_shift):
History(_alloc_size) {

	left_shift = _left_shift;
	alloc_size=_alloc_size;
	ave = _ave;
	hist_hi = 0;
	hist_num = 0;
	step = ave;
	take_state = step;
	if(left_shift > alloc_size) left_shift = alloc_size;
	if(left_shift < 1) left_shift = 1;
	data = new Scalar[alloc_size + 1];
	memset(data,0,(alloc_size+1)*sizeof(Scalar));
}

void Scalar_Ave_Local_History::add(Scalar value, Scalar time)
{
	if(take_state) {
		take_state--;
		data[hist_hi]+= value/step;		
		return;
	}
	take_state = step-1;  // reset the counter
	hist_hi++;
	hist_num = hist_hi;
	if(hist_hi>=alloc_size) shift_arrays();
	data[hist_hi] = value/ave;
	time_array[hist_hi] = time;
}


void Scalar_Ave_Local_History::shift_arrays()
{
	int i,j;

	// unfortunately, we can't just do this cyclically because
	// of limitations of our graphics library

	for(i=left_shift,j=0;i<hist_hi;i++,j++) {
		data[j]=data[i];
		time_array[j]=time_array[i];
	}
	hist_hi-=left_shift;
	hist_num = hist_hi;
}

void Scalar_Ave_Local_History::dump(FILE *DMPFile){
	History::dump(DMPFile);
	XGWrite(&hist_num, 4, 1, DMPFile, "int");
	XGWrite(&left_shift, 4, 1, DMPFile, "int");
	XGWrite(&ave, 4, 1, DMPFile, "int");
	XGWrite(&step, 4, 1, DMPFile, "int");
	XGWrite(&take_state, 4, 1, DMPFile, "int");

	// the following 7 ints are redundant, but necessary
	XGWrite(&take_state, 4, 1, DMPFile, "int");  //1
	XGWrite(&take_state, 4, 1, DMPFile, "int"); 
	XGWrite(&take_state, 4, 1, DMPFile, "int");  //3
	XGWrite(&take_state, 4, 1, DMPFile, "int");
	XGWrite(&take_state, 4, 1, DMPFile, "int");  //5
	XGWrite(&take_state, 4, 1, DMPFile, "int");
	XGWrite(&take_state, 4, 1, DMPFile, "int");  //7

	for(int i=0; i<hist_num; i++)
		XGWrite(&data[i], ScalarInt, 1, DMPFile, ScalarChar);
}
	
void Scalar_Ave_Local_History::restore_2_00(FILE *DMPFile){
	History::restore_2_00(DMPFile);
	XGRead(&left_shift, 4, 1, DMPFile, "int");
	XGRead(&ave, 4, 1, DMPFile, "int");
	XGRead(&step, 4, 1, DMPFile, "int");
	XGRead(&take_state, 4, 1, DMPFile, "int");
	for(int i=0; i<hist_num; i++)
		XGRead(&data[i], ScalarInt, 1, DMPFile, ScalarChar);
}	

void Scalar_Ave_Local_History::restore(FILE *DMPFile,Scalar _A1,
									  Scalar _A2,Scalar _B1,Scalar _B2,int xl, int yl,Scalar A1,Scalar A2, Scalar B1,Scalar B2, int j1, int k1, int j2, int k2){
	History::restore(DMPFile,_A1,_A2, _B1, _B2, xl, yl,A1, A2,  B1, B2, j1,  k1,  j2,  k2);
	int dummy[11];
	XGRead(&hist_num, 4, 1, DMPFile, "int");
	XGRead(&left_shift, 4, 1, DMPFile, "int");
	XGRead(&ave, 4, 1, DMPFile, "int");
	XGRead(&step, 4, 1, DMPFile, "int");
	XGRead(&take_state, 4, 1, DMPFile, "int");
	// read in unused ints
	XGRead(dummy,4,7,DMPFile,"int");
	for(int i=0; i<hist_num; i++)
		XGRead(&data[i], ScalarInt, 1, DMPFile, ScalarChar);
}	
/***************************************************************************/

Scalar_Ave_History::Scalar_Ave_History(int _alloc_size, int _ave, int _comb, int _left_shift):
History(_alloc_size) {
	comb = _comb;
	ave = _ave;
	left_shift = _left_shift;
	alloc_size=_alloc_size;
	hist_hi = -1;
	hist_num = 0;
	n_comb = 0;
	take_state_ave = 0;
	take_state_comb = 0;
	step = 1;
	if(comb < 2) comb = 2;
	if(left_shift > alloc_size) left_shift = alloc_size;
	if(left_shift < 1) left_shift = 1;
	data = new Scalar[alloc_size + 1];
	memset(data,0,(alloc_size+1)*sizeof(Scalar));
}

void Scalar_Ave_History::add(Scalar value, Scalar time)
{
	if(take_state_comb) {
		take_state_comb--;
		return;
	}
	take_state_comb = step-1;  // reset the counter
	if(take_state_ave) {
		take_state_ave--;
		data[hist_hi] += value/ave;		
		return;
	}
	take_state_ave = ave-1;  // reset the counter
	hist_hi++;
	hist_num = hist_hi;
	if(hist_hi>=alloc_size) comb_arrays();
	data[hist_hi] = value/ave;
	time_array[hist_hi] = time;
}

void Scalar_Ave_History::comb_arrays()
{
	int i,j;

	hist_hi/=comb;
	hist_num = hist_hi;

	n_comb++;
	
	// discard elements by taking every "comb"th element

	for(i=0,j=0;i<hist_hi;i++,j+=comb) {
		data[i]=data[j];
		time_array[i]=time_array[j];
	}
	step*=comb;  // increase the step.
}
void Scalar_Ave_History::dump(FILE *DMPFile){
	History::dump(DMPFile);
	XGWrite(&hist_num, 4, 1, DMPFile, "int");
	XGWrite(&left_shift, 4, 1, DMPFile, "int");
	XGWrite(&ave, 4, 1, DMPFile, "int");
	XGWrite(&step_ave, 4, 1, DMPFile, "int");
	XGWrite(&take_state_ave, 4, 1, DMPFile, "int");
	XGWrite(&comb, 4, 1, DMPFile, "int");
	XGWrite(&step_comb, 4, 1, DMPFile, "int");
	XGWrite(&n_comb, 4, 1, DMPFile, "int");
	XGWrite(&take_state_comb, 4, 1, DMPFile, "int");
	XGWrite(&step, 4, 1, DMPFile, "int");

	// the following 2 are redundant, but necessary
	XGWrite(&step, 4, 1, DMPFile, "int");
	XGWrite(&step, 4, 1, DMPFile, "int");

	for(int i=0; i<hist_num; i++)
		XGWrite(&data[i], ScalarInt, 1, DMPFile, ScalarChar);
}
	
void Scalar_Ave_History::restore_2_00(FILE *DMPFile){
	History::restore_2_00(DMPFile);
	XGRead(&left_shift, 4, 1, DMPFile, "int");
	XGRead(&ave, 4, 1, DMPFile, "int");
	XGRead(&step_ave, 4, 1, DMPFile, "int");
	XGRead(&take_state_ave, 4, 1, DMPFile, "int");
	XGRead(&comb, 4, 1, DMPFile, "int");
	XGRead(&step_comb, 4, 1, DMPFile, "int");
	XGRead(&n_comb, 4, 1, DMPFile, "int");
	XGRead(&take_state_comb, 4, 1, DMPFile, "int");
	XGRead(&step, 4, 1, DMPFile, "int");
	for(int i=0; i<hist_num; i++)
		XGRead(&data[i], ScalarInt, 1, DMPFile, ScalarChar);
}
	
void Scalar_Ave_History::restore(FILE *DMPFile,Scalar _A1,
									  Scalar _A2,Scalar _B1,Scalar _B2,int xl, int yl,Scalar A1,Scalar A2, Scalar B1,Scalar B2, int j1, int k1, int j2, int k2){
	History::restore(DMPFile,_A1, _A2, _B1, _B2, xl,  yl, A1, A2, B1, B2, j1,  k1,  j2,  k2);
	int dummy[11];

	XGRead(&hist_num, 4, 1, DMPFile, "int");
	XGRead(&left_shift, 4, 1, DMPFile, "int");
	XGRead(&ave, 4, 1, DMPFile, "int");
	XGRead(&step_ave, 4, 1, DMPFile, "int");
	XGRead(&take_state_ave, 4, 1, DMPFile, "int");
	XGRead(&comb, 4, 1, DMPFile, "int");
	XGRead(&step_comb, 4, 1, DMPFile, "int");
	XGRead(&n_comb, 4, 1, DMPFile, "int");
	XGRead(&take_state_comb, 4, 1, DMPFile, "int");
	XGRead(&step, 4, 1, DMPFile, "int");

	// read in the unused ints
	XGRead(dummy,4,2,DMPFile,"int");
	for(int i=0; i<hist_num; i++)
		XGRead(&data[i], ScalarInt, 1, DMPFile, ScalarChar);
}	
/**************************************************************************/

Vec_Pointers_History::Vec_Pointers_History(int _vector_size,int _alloc_size, int _comb):History(_alloc_size) {
	comb = _comb;
	vector_size = _vector_size;
	if(vector_size < 1) vector_size = 1;
	n_comb = 0;
	take_state = 0;
	hist_hi = -1;
	hist_num = 0;
	step = 1;
	if(comb < 2) comb = 2;
	data = new Scalar*[vector_size];
	for(int i=0;i<vector_size;i++) {
		data[i]=new Scalar[alloc_size+1];
		///memset(data[i],0,(alloc_size+1) * sizeof(Scalar));
		for ( int j = 0; j <= alloc_size; j++)
		  data[i][j] = 0.0;
	}

}

Vec_Pointers_History::~Vec_Pointers_History() {
  if(data) {
    for ( int i = 0; i < vector_size; i++ )
      delete [] data[i];
    delete [] data;
  }
}

void Vec_Pointers_History::add(Scalar **values, Scalar time) {
	if(take_state) {
		take_state--;
		return;
	}
	take_state = step-1;  // reset the counter
	hist_hi++;
	hist_num = hist_hi+1;
	if(hist_hi >= alloc_size) comb_arrays();

	for(int i=0;i<vector_size;i++)
		data[i][hist_hi] = *values[i];

	time_array[hist_hi] = time;
}


void Vec_Pointers_History::comb_arrays(void) {
	int i;
	int j;

	hist_hi/=comb;
	hist_num = hist_hi+1;

	n_comb++;
	
	// discard elements by taking every "comb"th element

	for(i=0,j=0;i<hist_hi;i++,j+=comb) {
		for(int k=0;k<vector_size;k++)
			data[k][i]=data[k][j];
		time_array[i]=time_array[j];
	}
	step*=comb;  // increase the step.
}
void Vec_Pointers_History::dump(FILE *DMPFile){
	History::dump(DMPFile);
	int datasize = vector_size * hist_num;
	XGWrite(&datasize, 4, 1, DMPFile, "int");
	XGWrite(&comb, 4, 1, DMPFile, "int");
	XGWrite(&n_comb, 4, 1, DMPFile, "int");
	XGWrite(&take_state, 4, 1, DMPFile, "int");
	XGWrite(&step, 4, 1, DMPFile, "int");
	XGWrite(&vector_size, 4, 1, DMPFile, "int");

	// the following 6 ints are redundant but necessary
	XGWrite(&vector_size, 4, 1, DMPFile, "int");
	XGWrite(&vector_size, 4, 1, DMPFile, "int");
	XGWrite(&vector_size, 4, 1, DMPFile, "int");
	XGWrite(&vector_size, 4, 1, DMPFile, "int");
	XGWrite(&vector_size, 4, 1, DMPFile, "int");
	XGWrite(&vector_size, 4, 1, DMPFile, "int");

	for(int v=0;v<vector_size;v++)
		for(int i=0; i<hist_num; i++)
			XGWrite(&data[v][i], ScalarInt, 1, DMPFile, ScalarChar);

}
	
void Vec_Pointers_History::restore_2_00(FILE *DMPFile){
	History::restore_2_00(DMPFile);
	XGRead(&comb, 4, 1, DMPFile, "int");
	XGRead(&n_comb, 4, 1, DMPFile, "int");
	XGRead(&take_state, 4, 1, DMPFile, "int");
	XGRead(&step, 4, 1, DMPFile, "int");
	XGRead(&vector_size, 4, 1, DMPFile, "int");
	for(int v=0;v<vector_size;v++)
		for(int i=0; i<hist_num; i++)
			XGRead(&data[v][i], ScalarInt, 1, DMPFile, ScalarChar);

}	

	
void Vec_Pointers_History::restore(FILE *DMPFile,Scalar _A1,
									  Scalar _A2,Scalar _B1,Scalar _B2,int xl, int yl,Scalar A1,Scalar A2, Scalar B1,Scalar B2, int j1, int k1, int j2, int k2){
	History::restore(DMPFile, _A1, _A2, _B1, _B2, xl,  yl, A1, A2,  B1, B2, j1,  k1,  j2, k2);
	int datasize;
	int dummy[11];
//	int i;
//	Scalar discard;

	XGRead(&datasize, 4, 1, DMPFile, "int");
	XGRead(&comb, 4, 1, DMPFile, "int");
	XGRead(&n_comb, 4, 1, DMPFile, "int");
	XGRead(&take_state, 4, 1, DMPFile, "int");
	XGRead(&step, 4, 1, DMPFile, "int");
	XGRead(&vector_size, 4, 1, DMPFile, "int");
	// read the unused ints
	XGRead(dummy,4,6,DMPFile,"int");

	// read in the history data on a line.
	History::load_line_history_from_file(DMPFile,_A1,_A2,_B1,_B2,xl,yl,
												  A1,A2,B1,B2,vector_size,hist_num,data,j1,k1,j2,k2);

}


	// Reads in the data from the dump file into the right part
	// of the array.
void History::load_line_history_from_file(FILE *DMPFile,Scalar _A1,Scalar _A2,
												 Scalar _B1,Scalar _B2,int xl,int yl,
												 Scalar A1,Scalar A2,Scalar B1,Scalar B2,
												 int _vector_size, int _hist_num, Scalar **_data,int j1,int k1,int j2,int k2)
{
	int is=0;
	int i;
	int datalength=MAX(xl,yl);
	int mydatalength=MAX((j2-j1),(k2-k1));
	Scalar ReadLength=MAX(_B1-_A1,_B2-_A2);
	Scalar MyLength=MAX(B1-A1,B2-A2);
	Scalar mydelta = MyLength/mydatalength;
	Scalar indelta = ReadLength/datalength;
	// starting (and counter) position for data read in
	Scalar currentDataPos=(_A1!=_B1)?MIN(_A1,_B1):MIN(_A2,_B2);
	// start "physical" position of the data in memory
	Scalar myStart = (A1!=B1)?MIN(A1,B1):MIN(A2,B2);
	Scalar myEnd = (A1!=B1)?MAX(A1,B1):MAX(A2,B2);
	Scalar discard;


	for(int v=0;v<_vector_size;v++) {
		int index;
		
		index = (int) ( (currentDataPos - myStart) / mydelta + 0.5);
		
		// make sure we didn't make a rounding mistake at 0.
		if(currentDataPos - myStart < -0.5) index = -1;
		if(currentDataPos > myEnd + mydelta/2.0) index = -1;
		currentDataPos += mydelta;
		if(index >=0) {
			for(i=0; i<_hist_num; i++)
				XGRead(&_data[index][i], ScalarInt, 1, DMPFile, ScalarChar);
		} else
			for(i=0;i<hist_num;i++) XGRead(&discard,ScalarInt,1, DMPFile,ScalarChar);
	}
}
	
/*********************************************************************/

Vec_Pointers_History_Ave::Vec_Pointers_History_Ave(int _vector_size,
								   int _alloc_size, int _ave, int _comb)
:History(_alloc_size) {
	comb = _comb;
	ave = _ave;
	vector_size = _vector_size;
	if(vector_size < 1) vector_size = 1;
	n_comb = 0;
	take_state = 0;
	hist_hi = -1;
	hist_num = 0;
	step = ave;
	if(comb < 2) comb = 2;
	data = new Scalar*[vector_size];
	for(int i=0;i<vector_size;i++) {
		data[i]=new Scalar[alloc_size+1];
		memset(data[i],0,(alloc_size+1) * sizeof(Scalar));
	}
}


void Vec_Pointers_History_Ave::add(Scalar **values, Scalar time) {
	if(take_state) {
		take_state--;
		for(int i=0;i<vector_size;i++)
			data[i][hist_hi] += *values[i]/step;		
		return;
	}
	take_state = step-1;  // reset the counter
	hist_hi++;
	hist_num = hist_hi;
	if(hist_hi >= alloc_size) comb_arrays();

	for(int i=0;i<vector_size;i++)
		data[i][hist_hi] = *values[i]/step;

	time_array[hist_hi] = time;
}


void Vec_Pointers_History_Ave::comb_arrays(void) {
	int i;
	int j;

	hist_hi/=comb;
	hist_num = hist_hi;

	n_comb++;
	
	// discard elements by taking every "comb"th element

	Scalar temp=0;

	for(i=0,j=0;i<hist_hi;i++,j+=comb) {
		for(int k=0;k<vector_size;k++)
			{
				for(int m=0;m<comb;m++)
					{
						temp += data[k][j+m];
						data[k][j+m] = 0.0;
					}
				data[k][i] =temp/comb;
				temp =0;
			}
		time_array[i]=time_array[j];
	}
	step*=comb;  // increase the step.
	take_state = step-1;
}
void Vec_Pointers_History_Ave::dump(FILE *DMPFile){
	History::dump(DMPFile);
	int datasize = vector_size * hist_num;
	XGWrite(&datasize, 4, 1, DMPFile, "int");
	XGWrite(&comb, 4, 1, DMPFile, "int");
	XGWrite(&ave, 4, 1, DMPFile, "int");
	XGWrite(&n_comb, 4, 1, DMPFile, "int");
	XGWrite(&take_state, 4, 1, DMPFile, "int");
	XGWrite(&step, 4, 1, DMPFile, "int");
	XGWrite(&vector_size, 4, 1, DMPFile, "int");

	// redundant, but necessary
	XGWrite(&vector_size, 4, 1, DMPFile, "int");
	XGWrite(&vector_size, 4, 1, DMPFile, "int");
	XGWrite(&vector_size, 4, 1, DMPFile, "int");
	XGWrite(&vector_size, 4, 1, DMPFile, "int");
	XGWrite(&vector_size, 4, 1, DMPFile, "int");

	for(int v=0;v<vector_size;v++)
		for(int i=0; i<hist_num; i++)
			XGWrite(&data[v][i], ScalarInt, 1, DMPFile, ScalarChar);
}
	
void Vec_Pointers_History_Ave::restore_2_00(FILE *DMPFile){
	History::restore_2_00(DMPFile);
	XGRead(&comb, 4, 1, DMPFile, "int");
	XGRead(&ave, 4, 1, DMPFile, "int");
	XGRead(&n_comb, 4, 1, DMPFile, "int");
	XGRead(&take_state, 4, 1, DMPFile, "int");
	XGRead(&step, 4, 1, DMPFile, "int");
	XGRead(&vector_size, 4, 1, DMPFile, "int");
	for(int v=0;v<vector_size;v++)
		for(int i=0; i<hist_num; i++)
			XGRead(&data[v][i], ScalarInt, 1, DMPFile, ScalarChar);
}	


void Vec_Pointers_History_Ave::restore(FILE *DMPFile,Scalar _A1,
									  Scalar _A2,Scalar _B1,Scalar _B2,int xl, int yl,Scalar A1,Scalar A2, Scalar B1,Scalar B2, int j1, int k1, int j2, int k2){
	History::restore(DMPFile, _A1,_A2, _B1, _B2, xl, yl, A1, A2,  B1, B2,j1, k1, j2, k2);
	int datasize;
	int dummy[11];
	XGRead(&datasize, 4, 1, DMPFile, "int");
	XGRead(&comb, 4, 1, DMPFile, "int");
	XGRead(&ave, 4, 1, DMPFile, "int");
	XGRead(&n_comb, 4, 1, DMPFile, "int");
	XGRead(&take_state, 4, 1, DMPFile, "int");
	XGRead(&step, 4, 1, DMPFile, "int");
	XGRead(&vector_size, 4, 1, DMPFile, "int");
	XGRead(dummy,4,5,DMPFile,"int");

	// load in the data along the line
	History::load_line_history_from_file(DMPFile,_A1,_A2,_B1,_B2,xl,yl,
												  A1,A2,B1,B2,vector_size,hist_num,data,j1,k1,j2,k2);

}	
/**************************************************************************/
Vec_Pointers_History_Local_Ave::Vec_Pointers_History_Local_Ave(int _vector_size, 
int _alloc_size, int _ave, int _left_shift)
:History(_alloc_size) {
	ave = _ave;
	vector_size = _vector_size;
	if(vector_size < 1) vector_size = 1;

	left_shift = _left_shift;
	if(left_shift > alloc_size) left_shift = alloc_size;
	if(left_shift < 1) left_shift = 1;

	take_state = 0;
	hist_hi = -1;
	hist_num = 0;
	step = ave;

	data = new Scalar*[vector_size];
	for(int i=0;i<vector_size;i++) {
		data[i]=new Scalar[alloc_size+1];
		memset(data[i],0,(alloc_size+1) * sizeof(Scalar));
	}
}


void Vec_Pointers_History_Local_Ave::add(Scalar **values, Scalar time) {
	if(take_state) {
		take_state--;
		for(int i=0;i<vector_size;i++)
			data[i][hist_hi] += *values[i]/step;		
		return;
	}
	take_state = step-1;  // reset the counter
	hist_hi++;
	hist_num = hist_hi;
	if(hist_hi >= alloc_size) shift_arrays();

	for(int i=0;i<vector_size;i++)
		data[i][hist_hi] = *values[i]/step;

	time_array[hist_hi] = time;
}


void Vec_Pointers_History_Local_Ave::shift_arrays(void) {
	int i;
	int j;

	// unfortunately, we can't just do this cyclically because
	// of limitations of our graphics library

	for(i=left_shift,j=0;i<hist_hi;i++,j++) 
		{
			for(int k=0;k<vector_size;k++)
				data[k][j]=data[k][i];
			time_array[j]=time_array[i];
		}
	hist_hi-=left_shift;
	hist_num = hist_hi;

}

void Vec_Pointers_History_Local_Ave::dump(FILE *DMPFile){
	History::dump(DMPFile);
	int datasize = vector_size * hist_num;
	XGWrite(&datasize, 4, 1, DMPFile, "int");
	XGWrite(&left_shift, 4, 1, DMPFile, "int");
	XGWrite(&ave, 4, 1, DMPFile, "int");
	XGWrite(&take_state, 4, 1, DMPFile, "int");
	XGWrite(&step, 4, 1, DMPFile, "int");
	XGWrite(&vector_size, 4, 1, DMPFile, "int");

	// following are redundant but necessary.
	XGWrite(&vector_size, 4, 1, DMPFile, "int");
	XGWrite(&vector_size, 4, 1, DMPFile, "int");
	XGWrite(&vector_size, 4, 1, DMPFile, "int");
	XGWrite(&vector_size, 4, 1, DMPFile, "int");
	XGWrite(&vector_size, 4, 1, DMPFile, "int");
	XGWrite(&vector_size, 4, 1, DMPFile, "int");

	for(int v=0;v<vector_size;v++)
		for(int i=0; i<hist_num; i++)
			XGWrite(&data[v][i], ScalarInt, 1, DMPFile, ScalarChar);
}
	
void Vec_Pointers_History_Local_Ave::restore_2_00(FILE *DMPFile){
	History::restore_2_00(DMPFile);
	XGRead(&left_shift, 4, 1, DMPFile, "int");
	XGRead(&ave, 4, 1, DMPFile, "int");
	XGRead(&take_state, 4, 1, DMPFile, "int");
	XGRead(&step, 4, 1, DMPFile, "int");
	XGRead(&vector_size, 4, 1, DMPFile, "int");
	for(int v=0;v<vector_size;v++)
		for(int i=0; i<hist_num; i++)
			XGRead(&data[v][i], ScalarInt, 1, DMPFile, ScalarChar);
}

void Vec_Pointers_History_Local_Ave::restore(FILE *DMPFile,Scalar _A1,
									  Scalar _A2,Scalar _B1,Scalar _B2,int xl, int yl,Scalar A1,Scalar A2, Scalar B1,Scalar B2, int j1, int k1, int j2, int k2){
	History::restore(DMPFile,_A1, _A2, _B1, _B2, xl,yl, A1, A2,  B1, B2,j1, k1,j2,k2);

	int datasize;
	int dummy[11];
	XGRead(&datasize,4,1,DMPFile,"int");
	XGRead(&left_shift, 4, 1, DMPFile, "int");
	XGRead(&ave, 4, 1, DMPFile, "int");
	XGRead(&take_state, 4, 1, DMPFile, "int");
	XGRead(&step, 4, 1, DMPFile, "int");
	XGRead(&vector_size, 4, 1, DMPFile, "int");
	XGRead(dummy,4,6,DMPFile,"int");
	History::load_line_history_from_file(DMPFile,_A1,_A2,_B1,_B2,xl,yl,
													 A1,A2,B1,B2,vector_size,hist_num,data,j1,k1,j2,k2);


}
	
/**************************************************************************/

Vector_History::Vector_History(int _vector_size,int _alloc_size, int _comb):History(_alloc_size) {
	comb = _comb;
	vector_size = _vector_size;
	if(vector_size < 1) vector_size = 1;
	n_comb = 0;
	take_state = 0;
	hist_hi = -1;
	hist_num = 0;
	step = 1;
	if(comb < 2) comb = 2;
	data = new Scalar*[vector_size];
	for(int i=0;i<vector_size;i++) {
		data[i]=new Scalar[alloc_size+1];
		memset(data[i],0,(alloc_size+1) * sizeof(Scalar));
	}
}


void Vector_History::add(Scalar *values, Scalar time) {
	if(take_state) {
		take_state--;
		return;
	}
	take_state = step-1;  // reset the counter
	hist_hi++;
	hist_num = hist_hi+1;
	if(hist_hi >= alloc_size) comb_arrays();

	for(int i=0;i<vector_size;i++)
		data[i][hist_hi] = values[i];

	time_array[hist_hi] = time;
}


void Vector_History::comb_arrays(void) {
	int i;
	int j;

	hist_hi/=comb;
	hist_num = hist_hi+1;

	n_comb++;
	
	// discard elements by taking every "comb"th element

	for(i=0,j=0;i<hist_hi;i++,j+=comb) {
		for(int k=0;k<vector_size;k++)
			data[k][i]=data[k][j];
		time_array[i]=time_array[j];
	}
	step*=comb;  // increase the step.
}
void Vector_History::dump(FILE *DMPFile){
	History::dump(DMPFile);
	int datasize = vector_size * hist_num;
	XGWrite(&datasize, 4, 1, DMPFile, "int");
	XGWrite(&comb, 4, 1, DMPFile, "int");
	XGWrite(&n_comb, 4, 1, DMPFile, "int");
	XGWrite(&take_state, 4, 1, DMPFile, "int");
	XGWrite(&step, 4, 1, DMPFile, "int");
	XGWrite(&vector_size, 4, 1, DMPFile, "int");

	// the following 6 ints are redundant but necessary
	XGWrite(&vector_size, 4, 1, DMPFile, "int");
	XGWrite(&vector_size, 4, 1, DMPFile, "int");
	XGWrite(&vector_size, 4, 1, DMPFile, "int");
	XGWrite(&vector_size, 4, 1, DMPFile, "int");
	XGWrite(&vector_size, 4, 1, DMPFile, "int");
	XGWrite(&vector_size, 4, 1, DMPFile, "int");

	for(int v=0;v<vector_size;v++)
		for(int i=0; i<hist_num; i++)
			XGWrite(&data[v][i], ScalarInt, 1, DMPFile, ScalarChar);
}
	
void Vector_History::restore_2_00(FILE *DMPFile){
	History::restore_2_00(DMPFile);
	XGRead(&comb, 4, 1, DMPFile, "int");
	XGRead(&n_comb, 4, 1, DMPFile, "int");
	XGRead(&take_state, 4, 1, DMPFile, "int");
	XGRead(&step, 4, 1, DMPFile, "int");
	XGRead(&vector_size, 4, 1, DMPFile, "int");
	for(int v=0;v<vector_size;v++)
		for(int i=0; i<hist_num; i++)
			XGRead(&data[v][i], ScalarInt, 1, DMPFile, ScalarChar);
}

void Vector_History::restore(FILE *DMPFile, Scalar _A1, Scalar _A2, Scalar _B1, Scalar _B2,
                            int xl, int yl, Scalar  A1, Scalar  A2, Scalar  B1, Scalar  B2,
                            int j1, int k1, int j2, int k2 ) {
	History::restore(DMPFile, _A1, _A2, _B1, _B2, xl,  yl, A1, A2,  B1, B2, j1, k1, j2, k2);

	int datasize;
	int dummy[11];
	XGRead(&datasize,4,1,DMPFile,"int");
	XGRead(&comb, 4, 1, DMPFile, "int");
	XGRead(&n_comb, 4, 1, DMPFile, "int");
	XGRead(&take_state, 4, 1, DMPFile, "int");
	XGRead(&step, 4, 1, DMPFile, "int");
	XGRead(&vector_size, 4, 1, DMPFile, "int");

	//read in the unused ints
	XGRead(&dummy, 4, 6, DMPFile, "int");
	History::load_line_history_from_file(DMPFile,_A1,_A2,_B1,_B2,xl,yl,
												  A1,A2,B1,B2,vector_size,hist_num,data,j1,k1,j2,k2);


}
	
/************************************************************************/

Vec_Pointers_Local_History::Vec_Pointers_Local_History(int _vector_size,int _alloc_size,int _left_shift):
History(_alloc_size) {

	vector_size = _vector_size;
	left_shift = _left_shift;
	if(left_shift > alloc_size) left_shift = alloc_size;
	if(left_shift < 1) left_shift = 1;
	data = new Scalar*[vector_size];
	for(int i=0;i<vector_size;i++) {
		data[i]=new Scalar[alloc_size+1];
		memset(data[i],0,(alloc_size+1) * sizeof(Scalar));
	}
}

void Vec_Pointers_Local_History::add(Scalar** values, Scalar time)
{
	hist_hi++;
	hist_num = hist_hi+1;
	if(hist_hi>=alloc_size) shift_arrays();
	for(int i=0;i<vector_size;i++)
		data[i][hist_hi] = *values[i];
	time_array[hist_hi] = time;
}


void Vec_Pointers_Local_History::shift_arrays()
{
	int i,j;

	// unfortunately, we can't just do this cyclically because
	// of limitations of our graphics library

	for(i=left_shift,j=0;i<hist_hi;i++,j++) {	
		for(int k=0;k<vector_size;k++)
			data[k][i]=data[k][j];
		time_array[j]=time_array[i];
	}
	hist_hi-=left_shift;
	hist_num = hist_hi+1;
}
void Vec_Pointers_Local_History::dump(FILE *DMPFile){
	History::dump(DMPFile);
	int datasize = vector_size * hist_num;
	XGWrite(&datasize, 4, 1, DMPFile, "int");
	XGWrite(&left_shift, 4, 1, DMPFile, "int");
	XGWrite(&vector_size, 4, 1, DMPFile, "int");

	//redundant but necessary
	XGWrite(&vector_size, 4, 1, DMPFile, "int");
	XGWrite(&vector_size, 4, 1, DMPFile, "int");
	XGWrite(&vector_size, 4, 1, DMPFile, "int");
	XGWrite(&vector_size, 4, 1, DMPFile, "int");
	XGWrite(&vector_size, 4, 1, DMPFile, "int");
	XGWrite(&vector_size, 4, 1, DMPFile, "int");
	XGWrite(&vector_size, 4, 1, DMPFile, "int");
	XGWrite(&vector_size, 4, 1, DMPFile, "int");
	XGWrite(&vector_size, 4, 1, DMPFile, "int");

	for(int v=0;v<vector_size;v++)
		for(int i=0; i<hist_num; i++)
			XGWrite(&data[v][i], ScalarInt, 1, DMPFile, ScalarChar);
}
	
void Vec_Pointers_Local_History::restore_2_00(FILE *DMPFile){
	History::restore_2_00(DMPFile);
	XGRead(&left_shift, 4, 1, DMPFile, "int");
	XGRead(&vector_size, 4, 1, DMPFile, "int");
	for(int v=0;v<vector_size;v++)
		for(int i=0; i<hist_num; i++)
			XGRead(&data[v][i], ScalarInt, 1, DMPFile, ScalarChar);
}


void Vec_Pointers_Local_History::restore(FILE *DMPFile,Scalar _A1,
									  Scalar _A2,Scalar _B1,Scalar _B2,int xl, int yl,Scalar A1,Scalar A2, Scalar B1,Scalar B2, int j1, int k1, int j2, int k2){
	History::restore(DMPFile,_A1, _A2,_B1, _B2, xl,  yl, A1, A2,  B1, B2,j1,k1,j2,k2);

	int dummy[11];
	int datasize;

	XGRead(&datasize, 4, 1, DMPFile, "int");
	XGRead(&left_shift, 4, 1, DMPFile, "int");
	XGRead(&vector_size, 4, 1, DMPFile, "int");
	// read in the unused ints
	XGRead(dummy,4,9,DMPFile,"int");
	History::load_line_history_from_file(DMPFile,_A1,_A2,_B1,_B2,xl,yl,
												  A1,A2,B1,B2,vector_size,hist_num,data,j1,k1,j2,k2);

}
/*******************************************************************************/

Vector_Local_History::Vector_Local_History(int _vector_size,int _alloc_size,int _left_shift):
History(_alloc_size) {

	vector_size = _vector_size;
	left_shift = _left_shift;
	if(left_shift > alloc_size) left_shift = alloc_size;
	if(left_shift < 1) left_shift = 1;
	data = new Scalar*[vector_size];
	for(int i=0;i<vector_size;i++) {
		data[i]=new Scalar[alloc_size+1];
		memset(data[i],0,(alloc_size+1) * sizeof(Scalar));
	}
        hist_hi = 0;
        hist_num = 1;
}

void Vector_Local_History::add(Scalar* values, Scalar time)
{
	hist_hi++;
	hist_num = hist_hi+1;
	if(hist_hi>=alloc_size) shift_arrays();
	for(int i=0;i<vector_size;i++)
		data[i][hist_hi] = values[i];
	time_array[hist_hi] = time;
}


void Vector_Local_History::shift_arrays()
{
	int i,j;

	// unfortunately, we can't just do this cyclically because
	// of limitations of our graphics library

	for(i=left_shift,j=0;i<hist_hi;i++,j++) {	
		for(int k=0;k<vector_size;k++)
			data[k][j]=data[k][i];
		time_array[j]=time_array[i];
	}
	hist_hi-=left_shift;
	hist_num = hist_hi+1;
}
void Vector_Local_History::dump(FILE *DMPFile){
	History::dump(DMPFile);

	int datasize = vector_size * hist_num;
	XGWrite(&datasize, 4, 1, DMPFile, "int");
	XGWrite(&left_shift, 4, 1, DMPFile, "int");
	XGWrite(&vector_size, 4, 1, DMPFile, "int");

	// the following 9 ints are redundant but necessary
	XGWrite(&vector_size, 4, 1, DMPFile, "int");
	XGWrite(&vector_size, 4, 1, DMPFile, "int");
	XGWrite(&vector_size, 4, 1, DMPFile, "int");
	XGWrite(&vector_size, 4, 1, DMPFile, "int");
	XGWrite(&vector_size, 4, 1, DMPFile, "int");
	XGWrite(&vector_size, 4, 1, DMPFile, "int");
	XGWrite(&vector_size, 4, 1, DMPFile, "int");
	XGWrite(&vector_size, 4, 1, DMPFile, "int");
	XGWrite(&vector_size, 4, 1, DMPFile, "int");


	for(int v=0;v<vector_size;v++)
		for(int i=0; i<hist_num; i++)
			XGWrite(&data[v][i], ScalarInt, 1, DMPFile, ScalarChar);
}
	
void Vector_Local_History::restore_2_00(FILE *DMPFile){
	History::restore_2_00(DMPFile);
	XGRead(&left_shift, 4, 1, DMPFile, "int");
	XGRead(&vector_size, 4, 1, DMPFile, "int");
	for(int v=0;v<vector_size;v++)
		for(int i=0; i<hist_num; i++)
			XGRead(&data[v][i], ScalarInt, 1, DMPFile, ScalarChar);
}

void Vector_Local_History::restore(FILE *DMPFile,Scalar _A1,
									  Scalar _A2,Scalar _B1,Scalar _B2,int xl, int yl,Scalar A1,Scalar A2, Scalar B1,Scalar B2, int j1, int k1, int j2, int k2){
	History::restore(DMPFile,_A1, _A2, _B1, _B2, xl,  yl, A1, A2,  B1, B2,j1,k1,j2,k2);
	int datasize;
	int dummy[11];
	XGRead(&datasize, 4, 1, DMPFile, "int");
	XGRead(&left_shift, 4, 1, DMPFile, "int");
	XGRead(&vector_size, 4, 1, DMPFile, "int");

	XGRead(dummy,4,9,DMPFile,"int");
	History::load_line_history_from_file(DMPFile,_A1,_A2,_B1,_B2,xl,yl,
												  A1,A2,B1,B2,vector_size,hist_num,data,j1,k1,j2,k2);



}
/***************************************************************************/

JE_Region_History::JE_Region_History(int _vector_size_x, int _vector_size_y, int _Ave):
History(_vector_size_x){
  Ave = _Ave;
  array_size_x = _vector_size_x;
  array_size_y = _vector_size_y;
  data = new Scalar*[array_size_x];
  for(int i=0;i<array_size_x;i++) {
    data[i]=new Scalar[array_size_y];
    memset(data[i],0,(array_size_y) * sizeof(Scalar));
  }
  hist_hi = -1;
}

void JE_Region_History::add(Vector3 ***values1, Vector3 ***values2, Vector3 **values3, Scalar time)
{
  hist_hi++;
  if ((hist_hi>=Ave)&&(Ave>0)) clear_array();
  for (int j=0; j<array_size_x; j++)
    for (int k=0; k<array_size_y; k++)
      data[j][k] = (data[j][k]*hist_hi +
		    *values1[j][k]*((*values2[j][k]).jvDivide((values3[j][k]))))/(hist_hi+1);
}

void JE_Region_History::clear_array()
{
  for (int j=0; j<array_size_x; j++)
    for (int k=0; k<array_size_y; k++)
      data[j][k] = 0;
  hist_hi = 0;
}

void JE_Region_History::dump(FILE *DMPFile){
  History::dump(DMPFile);
  int datasize = array_size_x*array_size_y;
  XGWrite(&datasize, 4, 1, DMPFile, "int");
  XGWrite(&Ave, 4, 1, DMPFile, "int");
  XGWrite(&array_size_x, 4, 1, DMPFile, "int");
  XGWrite(&array_size_y, 4, 1, DMPFile, "int");
  // the following 8 ints are redundant but necessary
  XGWrite(&datasize, 4, 1, DMPFile, "int");
  XGWrite(&datasize, 4, 1, DMPFile, "int");
  XGWrite(&datasize, 4, 1, DMPFile, "int");
  XGWrite(&datasize, 4, 1, DMPFile, "int");
  XGWrite(&datasize, 4, 1, DMPFile, "int");
  XGWrite(&datasize, 4, 1, DMPFile, "int");
  XGWrite(&datasize, 4, 1, DMPFile, "int");
  XGWrite(&datasize, 4, 1, DMPFile, "int");

  for(int j=0;j<array_size_x;j++)
    for(int k=0; k<array_size_y; k++)
      XGWrite(&data[j][k], ScalarInt, 1, DMPFile, ScalarChar);
}
	
void JE_Region_History::restore_2_00(FILE *DMPFile){
  History::restore_2_00(DMPFile);
  XGWrite(&Ave, 4, 1, DMPFile, "int");
  XGRead(&array_size_x, 4, 1, DMPFile, "int");
  XGRead(&array_size_y, 4, 1, DMPFile, "int");
  for(int j=0;j<array_size_x;j++)
    for(int k=0; k<array_size_y; k++)
      XGRead(&data[j][k], ScalarInt, 1, DMPFile, ScalarChar);
}

void JE_Region_History::restore(FILE *DMPFile,Scalar _A1, Scalar _A2,Scalar _B1, Scalar _B2,
				int xl, int yl,Scalar A1,Scalar A2, Scalar B1,Scalar B2, 
				int j1, int k1, int j2, int k2){
  History::restore(DMPFile,_A1, _A2, _B1, _B2, xl,  yl, A1, A2,  B1, B2,j1,k1,j2,k2);

/* NOT DONE */

}

/*************************************************************************/

Region_History::Region_History(int _vector_size_x, int _vector_size_y, int _Ave):
History(_vector_size_x){
	hist_hi = -1;
	Ave = _Ave;
	array_size_x = _vector_size_x;
	array_size_y = _vector_size_y;
	data = new Scalar*[array_size_x];
	for(int i=0;i<array_size_x;i++) {
		data[i]=new Scalar[array_size_y];
		memset(data[i],0,(array_size_y) * sizeof(Scalar));
	}
}

void Region_History::add(Scalar ***values, Scalar time)
{
	hist_hi++;
	if ((hist_hi>=Ave)&&(Ave>0)) clear_array();
	for (int j=0; j<array_size_x; j++)
		for (int k=0; k<array_size_y; k++)
			data[j][k] = (data[j][k]*hist_hi + 
										*values[j][k])/(hist_hi+1);
}

void Region_History::clear_array()
{
	for (int j=0; j<array_size_x; j++)
		for (int k=0; k<array_size_y; k++)
			data[j][k] = 0;
	hist_hi = 0;
}
void Region_History::dump(FILE *DMPFile){
	History::dump(DMPFile);
	int datasize = array_size_x*array_size_y;
	XGWrite(&datasize, 4, 1, DMPFile, "int");
	XGWrite(&hist_hi, 4, 1, DMPFile, "int");
	XGWrite(&Ave, 4, 1, DMPFile, "int");
	XGWrite(&array_size_x, 4, 1, DMPFile, "int");
	XGWrite(&array_size_y, 4, 1, DMPFile, "int");
	// the following 7 ints are redundant but necessary
	XGWrite(&datasize, 4, 1, DMPFile, "int");
	XGWrite(&datasize, 4, 1, DMPFile, "int");
	XGWrite(&datasize, 4, 1, DMPFile, "int");
	XGWrite(&datasize, 4, 1, DMPFile, "int");
	XGWrite(&datasize, 4, 1, DMPFile, "int");
	XGWrite(&datasize, 4, 1, DMPFile, "int");
	XGWrite(&datasize, 4, 1, DMPFile, "int");

	for(int j=0;j<array_size_x;j++)
		for(int k=0; k<array_size_y; k++)
			XGWrite(&data[j][k], ScalarInt, 1, DMPFile, ScalarChar);
}
	
void Region_History::restore_2_00(FILE *DMPFile){
//	History::restore_2_00(DMPFile);
	XGRead(&hist_hi, 4, 1, DMPFile, "int");
	XGRead(&Ave, 4, 1, DMPFile, "int");
	XGRead(&array_size_x, 4, 1, DMPFile, "int");
	XGRead(&array_size_y, 4, 1, DMPFile, "int");
	for(int j=0;j<array_size_x;j++)
		for(int k=0; k<array_size_y; k++)
			XGRead(&data[j][k], ScalarInt, 1, DMPFile, ScalarChar);
}


void Region_History::restore(FILE *DMPFile,Scalar _A1,
									  Scalar _A2,Scalar _B1,Scalar _B2,int xl, int yl,Scalar A1,Scalar A2, Scalar B1,Scalar B2, int j1, int k1, int j2, int k2){
	History::restore(DMPFile,_A1, _A2, _B1, _B2, xl,  yl, A1, A2,  B1, B2,j1,k1,j2,k2);

/* NOT DONE */

}
