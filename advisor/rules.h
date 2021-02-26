#ifndef   __RULES_H
#define   __RULES_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "ostring.h"
#include <fstream>
#include <iostream>

#ifdef _MSC_VER
#else

using namespace std;
#endif

#include <stdio.h>
#include "oopiclist.h"


//========================  Association
// Convenience for handling ostring-double pairs

class Association
{public:
  ostring name;
  double value;

 public:
  Association(ostring _name, double _value);
};

//========================  BinaryOperator
// Convenience for handling binary operators appearing in rules
// e.g., in lisp notation (+ arg1 arg2), (== arg1 arg2), etc.

class BinaryOperator
{private:
  ostring op;
  double value1, value2;

 public:
  BinaryOperator(ostring _op, double _value1, double _value2);

  void showBinaryOperator();
  // diagnostic -- shows object

  double evaluate();
  // evaluate the operator
};

//========================  RuleBase
// Abstract base class for rules

class RuleBase
{public:
  oopicList<Association> associationList;
  // name-value pairs of all variables involved in rule

  ostring op;
  ostring reason;
  int intrinsic; // distinguishes rules intrinsic to a parameter group
					  // from those externally added to the group

 public:
  RuleBase() {};

  void setRuleVariable(ostring aname, double avalue);
  // sets value of variable

  void showRuleVariables();
  // diagnostic -- show all variables and their values

  double getValue(ostring aname);
  // return value of a variable

  virtual void showRule()=0;
  // diagnostic -- show the rule

  virtual ostring checkRule()=0;
  // check the rule

  virtual void writeOutputFile(std::ofstream &fout)=0;
  //  writes object to file
	 virtual ~RuleBase() {};
};

//========================  LimitRule
// Handles  (variable op value) op = ==, >=, <=, >, <
// e.g.,   J <= 3

class LimitRule :public RuleBase
{protected:
	double limitingValue;
	ostring name;

 public:
	LimitRule() :RuleBase() {};

	LimitRule(ostring _name, ostring _op,
				 double _limitingValue, ostring _reason, int _intrinsic);

	~LimitRule() {};

	void showRule();
	// diagnostic -- show the rule

	ostring checkRule();
	// check the rule

	void writeOutputFile(std::ofstream &fout);
	// writes object to file
};

//========================  RelationRule
// Handles (variable1 op variable2) with op = ==, >=, <=, >, <
// e.g.,  x1s < x1f

class RelationRule :public RuleBase
{protected:
	ostring name1, name2;

 public:
	RelationRule() :RuleBase() {};

	RelationRule(ostring _name1, ostring _op,
					 ostring _name2, ostring _reason, int _intrinsic);

	~RelationRule() {};

	void showRule();
	// diagnostic -- show the rule

	ostring checkRule();
	// check the rule

    void writeOutputFile(std::ofstream &fout);
	// writes object to file
};

//========================  AlgebraRule
// Handles ((variable1 op1 variable2) op2 value)
// with op1 +, -, *, /
// with op2 = ==, >=, <=, >, <
// e.g.   J*K < 10000

class AlgebraRule :public RuleBase
{protected:
	ostring name1, name2, opalg;
	double limitingValue;

 public:
	AlgebraRule() :RuleBase() {};

	AlgebraRule(ostring _name1, ostring _opalg, ostring _name2,
					ostring _op, double _limitingValue, ostring _reason,
					int _intrinsic);

	~AlgebraRule() {};

	void showRule();
		// diagnostic -- show the rule

	ostring checkRule();
	// check the rule

	void writeOutputFile(std::ofstream &fout);
	//  writes object to file
};

#endif    //   ifndef __RULES_H
