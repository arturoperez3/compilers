//
// Jerry Roth, Gonzaga University, Spring 2001
//

#include "util.h"
#include "env.h"

// functions to create variable entry for value environment
E_enventry E_VarEntry(Ty_ty ty)
{
  E_enventry it = new E_enventry_;
  it->kind = E_varEntry;
  it->u.var.ty = ty;
  it->u.var.isFLIV = false;  // default is not a loop induction var
  return it;
}

// functions to create function entry for value environment
E_enventry E_FunEntry(Ty_tyList formals, Ty_ty result)
{
  E_enventry it = new E_enventry_;
  it->kind = E_funEntry;
  it->u.fun.formals = formals;
  it->u.fun.result = result;
  return it;
}

/* create base type environment */
t_tbl E_base_tenv()
{
  t_tbl it = S_empty();

  /* add code to initialize type env */

  return it;
}

/* create base value environment */
v_tbl E_base_venv()
{
  v_tbl it = S_empty();

  /* add code to initialize value env */

  return it;
}


// function to add entries into type environment
void T_tbl_enter (t_tbl t, S_symbol sym, Ty_ty ty)
{
  S_enter(t, sym, ty);
}

// function to add entries into value environment
void V_tbl_enter (v_tbl t, S_symbol sym, E_enventry entry)
{
  S_enter(t, sym, entry);
}

// function to look up entries in type environment
Ty_ty T_tbl_look(t_tbl t, S_symbol sym)
{
  return (Ty_ty) S_look(t, sym);
}

// function to look up entries in value environment
E_enventry V_tbl_look(v_tbl t, S_symbol sym)
{
  return (E_enventry) S_look(t, sym);
}

// functions to start/exit scopes in the type & value environments
void Env_beginScope(v_tbl t1, t_tbl t2)
{
  S_beginScope(t1);
  S_beginScope(t2);
}
void Env_endScope(v_tbl t1, t_tbl t2)
{
  S_endScope(t1);
  S_endScope(t2);
}
