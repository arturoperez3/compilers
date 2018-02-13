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

%x COMMENT
%x STR
%x ESC 
%x CTRL 

%%

" "	    {adjust(); continue;}
"/*"    {adjust(); BEGIN COMMENT;} 
\n	    {adjust(); EM_newline(); continue;}
","	    {adjust(); return COMMA;}
":"     {adjust(); return COLON}
";"     {adust(); return SEMICOLON}
"("     {adust(); return LPAREN}
")"     {adust(); return RPAREN}
"["     {adust(); return LBRACK}
"]"     {adust(); return RBRACK}
"{"     {adust(); return LBRACE}
"}"     {adust(); return RBRACE}
"."     {adust(); return DOT}
"+"     {adjust(); return PLUS}
"-"     {adjust(); return MINUS}
"*"     {adjust(); return TIMES}
"/"     {adjust(); return DIVIDE}
"="     {adust(); return EQ}
"<>"    {adjust(); return NEQ}
"<"     {adjust(); return LT}
"<="    {adjust(); return LE}
">"     {adust(); return GT}
">="    {adjust(); return GE}
"&"     {adjust(); return AND}
"|"     {adjust(); return OR}
":="    {adjust(); return ASSIGN}
array   {adjust(); return ARRAY;}
if      {adjust(); return IF}
then    {adjust(); return THEN}
else    {adjust(); return ELSE}
while   {adjust(); return WHILE}
for     {adjust(); return FOR;}
to      {adjust(); return TO;}
do      {adjust(); return DO;}
let     {adjust(); return LET}
in      {adjust(); return IN}
end     {adjust(); return END}
of      {adjust(); return OF}
break   {adjust(); return BREAK}
nil     {adjust(); return NIL}
function {adjust(); return FUNCTION}
var     {adjust(); return VAR}
type    {adjust(); return TYPE}
[0-9]+	{adjust(); yylval.ival=atoi(yytext); return INT;}
.	      {adjust(); EM_error(EM_tokPos, "illegal token: '%c'",yytext[0]);}

<COMMENT> "*/" {adjust(); BEGIN INITIAL;}
<COMMENT> {nonesc}     {adust(); continue;}  

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


