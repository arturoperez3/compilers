#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include <set>
#include "util.h"
#include "typecheck.h"
#include "errormsg.h"
#include "absyn.h"

using namespace std;

static bool inLoop = false;

/***********************************************************
******************* HELPERS ********************************
***********************************************************/

// indent for printing the AST
static void indent(FILE *out, int d) {
    int i;
    for (i = 0; i <= d; i++) fprintf(out, " ");
}


// chase name types to find real type.
Ty_ty actualTy(Ty_ty type1) {

    if (type1 == NULL) return NULL;

    if (type1->kind == Ty_name) {
        return actualTy(type1->u.name.ty);
    } else {
        return type1;
    }
}


// test two types to determine if they are compatible.
// this function is called from MANY places and makes life a bit easier.
bool matchTypes(Ty_ty type1, Ty_ty type2) {
    Ty_ty atype1, atype2;

    atype1 = actualTy(type1);
    atype2 = actualTy(type2);

    if (atype1 == atype2) {
        return true;
    } else if (atype1->kind == Ty_record && atype2 == Ty_Nil() ||
               atype2->kind == Ty_record && atype1 == Ty_Nil()) {
        return true;
    } else {
        return false;
    }
}

// Add your own helper functions here. You will find places where
// helper functions are very useful.


/***********************************************************
******************* END HELPERS ****************************
***********************************************************/

//=============================================================
// Absyn
absyn::Absyn::Absyn(int line) : lineno(line) {}

int absyn::Absyn::getPos(void) const { return lineno; }

absyn::Absyn::~Absyn() {}

//=============================================================
// Exp
absyn::Exp::Exp(int line) : absyn::Absyn(line) {}

absyn::Exp::~Exp() {}

//=============================================================
// Ty
absyn::Ty::Ty(int line) : absyn::Absyn(line) {}

absyn::Ty::~Ty() {}

//=============================================================
// Var
absyn::Var::Var(int line) : absyn::Absyn(line) {}

absyn::Var::~Var() {}

//=============================================================
// Dec
absyn::Dec::Dec(int line) : absyn::Absyn(line) {}

absyn::Dec::~Dec() {}

//=============================================================
// ArrayExp
absyn::ArrayExp::ArrayExp(int line, S_symbol t, absyn::Exp *s, absyn::Exp *i)
        : absyn::Exp(line), type(t), size(s), init(i) {}

S_symbol absyn::ArrayExp::getType(void) const { return type; }

absyn::Exp *absyn::ArrayExp::getSize(void) const { return size; }

absyn::Exp *absyn::ArrayExp::getInit(void) const { return init; }

absyn::ArrayExp::~ArrayExp() {
    delete size;
    delete init;
}

Ty_ty absyn::ArrayExp::typeCheck(v_tbl venv, t_tbl tenv) const {
    S_symbol s = this->getType();
    Ty_ty type = T_tbl_look(tenv, s);
    Ty_ty size = this->getSize()->typeCheck(venv, tenv);
    Ty_ty init = this->getInit()->typeCheck(venv, tenv);

    if (type == nullptr) {
        EM_error(this->getPos(), "Undeclared type: %s.", S_name(s));
        return Ty_Error();
    }

    type = actualTy(type);

    if (type->kind != Ty_array) {
        EM_error(this->getPos(), "%s is not array type.", S_name(s));
        return Ty_Error();
    } 

    if (!matchTypes(size, Ty_Int())) {
        EM_error(this->getPos(), "Array size specifier must be of type int.");
        return Ty_Error();
    } 

    init = actualTy(init);
    if (!matchTypes(type->u.array, init)) {
        EM_error(this->getPos(), "Array initialization expression is not of correct type.");
        return Ty_Error();
    }

    return type;
}

void absyn::ArrayExp::print(FILE *out, int d) {
    indent(out, d);
    fprintf(out, "arrayExp(%s,\n", S_name(type));
    size->print(out, d + 1);
    fprintf(out, ",\n");
    init->print(out, d + 1);
    fprintf(out, ")");
}

//=============================================================
// ArrayTy
absyn::ArrayTy::ArrayTy(int line, S_symbol a) : absyn::Ty(line), array(a) {}

S_symbol absyn::ArrayTy::getArray(void) const { return array; }

absyn::ArrayTy::~ArrayTy() {}

Ty_ty absyn::ArrayTy::typeCheck(t_tbl tenv) const {
    Ty_ty t = T_tbl_look(tenv, this->getArray());

    if (t == nullptr) {
        EM_error(this->getPos(), "Array type does not exist");
        return Ty_Error();
    }
    return Ty_Array(t);
}

void absyn::ArrayTy::print(FILE *out, int d) {
    indent(out, d);
    fprintf(out, "arrayTy(%s)", S_name(array));
}

//=============================================================
// AssignExp
absyn::AssignExp::AssignExp(int line, absyn::Var *v, absyn::Exp *e) : absyn::Exp(line),
                                                                      var(v), exp(e) {}

absyn::Var *absyn::AssignExp::getVar(void) const { return var; }

absyn::Exp *absyn::AssignExp::getExp(void) const { return exp; }

absyn::AssignExp::~AssignExp() {
    delete var;
    delete exp;
}

Ty_ty absyn::AssignExp::typeCheck(v_tbl venv, t_tbl tenv) const {
    // check LHS and RHS are same type
    if (!matchTypes(this->getVar()->typeCheck(venv, tenv), this->getExp()->typeCheck(venv, tenv))) {
        EM_error(this->getPos(), "Assignment statement type mismatch.");
        return Ty_Error();
    }

    SimpleVar* isInduc = dynamic_cast<SimpleVar*>(this->getVar());

    // check that we are not assigning to a for loop induction variable
    if (isInduc != nullptr) {
        E_enventry v = V_tbl_look(venv, isInduc->getSymbol());
        if (v->u.var.isFLIV == true) {
            EM_error(this->getPos(), "Cannot assign to for loop induction variable %s", S_name(isInduc->getSymbol()));
            return Ty_Error();
        }
    }

    return Ty_Void();
}

