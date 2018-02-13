%{

/***
Name:
VUnetid:
Email:
Descrption:
Last modified:
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

%%
" "	 {adjust(); continue;}
\n	 {adjust(); EM_newline(); continue;}
","	 {adjust(); return COMMA;}
for	 {adjust(); return FOR;}
[0-9]+	 {adjust(); yylval.ival=atoi(yytext); return INT;}
.	 {adjust(); EM_error(EM_tokPos, "illegal token: '%c'",yytext[0]);}


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


