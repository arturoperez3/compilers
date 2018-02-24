
/***
Name:
VUnetid:
Email:
Descrption:
Last modified:
***/


/* this is the definition section of the grammar file.          */
/* here is where you place your includes, function prototypes,  */
/* and Yacc/Bison special operators.                            */
%{
#include <cstdio>
#include <string>
#include "util.h"
#include "errormsg.h"
using namespace std;

int yylex(void); /* function prototype */

void yyerror(char *s);


%}


%union {
	int pos;
	int ival;
	string *sval; 
	}

%token <sval> ID STRING
%token <ival> INT

%token 
  COMMA COLON SEMICOLON LPAREN RPAREN LBRACK RBRACK 
  LBRACE RBRACE DOT 
  PLUS MINUS TIMES DIVIDE EQ NEQ LT LE GT GE
  AND OR ASSIGN
  ARRAY IF THEN ELSE WHILE FOR TO DO LET IN END OF 
  BREAK NIL
  FUNCTION VAR TYPE 



%start program



/* the next section is the rule section of the grammar file.  */
/* that is where you will place your grammar for the Tiger    */
/* language. note: all embedded actions should be empty for   */
/* for project #3.                                            */

%%

program:	exp	{ }

exp:		INT	{ }
   |		NIL	{ }
   |		exp PLUS exp	{ }



%%

/* this section is for user defined subroutines */

void yyerror(char *s)
{
 EM_error(EM_tokPos, "%s", s);
}

