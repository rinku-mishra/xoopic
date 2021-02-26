//part1g.cpp
#include "part1g.h"
#include "advisman.h"
using namespace std;

ostring Standardize(ostring in);
ostring parseFirst(ostring str);
ostring parseSecond(ostring str);
ostring parseThird(ostring str);
ostring parseFourth(ostring str);
ostring parseFifth(ostring str);


ostring stripComment(ostring &s)
{
  int pos = s.find("//", 0);
  if (pos >= 0) return s.substr(0, pos);
  else return s;
}
  
ostring stripWS(ostring s)
{ostring ns = s.strip(2);
 ns = stripComment(ns);
 ns.strip(3);
 return ns;}

ostring getname(ostring s, int p)
{ostring name = s.substr(0, p);
 //cout << "name = " << name << endl;
 return(stripWS(name));}

ostring getvalue(ostring s, int p)
{ostring value = s.substr(p+1, s.length());
 //cout << "value = " << value << endl;
 return(stripWS(value));}

int member(ostring s, oopicList<ostring>* l);

//=================== ParameterGroup Class

oopicList<BaseParameter>* ParameterGroup::getParameterList()
{return &parameterList;}

void ParameterGroup::setValues(oopicList<ostring> &ostringList)
{errorMessages = parseStringList(ostringList);
 if (errorMessages.length() > 0) legal = FALSE;
 else legal = TRUE;}

void ParameterGroup::addLimitRule(ostring _name, ostring _op,
	double _val, ostring _reason, int _intrinsic) {
// JRC: this fails to recognize that is a handoff, but when I
// recode, it crashes.  Valgrind says we have a memory leak.
  RuleList.add(new LimitRule( _name, _op, _val, _reason, _intrinsic));
  // LimitRule lr( _name, _op, _val, _reason, _intrinsic);
  // RuleList.add(&lr);
}

//  constructs rule from file stream
ostring ParameterGroup::addRule(ifstream &fin)
{char buffer[512];
 VarGroup::getLine(fin, buffer, 511); // skips the "{"
 VarGroup::getLine(fin,buffer, 511); // read the rule type
 ostring type = stripWS(ostring(buffer));
 VarGroup::getLine(fin,buffer, 511); // read the expression
 ostring expression = stripWS(ostring(buffer));
 VarGroup::getLine(fin,buffer, 511); // read the description
 ostring description = stripWS(ostring(buffer));
 if (type == (ostring)"Limit")
  {ostring name = parseFirst(expression);
	ostring op = parseSecond(expression);
	ostring value = parseThird(expression);
	addLimitRule(name, op, atof(value.c_str()), description, 0);}
 else if (type == (ostring)"Relation")
  {ostring name1 = parseFirst(expression);
	ostring op1 = parseSecond(expression);
	ostring name2 = parseThird(expression);
	addRelationRule(name1, op1, name2, description, 0);}
 else if (type == (ostring)"Algebra")
  {ostring name1 = parseFirst(expression);
	ostring op1 = parseSecond(expression);
	ostring name2 = parseThird(expression);
	ostring op2 = parseFourth(expression);
	ostring value = parseFifth(expression);
	addAlgebraRule(name1, op1, name2, op2, atof(value.c_str()),
						description, 0);}
 else return "addRule: unrecognized rule type"; // JohnV 06-21-95
 VarGroup::getLine(fin, buffer, 511); // skips the "}"
 return "";
}

void ParameterGroup::addRelationRule(ostring _name1, ostring _op, 
	ostring _name2, ostring _reason, int _intrinsic) {
// JRC: same as above memory leak
  RuleList.add(new RelationRule(_name1, _op, _name2, _reason, _intrinsic));
  // RelationRule rr(_name1, _op, _name2, _reason, _intrinsic);
  // RuleList.add(&rr);
}



void ParameterGroup::addAlgebraRule(ostring _name1, ostring _op1, 
	ostring _name2, ostring _op2, double _val, ostring _reason, 
	int _intrinsic) {
// JRC: same memory leak as above
  RuleList.add(new AlgebraRule(_name1, _op1, _name2, _op2, _val, _reason, 
	_intrinsic));
  // AlgebraRule ar(_name1, _op1, _name2, _op2, _val, _reason, _intrinsic);
  // RuleList.add(&ar);
}

void ParameterGroup::showRules()
{
 oopicListIter<RuleBase> nR(RuleList);
 for (nR.restart(); !nR.Done(); nR++)
  {nR.current()->showRule();}}

