//
// Jerry Roth, Gonzaga University, Spring 2001
//

#if ! defined _ENV_H
#define _ENV_H

/***************************************************
 *  ENV.H, BASED ON APPEL'S PROGRAM 5.9            *
 ***************************************************/

#include "symbol.h"
#include "types.h"

typedef struct E_enventry_ *E_enventry;

enum enventry_kind {E_varEntry, E_funEntry};

// structure for an entry in the value environment
struct E_enventry_ {
  enum enventry_kind kind;
  union {
    struct {Ty_ty ty; bool isFLIV;} var;   // isFLIV added to mark loop induction variables (Roth, 2001)
    struct {Ty_tyList formals; Ty_ty result;} fun;
  } u;
};

// functions to create value environment entries
E_enventry E_VarEntry(Ty_ty ty);
E_enventry E_FunEntry(Ty_tyList formals, Ty_ty result);



typedef S_table t_tbl;   // typedef for type environment
typedef S_table v_tbl;   // typedef for value environment

t_tbl E_base_tenv(); /* create base type environment */
v_tbl E_base_venv(); /* create base value environment */

// functions to add entries into type & value environments
void T_tbl_enter (t_tbl t, S_symbol sym, Ty_ty ty);
void V_tbl_enter (v_tbl t, S_symbol sym, E_enventry entry);

// functions to look up entries in type & value environments
Ty_ty T_tbl_look(t_tbl t, S_symbol sym);
E_enventry V_tbl_look(v_tbl t, S_symbol sym);

// functions to start/exit scopes in the type & value environments
void Env_beginScope(v_tbl t1, t_tbl t2);
void Env_endScope(v_tbl t1, t_tbl t2);


#endif
