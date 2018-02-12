#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include "util.h"
#include "errormsg.h"
#include "tokens.h"
using namespace std;

YYSTYPE yylval;

int yylex(void); /* prototype for the lexing function */



appel_string toknames[] = {
"ID", "STRING", "INT", "COMMA", "COLON", "SEMICOLON", "LPAREN",
"RPAREN", "LBRACK", "RBRACK", "LBRACE", "RBRACE", "DOT", "PLUS",
"MINUS", "TIMES", "DIVIDE", "EQ", "NEQ", "LT", "LE", "GT", "GE",
"AND", "OR", "ASSIGN", "ARRAY", "IF", "THEN", "ELSE", "WHILE", "FOR",
"TO", "DO", "LET", "IN", "END", "OF", "BREAK", "NIL", "FUNCTION",
"VAR", "TYPE"
};


appel_string tokname(int tok) {
  //  return tok<257 || tok>299 ? "BAD_TOKEN" : toknames[tok-257];
  if (tok<257 || tok>299)
    return "BAD_TOKEN";
  else
    return toknames[tok-257];
}

int main(int argc, char **argv) {
 appel_string fname; int tok;
 if (argc!=2) {cerr << "usage: " << argv[0] << " filename\n"; exit(1);}
 fname=argv[1];
 EM_reset(fname);
 for(;;) {
   tok=yylex();
   if (tok==0) break;
   switch(tok) {
   case ID: case STRING:
     cout << setw(10) << tokname(tok) << " " << setw(4) << EM_tokPos 
          << " " << *yylval.sval << endl;
     break;
   case INT:
     cout << setw(10) << tokname(tok) << " " << setw(4) << EM_tokPos
          << " " << yylval.ival << endl;
     break;
   default:
     cout << setw(10) << tokname(tok) << " " << setw(4) << EM_tokPos << endl;
   }
 }
 return 0;
}


