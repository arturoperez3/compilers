
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

   |    BREAK { $$ = new absyn::BreakExp(EM_tokPos); }

   |    lvalue { $$ = new absyn::VarExp(EM_tokPos,$1); }

   |    NIL { $$ = new absyn::NilExp(EM_tokPos); }

   |    exp PLUS exp { $$ = new absyn::OpExp(EM_tokPos, absyn::OpExp::EXP_PLUS, $1, $3) ; }

   |    exp MINUS exp { $$ = new absyn::OpExp(EM_tokPos, absyn::OpExp::EXP_MINUS, $1, $3) ;  }

   |    exp TIMES exp { $$ = new absyn::OpExp(EM_tokPos, absyn::OpExp::EXP_MUL, $1, $3) ; }

   |    exp DIVIDE exp { $$ = new absyn::OpExp(EM_tokPos, absyn::OpExp::EXP_DIV, $1, $3) ;  }

   |    MINUS exp %prec UMINUS  {  }

   |    exp EQ exp { $$ = new absyn::OpExp(EM_tokPos, absyn::OpExp::EXP_EQ, $1, $3) ; }

   |    exp NEQ exp { $$ = new absyn::OpExp(EM_tokPos, absyn::OpExp::EXP_NE, $1, $3) ;  }

   |    exp GT exp { $$ = new absyn::OpExp(EM_tokPos, absyn::OpExp::EXP_GT, $1, $3) ;  }

   |    exp LT exp { $$ = new absyn::OpExp(EM_tokPos, absyn::OpExp::EXP_LT, $1, $3) ;  }

   |    exp GE exp { $$ = new absyn::OpExp(EM_tokPos, absyn::OpExp::EXP_GE, $1, $3) ;  }

   |    exp LE exp { $$ = new absyn::OpExp(EM_tokPos, absyn::OpExp::EXP_LE, $1, $3) ;  }

   |    exp AND exp { $$ = new absyn::IfExp(EM_tokPos, $1, $3, nullptr) ; }

   |    exp OR exp { $$ = new absyn::IfExp(EM_tokPos, $1, new absyn::IntExp(EM_tokPos,1), $3) ; }

   |    lvalue ASSIGN exp { $$ = new absyn::AssignExp(EM_tokPos, $1, $3) ; }

   |    IF exp THEN exp { $$ = new absyn::IfExp(EM_tokPos, $2, $4, nullptr) ; }

   |    IF exp THEN exp ELSE exp { $$ = new absyn::IfExp(EM_tokPos, $2, $4, $6) ; }

   |    WHILE exp DO exp %prec STMT { $$ = new absyn::WhileExp(EM_tokPos, $2, $4) ; }

   |    FOR ID ASSIGN exp TO exp DO exp %prec STMT  { $$ = new absyn::ForExp(EM_tokPos, S_Symbol($2), $4, $6, $8) ; }

   |    LPAREN RPAREN { $$ = new absyn::SeqExp(EM_tokPos, nullptr) ;  }

   |    LPAREN exp RPAREN { $$ = $2 ; }

   |    LPAREN seq RPAREN { $$ = $2 ;  }

   |    lvalue LBRACK exp RBRACK OF exp %prec ARRAY_LITERAL { $$ = new absyn::ArrayExp(EM_tokPos, really_typename($1), $3, $6) ; }
        // NOTE: used 'lvalue' rather than 'ID' to avoid a conflict
        // (see hint #6 from project #3).
        // Must insure that the lvalue is really just an ID

   |    ID LPAREN arglist RPAREN  { $$ = new absyn::CallExp(EM_tokPos,S_Symbol($1), $3) ; }

   |    ID LBRACE reclist RBRACE  { $$ = new absyn::RecordExp(EM_tokPos, S_Symbol($1), $3) ; }

   |    LET decs IN expseq END { $$ = new absyn::LetExp(EM_tokPos, $2, $4 ) ; }


decs:   /* epsilon */	  { $$ = new absyn::DecList(nullptr, nullptr) ; }
   |    dec decs          { $$ = new absyn::DecList($1, $2); }

dec:    tydeclist   { $$ = new absyn::TypeDec(EM_tokPos, $1) ; }
   |    vardec      { $$ = $1 ; }
   |    fundeclist  { $$ = new absyn::FunctionDec(EM_tokPos, $1); }

// The following two nonterminals each introduce a shift/reduce conflict,
// since we can parse two types as a single list (by shifting) or reduce
// each to a separate list and the parse the two lists as two decs.
// We want to do the former, which corresponds to a shift, so its ok.

tydeclist: tydec tydeclist	{ $$ = new absyn::NametyList($1, $2) ; }
	 | tydec		{ $$ = new absyn::NametyList($1, nullptr) ; }

fundeclist: fundec fundeclist	{ $$ = new absyn::FunDecList($1, $2) ; }
	  | fundec		{  $$ = new absyn::FunDecList($1, nullptr) ; }

tydec:   TYPE ID EQ ty { $$ = new absyn::Namety(S_Symbol($2), $4) ; }

ty:      ID { $$ = new absyn::NameTy(EM_tokPos, S_Symbol($1)) ; }
  |      LBRACE tyfields RBRACE { $$ = new absyn::RecordTy(EM_tokPos, $2) ; }
  |      ARRAY OF ID { $$ = new absyn::ArrayTy(EM_tokPos, S_Symbol($3)) ; }

tyfields: /*epsilon*/    { $$ = new absyn::FieldList(nullptr, nullptr) ; }
        |  fieldseq      { $$ = $1 ; }

fieldseq:  tyfield { $$ = new absyn::FieldList($1, nullptr) ; }
        |  tyfield COMMA fieldseq  { $$ = new absyn::FieldList($1, $3) ; }


























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

reclist:   /*epsilon*/   { /* efieldList */ }
       |   reclist2      {  }
              

reclist2:    ID EQ exp   {  }
        |    ID EQ exp COMMA reclist2  {  }

arglist:   /*epsilon*/    { /* efieldList */ }
       |   arglist2       {  }

arglist2:   exp {  }
        |   exp COMMA arglist2 {  } 

lvalue:   ID {$$ = new absyn::SimpleVar(EM_tokPos,S_Symbol($1)); }
      |   lvalue DOT ID {  }
      |   lvalue LBRACK exp RBRACK { $$ = new absyn::SubscriptVar(EM_tokPos, new absyn::SimpleVar(EM_tokPos, really_typename($1)), $3) ; }
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
    if (dynamic_cast<absyn::SimpleVar*>(really_a_typename) == NULL) {
      EM_error(really_a_typename->getPos(), "%s", "Illegal type name");
      return S_Symbol("<ERROR--Bad Type Name>");
    } else {
      return dynamic_cast<absyn::SimpleVar*>(really_a_typename)->getSymbol();
    }
}