//param.h

#ifndef   __PARAMS_H
#define   __PARAMS_H

#ifdef UNIX
#include <config.h>
#endif

#include <string>
#include <stdlib.h>
#include <stdio.h>

#if defined(_MSC_VER)
#include <iomanip>
#include <sstream>
#include <iostream>
#include <fstream>
using std::ends;
#else

using namespace std;
#endif

#include "ostring.h"

#include "oopiclist.h"
#include "rules.h"
#include "misc.h"
#include "ovector.h"

class Evaluator;


//=================== BaseParameter Class
// base class for Parameters and ParameterGroups

class BaseParameter {
public:
	BaseParameter() {};
	virtual ~BaseParameter() {};
	virtual ostring getName()
		{ return ostring(""); }
	virtual ostring getValueString()
		{ return ostring(""); }  // checks type and sets new value
	virtual ostring setValue(ostring valueString)
		{ return ostring("");}  // checks type and sets new value
	virtual oopicList<BaseParameter>* getParameterList()
		{ return NULL; } // needed by InputParameterView
	virtual double  getValueAsDouble()
		{ return 0; } // needed by rules
	virtual void  showValue()
		{ } // display diagnostic
	virtual void checkRules()
		{ }
	virtual oopicList<ostring>* getErrorMessages()
		{ return NULL; } // needed by InputParameterView

	virtual void writeOutputFile(std::ofstream &fout)
		{}; //  writes to file
	virtual ostring getDescription()
		{ return ostring(""); };
//   static Evaluator *adv_eval;

};


//=================== Parameter Class
// base class for parameters

class Parameter :public BaseParameter
{protected:
  ostring name;
  ostring description;
	//List<ostring> ruleMessages;
	//  contains messages due to rule firings
	//ostring ruleMessage;
	
public:
  Parameter(ostring _name, ostring _description);

  Parameter();

	virtual ~Parameter() {};

  void setNameAndDescription(ostring _name, ostring _description);
  // sets name and description

  ostring getName();
  // returns name

  virtual ostring setValue(ostring valueString) = 0;  // checks type and sets new value
  virtual ostring getValueString() = 0; // returns value as ostring
  virtual ostring getPreviousValueString() = 0;
  virtual void  previousValue() = 0; // resets value to previous value
  virtual void  showValue() = 0; // display diagnostic
//  virtual double  getValueAsDouble() = 0; // needed by rules
  virtual void writeOutputFile(std::ofstream &fout) =0; // writes self to file
  virtual ostring getDescription()
		{ return description; }
  //void setNumericalValue(int v);
//	virtual oopicList<ostring>* getErrorMessages()
//		{ return ruleMessages.isEmpty() ? NULL : &ruleMessages; }

protected:
  // predicates for testing input valueostrings of parameters
  int isDigit(ostring s);
  int isDigitOrDecimal(ostring s);
  int isSign(ostring s);
  int isInteger(ostring s);
  int isScalar(ostring s);
  int isScientific(ostring s);
};


//=================== IntParameter Class
// integer parameters

class IntParameter : public Parameter
{int value;
 int prevValue;

public:
 IntParameter(ostring _name, ostring _description)
	  : Parameter(_name, _description) {value=prevValue=0;};

 IntParameter() :Parameter() {value=prevValue=0;};

 ostring setValue(ostring valueString);
 // set value from ostring (returns ostring("") if valueString is valid integer
 // if not, returns message about type of error.

 void setValue(int _value) {value = _value;};

 int getValue();
 // returns value

 double getValueAsDouble();
 // returns value as double

 ostring getValueString();
 // returns value as ostring

 ostring getPreviousValueString();
 // returns previousValue as ostring

 void  previousValue();
 // updates previous value

 void  showValue();
 // diagnostic -- shows value

 void writeOutputFile(std::ofstream &fout);
 //  writes to file
// void setNumericalValue(int v);
};


//=================== ScalarParameter Class
// scalar parameters

class ScalarParameter : public Parameter
{Scalar   value;     // Parser has been fixed, so these
 Scalar   prevValue; // can now be Scalar. RT, 2003/12/09

public:
 ScalarParameter(ostring _name, ostring _description)
  : Parameter(_name, _description) {value=prevValue=0;};

 ScalarParameter() :Parameter() {value=prevValue=0;};

 virtual ~ScalarParameter() {};

 ostring setValue(ostring valueString);
 ostring setValue(Scalar _value) {
	 value=_value;
	return ostring("");
 }
 // set value from ostring (returns ostring("") if valueString is valid scalar
 // if not, returns message about type of error.

 Scalar getValue();
 // returns value

 double getValueAsDouble();
 // returns value as double

 ostring getValueString();
 // returns value as ostring

 ostring getPreviousValueString();
  // returns previousValue as ostring

 void previousValue();
 //  updates previousValue

 void showValue();
 // diagnostic -- shows value

 void writeOutputFile(std::ofstream &fout);
 //  writes to file
};


//=================== StringParameter Class
// String parameters

class StringParameter : public Parameter
{
	ostring value;
	ostring prevValue;

public:
	StringParameter(ostring _name, ostring _description)
	  : Parameter(_name, _description) {value=prevValue="";};

	StringParameter() :Parameter() {value=prevValue="";}

	ostring setValue(ostring valueString);
	// set value from ostring (returns ostring("") if valueString is valid integer
	// if not, returns message about type of error.

	ostring getValue();
	// returns value

	ostring getValueString();
	// returns value as ostring

	ostring getPreviousValueString();
	// returns previousValue as ostring

	void  previousValue();
	// updates previous value

	void  showValue();
	// diagnostic -- shows value

	void writeOutputFile(std::ofstream &fout);
	//  writes to file
// void setNumericalValue(int v);
};

#endif    //   ifndef __PARAMS_H
