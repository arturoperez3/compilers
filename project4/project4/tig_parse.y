
/***
Name:
Userid:
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
#include "symbol.h"
#include "absyn.h"
using namespace std;

absyn::Exp* absyn_root;

int yylex(void); /* function prototype */

void yyerror(char *s);

/* test that an exp is really an lvalue */
const absyn::Var* really_lvalue(absyn::Exp* really_an_lvalue);

/* test that an lvalue is really a simple ID that could be a typename */
S_symbol really_typename(absyn::Var* really_a_typename);


%}


%union {
	int pos;
	int ival;
	string *sval;
	absyn::Var* var;
	absyn::Exp* exp;
	absyn::Dec* dec;
	absyn::Ty* ty;
	absyn::Field* field;
	absyn::FieldList* fieldList;
	absyn::ExpList* expList;
	absyn::FunDec* fundec;
	absyn::FunDecList* fundecList;
	absyn::DecList* decList;
	absyn::Namety* namety;
	absyn::NametyList* nametyList;
	absyn::EFieldList* efieldList;
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


/* THEN/ELSE stuff taken from page 234 of O'Reilly book */
%nonassoc THEN
%nonassoc ELSE STMT
%nonassoc ASSIGN
%nonassoc ARRAY_LITERAL
%left OR
%left AND 
%nonassoc EQ NEQ GT LT GE LE
%left PLUS MINUS
%left TIMES DIVIDE
%left UMINUS
%nonassoc LBRACK LPAREN LBRACE DOT


%type <exp> exp program expseq seq 
%type <expList> list arglist arglist2
%type <var> lvalue
%type <efieldList> reclist reclist2
%type <dec> dec vardec
%type <decList> decs
%type <nametyList> tydeclist
%type <namety> tydec
%type <fieldList> tyfields fieldseq
%type <field> tyfield
%type <fundecList> fundeclist
%type <fundec> fundec
%type <ty> ty

%start program



/* The next section is the rule section of the grammar file.  */
/* That is where you will place your grammar for the Tiger    */
/* language.                                                  */
/* For project #4, you will need to add semantic actions that */
/* build an abstract syntax tree.                             */
%%

program:	exp	{ absyn_root = $1; }

exp:	INT { $$ = new absyn::IntExp(EM_tokPos, $1); }

   |    STRING { $$ = new absyn::StringExp(EM_tokPos, Appel_String($1)); }

   |    BREAK {  }

   |    lvalue { $$ = new absyn::VarExp(EM_tokPos,$1); }

   |    NIL {  }

   |    exp PLUS exp {  }

   |    exp MINUS exp {  }

   |    exp TIMES exp {  }

   |    exp DIVIDE exp {  }

   |    MINUS exp %prec UMINUS  {  }

   |    exp EQ exp {  }

   |    exp NEQ exp {  }

   |    exp GT exp {  }

   |    exp LT exp {  }

   |    exp GE exp {  }

   |    exp LE exp {  }

   |    exp AND exp {  }

   |    exp OR exp {  }

   |    lvalue ASSIGN exp {  }

   |    IF exp THEN exp {  }

   |    IF exp THEN exp ELSE exp {  }

   |    WHILE exp DO exp %prec STMT {  }

   |    FOR ID ASSIGN exp TO exp DO exp %prec STMT  {  }

   |    LPAREN RPAREN {  }

   |    LPAREN exp RPAREN {  }

   |    LPAREN seq RPAREN {  }

   |    lvalue LBRACK exp RBRACK OF exp %prec ARRAY_LITERAL {  }
        // NOTE: used 'lvalue' rather than 'ID' to avoid a conflict
        // (see hint #6 from project #3).
        // Must insure that the lvalue is really just an ID

   |    ID LPAREN arglist RPAREN  {  }

   |    ID LBRACE reclist RBRACE  {  }

   |    LET decs IN expseq END {  }


decs:   /* epsilon */	  {  }
   |    dec decs          {  }

dec:    tydeclist   {  }
   |    vardec      {  }
   |    fundeclist  {  }

// The following two nonterminals each introduce a shift/reduce conflict,
// since we can parse two types as a single list (by shifting) or reduce
// each to a separate list and the parse the two lists as two decs.
// We want to do the former, which corresponds to a shift, so its ok.

tydeclist: tydec tydeclist	{  }
	 | tydec		{  }

fundeclist: fundec fundeclist	{  }
	  | fundec		{  }

tydec:   TYPE ID EQ ty {  }

ty:      ID {  }
  |      LBRACE tyfields RBRACE {  }
  |      ARRAY OF ID {  }

tyfields: /*epsilon*/    {  }
        |  fieldseq      {  }

fieldseq:  tyfield {  }
        |  tyfield COMMA fieldseq  {  }

tyfield:   ID COLON ID   {  }

vardec:    VAR ID ASSIGN exp  { }
      |    VAR ID COLON ID ASSIGN exp  {  }

fundec:    FUNCTION ID LPAREN tyfields RPAREN EQ exp   {  }
      |    FUNCTION ID LPAREN tyfields RPAREN COLON ID EQ exp  {  } 

expseq:    /*epsilon*/    {  }
      |    list           {  }

list:      exp {  }
    |      exp SEMICOLON list  {  }

seq:       exp SEMICOLON list {  }

reclist:   /*epsilon*/   {  }
       |   reclist2      {  }
              

reclist2:    ID EQ exp   {  }
        |    ID EQ exp COMMA reclist2  {  }

arglist:   /*epsilon*/    {  }
       |   arglist2       {  }

arglist2:   exp {  }
        |   exp COMMA arglist2 {  } 

lvalue:   ID {$$ = new absyn::SimpleVar(EM_tokPos,S_Symbol($1)); }
      |   lvalue DOT ID {  }
      |   lvalue LBRACK exp RBRACK {  }
%%

/* this section is for user defined subroutines */

void yyerror(char *s)
{
 EM_error(EM_tokPos, "%s", s);
}


/* test that an exp is really an lvalue */
const absyn::Var* really_lvalue(absyn::Exp* really_an_lvalue)
{
    if (dynamic_cast<absyn::VarExp*>(really_an_lvalue) == NULL)
    {
      EM_error(really_an_lvalue->getPos(), "%s", "illegal lvalue");
      return nullptr;
    }
    else
    {
      return dynamic_cast<absyn::VarExp*>(really_an_lvalue)->getVar();
    }
}

/* test that an lvalue is really a simple ID that could be a typename */
S_symbol really_typename(absyn::Var* really_a_typename)
{
    if (dynamic_cast<absyn::SimpleVar*>(really_a_typename) == nullptr)
    {
      EM_error(really_a_typename->getPos(), "%s", "Illegal type name");
      return 0;
    }
    else
    {
      return dynamic_cast<absyn::SimpleVar*>(really_a_typename)->getSymbol();
    }
}

