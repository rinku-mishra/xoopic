/*  This class encapsulates an evaluator.  */

#ifndef EVAL_H
#define EVAL_H

#include "evaluator.h"
#include "evaluator.tab.h"
#include <string.h>
class Evaluator { 
 private:
	symrec *symbol_table;  // our local symbol table
	
  public:
	void add_variable(const char *symbol,Scalar val) {
		symrec *ptr = getsym(symbol_table,symbol);
		if(ptr!=(symrec *)0) {  //symbol already exists, override it.
			ptr->value.var = val;
			ptr->type = VAR;
		}
		else {
			symbol_table = putsym(symbol_table,symbol,VAR);
			symbol_table->value.var = val;
		}
	};
	void add_indirect_variable(const char *symbol,Scalar *val) {
		symrec *ptr = getsym(symbol_table,symbol);
		if(ptr!=(symrec *)0) {  //symbol already exists, override it.
			ptr->value.ivar = val;
			ptr->type = IVAR;
		}
		else {
			symbol_table = putsym(symbol_table,symbol,IVAR);
			symbol_table->value.ivar = val;
		}
	};
	Evaluator() {
		symbol_table=init_table(0); 
	};

	// A copy constructor, copies the old symbol table to our new one.
	Evaluator(Evaluator *old_evaluator)  {
	  symbol_table = copy_table(old_evaluator->symbol_table);
	};

	~Evaluator() {
		//we need to delete the symbol table.
		delete_table(symbol_table);
		symbol_table = 0;
	};

	Scalar Evaluate(char *expression) {
		EVAL_STRING=expression;
		SYMBOL_TABLE=symbol_table;
		Sindex = 0;
		yyparse();
		symbol_table=SYMBOL_TABLE;
		return EVAL_RESULT;
	}

	// replaces all known variables in expression with constants
	char *preprocess_string(const char *expression) {
	  char *tempStr = new char[2*strlen(expression)];
	  for (symrec* symPtr = symbol_table; symPtr != NULL; symPtr = symPtr->next) {
		 if (symPtr->type == FNCT) continue; // do not replace functions;
		 
		 //parse expression for symbol, replace where found;
	  }
	 return 0; 
	}
};
			
#endif				
	
