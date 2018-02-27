
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

%nonassoc IF
%nonassoc THEN
%nonassoc AND OR
%nonassoc EQ NEQ LT LE GT GE
%left PLUS MINUS
%left TIMES DIVIDE

//fix if dangling else by making them nonassoc 



%start program



/* the next section is the rule section of the grammar file.  */
/* that is where you will place your grammar for the Tiger    */
/* language. note: all embedded actions should be empty for   */
/* for project #3.                                            */

%%

program:	exp 	{ }
   |		ty 	{ }
   |		tyfields { } 
   | 		vardec  { }
   |		fundec	{ }
   |		parantheses	{ }

exp:		INT	{ }
   |		NIL	{ }
   |		LPAREN  { }
   |		exp PLUS exp	{ }
   |		exp MINUS exp	{ }
   | 		exp TIMES exp   { }
   |		exp DIVIDE exp	{ }
   |		exp EQ exp	{ }
   | 		exp NEQ exp	{ } 
   | 		exp LT exp	{ }
   |		exp LE exp 	{ }
   |		exp GT exp 	{ }
   | 		exp GE exp	{ }
   |		LPAREN exp	{ }
   |		exp SEMICOLON exp	{ }
   |		exp RPAREN	{ }

ty:		ID	{ }
   |		TYPE	{ } 

tyfields:	ID COLON ID		{ }
   |		/* epsilon */		{ }

//START HERE WITH TYFIELDS ///////////////////////////

vardec:		VAR ID ASSIGN exp 	{ }
   |		VAR ID COLON ID ASSIGN exp	{ }

fundec:		FUNCTION ID RPAREN tyfields LPAREN EQ exp	{ }
   |    	FUNCTION ID RPAREN tyfields LPAREN COLON ID EQ exp	{ }

parantheses: LPAREN RPAREN	{ }

%%

/* this section is for user defined subroutines */

void yyerror(char *s)
{
 EM_error(EM_tokPos, "%s", s);
}