void absyn::AssignExp::print(FILE *out, int d) {
    indent(out, d);
    fprintf(out, "assignExp(\n");
    var->print(out, d + 1);
    fprintf(out, ",\n");
    exp->print(out, d + 1);
    fprintf(out, ")");
}

//=============================================================
// BreakExp
absyn::BreakExp::BreakExp(int line) : Exp(line) {}

absyn::BreakExp::~BreakExp() {}

Ty_ty absyn::BreakExp::typeCheck(v_tbl venv, t_tbl tenv) const {
    // break is only valid when we are inside of a loop construct
    if (inLoop) {
        return Ty_Void();
    } else {
        EM_error(this->getPos(), "Break not inside a for or while loop.");
        return Ty_Error();
    }
}

void absyn::BreakExp::print(FILE *out, int d) {
    indent(out, d);
    fprintf(out, "breakExp()");
}

//=============================================================
// DecList
absyn::DecList::DecList(absyn::Dec *h, absyn::DecList *r) : head(h), rest(r) {}

absyn::Dec *absyn::DecList::getHead(void) const { return head; }

absyn::DecList *absyn::DecList::getRest(void) const { return rest; }

absyn::DecList::~DecList() {
    delete head;
    delete rest;
}

void absyn::DecList::print(FILE *out, int d) {
    indent(out, d);
    if (getHead()) {
        fprintf(out, "decList(\n");
        head->print(out, d + 1);
        fprintf(out, ",\n");
        if (rest) {
            rest->print(out, d + 1);
        } else {
            indent(out, d + 1);
            fprintf(out, "decList()");
        }
        fprintf(out, ")");
    } else fprintf(out, "decList()");
}

//=============================================================
// ExpList
absyn::ExpList::ExpList(absyn::Exp *h, absyn::ExpList *r) : head(h), rest(r) {}

absyn::Exp *absyn::ExpList::getHead(void) const { return head; }

absyn::ExpList *absyn::ExpList::getRest(void) const { return rest; }

absyn::ExpList::~ExpList() {
    delete head;
    delete rest;
}

void absyn::ExpList::print(FILE *out, int d) {
    indent(out, d);
    if (getHead()) {
        fprintf(out, "expList(\n");
        head->print(out, d + 1);
        fprintf(out, ",\n");
        if (rest) {
            rest->print(out, d + 1);
        } else {
            indent(out, d + 1);
            fprintf(out, "expList()");
        }
        fprintf(out, ")");
    } else fprintf(out, "expList()");
}

//=============================================================
// Field
absyn::Field::Field(int line, S_symbol n, S_symbol t) : absyn::Absyn(line), name(n),
                                                        type(t), escape(TRUE) {}

S_symbol absyn::Field::getName(void) const { return name; }

S_symbol absyn::Field::getType(void) const { return type; }

bool absyn::Field::getEscape(void) const { return escape; }

absyn::Field::~Field() {}

void absyn::Field::print(FILE *out, int d) {
    indent(out, d);
    fprintf(out, "field(%s,\n", S_name(name));
    indent(out, d + 1);
    fprintf(out, "%s,\n", S_name(type));
    indent(out, d + 1);
    fprintf(out, "%s", escape ? "TRUE)" : "FALSE)");
}

//=============================================================
// FieldList
absyn::FieldList::FieldList(absyn::Field *h, absyn::FieldList *r) : head(h), rest(r) {}

absyn::Field *absyn::FieldList::getHead(void) const { return head; }

absyn::FieldList *absyn::FieldList::getRest(void) const { return rest; }

absyn::FieldList::~FieldList() {
    delete head;
    delete rest;
}

void absyn::FieldList::print(FILE *out, int d) {
    indent(out, d);
    if (getHead()) {
        fprintf(out, "fieldList(\n");
        head->print(out, d + 1);
        fprintf(out, ",\n");
        if (rest) {
            rest->print(out, d + 1);
        } else {
            indent(out, d + 1);
            fprintf(out, "fieldList()");
        }
        fprintf(out, ")");
    } else fprintf(out, "fieldList()");
}

//=============================================================
// FunDec
absyn::FunDec::FunDec(int line, S_symbol n, absyn::FieldList *p, S_symbol r,
                      absyn::Exp *b) : absyn::Absyn(line), name(n), params(p), result(r),
                                       body(b) {}

S_symbol absyn::FunDec::getName(void) const { return name; }

absyn::FieldList *absyn::FunDec::getParams(void) const { return params; }

S_symbol absyn::FunDec::getResult(void) const { return result; }

absyn::Exp *absyn::FunDec::getBody(void) const { return body; }

absyn::FunDec::~FunDec() {
    delete params;
    delete body;
}

void absyn::FunDec::print(FILE *out, int d) {
    indent(out, d);
    fprintf(out, "fundec(%s,\n", S_name(name));
    if (params) {
        params->print(out, d + 1);
    } else {
        indent(out, d + 1);
        fprintf(out, "fieldList()");
    }
    fprintf(out, ",\n");

    if (result) {
        indent(out, d + 1);
        fprintf(out, "%s,\n", S_name(result));
    }
    body->print(out, d + 1);
    fprintf(out, ")");
}

//=============================================================
// FunDecList
absyn::FunDecList::FunDecList(absyn::FunDec *h, absyn::FunDecList *r) : head(h),
                                                                        rest(r) {}

absyn::FunDec *absyn::FunDecList::getHead(void) const { return head; }

absyn::FunDecList *absyn::FunDecList::getRest(void) const { return rest; }

absyn::FunDecList::~FunDecList() {
    delete head;
    delete rest;
}

void absyn::FunDecList::print(FILE *out, int d) {
    indent(out, d);
    if (getHead()) {
        fprintf(out, "fundecList(\n");
        head->print(out, d + 1);
        fprintf(out, ",\n");
        if (rest) {
            rest->print(out, d + 1);
        } else {
            indent(out, d + 1);
            fprintf(out, "fundecList()");
        }
        fprintf(out, ")");
    } else fprintf(out, "fundecList()");
}

