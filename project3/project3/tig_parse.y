
/***
Name: Arturo Perez
VUnetid: perezae
Email: arturo.e.perez@vanderbilt.edu
Descrption: A parser for the Tiger language written using Bison.
Last modified: 2/28/2018
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

%nonassoc 		THEN
%nonassoc 		ELSE
%nonassoc 		DO
%nonassoc 		OF
%nonassoc 		ASSIGN
%left	 		OR 
%left 			AND
%nonassoc 		EQ NEQ LT LE GT GE
%left 			SEMICOLON /*MAYBE */
%left 			PLUS MINUS
%left 			TIMES DIVIDE
%left 			LPAREN RPAREN
%left			UMINUS

//fix if dangling else by making them nonassoc 

%start program

/* the next section is the rule section of the grammar file.  */
/* that is where you will place your grammar for the Tiger    */
/* language. note: all embedded actions should be empty for   */
/* for project #3.                                            */

%%

program:	exp 							{ }

exp:		INT							{ }
   |		NIL							{ }
   |		STRING							{ }
   |		exp PLUS exp						{ }
   |		exp MINUS exp						{ }
   | 		exp TIMES exp						{ }
   |		exp DIVIDE exp						{ }
   |		MINUS exp %prec UMINUS					{ }
   |		exp EQ exp						{ }
   | 		exp NEQ exp						{ } 
   | 		exp LT exp						{ }
   |		exp LE exp						{ }
   |		exp GT exp						{ }
   | 		exp GE exp						{ }
   |		exp AND exp						{ }
   |		exp OR exp						{ }
   |		LPAREN RPAREN 						{ }
   |		LPAREN expSC RPAREN 					{ }
   |		LET dec IN expSC END 					{ }
   |		IF exp THEN exp ELSE exp 				{ }
   |		IF exp THEN exp						{ }
   |		ID LPAREN funcparams RPAREN  				{ }	
   |		ID LBRACE recparams RBRACE 				{ }
   |		ID LBRACE recparams RBRACE 				{ }
   |		lvalue LBRACK exp RBRACK OF exp				{ }
   |		lvalue							{ }
   |		lvalue ASSIGN exp 					{ }
   |		FOR lvalue ASSIGN exp TO exp DO expSC			{ }
   |		WHILE exp DO expSC					{ }
   |		BREAK

/* deals with semicolons and break in exp */
expSC:		exp							{ }
   |		expSC SEMICOLON expSC					{ }
   |		expSC SEMICOLON BREAK SEMICOLON expSC			{ }
   |		/* epsilon */						{ }

/* declaration-sequence */
dec:		tydec dec						{ }
   |		vardec dec						{ }
   |		fundec dec						{ }
   |		/* epsilon */						{ }

/* type declaration */
tydec:		TYPE ID EQ ty						{ }

/* variable declaration */ 
vardec:		VAR ID ASSIGN exp					{ }
   |		VAR ID COLON ID ASSIGN exp				{ }

/* function declaration */
fundec:		FUNCTION ID LPAREN tyfields RPAREN EQ exp		{ }
   |		FUNCTION ID LPAREN tyfields RPAREN COLON ID EQ exp	{ }


ty:		ID							{ }
   |		LBRACE tyfields RBRACE					{ }
   |		ARRAY OF ID						{ }

/* used in function declaration */
tyfields:	/* epsilon */						{ }
   |		ID COLON ID						{ }
   |		tyfields COMMA ID COLON ID				{ }

lvalue:		ID							{ }
   |		lvalue DOT ID						{ }
   |		lvalue LBRACK exp RBRACK				{ }

/* function parameters */
funcparams:	funcparams COMMA exp					{ }
   |		exp							{ }
   |		/* epsilon */						{ }

recparams:	recparams COMMA ID EQ exp				{ }
   |		ID EQ exp						{ }
   |		/* epsilon */						{ }


%%

/* this section is for user defined subroutines */

void yyerror(char *s)
{
 EM_error(EM_tokPos, "%s", s);
}

