//part1.h

#ifndef   __PART1G_H
#define   __PART1G_H

#include "param.h"
class Evaluator;
//=================== ParameterGroup Class
// Abstract base class for conceptual groups of parameters

class ParameterGroup :public BaseParameter
{protected:

	  oopicList<BaseParameter> parameterList;
	  oopicList<RuleBase> RuleList;
	  oopicList<ParameterGroup> parameterGroupList;
	  //  list of rules constraining parameters in the group

public:
	  ostring name; // name of group
	  ostring errorMessages;
	  // contains errors in input (not an integer or not a scalar)
	  BOOL legal;
	  oopicList<ostring> ruleMessages;

	  int LegalParamName(ostring pname) {
		 // search the parameterList for this name 
		int ans=0;
		oopicListIter<BaseParameter> plistI(parameterList);
		for(plistI.restart();!plistI.Done();plistI++) 
		  ans|=plistI.current()->getName() == pname;
		return ans;
	  }

	  ParameterGroup * LookupGroupByName(ostring pname); 
			 
		 
	  //  contains messages due to rule firings

public:
	  ParameterGroup() : BaseParameter() {};

	  virtual ~ParameterGroup() {};

	  virtual void setValues(oopicList<ostring> &ostringList);
	  // Set the values of the parameters in the group from a list
	  // of ostrings of the form   ostring("name value")

	  oopicList<BaseParameter>* getParameterList();
	  // Return list of parameters in group

	BaseParameter* getParameter(ostring name);
	  // Return a sub-parameter by name

	  ostring GetName() {return name;};
	  void setName(ostring _name) {name = _name;};

	  void addLimitRule(ostring _name, ostring _op,
							  double _val, ostring _reason, int _intrinsic);

	  ostring addRule(std::ifstream &fin);
	  // Add a Rule to known rules via a stream

	  void addRelationRule(ostring _name1, ostring _op, ostring _name2,
								  ostring _reason, int _intrinsic);

	  void addAlgebraRule(ostring _name1, ostring _op1, ostring _name2,
		 ostring _op2, double _val, ostring _reason, int _intrinsic);

	  virtual void checkRules();
	  //  Fire all applicable rules and accumulate results into Messages.

	  void showRules();
	  //  Diagnostic -- show all rules known by group

	  void showRuleMessages();
	  //  Diagnostic -- show results of all rule violations

	  void showValues();
	  //  Diagnostic  -- show names and values of all parameters in group

	  void describe();

	  virtual void writeOutputFile(std::ofstream &fout);
	  //  writes to file

	  virtual ostring InitializeFromStream(std::ifstream &fin);
	  //  initializes parametergroup from stream

	ostring getName();

	virtual oopicList<ostring>* getErrorMessages()
		{ return ruleMessages.isEmpty() ?(oopicList<ostring>*) NULL : &ruleMessages; }

	virtual ostring getDescription()
		{ return getName(); };


private:

	  ostring parseStringList(oopicList<ostring> &ostringList);
	  // Support function for setValues

	  ostring parseName(ostring str);
	  // Support function for parseStringList

	  ostring parseValue(ostring str);
	  // Support function for parseStringList
};



#endif  //  __PART1G_H
