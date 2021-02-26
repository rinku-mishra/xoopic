/*  This is a class which maintains histories of quantities,
 *		for diagnostic purposes.
 */

#ifndef	__HISTORIES_H
#define	__HISTORIES_H

#include <string>
#include <stdio.h>
#include "xgio.h"


using namespace std;
class ostring;

class History
{
public:
	int hist_num; //number of elements that is being recorded (one more that hist_hi)
	int hist_hi;  //number of elements into the history
	int alloc_size; //size of the data array, so we don't overwrite it with history
	Scalar *time_array;  //  the array of the times for this history
	Scalar A1,A2,B1,B2;  // locations in space of the data 
	int j1,j2,k1,k2;  // locations in grid of the data.

	History(int _alloc_size = 1024) {
		alloc_size = _alloc_size;
		if(alloc_size < 0) alloc_size = 2;
		time_array = new Scalar[alloc_size+1];
		memset(time_array,0,(alloc_size+1) * sizeof(Scalar));
	}
	virtual ~History() {
		delete [] time_array;
		time_array = 0;
	}
	int *get_hi() {return &hist_num; };

	virtual void add(Scalar **data,Scalar time) { };
	virtual void add(Scalar data, Scalar time) { };
	virtual void add(Vector3*** data1, Vector3*** data2, Vector3** data3, Scalar time) { };
	virtual void add(Scalar ***data, Scalar time) { };
	virtual void add(Scalar *data, Scalar time) {};

	virtual void dump(FILE *DMPFile);
	virtual void restore_2_00(FILE *DMPFile);
	virtual void restore(FILE *DMPFile,Scalar _A1,Scalar _A2,Scalar _B1,Scalar _B2,int xl,int yl,Scalar A1,Scalar A2,Scalar B1,Scalar B2,int j1,int k1,int j2,int k2);
	virtual Scalar *get_time_array() { return time_array; };
	virtual Scalar **get_vdata() { return 0; };
	virtual Scalar **get_vpdata() { return 0; };
	virtual Scalar *get_data() { return 0; };
	virtual Scalar **get_region_data() { return 0; };
	void load_line_history_from_file(FILE *DMPFile,Scalar _A1,Scalar _A2,
			Scalar _B1,Scalar _B2,int xl,int yl,
			Scalar A1,Scalar A2,Scalar B1,Scalar B2,
			int vector_size, int hist_num, Scalar **data,
			int j1, int k1, int j2, int k2);
};

class Scalar_History: public History
{
	Scalar *data;
	int comb;  //how much to comb the history data when we've gone past the data size
	int n_comb;  //  number of times this history has been combed.
	int take_state;  //  should we take this entry as a call from add?
	int step;  //  take every "step"th element
	virtual void apply_array_keeper() { };
	void comb_arrays();

public:

	Scalar_History(int _alloc_size=1024, int _comb=4);
	virtual ~Scalar_History() {
		delete [] data;
		data = 0;
	}
	virtual void dump(FILE *DMPFile);
	virtual void restore_2_00(FILE *DMPFile);
	virtual void restore(FILE *DMPFile,Scalar _A1,Scalar _A2,Scalar _B1,Scalar _B2,int xl,int yl,Scalar A1,Scalar A2,Scalar B1,Scalar B2,int j1,int k1,int j2,int k2);

	virtual void add(Scalar value, Scalar time);
	virtual void add(Scalar **data,Scalar time) { History::add(data,time); };
	virtual void add(Vector3*** data1, Vector3*** data2, Vector3** data3, Scalar time) {
		History::add(data1,data2,data3,time);
	};
	virtual void add(Scalar ***data, Scalar time) { History::add(data,time); };
	virtual void add(Scalar *data, Scalar time) { History::add(data,time); };

	Scalar * get_data() { return data; };

};


class Scalar_Local_History: public History
{
	Scalar *data;
	int left_shift;  // how much to shift the whole history over when
	// we've exceeded the amount of memory we have
	// a value of 1 means we have to shift the whole
	// array every timestep
public:

	Scalar_Local_History(int _alloc_size=1024,int _left_shift=1);
	virtual ~Scalar_Local_History() {
		delete [] data;
		data = 0;
	}
	virtual void dump(FILE *DMPFile);
	virtual void restore_2_00(FILE *DMPFile);
	virtual void restore(FILE *DMPFile,Scalar _A1,Scalar _A2,Scalar _B1,Scalar _B2,int xl,int yl,Scalar A1,Scalar A2,Scalar B1,Scalar B2,int j1,int k1,int j2,int k2);