//=============================================================
// Namety
absyn::Namety::Namety(S_symbol n, absyn::Ty *t) : name(n), ty(t) {}

S_symbol absyn::Namety::getName(void) const { return name; }

absyn::Ty *absyn::Namety::getType(void) const { return ty; }

absyn::Namety::~Namety() { delete ty; }

void absyn::Namety::print(FILE *out, int d) {
    indent(out, d);
    fprintf(out, "namety(%s,\n", S_name(name));
    ty->print(out, d + 1);
    fprintf(out, ")");
}

//=============================================================
// NametyList
absyn::NametyList::NametyList(absyn::Namety *h, absyn::NametyList *r) : head(h),
                                                                        rest(r) {}

absyn::Namety *absyn::NametyList::getHead(void) const { return head; }

absyn::NametyList *absyn::NametyList::getRest(void) const { return rest; }

absyn::NametyList::~NametyList() {
    delete head;
    delete rest;
}

void absyn::NametyList::print(FILE *out, int d) {
    indent(out, d);
    if (getHead()) {
        fprintf(out, "nametyList(\n");
        head->print(out, d + 1);
        fprintf(out, ",\n");
        if (rest) {
            rest->print(out, d + 1);
        } else {
            indent(out, d + 1);
            fprintf(out, "nametyList()");
        }
        fprintf(out, ")");
    } else fprintf(out, "nametyList()");
}

//=============================================================
// IfExp
absyn::IfExp::IfExp(int line, absyn::Exp *x, absyn::Exp *y) : absyn::Exp(line), test(x),
                                                              thenclause(y),
                                                              elseclause(NULL) {}

absyn::IfExp::IfExp(int line, absyn::Exp *x, absyn::Exp *y, absyn::Exp *z) : absyn::Exp(
        line), test(x), thenclause(y), elseclause(z) {}

absyn::Exp *absyn::IfExp::getTest(void) const { return test; }

absyn::Exp *absyn::IfExp::getThen(void) const { return thenclause; }

absyn::Exp *absyn::IfExp::getElse(void) const { return elseclause; }

absyn::IfExp::~IfExp() {
    delete test;
    delete thenclause;
    delete elseclause;
}

Ty_ty absyn::IfExp::typeCheck(v_tbl venv, t_tbl tenv) const {

    // check Test clause is Ty_Int()
    if (!matchTypes(this->getTest()->typeCheck(venv, tenv), Ty_Int())) {
        EM_error(this->getPos(), "Test clause needs to be of type int.");
        return Ty_Error();
    }

    // if there is an Else clause, then Then and Else clauses need to be of the same type
    if (this->getElse() != nullptr) {
        if (!matchTypes(this->getThen()->typeCheck(venv, tenv), this->getElse()->typeCheck(venv, tenv))) {
            EM_error(this->getPos(), "Then and Else clauses need to be of the same type.");
            return Ty_Error();
        }
        return this->getThen()->typeCheck(venv, tenv);
    }

    // if no Else clause, then Then clause must produce no value
    if (!matchTypes(this->getThen()->typeCheck(venv, tenv), Ty_Void())) {
        EM_error(this->getPos(), "Then clause must be type void.");
    }
    return Ty_Void();
}

void absyn::IfExp::print(FILE *out, int d) {
    indent(out, d);
    fprintf(out, "iffExp(\n");
    test->print(out, d + 1);
    fprintf(out, ",\n");
    thenclause->print(out, d + 1);
    if (elseclause) { /* else is optional */
        fprintf(out, ",\n");
        elseclause->print(out, d + 1);
    }
    fprintf(out, ")");
}

//=============================================================
// IntExp
absyn::IntExp::IntExp(int line, int i) : absyn::Exp(line), intt(i) {}

int absyn::IntExp::getIntt(void) const { return intt; }

absyn::IntExp::~IntExp() {}

Ty_ty absyn::IntExp::typeCheck(v_tbl venv, t_tbl tenv) const {
    return Ty_Int();
}

void absyn::IntExp::print(FILE *out, int d) {
    indent(out, d);
    fprintf(out, "intExp(%d)", intt);
}

//=============================================================
// LetExp
absyn::LetExp::LetExp(int line, absyn::DecList *d, absyn::Exp *b) : absyn::Exp(line),
                                                                    decs(d), body(b) {}

absyn::DecList *absyn::LetExp::getDecs(void) const { return decs; }

absyn::Exp *absyn::LetExp::getBody(void) const { return body; }

absyn::LetExp::~LetExp() {
    delete decs;
    delete body;
}

Ty_ty absyn::LetExp::typeCheck(v_tbl venv, t_tbl tenv) const {
    Env_beginScope(venv, tenv);
    for (absyn::DecList* d = this -> getDecs(); d != nullptr; d = d->getRest()){ 
        d -> getHead() -> typeCheck(venv, tenv);
    }
    Ty_ty t = (this -> getBody()) -> typeCheck(venv, tenv);
    Env_endScope(venv, tenv);
    return t;
}

void absyn::LetExp::print(FILE *out, int d) {
    indent(out, d);
    fprintf(out, "letExp(\n");
    if (decs) {
        decs->print(out, d + 1);
        fprintf(out, ",\n");
    } else {
        indent(out, d + 1);
        fprintf(out, "decList()\n");
    }
    body->print(out, d + 1);
    fprintf(out, ")");
}

//=============================================================
// NameTy
absyn::NameTy::NameTy(int line, S_symbol n) : absyn::Ty(line), name(n) {}

S_symbol absyn::NameTy::getName(void) const { return name; }

absyn::NameTy::~NameTy() {}

Ty_ty absyn::NameTy::typeCheck(t_tbl tenv) const {
    return Ty_Name(this->getName(), T_tbl_look(tenv, this->getName()));
}

void absyn::NameTy::print(FILE *out, int d) {
    indent(out, d);
    fprintf(out, "nameTy(%s)", S_name(name));
}

