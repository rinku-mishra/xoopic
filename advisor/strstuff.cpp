//strstuff.cpp

#include "ostring.h"
#include "oopiclist.h"

int member(ostring s, oopicList<ostring>* l)
{int ans = 0;
 oopicListIter<ostring> sI(*l);
 for (sI.restart(); !sI.Done(); sI++)
  {if (*sI.current() == s)
	 {ans = 1; break;}}
 return ans;
}


ostring Standardize(ostring in)
{ostring out;
 int kount = 0;
 char c;
 for (int i = 0; i < in.length(); i++)
  {c = in[i];
	if (c != ' ')
	 {out.append(c);
	  kount = 0;}
	else
	 {if (kount == 0)
		{out.append(c);
		 kount++;}}}
 return out.strip(0);}

ostring parseFirst(ostring str)
{return str.substr(0, str.find(" ", 0));}

ostring parseSecond(ostring str)
{int pos2 = str.find(" ", 0);
 int pos3 = str.find(" ", pos2 + 1);
 return str.substr(pos2 + 1, pos3 - pos2);}

ostring parseThird(ostring str)
{int pos2 = str.find(" ", 0);
 int pos3 = str.find(" ", pos2 + 1);
 int pos4 = str.find(" ", pos3 + 1);
 return str.substr(pos3 + 1, pos4 - pos3);}

ostring parseFourth(ostring str)
{int pos2 = str.find(" ", 0);
 int pos3 = str.find(" ", pos2 + 1);
 int pos4 = str.find(" ", pos3 + 1);
 int pos5 = str.find(" ", pos4 + 1);
 return str.substr(pos4 + 1, pos5 - pos4);}

ostring parseFifth(ostring str)
{int pos2 = str.find(" ", 0);
 int pos3 = str.find(" ", pos2 + 1);
 int pos4 = str.find(" ", pos3 + 1);
 int pos5 = str.find(" ", pos4 + 1);
 int pos6 = str.find(" ", pos5 + 1);
 return str.substr(pos5 + 1, pos6 - pos5);}
