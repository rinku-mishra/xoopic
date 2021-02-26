/*  Our Own Local String Library, by PeterM */

/* conversion from ANSI string to ostring added by R. Busby 5/02 */
/* str() method, conversion to ANSI string added by R. Busby 5/02 */

#ifndef _OSTRING_H
#define _OSTRING_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif



extern "C" {
#include <string.h>
}
#include <string>
#include <sstream>

using namespace std;

class ostring
{
protected:
#if defined(__BCPLUSPLUS__)
	int len;
	int size;
#else
	unsigned short len;
	unsigned short size;
#endif
	char *d;

	inline  ostring *scopy(const ostring *source);
	inline  ostring *scopy(const char *source);
	inline  ostring *scopy(const string *source);

	inline  ostring *srealloc(int newsize);

public:

	//constructors and destructor
	ostring();
	~ostring() {len=size=0; delete[] d;};

	ostring(const ostring& copy);
	ostring(const char *copy);

	ostring(char c);

	//conversion
	operator char *() { return d;};

	// assignment
	ostring& operator = (const ostring& two);
	ostring& operator = (const char *two);
	ostring& operator = (char c);
	ostring& operator = (const string& two);

	// comparison
	friend  int operator==(const ostring& a,const ostring& b);
	friend  int operator!= (const ostring& a,const ostring& b);

	//  concatenation
	ostring& operator += (const ostring& two);
	ostring& operator += (const char *two);
	ostring& operator += (char c);
	friend  ostring operator + (const ostring& a,const ostring& b);

	void append(const ostring &add);
	void append(const char add);
	void append(const char *add);

	// output
	char *c_str() const {return d;}

	char * operator ()() {return d;}
	string str() const {
		//    strstream sstrm;
		std::stringstream sstrm;

		sstrm << d << std::ends;
		string s = sstrm.str();
		return s;
	}


	// length
	int length() { return len; } ;

	// searching
	inline int find(const char *lookfor,int startpos=0) const;
	inline int find(char lookfor,int startpos=0) const;
	inline int find(const ostring& str, int startpos=0) const;

	int contains(char c) { return (find(c)!=-1)? 1:0;};
	int contains(char *c) { return (strstr(d,c))? 1:0;};
	int contains(ostring two) { return (strstr(d,two.d))? 1:0;};

	// getting part, from startpos to endpos, and returning the ostring.

	inline ostring substr(int startpos, int length);
	inline ostring substr(int startpos);

	//  bracket operator

	char& operator [](int index);

	//  strip(int) removes the nuke_me character from the beginning or end
	//  depending on the flag flag=0 = beginning.  flag=1 not tested.
	ostring & strip(int flag=0,char nuke_me=' ');

	// I/O
#if defined(UNIX) && !defined(__xlC__)
	friend ostream&   operator<<(ostream& s, const ostring& out);
#endif
};

inline ostring * ostring::scopy(const char *source){
	if (d) delete[] d;
	d = new char[strlen(source)+1];
	if(source) strcpy(d,source);
	else d[0]='\0';
	len=size=strlen(source);
	return this;
}

inline ostring * ostring::scopy(const ostring *source){
	if(this==source) return this;
	if (d) delete[] d;
	d = new char[(source->size)+1];
	if(source->d) strcpy(d,source->d);
	else d[0]='\0';
	size=source->size;
	len=source->len;
	return this;
}

inline ostring * ostring::scopy(const string *source){


	if (d) delete[] d;
	d = new char[(source->size() )+1];
	strcpy(d,source->c_str());
	size=source->size();
	len=source->length();
	return this;
}

inline ostring * ostring::srealloc(int newsize)
{
	char *tmp= new char[newsize+1];
	if(d)
	{
		strcpy(tmp, d);
		delete[] d;
	}	
	else tmp[0]='\0';
	d = tmp;
	size=newsize;
	len = strlen(d);
	return this;
}

inline ostring::ostring() { len = 0; size = 0; d = 0; }
inline ostring::ostring(const ostring &copy) { len = 0; size = 0; d = 0; scopy(&copy); }
inline ostring::ostring(const char *copy) {len=0; size=0; d=0; scopy(copy);}
inline ostring::ostring(char c) { len=1;size=1;d=new char[2];d[0]=c;d[1]='\0';}

inline ostring & ostring::operator =(const ostring& two)
{
	return *(scopy(&two));
}

inline ostring & ostring::operator =(const string& two)
{
	return *(scopy(&two));
}

inline ostring & ostring::operator =(const char * two)
{
	return *(scopy(two));
}

inline ostring & ostring::operator =( char c)
{ len=1; size=2; if(d) delete[] d; d = new char[2];
d[0]=c;d[1]='\0';
return *this;
}

inline void ostring::append(const ostring& add)
{ register int newlen;
if(size < (newlen = len + add.len)) srealloc(newlen);
len = newlen;
strcat(d,add.d);
}