	virtual void add(Scalar value, Scalar time);
	virtual void add(Scalar **data,Scalar time) { History::add(data,time); };
	virtual void add(Vector3*** data1, Vector3*** data2, Vector3** data3, Scalar time) {
		History::add(data1,data2,data3,time);
	};
	virtual void add(Scalar ***data, Scalar time) { History::add(data,time); };
	virtual void add(Scalar *data, Scalar time) { History::add(data,time); };

	void shift_arrays();
	Scalar * get_data() { return data; };
};	

class Scalar_Ave_Local_History: public History
{
	Scalar *data;
	int left_shift;  // how much to shift the whole history over when
	// we've exceeded the amount of memory we have
	// a value of 1 means we have to shift the whole
	// array every timestep
	int ave;
	int step;
	int take_state;

public:

	Scalar_Ave_Local_History(int _alloc_size=1024,int _ave=1, int _left_shift=1);
	virtual ~Scalar_Ave_Local_History() {
		delete [] data;
		data = 0;
	}
	virtual void dump(FILE *DMPFile);
	virtual void restore_2_00(FILE *DMPFile);
	virtual void restore(FILE *DMPFile,Scalar _A1,Scalar _A2,Scalar _B1,Scalar _B2,int xl,int yl,Scalar A1,Scalar A2,Scalar B1,Scalar B2,int j1,int k1,int j2,int k2);

	virtual void add(Scalar value, Scalar time);
	virtual void add(Scalar **data,Scalar time) { History::add(data,time); };
	virtual void add(Vector3*** data1, Vector3*** data2, Vector3** data3, Scalar time) {
		History::add(data1,data2,data3,time);
	};
	virtual void add(Scalar ***data, Scalar time) { History::add(data,time); };
	virtual void add(Scalar *data, Scalar time) { History::add(data,time); };

	void shift_arrays();
	Scalar * get_data() { return data; };
};

class Scalar_Ave_History: public History
{
	Scalar *data;
	int left_shift;  // how much to shift the whole history over when
	// we've exceeded the amount of memory we have
	// a value of 1 means we have to shift the whole
	// array every timestep
	int ave, step_ave, take_state_ave;
	int comb, step_comb, n_comb, take_state_comb;
	int step;

public:

	Scalar_Ave_History(int _alloc_size=1024,int _ave=1, int _comb=2, int _left_shift=1);
	virtual ~Scalar_Ave_History() {
		delete [] data;
		data = 0;
	}
	virtual void dump(FILE *DMPFile);
	virtual void restore_2_00(FILE *DMPFile);
	virtual void restore(FILE *DMPFile,Scalar _A1,Scalar _A2,Scalar _B1,Scalar _B2,int xl,int yl,Scalar A1,Scalar A2,Scalar B1,Scalar B2,int j1,int k1,int j2,int k2);

	virtual void add(Scalar value, Scalar time);
	virtual void add(Scalar **data,Scalar time) { History::add(data,time); };
	virtual void add(Vector3*** data1, Vector3*** data2, Vector3** data3, Scalar time) {
		History::add(data1,data2,data3,time);
	};
	virtual void add(Scalar ***data, Scalar time) { History::add(data,time); };
	virtual void add(Scalar *data, Scalar time) { History::add(data,time); };

	void comb_arrays();
	Scalar * get_data() { return data; };
};

//  This class stores arrays as histories
class Vec_Pointers_History : public History
{
	Scalar **data;
	int comb;
	int n_comb;
	int take_state;
	int step;
	void comb_arrays();
	int vector_size;   //size of vector we're recording

public:
	virtual void dump(FILE *DMPFile);
	virtual void restore_2_00(FILE *DMPFile);
	virtual void restore(FILE *DMPFile,Scalar _A1,Scalar _A2,Scalar _B1,Scalar _B2,int xl,int yl,Scalar A1,Scalar A2,Scalar B1,Scalar B2,int j1,int k1,int j2,int k2);
	Vec_Pointers_History(int _vector_size, int _alloc_size=64, int _comb = 4);
	virtual ~Vec_Pointers_History();
	virtual void add(Scalar value, Scalar time) { History::add(value,time); };
	virtual void add(Scalar **values, Scalar time);
	virtual void add(Vector3*** data1, Vector3*** data2, Vector3** data3, Scalar time) {
		History::add(data1,data2,data3,time);
	};
	virtual void add(Scalar ***data, Scalar time) { History::add(data,time); };
	virtual void add(Scalar *data, Scalar time) { History::add(data,time); };

