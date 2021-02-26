//rules.cpp

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iomanip>

#include "rules.h"
using namespace std;


extern std::ostream& fformat(ostream& os);
//  needed for formatting output

//========================  Association
// Convenience for handling ostring-double pairs

Association::Association(ostring _name, double _value)
{name = _name; value = _value;}

//========================  BinaryOperator
// Convenience for handling binary operators
// e.g., in lisp notation (+ arg1 arg2), (== arg1 arg2), etc.

BinaryOperator::BinaryOperator(ostring _op, double _value1, double _value2)
{op = _op; value1 = _value1; value2 = _value2;}

void BinaryOperator::showBinaryOperator()
{std::cout << value1 << " " << op << " " << value2 << endl;}

double BinaryOperator::evaluate() {
  double result=0.;
  op.strip(1);  //strip any trailing whitespace.
  if (op == ostring("==")) result = (value1 == value2);
  if (op == ostring("<=")) result = (value1 <= value2);
  if (op == ostring(">=")) result = (value1 >= value2);
  if (op == ostring("<")) result = (value1 < value2);
  if (op == ostring(">")) result = (value1 > value2);
  if (op == ostring("+")) result = (value1 + value2);
  if (op == ostring("-")) result = (value1 - value2);
  if (op == ostring("*")) result = (value1 * value2);
  if (op == ostring("/")) result = (value1/value2);
  return result;
}

//========================  RuleBase
// Abstract base class for rules

void RuleBase::setRuleVariable(ostring aname, double avalue)
{
  oopicListIter<Association> nA(associationList);
 for(nA.restart(); !nA.Done(); nA++)
  if (nA.current()->name == aname)
	nA.current()->value = avalue;}

void RuleBase::showRuleVariables()
{
 oopicListIter<Association> nA(associationList);
 for(nA.restart(); !nA.Done(); nA++)
  std::cout << nA.current()->name << " = " << nA.current()->value << endl;}

double RuleBase::getValue(ostring aname)
{
 oopicListIter<Association> nA(associationList);
 double val = 0.0;
 for(nA.restart(); !nA.Done(); nA++)
  if (nA.current()->name == aname)
	val = (nA.current()->value);
 return(val);}

//========================  LimitRule
// Handles  (variable op value) with op = ==, >=, <=, >, <

LimitRule::LimitRule(ostring _name, ostring _op,
							double _limitingValue, ostring _reason, int _intrinsic)
	 :RuleBase()
{name = _name; op = _op; limitingValue = _limitingValue; reason = _reason;
 intrinsic = _intrinsic;
 associationList.add(new Association(_name, 0.0));}

void LimitRule::showRule()
{std::cout << endl;
 std::cout << "RULE LIMIT" << endl
		<< "PARAMETER CONSTRAINT " << name
		<< " " << op << " " << limitingValue << endl;
 std::cout << "REASON " << reason << endl;}

ostring LimitRule::checkRule()
{double pvalue = getValue(name);
 BinaryOperator e(op, pvalue, limitingValue);
 if (e.evaluate() == 1.0) // return error message
  {char buffer[200];
	sprintf(buffer, "Attention!  %s = %5.3f \n%s\n",
	name.c_str(), pvalue, reason.c_str());
	return(ostring(buffer));}
 else
	return(ostring(""));}

void LimitRule::writeOutputFile(std::ofstream &fout)
{if(!intrinsic) // don't output the intrinsic rules
  {fout << "\tRule" << endl;
	fout << "\t{" << endl;
	fout << "\t\t" << name << " " << op << fformat << limitingValue << endl;
	fout << "\t\t" << reason << endl;
	fout << "\t}" << endl;}}

//========================  RelationRule
// Handles (variable1 op variable2) with op = ==, >=, <=, >, <

RelationRule::RelationRule(ostring _name1, ostring _op,
									ostring _name2, ostring _reason, int _intrinsic)
	 :RuleBase()
{name1 = _name1; op = _op; name2 = _name2;
 reason = _reason; intrinsic = _intrinsic;
 associationList.add(new Association(_name1, 0.0));
 associationList.add(new Association(_name2, 0.0));}

void RelationRule::showRule()
{std::cout << endl;
 std::cout << "RULE RELATION" << endl
		<< "PARAMETER CONSTRAINT " << name1
		<< " " << op << " " << name2 << endl;
 std::cout << "REASON " << reason << endl;}

ostring RelationRule::checkRule()
{double pvalue1 = getValue(name1), pvalue2 = getValue(name2);
 BinaryOperator e(op, pvalue1, pvalue2);
 if (e.evaluate() == 1.0) // return error message
 {char buffer[200];
  sprintf(buffer, "Attention!  %s = %5.3f %s = %5.3f \n%s\n",
  name1.c_str(), pvalue1,
  name2.c_str(), pvalue2, reason.c_str());
  return(ostring(buffer));}
	else
  return(ostring(""));}

void RelationRule::writeOutputFile(std::ofstream &fout)
{if (!intrinsic) // don't output the intrinsic rules
  {fout << "\tRule" << endl;
	fout << "\t{" << endl;
	fout << "\t " << name1 << " " << op << name2 << endl;
	fout << "\t " << reason << endl;
	fout << "\t}" << endl;}}

//========================  AlgebraRule

AlgebraRule::AlgebraRule(ostring _name1, ostring _opalg, ostring _name2,
					ostring _op, double _limitingValue, ostring _reason,
					int _intrinsic)
	 :RuleBase()
{name1 = _name1; opalg = _opalg; name2 = _name2;
 op = _op; limitingValue = _limitingValue;
 reason = _reason; intrinsic = _intrinsic;
 associationList.add(new Association(_name1, 0.0));
 associationList.add(new Association(_name2, 0.0));}

void AlgebraRule::showRule()
{std::cout << endl;
 std::cout << "RULE ALGEBRA" << endl
		<< "PARAMETER CONSTRAINT " << name1 << " " << opalg << " " << name2
								<< " " << op << " " << limitingValue << endl;
	  std::cout << "REASON " << reason << endl;}

ostring AlgebraRule::checkRule()
{double pvalue1 = getValue(name1), pvalue2 = getValue(name2);
 BinaryOperator e1(opalg, pvalue1, pvalue2);
 BinaryOperator e2(op, e1.evaluate(), limitingValue);
 if (e2.evaluate() == 1.0) // return message
  {char buffer[200];
	sprintf(buffer, "Attention!  %s = %5.3f %s = %5.3f \n%s\n",
	name1.c_str(), pvalue1,
	name2.c_str(), pvalue2, reason.c_str());
	return(ostring(buffer));}
  else
	return(ostring(""));}

void AlgebraRule::writeOutputFile(std::ofstream &fout)
{if(!intrinsic) // don't output the intrinsic rules
  {fout << "\tRule" << endl;
	fout << "\t{" << endl;
	fout << "\t " << name1 << " " << opalg << " " << name2 << " " << op
		  << " " << fformat << limitingValue << endl;
	fout << "\t " << reason << endl;
	fout << "\t}" << endl;}}



