//param.cpp

/*
(9-10-94 Hadon) fixed scalarparams::getValueString
(10-7-94 Hadon) fixed scalarparams::getValueString (again)
(10-28-94 Hadon) fixed scalarparams::getValueString (again)
(11-11-94 Hadon) added Parameter::getErrorMessages (again)
(11-11-94 Hadon) added ruleMessages, ruleMessage (again) 
(05-15-95 JohnV, NTG) commented out ruleMessages (unused!)
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

//#if !defined(_MSC_VER)
#include <fstream>
#include <iomanip>
#define itoa(x,y,z) (sprintf((y),"%d",(x)))

//#endif

#include "ptclgrp.h"
#include "sptlrgn.h"

#include "param.h"
#include "part1g.h"

//Evaluator *BaseParameter::adv_eval=new Evaluator();
extern Evaluator *adv_eval;

extern "C" {
Scalar xoopic_nint(Scalar x);  //from the math library
};

// set the state of the output stream to 7 wide, 3 digits of precision.
using namespace std;

ostream& fformat(ostream& os)
{return os /*<< setiosflags(ios::showpoint | ios::fixed)
		<< setprecision(3) << setw(7) << setiosflags(ios::right)*/;}

//=================== Parameter Class

Parameter::Parameter(ostring _name, ostring _description)
	: BaseParameter()
{name = _name; description = _description;}

Parameter::Parameter()
	: BaseParameter()
{name = description = ostring("");}

void Parameter::setNameAndDescription(ostring _name, ostring _description)
{name = _name; description = _description;}

ostring Parameter::getName() {return(name);}

int Parameter::isDigit(ostring s)
{int test = ostring("0123456789").find(s);
 return (test >= 0) ? 1:0;}

int Parameter::isDigitOrDecimal(ostring s)
{int test = ostring("0123456789.").find(s);
 return (test >= 0) ? 1:0;}

int Parameter::isSign(ostring s)
{int test = ostring("+-").find(s);
 return (test >= 0) ? 1:0;}

int Parameter::isInteger(ostring s)
{int len = s.length(), isdig = isDigit(s[0]);
 // single digit
 if ((len == 1) && (isdig == 1)) return 1;
 // first character is not a digit or a sign
 int issig = isSign(s[0]);
 if ((issig == 0) && (isdig == 0)) return 0;
 // first character is sign, are all other characters digits?
 for (int i = 1; i < s.length(); i++)
  {isdig = isDigit(s[i]);
	if (isdig == 0) return 0;}
 return 1;}

int Parameter::isScalar(ostring s)
{// a valid integer is a valid scalar
 int test = isInteger(s);
 if (test == 1) return 1;
 // a valid scalar has only one decimal point
 ostring dec(".");
 int posdec = s.find(dec);
 int twodec;
 if (posdec >= 0)
  {twodec = s.find(dec, posdec+1);
	if (twodec >= 0) return 0;}
 // first character is not a digit or decimal or a sign
 int issig = isSign(s[0]), isdigordec = isDigitOrDecimal(s[0]);
 if ((issig == 0) && (isdigordec == 0)) return 0;
 // first character is sign, are all other characters digits or a decimal?
 for (int i = 1; i < s.length(); i++)
  {isdigordec = isDigitOrDecimal(s[i]);
	if (isdigordec == 0) return 0;}
 return 1;}

int Parameter::isScientific(ostring s)
{if ((s.contains('e') || s.contains('E')) == 0) return 0;
 int pose = s.find('e');
 if (pose == -1) pose = s.find('E');
 ostring mantissa = s.substr(0, pose);
 ostring exponent = s.substr(pose+1);
 if (isScalar(mantissa) && isInteger(exponent))
  return 1;
 else
  return 0;}

//=================== IntParameter Class

int IntParameter::getValue()
{return value;}

double IntParameter::getValueAsDouble()
{return (double)value;}

ostring IntParameter::getValueString()
{char str[20];
 itoa(value, str, 10);
 return ostring(str);}

ostring IntParameter::getPreviousValueString()
{char str[20];
 itoa(prevValue, str, 10);
 return ostring(str);}

void IntParameter::previousValue()
{value = prevValue;}

void IntParameter::showValue()
{cout << getName() << " " << value << endl;}

ostring IntParameter::setValue(ostring v)
{char buffer[100];
 //if (isInteger(v) == 1)
  {prevValue = value;
	v+='\n';  //needed for evaluator
	         // round any non-floating results to nearest int
	value = (int)xoopic_nint (adv_eval->Evaluate(v.c_str()));
	sprintf(buffer, "%s","");}
 //else
//  {sprintf(buffer, "%s: %s is not a valid integer \n",
//			  name.c_str(), v.c_str());
// return
 

// These don't seem to be used:
//	ruleMessages.removeAll();
//	ruleMessage = ostring(buffer);
//	if (ruleMessage != "") ruleMessages.add(&ruleMessage);

return ostring(buffer);}

void IntParameter::writeOutputFile(ofstream &fout)
{fout << "\t" << name << " = " << value << endl;}

/*
void IntParameter::setNumericalValue(int v)
{
	value = v;
}
*/

//=================== ScalarParameter Class

Scalar ScalarParameter::getValue()
{return value;}

double ScalarParameter::getValueAsDouble()
{return (double)value;}

ostring ScalarParameter::getValueString()
{char str[100];
 sprintf(str, "%g", value);
 return(ostring(str));}

ostring ScalarParameter::getPreviousValueString()
{char str[100];
 sprintf(str, "%g", prevValue);
 return(ostring(str));}

void ScalarParameter::previousValue()
{value = prevValue;}

void ScalarParameter::showValue()
{cout << name << " " << fformat << value << endl;}

ostring ScalarParameter::setValue(ostring v)
{char buffer[100];
// if ((isScalar(v) || isScientific(v)))
  {prevValue = value;
	v+='\n'; //needed for evaluator
	value = adv_eval->Evaluate(v.c_str());
	sprintf(buffer, "%s","");}
 //else
//  {sprintf(buffer, "%s: %s is not a valid Scalar \n",
//	name.c_str(), v.c_str());}
 return ostring(buffer);}

void ScalarParameter::writeOutputFile(ofstream &fout)
{fout << "\t" << name << " = " << fformat << value << endl;}

//=================== StringParameter Class

ostring StringParameter::getValue()
{
	return value;
}

ostring StringParameter::getValueString()
{
	return getValue();
}

ostring StringParameter::getPreviousValueString()
{
	return prevValue;
}

void StringParameter::previousValue()
{
	value = prevValue;
}

void StringParameter::showValue()
{
	cout << getName() << " " << value << endl;
}

ostring StringParameter::setValue(ostring v)
{
	value = v; // no testing on string is allowed
	return "";
}

void StringParameter::writeOutputFile(ofstream &fout)
{
	fout << "\t" << name << " = " << value << endl;
}