//=============================================================
// RecordTy
absyn::RecordTy::RecordTy(int line, absyn::FieldList *r) : absyn::Ty(line), record(r) {}

absyn::FieldList *absyn::RecordTy::getRecord(void) const { return record; }

absyn::RecordTy::~RecordTy() { delete record; }

Ty_ty absyn::RecordTy::typeCheck(t_tbl tenv) const {

    Field* rec;
    Ty_fieldList rectylist = nullptr;
    Ty_ty ty; 
    FieldList* reclist = this->getRecord();

    // handle empty record 
    if (reclist == nullptr) {
        EM_error(this->getPos(), "WARNING: empty record.");
        return Ty_Error();
    }

    std::set<appel_string> mySet;
    std::set<appel_string>::iterator it;
    for (reclist; reclist != nullptr && ty != Ty_Error(); reclist = reclist->getRest()) {
        rec = reclist->getHead();

        // check types inside record are valid
        ty = T_tbl_look(tenv, rec->getType());
        if (ty != nullptr) {
            rectylist = Ty_FieldList(Ty_Field(rec->getName(), ty), rectylist);
        } else {
            EM_error(this->getPos(), "Unknown record type in declaration: %s", S_name(rec->getType()));
            return Ty_Error();
        }

        // check that parameters within record are unique
        for (it = mySet.begin(); it != mySet.end(); ++it) {
            if (*it == S_name(rec->getName())) {
                EM_error(this->getPos(), "Duplicate field name (%s) in record declaration.", S_name(rec->getName()));
                return Ty_Error();
            }
        }
        mySet.insert(it, S_name(rec->getName()));
    }
    return Ty_Record(rectylist);
}

void absyn::RecordTy::print(FILE *out, int d) {
    indent(out, d);
    fprintf(out, "recordTy(\n");
    if (record) {
        record->print(out, d + 1);
    } else {
        indent(out, d + 1);
        fprintf(out, "fieldList()");
    }
    fprintf(out, ")");
}

//=============================================================
// NilExp
absyn::NilExp::NilExp(int line) : absyn::Exp(line) {}

absyn::NilExp::~NilExp() {}

Ty_ty absyn::NilExp::typeCheck(v_tbl venv, t_tbl tenv) const {
    return Ty_Nil();
}

void absyn::NilExp::print(FILE *out, int d) {
    indent(out, d);
    fprintf(out, "nilExp()");
}

//=============================================================
// OpExp
static const char *str_oper[12] = {"PLUS", "MINUS", "TIMES", "DIVIDE", "EQUAL",
                                   "NOTEQUAL", "LESSTHAN", "LESSEQ", "GREAT", "GREATEQ"};

absyn::OpExp::OpExp(int line, absyn::OpExp::OpType o, absyn::Exp *l, absyn::Exp *r)
        : absyn::Exp(line), oper(o), left(l), right(r) {}

absyn::Exp *absyn::OpExp::getLeft(void) const { return left; }

absyn::Exp *absyn::OpExp::getRight(void) const { return right; }

absyn::OpExp::OpType absyn::OpExp::getOper(void) const { return oper; }

const char *absyn::OpExp::getOperStr(void) const { return str_oper[oper]; }

absyn::OpExp::~OpExp() {
    delete left;
    delete right;
}

Ty_ty absyn::OpExp::typeCheck(v_tbl venv, t_tbl tenv) const {

    // check operands that require Ty_Int() on both sides 
    absyn::OpExp::OpType oper = this->getOper();
    if (oper == absyn::OpExp::EXP_PLUS || oper == absyn::OpExp::EXP_MINUS
    || oper == absyn::OpExp::EXP_MUL || oper == absyn::OpExp::EXP_DIV) {
         if (!matchTypes(this->getLeft()->typeCheck(venv, tenv), Ty_Int())) {
             EM_error(this->getPos(), "Cannot use arithmetic operator where left operand "
             "is not of type int.");
             return Ty_Error();
         } if (!matchTypes(this->getRight()->typeCheck(venv, tenv), Ty_Int())) {
             EM_error(this->getPos(), "Cannot use arithmetic operator where right operand "
             "is not of type int.");
             return Ty_Error();
         }
    } 
    // check operands that require Ty_Int() or Ty_String() on both sides 
    else if (oper == absyn::OpExp::EXP_GT || oper == absyn::OpExp::EXP_GE
    || oper == absyn::OpExp::EXP_LT || oper == absyn::OpExp::EXP_LE) {
        if (!matchTypes(this->getLeft()->typeCheck(venv, tenv), Ty_Int()) && 
            !matchTypes(this->getLeft()->typeCheck(venv, tenv), Ty_String())) {
             EM_error(this->getPos(), "Cannot use relational operator where left operand is not "
             "of type int or string.");
             return Ty_Error();
        } if (!matchTypes(this->getRight()->typeCheck(venv, tenv), Ty_Int()) && 
            !matchTypes(this->getRight()->typeCheck(venv, tenv), Ty_String())) {
             EM_error(this->getPos(), "Cannot use relational operator where right operand is not "
             "of type int or string.");
             return Ty_Error();
        } if (!matchTypes(this->getLeft()->typeCheck(venv, tenv), this->getRight()->typeCheck(venv, tenv))) {
            EM_error(this->getPos(), "Cannot use comparison operator on operands of different types.");
            return Ty_Error();
        }
        
    }
    // check operands that require only the same type on both sides
    else {
        if (!matchTypes(this->getLeft()->typeCheck(venv, tenv), this->getRight()->typeCheck(venv, tenv))) {
            EM_error(this->getPos(), "Cannot use equality operator on operands of different types.");
            return Ty_Error();
        }
    } 
    return Ty_Int(); 
}

void absyn::OpExp::print(FILE *out, int d) {
    indent(out, d);
    fprintf(out, "opExp(\n");
    indent(out, d + 1);
    fprintf(out, "%s", getOperStr());
    fprintf(out, ",\n");
    left->print(out, d + 1);
    fprintf(out, ",\n");
    right->print(out, d + 1);
    fprintf(out, ")");
}

