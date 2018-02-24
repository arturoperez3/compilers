%{

/***
Name: Arturo Perez
VUnetid: perezae
Email: arturo.e.perez@vanderbilt.edu
Descrption: A lexical analyzer for the Tiger language built using flex.
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

int count = 0;

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
%x STRNG

%%

" "	    {adjust(); continue;}
"/*"    {adjust(); ++count; BEGIN COMMENT;} 
"\""    {adjust(); yylval.sval=new string(yytext); BEGIN STRNG;}
\n	    {adjust(); EM_newline(); continue;}
\t      {adjust(); continue;}
\f      {adjust(); continue;}
","	    {adjust(); return COMMA;}
":"     {adjust(); return COLON;}
";"     {adjust(); return SEMICOLON;}
"("     {adjust(); return LPAREN;}
")"     {adjust(); return RPAREN;}
"["     {adjust(); return LBRACK;}
"]"     {adjust(); return RBRACK;}
"{"     {adjust(); return LBRACE;}
"}"     {adjust(); return RBRACE;}
"."     {adjust(); return DOT;}
"+"     {adjust(); return PLUS;}
"-"     {adjust(); return MINUS;}
"*"     {adjust(); return TIMES;}
"/"     {adjust(); return DIVIDE;}
"="     {adjust(); return EQ;}
"<>"    {adjust(); return NEQ;}
"<"     {adjust(); return LT;}
"<="    {adjust(); return LE;}
">"     {adjust(); return GT;}
">="    {adjust(); return GE;}
"&"     {adjust(); return AND;}
"|"     {adjust(); return OR;}
":="    {adjust(); return ASSIGN;}
"*/"    {adjust(); EM_error(EM_tokPos, "unmatched closing comment");}
array   {adjust(); return ARRAY;}
if      {adjust(); return IF;}
then    {adjust(); return THEN;}
else    {adjust(); return ELSE;}
while   {adjust(); return WHILE;}
for     {adjust(); return FOR;}
to      {adjust(); return TO;}
do      {adjust(); return DO;}
let     {adjust(); return LET;}
in      {adjust(); return IN;}
end     {adjust(); return END;}
of      {adjust(); return OF;}
break   {adjust(); return BREAK;}
nil     {adjust(); return NIL;}
function {adjust(); return FUNCTION;}
var     {adjust(); return VAR;}
type    {adjust(); return TYPE;}
[0-9]+	{adjust(); yylval.ival=atoi(yytext); return INT;}
.	      {adjust(); EM_error(EM_tokPos, "illegal token: '%c'",yytext[0]);}
([a-zA-Z])([a-zA-Z0-9_])*    {adjust(); yylval.sval = new string(yytext); return ID;}

<STRNG>"\""        {adjust(); BEGIN INITIAL; yylval.sval->erase(0,1); return STRING; }
<STRNG>{nonesc}    {adjust(); yylval.sval->push_back(*yytext);}
<STRNG><<EOF>>     {adjust(); EM_error(EM_tokPos, "end of file encountered"); BEGIN INITIAL;}
<STRNG>\n          {adjust(); EM_error(EM_tokPos, "new line encountered"); EM_newline(); BEGIN INITIAL;}
<STRNG>\f          {adjust(); continue;}

<COMMENT>"/*" {adjust(); ++count; continue;}
<COMMENT>"*/" {adjust(); --count; if (count < 1) BEGIN INITIAL; continue;}
<COMMENT>.    {adjust(); continue;}  
<COMMENT><<EOF>>   {adjust(); EM_error(EM_tokPos, "unterminated comment"); BEGIN INITIAL;}
<COMMENT>\n   {adjust(); EM_newline(); continue;}

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