	Scalar **get_vpdata() { return data; };
};

//  This class stores arrays as histories
class Vec_Pointers_History_Ave : public History
{
	Scalar **data;
	int comb;
	int ave;
	int n_comb;
	int take_state;
	int step;
	void comb_arrays();
	int vector_size;   //size of vector we're recording

public:
	virtual void dump(FILE *DMPFile);
	virtual void restore_2_00(FILE *DMPFile);
	virtual void restore(FILE *DMPFile,Scalar _A1,Scalar _A2,Scalar _B1,Scalar _B2,int xl,int yl,Scalar A1,Scalar A2,Scalar B1,Scalar B2,int j1,int k1,int j2,int k2);
	Vec_Pointers_History_Ave(int _vector_size, int _alloc_size=64, int _ave = 2, int _comb = 4);

	virtual void add(Scalar value, Scalar time) { History::add(value,time); };
	virtual void add(Scalar **values, Scalar time);
	virtual void add(Vector3*** data1, Vector3*** data2, Vector3** data3, Scalar time) {
		History::add(data1,data2,data3,time);
	};
	virtual void add(Scalar ***data, Scalar time) { History::add(data,time); };
	virtual void add(Scalar *data, Scalar time) { History::add(data,time); };

	Scalar **get_vpdata() { return data; };
};

//  This class stores arrays as histories
class Vec_Pointers_History_Local_Ave : public History
{
	Scalar **data;
	int left_shift;
	int ave;
	int take_state;
	int step;
	void shift_arrays();
	int vector_size;   //size of vector we're recording

public:
	virtual void dump(FILE *DMPFile);
	virtual void restore_2_00(FILE *DMPFile);
	virtual void restore(FILE *DMPFile,Scalar _A1,Scalar _A2,Scalar _B1,Scalar _B2,int xl,int yl,Scalar A1,Scalar A2,Scalar B1,Scalar B2,int j1,int k1,int j2,int k2);
	Vec_Pointers_History_Local_Ave(int _vector_size, int _alloc_size=64, int _ave = 2, int _comb = 4);

	virtual void add(Scalar value, Scalar time) { History::add(value,time); };
	virtual void add(Scalar **values, Scalar time);
	virtual void add(Vector3*** data1, Vector3*** data2, Vector3** data3, Scalar time) {
		History::add(data1,data2,data3,time);
	};
	virtual void add(Scalar ***data, Scalar time) { History::add(data,time); };
	virtual void add(Scalar *data, Scalar time) { History::add(data,time); };

	Scalar **get_vpdata() { return data; };
};


class Vec_Pointers_Local_History: public History
{
	Scalar **data;
	int left_shift;  // how much to shift the whole history over when
	// we've exceeded the amount of memory we have
	// a value of 1 means we have to shift the whole
	// array every timestep
	int vector_size; //size of vector we're recording
public:
	virtual void dump(FILE *DMPFile);
	virtual void restore_2_00(FILE *DMPFile);
	virtual void restore(FILE *DMPFile,Scalar _A1,Scalar _A2,Scalar _B1,Scalar _B2,int xl,int yl,Scalar A1,Scalar A2,Scalar B1,Scalar B2,int j1,int k1,int j2,int k2);
	Vec_Pointers_Local_History(int _vector_size, int _alloc_size=64,int _left_shift=1);

	virtual void add(Scalar value, Scalar time) { History::add(value,time); };
	virtual void add(Scalar **values, Scalar time);
	virtual void add(Vector3*** data1, Vector3*** data2, Vector3** data3, Scalar time) {
		History::add(data1,data2,data3,time);
	};
	virtual void add(Scalar ***data, Scalar time) { History::add(data,time); };
	virtual void add(Scalar *data, Scalar time) { History::add(data,time); };

	void shift_arrays();
	Scalar ** get_vdata() { return data; };
};	