//=============================================================
// SeqExp
absyn::SeqExp::SeqExp(int line, absyn::ExpList *s) : absyn::Exp(line), seq(s) {}

absyn::ExpList *absyn::SeqExp::getSeq(void) const { return seq; }

absyn::SeqExp::~SeqExp() { delete seq; }

Ty_ty absyn::SeqExp::typeCheck(v_tbl venv, t_tbl tenv) const {

    ExpList* e;
    Ty_ty t = Ty_Void();
    for (e = this -> getSeq(); e != nullptr; e = e -> getRest()) { 
        t = e -> getHead() -> typeCheck(venv, tenv);
    }

    return t;
}

void absyn::SeqExp::print(FILE *out, int d) {
    indent(out, d);
    fprintf(out, "seqExp(\n");
    if (seq) {
        seq->print(out, d + 1);
    } else {
        indent(out, d + 1);
        fprintf(out, "expList()");
    }
    fprintf(out, ")");
}

//=============================================================
// SimpleVar
absyn::SimpleVar::SimpleVar(int line, S_symbol s) : absyn::Var(line), sym(s) {}

S_symbol absyn::SimpleVar::getSymbol(void) const { return sym; }

absyn::SimpleVar::~SimpleVar() {}

Ty_ty absyn::SimpleVar::typeCheck(v_tbl venv, t_tbl tenv) const {
    E_enventry v = V_tbl_look(venv, (this->getSymbol()));

    // check variable has been declared
    if (v == NULL) {
        EM_error(this->getPos(), "Undeclared variable: %s", S_name(this->getSymbol()));
        return Ty_Error();
    }

    // check variable has been declared a variable
    if (v->kind != E_varEntry) {
        EM_error(this->getPos(), "%s is not a variable.", S_name(this->getSymbol()));
        return Ty_Error();
    }

    Ty_ty t = v->u.var.ty;

    // check variable has a type
    if (t == nullptr) {
        EM_error(this->getPos(), "%s is of undeclared type", S_name(this->getSymbol()));
        return Ty_Error();
    }

    return t;

}

void absyn::SimpleVar::print(FILE *out, int d) {
    indent(out, d);
    fprintf(out, "simpleVar(%s)", S_name(sym));
}

//=============================================================
// FieldVar
absyn::FieldVar::FieldVar(int line, absyn::Var *v, S_symbol s) : absyn::Var(line), var(v),
                                                                 sym(s) {}

absyn::Var *absyn::FieldVar::getVar(void) const { return var; }

S_symbol absyn::FieldVar::getSymbol(void) const { return sym; }

absyn::FieldVar::~FieldVar() { delete var; }

Ty_ty absyn::FieldVar::typeCheck(v_tbl venv, t_tbl tenv) const {
    Ty_ty var = this->getVar()->typeCheck(venv, tenv);

    // check FieldVar essentials
    if (matchTypes(var, Ty_Error())) {
        return Ty_Error();
    } else if (var->kind != Ty_record) {
        EM_error(this->getPos(), "Attempt to access a non-record variable as a record.");
        return Ty_Error();
    } else {
        Ty_fieldList fields = var->u.record;
        while (fields != nullptr) {
            if (S_name(fields->head->name) == S_name(this->getSymbol())) {
                return fields->head->ty;
            }
            fields = fields->tail;
        }
        EM_error(this->getPos(), "Field not defined for given record type.");
        return Ty_Error();
    }
 }

void absyn::FieldVar::print(FILE *out, int d) {
    indent(out, d);
    fprintf(out, "%s\n", "fieldVar(");
    var->print(out, d + 1);
    fprintf(out, "%s\n", ",");
    indent(out, d + 1);
    fprintf(out, "%s)", S_name(sym));
}

//=============================================================
// StringExp
absyn::StringExp::StringExp(int line, appel_string a) : absyn::Exp(line), stringg(a) {}

appel_string absyn::StringExp::getStringg(void) const { return stringg; }

absyn::StringExp::~StringExp() {}

Ty_ty absyn::StringExp::typeCheck(v_tbl venv, t_tbl tenv) const {
    return Ty_String();
}

void absyn::StringExp::print(FILE *out, int d) {
    indent(out, d);
    fprintf(out, "stringExp(%s)", stringg);
}

//=============================================================
// SubscriptVar
absyn::SubscriptVar::SubscriptVar(int line, absyn::Var *v, absyn::Exp *e) : absyn::Var(
        line), var(v), exp(e) {}

absyn::Var *absyn::SubscriptVar::getVar(void) const { return var; }

absyn::Exp *absyn::SubscriptVar::getExp(void) const { return exp; }

absyn::SubscriptVar::~SubscriptVar() {
    delete var;
    delete exp;
}

Ty_ty absyn::SubscriptVar::typeCheck(v_tbl venv, t_tbl tenv) const {

    Ty_ty var = this->getVar()->typeCheck(venv, tenv);
    Ty_ty ex = this->getExp()->typeCheck(venv, tenv);

    // check SubscriptVar essentials
    if (var->kind != Ty_array) {
        EM_error(this->getPos(), "Variable being indexed into is not of array type.");
        return Ty_Error();
    }

    if (!matchTypes(ex, Ty_Int())) {
        EM_error(this->getPos(), "Indexing expression is not of type int.");
        return Ty_Error();
    }

    var = actualTy(var);

    return var->u.array;

}

void absyn::SubscriptVar::print(FILE *out, int d) {
    indent(out, d);
    fprintf(out, "%s\n", "subscriptVar(");
    var->print(out, d + 1);
    fprintf(out, "%s\n", ",");
    exp->print(out, d + 1);
    fprintf(out, "%s", ")");
}

//=============================================================
// FunctionDec
absyn::FunctionDec::FunctionDec(int line, absyn::FunDecList *f) : absyn::Dec(line),
                                                                  function(f) {}

absyn::FunDecList *absyn::FunctionDec::getFunction(void) const { return function; }

absyn::FunctionDec::~FunctionDec() { delete function; }