void ParameterGroup::checkRules()
{ruleMessages.removeAll();
 oopicListIter<RuleBase> nR(RuleList);
 oopicListIter<BaseParameter> nP(parameterList);
 // Loop thru rules
 for (nR.restart(); !nR.Done(); nR++)
 // Loop thru parameters and set parameter values required by rule
  {for (nP.restart(); !nP.Done(); nP++)
	 nR.current()->setRuleVariable(nP.current()->getName(),
	 nP.current()->getValueAsDouble());
 // Check the rule
	ostring result = nR.current()->checkRule();
	if (strlen(result.c_str()) > 0)
	 ruleMessages.add(new ostring(result));}}

void ParameterGroup::showRuleMessages()
{//cout << "rule messages" << endl;
 oopicListIter<ostring> nS(ruleMessages);
 for (nS.restart(); !nS.Done(); nS++)
  cout << nS.current()->c_str() << endl;}

void ParameterGroup::showValues()
{
 oopicListIter<BaseParameter> nP(parameterList);
 for (nP.restart(); !nP.Done(); nP++)
  {nP.current()->showValue();}}

void ParameterGroup::writeOutputFile(ofstream &fout)
{fout << name << endl;
 fout << "{" << endl;
 oopicListIter<BaseParameter> nP(parameterList);
 for (nP.restart(); !nP.Done(); nP++)
  nP.current()->writeOutputFile(fout);
 oopicListIter<RuleBase> nR(RuleList);
 for (nR.restart(); !nR.Done(); nR++)
  nR.current()->writeOutputFile(fout);
 fout << "}" << endl;}

void ParameterGroup::describe()
{cout << name << endl;
 showValues();}

ostring ParameterGroup::InitializeFromStream(ifstream &fin) {
  oopicList<ostring> alist;
  ParameterGroup *subgroup;

 ostring msg = ostring("");
 char buffer[512];
 ostring line, header, name, value;

 while(!fin.eof()) {
	VarGroup::getLine(fin,buffer, 511); // get a line
	line = stripWS(ostring(buffer));
	if (line == (ostring)"{") { // skip this line and read another                     
	  VarGroup::getLine(fin, buffer, 511);
	  line = stripWS(ostring(buffer));}
	//	cout << line << endl;
	if (line == (ostring)"") continue;
	if (line == (ostring)"}"){  // terminates the input for this group
	  setValues(alist);
	  break;
	}
	int pos = line.find("=", 0); // is this a parameter = value line?
	if (pos > 0) {
	  name = getname(line, pos);

	  if(LegalParamName(name)) {
		 value = getvalue(line, pos);
		 alist.add(new ostring(name + ostring(" ") + value));
	  }
	  else {
		 msg = line; break;
	  }
	}
	//  this clause will recurse through nested groups if any.
	else if((subgroup=LookupGroupByName(line))!=0) {
	  msg= subgroup->InitializeFromStream(fin);
	}
	else if (line.contains('R') == 1) {
	  ostring type = parseSecond(line);
	  msg = addRule(fin);
	}					//	JohnV 06-21-95
	else {
	  msg = line;
	  break;
	}
 }
 alist.deleteAll();
 return(msg);
}

ostring ParameterGroup::parseStringList(oopicList<ostring> &ostringList)
{
 oopicListIter<ostring> nS(ostringList);
 oopicListIter<BaseParameter> nP(parameterList);
 ostring msg;
 for (nS.restart(); !nS.Done(); nS++)
  {ostring*  str = nS.current();
	ostring nameString = parseName(*str);
	ostring valueString = parseValue(*str);
	for (nP.restart(); !nP.Done(); nP++)
	 {BaseParameter*   p = nP.current();
	  ostring pname = ostring(p->getName());
	  if (pname == nameString)
		{ostring pmsg = p->setValue(valueString);
	  if (pmsg.length() > 0) msg.append(pmsg);}}}
 return msg;}

ostring ParameterGroup::parseName(ostring str)
{return str.substr(0, str.find(" ", 0));}

ostring ParameterGroup::parseValue(ostring str)
{return str.substr(str.find(" ", 0) + 1);}

BaseParameter* ParameterGroup::getParameter(ostring name)
{
	oopicListIter<BaseParameter> nP(parameterList);
	for (nP.restart(); !nP.Done(); nP++)
	if (name == nP.current()->getName()) {
		return nP.current();
	}
	return NULL;
}

ostring ParameterGroup::getName()
{
	return GetName();
}

ParameterGroup * ParameterGroup::LookupGroupByName(ostring pname) {
  // search the parameterGroupList for this name 
  // return that group if it exists
  oopicListIter<ParameterGroup> plistI(parameterGroupList);
  for(plistI.restart();!plistI.Done();plistI++) 
	 if(plistI.current()->GetName() == pname) return plistI.current();
  return 0;
}
