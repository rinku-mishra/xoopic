//varg.h

#ifndef __VARG_H
#define __VARG_H

#include "part1g.h"
#include "param.h"

extern "C++" void printf(char *);

ostring stripWS(ostring s);
#include "eval.h"
extern Evaluator *adv_eval;

class VarGroup : public ParameterGroup {
 public:
    static void getLine(ifstream& input, char* buffer, int buf_size){
    char c;
      int i = 0;
      while (1){
        if (i >= (buf_size - 1) ){
          buffer[i] = '\0';
          return;
        }
        if (!input.eof()){
        input.get(c);
       
        }
        else {
          if (i == 0)
            i++;
          buffer[i-1] = '\0';
          return;
        }
        
        if (c != '\r' && c!='\n'){
          buffer[i] = c;
        }
        else if (c == '\r'){
          buffer[i] = '\0'; // terminate this.
          if (input.peek() == '\n')
            input.ignore();  
          return;
        }
        else if (c== '\n'){  // \n
          buffer[i] = '\0'; // terminate this
          return;
        }
       
        i++;
      }
   }
  virtual ostring InitializeFromStream(std::ifstream &fin) {
	  char buffer[512];
	  ostring line;
	  while(!fin.eof()) {
		  VarGroup::getLine(fin, buffer, 511); // get a line
		  line = stripWS(ostring(buffer));
			  if (line == (ostring)"{") { //skip line, get another 
					VarGroup::getLine(fin,buffer, 511);
					line = stripWS(ostring(buffer));
					}
		  // cout << line << endl;
                  printf(line);
		  if (line == (ostring)"}")  // terminates the input for this group
			  return ostring("");
		  line+=(ostring)'\n';
		  adv_eval->Evaluate(line.c_str());
	  }
		  return(ostring(""));

  }
  VarGroup() {
	name = "Variables";
  }
};
#endif
