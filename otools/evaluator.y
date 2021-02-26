/*  This is the evaluator */

     %{
#if defined(__BORLANDC__) || defined(_MSC_VER)
#define CINTERFACE
#endif

#ifdef _MSC_VER
#include <Malloc.h>
#include <string.h>
#include <stdlib.h>
#else
#include <stdlib.h>
#endif

#include <stdio.h>
#include <ctype.h>
#include <math.h>  /* For math functions, cos(), sin(), etc. */
#include "evaluator.h"  /* Contains definition of `symrec'        */
     %}
     %union {
     Scalar     val;  /* For returning numbers.                   */
     symrec  *tptr;   /* For returning symbol-table pointers      */
     Scalar *indirect_val;  /* For using external data */
     }
     
     %token <val>  NUM        /* Simple Scalar precision number   */
     %token <tptr> VAR FNCT IVAR   /* Variable and Function            */
     %type  <val>  exp
     
     %right '='
     %left '-' '+'
     %left '*' '/' '%'
     %left NEG     /* Negation--unary minus */
     %right '^'    /* Exponentiation        */
     
     /* Grammar follows */
     
     %%
     input:   /* empty */
             | input line
     ;
     
     line:
               '\n'
             | exp '\n'   { EVAL_RESULT=$1; }
             | error '\n' { yyerrok;                  }
     ;
     
     exp:      NUM                { $$ = $1;                         }
             | VAR                { $$ = $1->value.var;              }
	     | IVAR		  { $$ = *($1->value.ivar);	     } 
             | VAR '=' exp        { $$ = $3; $1->value.var = $3;     }
	     | IVAR '=' exp	  { $$ = *($1->value.ivar) = $3;	     } 
             | FNCT '(' exp ')'   { $$ = (*($1->value.fnctptr))( $3 ); }
             | FNCT '(' exp ',' exp ')'   { $$ = (*($1->value.fnctptr))($3,$5); }
             | FNCT '(' exp ',' exp ',' exp ')'   { $$ = (*($1->value.fnctptr))($3,$5,$7); } 
             | FNCT '(' exp ',' exp ',' exp ',' exp ')'   { $$ = (*($1->value.fnctptr))($3,$5,$7,$9); } 
             | FNCT '(' exp ',' exp ',' exp ',' exp ',' exp')'   { $$ = (*($1->value.fnctptr))($3,$5,$7,$9,$11); } 
             | FNCT '(' exp ',' exp ',' exp ',' exp ',' exp ',' exp')'   { $$ = (*($1->value.fnctptr))($3,$5,$7,$9,$11,$13); } 
             | exp '+' exp        { $$ = $1 + $3;                    }
             | exp '-' exp        { $$ = $1 - $3;                    }
             | exp '*' exp        { $$ = $1 * $3;                    }
             | exp '/' exp        { $$ = $1 / $3;                    }
             | exp '%' exp        { $$ = fmod($1,$3);                }
             | '-' exp  %prec NEG { $$ = -$2;                        }
             | exp '^' exp        { $$ = pow ($1, $3);               }
             | '(' exp ')'        { $$ = $2;                         }
     ;
     /* End of grammar */
     %%
/*  HERE BEGINS THE CODE FOR THE SYMOL TABLE  */

#ifdef UNIX
#include <string.h>
#endif
     struct init_func
     {
       char *fname;
       Scalar (*fnct)();
     };
     struct init_var
     {
       char *vname;
       Scalar value;
     };

     struct init_func arith_fncts[]
       = {
           {"sin",  SINW},
           {"cos",  COSW},
           {"atan", ATANW},
           {"ln",   LOGW},
           {"exp",  EXPW},
           {"sqrt", SQRTW},
           {"pow",  POWW},
           {"step", step},
           {"pulse",pulse},
           {"ramp", ramp},
           {"tanh", TANHW},
           {"Gauss",gaussian},
	   {"fabs", FABSW},
           {0, 0}
         };

     struct init_var init_vars[] = {
		  {"PI", 3.141592654},
		  {"e", 2.18281828},
		  {0,0}
	  };
     /* The symbol table: a chain of `struct symrec'.  */

symrec *init_table (symrec *symbol_table)  /* puts arithmetic functions in table. */
     {
       int i;
       for (i = 0; arith_fncts[i].fname != 0; i++)
         {
           symbol_table = putsym (symbol_table,arith_fncts[i].fname, FNCT);
           symbol_table->value.fnctptr = arith_fncts[i].fnct;
         }
       for (i = 0; init_vars[i].vname !=0; i++)
       {
	 symbol_table = putsym (symbol_table, init_vars[i].vname,VAR);
	 symbol_table->value.var = init_vars[i].value;
       }
       return symbol_table;

     }