//  This class stores arrays as histories
class Vector_History : public History
{
	Scalar **data;
	int comb;
	int n_comb;
	int take_state;
	int step;
	void comb_arrays();
	int vector_size;   //size of vector we're recording

public:
	virtual void dump(FILE *DMPFile);
	virtual void restore_2_00(FILE *DMPFile);
	virtual void restore(FILE *DMPFile,Scalar _A1,Scalar _A2,Scalar _B1,Scalar _B2,int xl,int yl,Scalar A1,Scalar A2,Scalar B1,Scalar B2,int j1,int k1,int j2,int k2);
	Vector_History(int _vector_size, int _alloc_size=64, int _comb = 4);

	virtual void add(Scalar value, Scalar time) { History::add(value,time); };
	virtual void add(Scalar **values, Scalar time) { History::add(values,time); };
	virtual void add(Vector3*** data1, Vector3*** data2, Vector3** data3, Scalar time) {
		History::add(data1,data2,data3,time);
	};
	virtual void add(Scalar ***data, Scalar time) { History::add(data,time); };
	virtual void add(Scalar *values, Scalar time);

	Scalar **get_vdata() { return data; };
};


class Vector_Local_History: public History
{
	Scalar **data;
	int left_shift;  // how much to shift the whole history over when
	// we've exceeded the amount of memory we have
	// a value of 1 means we have to shift the whole
	// array every timestep
	int vector_size; //size of vector we're recording
public:
	virtual void dump(FILE *DMPFile);
	virtual void restore_2_00(FILE *DMPFile);
	virtual void restore(FILE *DMPFile,Scalar _A1,Scalar _A2,Scalar _B1,Scalar _B2,int xl,int yl,Scalar A1,Scalar A2,Scalar B1,Scalar B2,int j1,int k1,int j2,int k2);
	Vector_Local_History(int _vector_size, int _alloc_size=64,int _left_shift=1);

	virtual void add(Scalar value, Scalar time) { History::add(value,time); };
	virtual void add(Scalar **values, Scalar time) { History::add(values,time); };
	virtual void add(Vector3*** data1, Vector3*** data2, Vector3** data3, Scalar time) {
		History::add(data1,data2,data3,time);
	};
	virtual void add(Scalar ***data, Scalar time) { History::add(data,time); };
	virtual void add(Scalar *values, Scalar time);

	void shift_arrays();
	Scalar **get_region_data() { return data; };
};	

class JE_Region_History: public History
{
	Scalar **data;
	int Ave;
	int array_size_x, array_size_y;

public:
	virtual void dump(FILE *DMPFile);
	virtual void restore_2_00(FILE *DMPFile);
	virtual void restore(FILE *DMPFile,Scalar _A1,Scalar _A2,Scalar _B1,Scalar _B2,int xl,int yl,Scalar A1,Scalar A2,Scalar B1,Scalar B2,int j1,int k1,int j2,int k2);
	JE_Region_History(int _array_size_x, int _array_size_y, int _Ave);

	virtual void add(Scalar value, Scalar time) { History::add(value,time); };
	virtual void add(Scalar **values, Scalar time) { History::add(values,time); };
	virtual void add(Vector3 ***values1, Vector3 ***values2, Vector3 **values3, Scalar time);
	virtual void add(Scalar ***data, Scalar time) { History::add(data,time); };
	virtual void add(Scalar *values, Scalar time) { History::add(values,time); };

	void clear_array();
	Scalar **get_region_data() {return data; };
};

class Region_History: public History
{
	Scalar **data;
	Scalar Ave;
	int array_size_x, array_size_y;

public:
	virtual void dump(FILE *DMPFile);
	virtual void restore_2_00(FILE *DMPFile);
	virtual void restore(FILE *DMPFile,Scalar _A1,Scalar _A2,Scalar _B1,Scalar _B2,int xl,int yl,Scalar A1,Scalar A2,Scalar B1,Scalar B2,int j1,int k1,int j2,int k2);
	Region_History(int _array_size_x, int _array_size_y, int _Ave);

	virtual void add(Scalar value, Scalar time) { History::add(value,time); };
	virtual void add(Scalar **values, Scalar time) { History::add(values,time); };
	virtual void add(Vector3*** data1, Vector3*** data2, Vector3** data3, Scalar time) {
		History::add(data1,data2,data3,time);
	};
	virtual void add(Scalar ***values, Scalar time);
	virtual void add(Scalar *values, Scalar time) { History::add(values,time); };

	void clear_array();
	Scalar **get_region_data() {return data; };
};	
#endif  // ifndef __HISTORIES_H