inline void ostring::append(const char * add)
{ register int newlen;
if(size < (newlen=len + strlen(add))) srealloc(newlen);
len=newlen;
strcat(d,add);
}

inline void ostring::append(const char add)
{
	if(size < (len + 1)) srealloc(size+1);
	d[len]=add;
	d[len+1]='\0';
	len+=1;
}

inline ostring& ostring::operator +=  (const ostring& two) {
	append(two);
	return *this;
}

inline ostring& ostring::operator +=  (const char * two) {
	append(two);
	return *this;
}
inline ostring& ostring::operator +=  (char c) {
	append(c);
	return *this;
}

//--------------------------------------------

inline    ostring operator + (const ostring&a, const ostring& b)
{
	ostring r;
	r.d = new char[(r.len=a.len + b.len) +1];
	strcpy(r.d,a.d);
	strcat(r.d,b.d);
	r.size=r.len;
	return r;
}


inline int operator==(const ostring& a, const ostring& b) 
		{
	return (strcmp(a.d,b.d)==0)? 1:0;
		}
inline int operator!=(const ostring& a, const ostring& b) 
		{
	return (strcmp(a.d,b.d)!=0)? 1:0;
		}

/*  true substring search instead of search for an assemblage of characters */

inline int ostring::find(const char *lookfor, int startpos) const
{ register int i=startpos;
register int j;
register int jmax=strlen(lookfor);
register const char *c,*b,*e;
if(len==0) return -1;
for(c=&d[startpos];*c;c++,i++) {
	/* check for the entire lookfor string in d */
	for(b=lookfor,j=0,e=c;j<jmax&&((i+j)<len);j++,b++,e++)
		if(*b!=*e) break;
	if(j==jmax) return i;
}
return -1;
}

inline int ostring::find(char lookfor, int startpos) const
{ register char *c;
register int i;
for(c=&d[startpos],i=0;*c;c++,i++)
	if(lookfor==*c) return i;
return -1;
}

inline int ostring::find(const ostring &lookfor, int startpos) const
{ 
	return find(lookfor.d,startpos);
}

inline char & ostring::operator [] (int i)
{
	return d[i];
}

inline ostring& ostring::strip(int flag,char nuke_me)
{ int i;
char *c, *e;
if(size==0||len==0) return *this;
switch (flag) {
case 0:  //strip leading
	for(c=d,i=0;*c==nuke_me&&i<len;i++,c++);
	if(i==0) break;
	e = new char[len - i + 1];
	strcpy(e,c);
	delete [] d;
	d = e;
	size = len-i;
	len=strlen(d);
	break;
case 1: //strip following
	for(c=d+len-1,i=len;*c==nuke_me&&i>0;i--,*c='\0',c--);
	break;
case 2:  //strip leading white space (tabs and spaces)
	for(c=d,i=0;(*c==' '||*c=='\t')&&i<len;i++,c++);
	if(i==0) break;
	e = new char[len - i + 1];
	strcpy(e,c);
	delete [] d;
	d = e;
	size = len-i;
	len=strlen(d);
	break;
case 3: //strip following white space (tabs and spaces)
	for(c=d+len-1,i=len;(*c==' '||*c=='\t')&&i>0;i--,*c='\0',c--);
	len = strlen(d);
	break;
case 4: //strip leading "nuke me"
	for(c=d,i=0;*c==nuke_me&&i<len;i++,c++);
	if(i==0) break;
	e = new char[len - i + 1];
	strcpy(e,c);
	delete [] d;
	d = e;
	size = len-i;
	len=strlen(d);
	break;
case 5:
	for(c=d+len-1,i=len;*c==nuke_me&&i>0;i--,*c='\0',c--);
	len = strlen(d);
	break;
}
return *this;
}
#if defined(UNIX) && !defined(__xlC__)
inline  ostream& operator<<(ostream& s, const ostring& out)
{
	s << out.c_str();
	return s;
}
#endif

inline ostring ostring::substr(int startpos, int length) {
	ostring r;
	register int i,j;
#ifdef UNIX
	if(len<1) return r;
#else
	if (length < 1) return ostring("");
#endif

	//  if(length<1) {return substr(startpos);}

	r.d = new char[(r.size=length)+1];
	for(i=startpos,j=0;j<length&&i<len;i++,j++) r.d[j]=d[i];
	r.d[j]='\0';
	r.len=strlen(r.d);
	return r;
}

inline ostring ostring::substr(int startpos)
{
	return substr(startpos, len);
	/*  ostring r;
  register int i,j;
  if(len<1) return r;
  r.d=new char[(r.size=len-startpos)+1];
  r->len=len-startpos;
  for(i=startpos,j=0;i<len;i++,j++) r->d[j]=d[i];
  r->d[j]='\0';
  return *r;
	 */
}


#endif