/* copy an old symbol table and return a new symbol table. */

symrec *copy_table (symrec *symbol_table)  
     {
       symrec *walk = symbol_table;
       symrec *new_table = 0;
       /* traverse the list */
       for(walk = symbol_table; walk != 0; walk = walk->next) {
	 new_table = putsym( new_table,walk->name,walk->type);
	 switch(walk->type) {
	     case FNCT:
	       new_table->value.fnctptr = walk->value.fnctptr;
	       break;
	     case VAR:
	       new_table->value.var = walk->value.var;
	       break;
	     case IVAR:
	       new_table->value.ivar = walk->value.ivar;
	       break;
	 }
       }
       return new_table;
     }


void     delete_table (symrec *symbol_table)  /* deletes symbol table */
     {
       symrec *ptr,*old;
       for(ptr=symbol_table;ptr!=(symrec *)0; 
	   old=ptr,ptr=(symrec *) ptr->next,free(old->name),free(old));
     }

     symrec *
     putsym (symrec *sym_table,const char *sym_name,int sym_type)
     {
       symrec *ptr;
       ptr = (symrec *) malloc (sizeof (symrec));
       ptr->name = (char *) malloc (strlen (sym_name) + 1);
       strcpy (ptr->name,sym_name);
       ptr->type = sym_type;
       ptr->value.var = 0; /* set value to 0 even if fctn.  */
       ptr->next = (struct symrec *)sym_table;
       return ptr;
     }
     
     symrec *
     getsym (symrec *sym_table,const char *sym_name)
     {
       symrec *ptr;
       for (ptr = sym_table; ptr != (symrec *) 0;
            ptr = (symrec *)ptr->next)
         if (strcmp (ptr->name,sym_name) == 0)
           return ptr;
       return 0;
     }



/*  THIS IS THE LEXER  */

/*  Some helper functions for the lexer */
int Sindex;
char *EVAL_STRING;
Scalar EVAL_RESULT;
symrec *SYMBOL_TABLE;

char Sgetchar() {
	Sindex++;
  return EVAL_STRING[(Sindex-1)];
}

void Sungetc() {
  Sindex--;
}

Scalar Sgetnum() {
	Scalar value;
	char c;
#ifdef SCALAR_DOUBLE
	sscanf(EVAL_STRING + Sindex,"%lf",&value);
#else
	sscanf(EVAL_STRING + Sindex,"%f",&value);
#endif
	do {
		Sindex++;
		c=EVAL_STRING[Sindex];
	} while( isdigit(c) || c==0 || c=='e' || c=='E' 
		 || c=='.' || ((c=='-'||c=='+') && 
	        (EVAL_STRING[Sindex-1]=='e' || EVAL_STRING[Sindex-1]=='E')) 
	  );
	return value;
}

	
     int yylex ()
     {
       int c;
     
       /* Ignore whitespace, get first nonwhite character.  */
       while ((c = Sgetchar ()) == ' ' || c == '\t');
     
       if (c == 0)
         return 0;

     /* Char starts a number => parse the number.         */
       if (c == '.' || isdigit (c))
         {
           Sungetc ();
           yylval.val=Sgetnum();
           return NUM;
         }

     /* Char starts an identifier => read the name.       */
       if (isalpha (c))
         {
           symrec *s;
           static char *symbuf = 0;
           static int length = 0;
           int i;

     /* Initially make the buffer long enough
              for a 40-character symbol name.  */
           if (length == 0)
             length = 40, symbuf = (char *)malloc (length + 1);
     
           i = 0;
           do

     {
               /* If buffer is full, make it bigger.        */
               if (i == length)
                 {
                   length *= 2;
                   symbuf = (char *)realloc (symbuf, length + 1);
                 }
               /* Add this character to the buffer.         */
               symbuf[i++] = c;
               /* Get another character.                    */
               c = Sgetchar ();
             }

     while (c != 0 && (isalnum (c)|| c=='_'));
     
           Sungetc ();
           symbuf[i] = '\0';

     s = getsym (SYMBOL_TABLE,symbuf);
           if (s == 0)
             s=SYMBOL_TABLE = putsym (SYMBOL_TABLE,symbuf, VAR);
           yylval.tptr = s;
           return s->type;
         }
     
       /* Any other character is a token by itself.        */
       return c;
     }


/*  THIS IS THE MAIN  */
/*     main ()
     {
       init_table ();
       yyparse ();
     } */

     void yyerror (s)  /* Called by yyparse on error */
          char *s;
     {
       printf ("%s: %s\n", s,EVAL_STRING);
		 exit(1);  /* DIE!  DIE!  DIE! */
     }

/*  THE END */
