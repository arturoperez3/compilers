#ifndef ABSYN_H
#define ABSYN_H

#include "symbol.h"
#include "util.h"

using namespace std;


namespace absyn
{
    /*****************************************************************/
    /*****************************************************************/
    /*                       Definition                              */
    /*****************************************************************/
    /*****************************************************************/
    //abstract base class, the ancestor of all type of tree nodes
    class Absyn {
    private:
        int		lineno;
    public:
        //Constructor
        Absyn(int line);

        int getPos( void ) const;

        virtual void print(FILE* out, int d) = 0;

        //pure virtual destructor
        //its only purpose is to make the class abstract.
        virtual ~Absyn() = 0;
    };

    //abstract base class, the ancestor of all type of expression nodes
    class Exp : public Absyn {
    public:
        Exp(int line);

        virtual void print(FILE* out, int d) = 0;

        //pure virtual destructor
        //its only purpose is to make the class abstract.
        virtual ~Exp()=0;
    };

    //abstract base class, the ancestor of all type of type nodes
    class Ty : public  Absyn {
    public:
        Ty(int line);

        virtual void print(FILE* out, int d) = 0;

        //pure virtual destructor
        //its only purpose is to make the class abstract.
        virtual ~Ty()=0;
    };

    //abstract base class, the ancestor of all type of variable nodes
    class Var : public  Absyn {
    public:
        Var(int line);

        virtual void print(FILE* out, int d) = 0;

        //pure virtual destructor
        //its only purpose is to make the class abstract.
        virtual ~Var()=0;
    };

    //abstract base class, the ancestor of all type of declaration nodes
    class Dec : public  Absyn {
    public:
        Dec(int line);

        virtual void print(FILE* out, int d) = 0;

        //pure virtual destructor
        //its only purpose is to make the class abstract.
        virtual ~Dec()=0;
    };

    class ArrayExp : public Exp {
    private:
        S_symbol		type;		//type of the array
        Exp			*size;		//size of the array
        Exp			*init;		//initial value of the array
    public:
        ArrayExp(int line, S_symbol t, Exp *s, Exp *i);
        void print(FILE* out, int d);
        S_symbol getType( void ) const;
        Exp * getSize( void ) const;
        Exp * getInit( void ) const;
        ~ArrayExp();
    };

    class ArrayTy : public Ty {
    private:
        S_symbol		array;
    public:
        ArrayTy(int line, S_symbol a);
        void print(FILE* out, int d);
        S_symbol getArray( void ) const;
        ~ArrayTy();
    };

    class AssignExp : public Exp {
    private:
        Var		*var;
        Exp		*exp;
    public:
        AssignExp(int line, Var *v, Exp *e);
        void print(FILE* out, int d);
        Var* getVar( void ) const;
        Exp* getExp( void ) const;
        ~AssignExp();
    };

    class BreakExp : public Exp {
    public:
        BreakExp(int line);
        void print(FILE* out, int d);
        ~BreakExp();
    };

    class DecList {
    private:
        Dec			*head;
        DecList		*rest;
    public:
        DecList(Dec *h, DecList *r);
        void print(FILE* out, int d);
        Dec* getHead( void ) const;
        DecList* getRest( void ) const;
        ~DecList();
    };

    class ExpList {
    private:
        Exp			*head;
        ExpList		*rest;
    public:
        ExpList(Exp *h, ExpList *r);
        void print(FILE* out, int d);
        Exp* getHead( void ) const;
        ExpList* getRest( void ) const;
        ~ExpList();
    };

    class Field : public Absyn {
    private:
        S_symbol		name;
        S_symbol		type;
        bool				escape;
    public:
        Field(int line, S_symbol n, S_symbol t);
        void print(FILE* out, int d);
        S_symbol getName( void ) const;
        S_symbol getType( void ) const;
        bool getEscape( void ) const;
        ~Field();
    };

    class FieldList {
    private:
        Field			*head;
        FieldList		*rest;
    public:
        FieldList(Field *h, FieldList *r);
        void print(FILE* out, int d);
        Field* getHead( void ) const;
        FieldList* getRest( void ) const;
        ~FieldList();
    };

