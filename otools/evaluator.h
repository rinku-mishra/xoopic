#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "precision.h"
/* Data type for links in the chain of symbols.      */
struct symrec
{
  char *name;  /* name of symbol                     */
  int type;    /* type of symbol: either VAR or FNCT */
  union {
    Scalar var;           /* value of a VAR          */
    Scalar (*fnctptr)();  /* value of a FNCT         */
    Scalar *ivar;         /* pointer to an external var */
  } value;
  struct symrec *next;    /* link field              */
};

typedef struct symrec symrec;
     

#ifdef __cplusplus
extern "C" {     
#endif
  symrec *putsym (symrec *sym_table,const char *varname,int sym_type);
  symrec *getsym (symrec *sym_table,const char *varname);
  extern int yyparse();
  extern void yyerror(char *);
  extern int yylex();
  void delete_table(symrec *sym_table);
  symrec *init_table(symrec *sym_table);
  symrec *copy_table(symrec *sym_table);
  extern char *EVAL_STRING;
  extern Scalar EVAL_RESULT;
  extern symrec *SYMBOL_TABLE;
  extern int Sindex;
  Scalar ramp(double);
  Scalar step(double);
  Scalar pulse(double,double,double,double,double);
  Scalar SINW(double);
  Scalar COSW(double);
  Scalar ATANW(double);
  Scalar TANHW(double);
  Scalar LOGW(double);
  Scalar EXPW(double);
  Scalar SQRTW(double);
  Scalar FABSW(double);
  Scalar POWW(double,double);
  Scalar gaussian(double,double,double);
#ifdef __cplusplus
}
#endif


#endif