void absyn::FunctionDec::typeCheck(v_tbl venv, t_tbl tenv) const {
    FunDecList* fundeclist = this->getFunction();
    FunDec* fundec;

    for (fundeclist; fundeclist != nullptr; fundeclist = fundeclist->getRest()) {

        // start checking parameters of a function (params)
        fundec = fundeclist->getHead();
        FieldList* params = fundec->getParams();
        Ty_tyList typesList = nullptr; 
        std::set<appel_string> mySet;
        std::set<appel_string>::iterator it;
        Env_beginScope(venv, tenv);
        Ty_ty paramType;
        for (params; params != nullptr; params = params->getRest()) {
            paramType = T_tbl_look(tenv, params->getHead()->getType());
            S_symbol paramName = params->getHead()->getName();

            // check param is a valid type
            if (paramType == nullptr) {
                EM_error(this->getPos(), "Parameter %s is not a valid type.", S_name(params->getHead()->getType()));
                paramType = Ty_Error();
            }

            typesList = Ty_TyList(paramType, typesList);

            // check param is unique in function declaration
            for (it = mySet.begin(); it != mySet.end(); ++it) {
                if (*it == S_name(paramName)) {
                    EM_error(this->getPos(), "Duplicate field name (%s) in function declaration.", S_name(paramName));
                    paramType = Ty_Error();
                }
            }
            // since param is unique, add to set so we can compare to later params in function declaration
            mySet.insert(it, S_name(paramName)); 
            // add to venv so param available within the function's scope
            V_tbl_enter(venv, paramName, E_VarEntry(paramType));
        }

        // check if no return type given, then body must be of type void
        Ty_ty returnType;
        if (fundec->getResult() == nullptr) {
            returnType = Ty_Void();
            if (!matchTypes(fundec->getBody()->typeCheck(venv, tenv), Ty_Void())) {
                EM_error(this->getPos(), "The body must be of type void because %s has no return type.", 
                S_name(fundec->getName()));
                returnType = Ty_Error();
            }
            // else if return type is given, check that return type and the type of the body do match,
        } else {
            returnType = T_tbl_look(tenv, fundec->getResult());
            // is return type valid?
            if (returnType == nullptr) {
                EM_error(this->getPos(), "Undeclared return type for function %s", 
                S_name(fundec->getName()));
                returnType = Ty_Error();
            }
            else if (!matchTypes(returnType, fundec->getBody()->typeCheck(venv, tenv))) {
                EM_error(this->getPos(), "Type mismatch: return type and body type do not match for function %s.", 
                S_name(fundec->getName()));
                returnType = Ty_Error();
            }
        }
        Env_endScope(venv, tenv);

        // if we had an error in the function declaration
        if (matchTypes(paramType, Ty_Error()) || matchTypes(returnType, Ty_Error())) {
            V_tbl_enter(venv, fundec->getName(), E_FunEntry(typesList, Ty_Error()));
        } else {
            V_tbl_enter(venv, fundec->getName(), E_FunEntry(typesList, returnType));
        }
    }
}

void absyn::FunctionDec::print(FILE *out, int d) {
    indent(out, d);
    fprintf(out, "functionDec(\n");
    if (function) {
        function->print(out, d + 1);
    } else {
        indent(out, d + 1);
        fprintf(out, "fundecList()");
    }
    fprintf(out, ")");
}

//=============================================================
// TypeDec
absyn::TypeDec::TypeDec(int line, absyn::NametyList *t) : absyn::Dec(line), type(t) {}

absyn::NametyList *absyn::TypeDec::getType(void) const { return type; }

absyn::TypeDec::~TypeDec() { delete type; }

void absyn::TypeDec::typeCheck(v_tbl venv, t_tbl tenv) const {
    NametyList* t;

    // loop through the types 
    // check that they haven't already been defined 
    // enter into tenv
    for (t = this->getType(); t != nullptr; t = t->getRest()) { 
        if (T_tbl_look(tenv, t->getHead()->getName()) == nullptr) {
            T_tbl_enter(tenv, t->getHead()->getName(), t->getHead()->getType()->typeCheck(tenv));
        } else {
            EM_error(this->getPos(), "Duplicate type name (%s)", S_name(t->getHead()->getName()));
        }
    }
}

void absyn::TypeDec::print(FILE *out, int d) {
    indent(out, d);
    fprintf(out, "typeDec(\n");
    if (type) {
        type->print(out, d + 1);
    } else {
        indent(out, d + 1);
        fprintf(out, "nametyList()");
    }
    fprintf(out, ")");
}

//=============================================================
// VarDec
absyn::VarDec::VarDec(int line, S_symbol v, S_symbol t, absyn::Exp *i) : absyn::Dec(line),
                                                                         var(v), type(t),
                                                                         init(i),
                                                                         escape(TRUE) {}

S_symbol absyn::VarDec::getVar(void) const { return var; }

S_symbol absyn::VarDec::getType(void) const { return type; }

absyn::Exp *absyn::VarDec::getInit(void) const { return init; }

bool absyn::VarDec::getEscape(void) const { return escape; }

absyn::VarDec::~VarDec() { delete init; }

void absyn::VarDec::typeCheck(v_tbl venv, t_tbl tenv) const {
    Ty_ty init =  this->getInit()->typeCheck(venv, tenv);
    init = actualTy(init); 

    S_symbol t = this -> getType();

    // if variable was not given an intializing type, make sure RHS is not nil nor void
    if (t == nullptr ) {
        if (init->kind == Ty_nil) {
            EM_error(this->getPos(), "Variable initialization to NIL requires a type specifier.");
        } else if (init->kind == Ty_void) {
            EM_error(this->getPos(), "Variable initialization cannot be of type Void.");
        } else {
            V_tbl_enter(venv, this->getVar() , E_VarEntry(init));
        }
        return;
    } 

    // look up type of intializing type
    Ty_ty tt = T_tbl_look(tenv, t);

    // make sure it is a valid type
    if (tt == nullptr) {
        EM_error(this->getPos(), "Type %s has not been declared.", S_name(t));
        return;
    }

    // get the root type
    tt = actualTy(tt);

    // do some sanity checking
    if (matchTypes(init, Ty_Nil()) && tt->kind != Ty_record) {
        EM_error(this->getPos(), "Variable initialization to NIL requires a record type.");
        return;
    }
    if (matchTypes(init, Ty_Void())) {
        EM_error(this->getPos(), "Type of RHS cannot be void.");
        return;
    }

    if (!matchTypes(init, tt)) {
        EM_error(this->getPos(), "Type of LHS does not match type of RHS.");
        return;
    }

    // if we got here, we have a valid variable declaration, so we enter it into the venv
    V_tbl_enter(venv, this->getVar() , E_VarEntry(tt));
}