    class FunDec : public Absyn {
    private:
        S_symbol			name;
        FieldList*		params;
        S_symbol			result;
        Exp*					body;
    public:
        FunDec(int line, S_symbol n, FieldList* p, S_symbol r, Exp* b);
        void print(FILE* out, int d);
        S_symbol getName( void ) const;
        FieldList* getParams( void ) const;
        S_symbol getResult( void ) const;
        Exp* getBody( void ) const;
        ~FunDec();
    };

    class FunDecList {
    private:
        FunDec				*head;
        FunDecList		*rest;
    public:
        FunDecList(FunDec *h, FunDecList *r);
        void print(FILE* out, int d);
        FunDec* getHead( void ) const;
        FunDecList* getRest( void ) const;
        ~FunDecList();
    };

    class Namety {
    private:
        S_symbol		name;
        Ty*					ty;
    public:
        Namety(S_symbol n, Ty* t);
        void print(FILE* out, int d);
        S_symbol getName( void ) const;
        Ty* getType ( void ) const;
        ~Namety();
    };

    class NametyList {
    private:
        Namety				*head;
        NametyList		*rest;
    public:
        NametyList(Namety *h, NametyList *r);
        void print(FILE* out, int d);
        Namety* getHead( void ) const;
        NametyList* getRest( void ) const;
        ~NametyList();
    };

    class IfExp : public Exp {
    private:
        S_symbol 		var;
        Exp					*test;
        Exp					*thenclause;
        Exp					*elseclause; /* optional */
    public:
        IfExp(int line, Exp *x, Exp *y);
        IfExp(int line, Exp *x, Exp *y, Exp *z);
        void print(FILE* out, int d);
        Exp* getTest( void ) const;
        Exp* getThen( void ) const;
        Exp* getElse( void ) const;
        ~IfExp();
    };

    class IntExp : public Exp {
    private:
        int		intt;
    public:
        IntExp(int line, int i);
        void print(FILE* out, int d);
        int getIntt( void ) const;
        ~IntExp();
    };

    class LetExp : public Exp {
    private:
        DecList			*decs;
        Exp				*body;
    public:
        LetExp(int line, DecList *d, Exp *b);
        void print(FILE* out, int d);
        DecList* getDecs( void ) const;
        Exp* getBody( void ) const;
        ~LetExp();
    };

    class NameTy : public Ty {
    private:
        S_symbol		name;
    public:
        NameTy(int line, S_symbol n);
        void print(FILE* out, int d);
        S_symbol getName( void ) const;
        ~NameTy();
    };

    class RecordTy : public Ty {
    private:
        FieldList*		record;
    public:
        RecordTy(int line, FieldList* r);
        void print(FILE* out, int d);
        FieldList* getRecord( void ) const;
        ~RecordTy();
    };

    class NilExp : public Exp {
    public:
        NilExp(int line);
        void print(FILE* out, int d);
        ~NilExp();
    };

    class OpExp : public Exp {
    public:
        enum OpType {EXP_PLUS=0, EXP_MINUS=1, EXP_MUL=2, EXP_DIV=3, EXP_EQ=4, EXP_NE=5, EXP_LT=6, EXP_LE=7, EXP_GT=8, EXP_GE=9};
    private:
        Exp			*left, *right;
        OpType		oper;
    public:
        OpExp(int line, OpType o, Exp *l, Exp *r);
        void print(FILE* out, int d);
        Exp* getLeft( void ) const;
        Exp* getRight( void ) const;
        OpType getOper( void ) const;
        const char* getOperStr( void ) const;
        ~OpExp();
    };


    class SeqExp : public Exp {
    private:
        ExpList			*seq;
    public:
        SeqExp(int line, ExpList *s);
        void print(FILE* out, int d);
        ExpList* getSeq( void ) const;
        ~SeqExp();
    };

    class SimpleVar : public Var {
    private:
        S_symbol		sym;
    public:
        SimpleVar (int line, S_symbol s);
        void print(FILE* out, int d);
        S_symbol getSymbol( void ) const;
        ~SimpleVar();
    };

