%{

/***
Name: Arturo Perez
VUnetid: perezae
Email: arturo.e.perez@vanderbilt.edu
Descrption:
Last modified: 11 February 2018
***/


/*****************************
  code in this section gets copied to the output
  file verbatim. Things to place here are your includes,
  declarations of global variables, and function prototypes
*****************************/

#include <string>
#include <cstdlib>
using namespace std;
#include "util.h"
#include "tokens.h"
#include "errormsg.h"

int charPos=1;

extern "C"
int yywrap(void);

void adjust(void);


%}

/*****************************
  this section can be used to declare patterns that we can
  refer to in the next section
*****************************/
 

/* Define the non-escape chars that are legal in a string:
	Printable characters except quote and backslash.
	quote comes between # and [  backslash between [ and ]  */

nonesc [ 0-9A-Za-z\!\#\$\%\&\'\(\)\*\+\,\-\.\/\:\;\<\=\>\?\@\[\]\^\_\`\{\|\}\~]



/*****************************
  the following section is where we place all our rules and actions
*****************************/

%x INITIAL
%x COMMENT
%x STR
%x ESC 
%x CTRL 

%%

<INITIAL> " "	    {adjust(); continue;}
<INITIAL> "/*"    {adjust(); BEGIN COMMENT;} 
<INITIAL> \n	    {adjust(); EM_newline(); continue;}
<INITIAL> ","	    {adjust(); return COMMA;}
<INITIAL> ":"     {adjust(); return COLON}
<INITIAL> ";"     {adust(); return SEMICOLON}
<INITIAL> "("     {adust(); return LPAREN}
<INITIAL> ")"     {adust(); return RPAREN}
<INITIAL> "["     {adust(); return LBRACK}
<INITIAL> "]"     {adust(); return RBRACK}
<INITIAL> "{"     {adust(); return LBRACE}
<INITIAL> "}"     {adust(); return RBRACE}
<INITIAL> "."     {adust(); return DOT}
<INITIAL> "+"     {adjust(); return PLUS}
<INITIAL> "-"     {adjust(); return MINUS}
<INITIAL> "*"     {adjust(); return TIMES}
<INITIAL> "/"     {adjust(); return DIVIDE}
<INITIAL> "="     {adust(); return EQ}
<INITIAL> "<>"    {adjust(); return NEQ}
<INITIAL> "<"     {adjust(); return LT}
<INITIAL> "<="    {adjust(); return LE}
<INITIAL> ">"     {adust(); return GT}
<INITIAL> ">="    {adjust(); return GE}
<INITIAL> "&"     {adjust(); return AND}
<INITIAL> "|"     {adjust(); return OR}
<INITIAL> ":="    {adjust(); return ASSIGN}
<INITIAL> array   {adjust(); return ARRAY;}
<INITIAL> if      {adjust(); return IF}
<INITIAL> then    {adjust(); return THEN}
<INITIAL> else    {adjust(); return ELSE}
<INITIAL> while   {adjust(); return WHILE}
<INITIAL> for     {adjust(); return FOR;}
<INITIAL> to      {adjust(); return TO;}
<INITIAL> do      {adjust(); return DO;}
<INITIAL> let     {adjust(); return LET}
<INITIAL> in      {adjust(); return IN}
<INITIAL> end     {adjust(); return END}
<INITIAL> of      {adjust(); return OF}
<INITIAL> break   {adjust(); return BREAK}
<INITIAL> nil     {adjust(); return NIL}
<INITIAL> function {adjust(); return FUNCTION}
<INITIAL> var     {adjust(); return VAR}
<INITIAL> type    {adjust(); return TYPE}
<INITIAL> [0-9]+	{adjust(); yylval.ival=atoi(yytext); return INT;}
<INITIAL> .	      {adjust(); EM_error(EM_tokPos, "illegal token: '%c'",yytext[0]);}

<COMMENT> "*/" {adjust(); BEGIN INITIAL;}
<COMMENT> {nonesc}    {adust(); continue;}  

%%

/*****************************
  this section is where we place all our user-defined functions (prototypes
  appear in the first section
*****************************/

int yywrap(void)
{
 charPos=1;
 return 1;
}


void adjust(void)
{
 EM_tokPos=charPos;
 charPos+=yyleng;
}