void absyn::VarDec::print(FILE *out, int d) {
    indent(out, d);
    fprintf(out, "varDec(%s,\n", S_name(var));
    if (type) {
        indent(out, d + 1);
        fprintf(out, "%s,\n", S_name(type));
    }
    init->print(out, d + 1);
    fprintf(out, ",\n");
    indent(out, d + 1);
    fprintf(out, "%s", escape ? "TRUE)" : "FALSE)");
}

//=============================================================
// VarExp
absyn::VarExp::VarExp(int line, absyn::Var *v) : absyn::Exp(line), var(v) {}

absyn::Var *absyn::VarExp::getVar(void) const { return var; }

absyn::VarExp::~VarExp() { delete var; }

Ty_ty absyn::VarExp::typeCheck(v_tbl venv, t_tbl tenv) const {
    return (this->getVar())->typeCheck(venv, tenv);
}

void absyn::VarExp::print(FILE *out, int d) {
    indent(out, d);
    fprintf(out, "varExp(\n");
    var->print(out, d + 1);
    fprintf(out, "%s", ")");
}

//=============================================================
// WhileExp
absyn::WhileExp::WhileExp(int line, absyn::Exp *t, absyn::Exp *b) : absyn::Exp(line),
                                                                    test(t), body(b) {}

absyn::Exp *absyn::WhileExp::getTest(void) const { return test; }

absyn::Exp *absyn::WhileExp::getBody(void) const { return body; }

absyn::WhileExp::~WhileExp() {
    delete test;
    delete body;
}

Ty_ty absyn::WhileExp::typeCheck(v_tbl venv, t_tbl tenv) const {

    inLoop = true;

    // check while loop essentials
    if (!matchTypes(this->getTest()->typeCheck(venv, tenv), Ty_Int())) {
        EM_error(this->getPos(), "Lo expression in for loop must be of type int.");
        return Ty_Error();
    }

    if (!matchTypes(this->getBody()->typeCheck(venv,tenv), Ty_Void())) {
        EM_error(this->getPos(), "Body of while loop must be of type void.");
        return Ty_Error();
    }

    inLoop = false;

    return Ty_Void();
}

void absyn::WhileExp::print(FILE *out, int d) {
    indent(out, d);
    fprintf(out, "whileExp(\n");
    test->print(out, d + 1);
    fprintf(out, ",\n");
    body->print(out, d + 1);
    fprintf(out, ")");
}

//=============================================================
// CallExp
absyn::CallExp::CallExp(int line, S_symbol f, absyn::ExpList *a) : absyn::Exp(line),
                                                                   func(f), args(a) {}

S_symbol absyn::CallExp::getFunc(void) const { return func; }

absyn::ExpList *absyn::CallExp::getArgs(void) const { return args; }

absyn::CallExp::~CallExp() { delete args; }

Ty_ty absyn::CallExp::typeCheck(v_tbl venv, t_tbl tenv) const {
    absyn::ExpList *argsptr = this->getArgs();
    Ty_tyList baseptr;
    Ty_ty giventype;

    // look up the function
    E_enventry funcInfo = V_tbl_look(venv, this->getFunc());

    if (funcInfo == NULL) {
        EM_error(this->getPos(), "Undeclared function: %s", S_name(this->getFunc()));
        return Ty_Error();
    }

    if (funcInfo->kind != E_funEntry) {
        EM_error(this->getPos(), "%s is not a function.", S_name(this->getFunc()));
        return Ty_Error();
    }

    // test all formal args against expected types
    baseptr = funcInfo->u.fun.formals;

    while (argsptr != NULL && baseptr != NULL) {
        giventype = argsptr->getHead()->typeCheck(venv, tenv);

        if (giventype == Ty_Error() || baseptr->head == Ty_Error())
            return Ty_Error();

        if (!matchTypes(baseptr->head, giventype)) {
            EM_error(argsptr->getHead()->getPos(),
                     "Argument does not match declared type");
            return Ty_Error();
        }

        baseptr = baseptr->tail;
        argsptr = argsptr->getRest();
    }

    if (argsptr != NULL || baseptr != NULL) {  //lists have different lengths
        EM_error(this->getPos(), "Wrong number of arguments in call to function %s",
                 S_name(this->getFunc()));
        return Ty_Error();
    }

    return funcInfo->u.fun.result;
}

void absyn::CallExp::print(FILE *out, int d) {
    indent(out, d);
    fprintf(out, "callExp(%s,\n", S_name(func));
    if (args) {
        args->print(out, d + 1);
    } else {
        indent(out, d + 1);
        fprintf(out, "expList()");
    }
    fprintf(out, ")");
}

//=============================================================
// ForExp
absyn::ForExp::ForExp(int line, S_symbol v, absyn::Exp *l, absyn::Exp *h, absyn::Exp *b)
        : absyn::Exp(line), var(v), lo(l), hi(h), body(b), escape(TRUE) {}

S_symbol absyn::ForExp::getVar(void) const { return var; }

absyn::Exp *absyn::ForExp::getLo(void) const { return lo; }

absyn::Exp *absyn::ForExp::getHi(void) const { return hi; }

absyn::Exp *absyn::ForExp::getBody(void) const { return body; }

bool absyn::ForExp::getEscape(void) const { return escape; }

absyn::ForExp::~ForExp() {
    delete lo;
    delete hi;
    delete body;
}