    class FieldVar : public Var {
    private:
        Var*				var;
        S_symbol		sym;
    public:
        FieldVar (int line, Var* v, S_symbol s);
        void print(FILE* out, int d);
        Var* getVar( void ) const;
        S_symbol getSymbol( void ) const;
        ~FieldVar();
    };

    class StringExp : public Exp {
    private:
        appel_string		stringg;
    public:
        StringExp(int line, appel_string a);
        void print(FILE* out, int d);
        appel_string getStringg( void ) const;
        ~StringExp();
    };

    class SubscriptVar : public Var {
    private:
        Var			*var;
        Exp			*exp;
    public:
        SubscriptVar(int line, Var *v, Exp *e);
        void print(FILE* out, int d);
        Var* getVar( void ) const;
        Exp* getExp( void ) const;
        ~SubscriptVar();
    };

    class FunctionDec : public Dec {
    private:
        FunDecList*			function;
    public:
        FunctionDec(int line, FunDecList* f);
        void print(FILE* out, int d);
        FunDecList* getFunction( void ) const;
        ~FunctionDec();
    };

    class TypeDec : public Dec {
    private:
        NametyList			*type;
    public:
        TypeDec(int line, NametyList *t);
        void print(FILE* out, int d);
        NametyList* getType( void ) const;
        ~TypeDec();
    };

    class VarDec : public Dec {
    private:
        S_symbol		var;
        S_symbol		type;
        Exp					*init;
        bool 				escape;
    public:
        VarDec(int line, S_symbol v, S_symbol t, Exp *i);
        void print(FILE* out, int d);
        S_symbol getVar( void ) const;
        S_symbol getType( void ) const;
        Exp* getInit( void ) const;
        bool getEscape( void ) const;
        ~VarDec();
    };

    class VarExp : public Exp {
    private:
        Var			*var;
    public:
        VarExp(int line, Var *v);
        void print(FILE* out, int d);
        Var* getVar( void ) const;
        ~VarExp();
    };

    class WhileExp : public Exp {
    private:
        Exp			*test, *body;
    public:
        WhileExp(int line, Exp *t, Exp *b);
        void print(FILE* out, int d);
        Exp* getTest( void ) const;
        Exp* getBody( void ) const;
        ~WhileExp();
    };

    class CallExp : public Exp {
    private:
        S_symbol		func;
        ExpList		*args;
    public:
        CallExp(int line, S_symbol f, ExpList *a);
        void print(FILE* out, int d);
        S_symbol getFunc( void ) const;
        ExpList* getArgs( void ) const;
        ~CallExp();
    };

    class ForExp : public Exp {
    private:
        S_symbol		var;
        Exp					*lo, *hi, *body;
        bool				escape;
    public:
        ForExp(int line, S_symbol v, Exp* l, Exp *h, Exp *b);
        void print(FILE* out, int d);
        S_symbol getVar( void ) const;
        Exp* getLo( void ) const;
        Exp* getHi( void ) const;
        Exp* getBody( void ) const;
        bool getEscape( void ) const;
        ~ForExp();
    };

    class EField {
    private:
        S_symbol		name;
        Exp*				exp;
    public:
        EField(S_symbol n, Exp* e);
        void print(FILE* out, int d);
        S_symbol getName( void ) const;
        Exp* getExp( void ) const;
        ~EField();
    };

    class EFieldList {
    private:
        EField*				head;
        EFieldList*		rest;
    public:
        EFieldList(EField* h, EFieldList* r);
        void print(FILE* out, int d);
        EField* getHead( void ) const;
        EFieldList* getRest( void ) const;
        ~EFieldList();
    };

    class RecordExp : public Exp {
    private:
        S_symbol			type;
        EFieldList*		fields;
    public:
        RecordExp(int line, S_symbol t, EFieldList* f);
        void print(FILE* out, int d);
        S_symbol getType( void ) const;
        EFieldList* getFields( void ) const;
        ~RecordExp();
    };


}	//end of namespace Absyn

#endif
