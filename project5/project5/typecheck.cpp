//
// Jerry Roth, Gonzaga University, Spring 2001
//

#include "typecheck.h"
#include "types.h"

bool typecheck_prog(absyn::Exp* exp)
{
  /* our environments */
  t_tbl tenv;  /* type env  */
  v_tbl venv;  /* value env */

  tenv = E_base_tenv();    /* create and initialize env's */
  venv = E_base_venv();

  Ty_ty final_ty = exp->typeCheck(venv, tenv);
  if (final_ty != Ty_Error()) {
    return TRUE;
  } else {
    return FALSE;
  }
}