Ty_ty absyn::ForExp::typeCheck(v_tbl venv, t_tbl tenv) const {

    inLoop = true;

    // begin for loop scope
    Env_beginScope(venv, tenv);
    S_symbol var = this->getVar();
    E_enventry inducVar = E_VarEntry(Ty_Int());
    inducVar->u.var.isFLIV = true;
    V_tbl_enter(venv, var, inducVar);

    // check for loop essentials
    if (!matchTypes(this->getLo()->typeCheck(venv, tenv), Ty_Int())) {
        EM_error(this->getPos(), "Lower bound of FOR loop must be an integer expression.");
        return Ty_Error();
    }

    if (!matchTypes(this->getHi()->typeCheck(venv, tenv), Ty_Int())) {
        EM_error(this->getPos(), "Upper bound of FOR loop must be an integer expression.");
        return Ty_Error();
    }

    if (!matchTypes(this->getBody()->typeCheck(venv,tenv), Ty_Void())) {
        EM_error(this->getPos(), "Body of for loop must be of type void.");
        return Ty_Error();
    }

    // end for loop scope 
    Env_endScope(venv, tenv);

    inLoop = false;

    return Ty_Void();

}

void absyn::ForExp::print(FILE *out, int d) {
    indent(out, d);
    fprintf(out, "forExp(%s,\n", S_name(var));
    lo->print(out, d + 1);
    fprintf(out, ",\n");
    hi->print(out, d + 1);
    fprintf(out, "%s\n", ",");
    body->print(out, d + 1);
    fprintf(out, ",\n");
    indent(out, d + 1);
    fprintf(out, "%s", escape ? "TRUE)" : "FALSE)");
}

//=============================================================
// EField
absyn::EField::EField(S_symbol n, absyn::Exp *e) : name(n), exp(e) {}

S_symbol absyn::EField::getName(void) const { return name; }

absyn::Exp *absyn::EField::getExp(void) const { return exp; }

absyn::EField::~EField() { delete exp; }

void absyn::EField::print(FILE *out, int d) {
    indent(out, d);
    fprintf(out, "efield(%s,\n", S_name(name));
    exp->print(out, d + 1);
    fprintf(out, ")");
}

//=============================================================
// EFieldList
absyn::EFieldList::EFieldList(absyn::EField *h, absyn::EFieldList *r) : head(h),
                                                                        rest(r) {}

absyn::EField *absyn::EFieldList::getHead(void) const { return head; }

absyn::EFieldList *absyn::EFieldList::getRest(void) const { return rest; }

absyn::EFieldList::~EFieldList() {
    delete head;
    delete rest;
}

void absyn::EFieldList::print(FILE *out, int d) {
    indent(out, d);
    if (getHead()) {
        fprintf(out, "efieldList(\n");
        head->print(out, d + 1);
        fprintf(out, ",\n");
        if (rest) {
            rest->print(out, d + 1);
        } else {
            indent(out, d + 1);
            fprintf(out, "efieldList()");
        }
        fprintf(out, ")");
    } else fprintf(out, "efieldList()");
}

//=============================================================
// RecordExp
absyn::RecordExp::RecordExp(int line, S_symbol t, absyn::EFieldList *f) : absyn::Exp(
        line), type(t), fields(f) {}

S_symbol absyn::RecordExp::getType(void) const { return type; }

absyn::EFieldList *absyn::RecordExp::getFields(void) const { return fields; }

absyn::RecordExp::~RecordExp() { delete fields; }

Ty_ty absyn::RecordExp::typeCheck(v_tbl venv, t_tbl tenv) const {
    Ty_ty recType = T_tbl_look(tenv, this->getType());

    // check type exists 
    if (recType == nullptr) {
        EM_error(this->getPos(), "Undeclared type: %s", S_name(this->getType()));
        return Ty_Error();
    }

    // check type is a Record Type
    if (recType->kind != Ty_record) {
        EM_error(this->getPos(), "%s is not a record type. Invalid Reference.", S_name(this->getType()));
        return Ty_Error();
    }

    Ty_fieldList types = Ty_FieldList(recType->u.record->head, recType->u.record->tail);
    EFieldList* fields = this -> getFields();

    // check that lengths are equivalent in type declaration and record declaration 
    while (types != nullptr && fields != nullptr) {
        types = types->tail;
        fields = fields->getRest();
    }

    // check that lengths are equivalent in type declaration and record declaration 
    if (types != nullptr || fields != nullptr) {
        EM_error(this->getPos(), "Invalid record creation. Incorrect number of fields specified for "
        "record %s.", S_name(this->getType()));
        return Ty_Error();
    }

    Ty_fieldList reversedTypes = Ty_FieldList(recType->u.record->head, recType->u.record->tail);
    fields = this -> getFields();
    Ty_field type;
    EField* field; 

    // reverse the types list 
    while (reversedTypes != nullptr) {
        type = reversedTypes->head;
        types = Ty_FieldList(type, types);
        reversedTypes = reversedTypes->tail;
    }

    // check that both have same fields and same types 
    while (types != nullptr && fields != nullptr) {
        type = types->head;
        field = fields->getHead();


        if (S_name(type->name) != S_name(field->getName())) {
            EM_error(this->getPos(), "Invalid record creation. Field '%s' encountered where "
            "'%s' was expected.", S_name(field->getName()), S_name(type->name));
            return Ty_Error();
        }

        if (!matchTypes(type->ty, field->getExp()->typeCheck(venv, tenv))) {
            EM_error(this->getPos(), "Invalid record creation. Type mismatch for field %s.", S_name(type->name));
            return Ty_Error();
        }


        types = types->tail;
        fields = fields->getRest();
    }


    return recType;

}

void absyn::RecordExp::print(FILE *out, int d) {
    indent(out, d);
    fprintf(out, "recordExp(%s,\n", S_name(type));
    if (fields) {
        fields->print(out, d + 1);
    } else {
        indent(out, d + 1);
        fprintf(out, "efieldList()");
    }
    fprintf(out, ")");
}
