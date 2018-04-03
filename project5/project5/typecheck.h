//
// Jerry Roth, Gonzaga University, Spring 2001
//

#if ! defined _TYPECHECK_H
#define _TYPECHECK_H

#include "absyn.h"
#include "env.h"

/********************************************************
 *  THIS FILE IS BASED ON APPEL'S SEMANT.H, BUT ONLY    *
 *  DOES TYPE CHECKING, AND GENERATES A "Ty_ty"         *
 *  FOR EACH NODE (RATHER THAN Exp_Ty)                  *
 ********************************************************/

/********************************************************
 *  THIS IS THE MAIN ENTRY POINT FOR TYPE CHECKING      *
 *  IT RETURNS TRUE IFF THE PROGRAM IS TYPE CORRECT     *
 ********************************************************/

bool typecheck_prog(absyn::Exp* exp);

/*
 * Each of the following is in a separate .c file.
 * They will be used in typecheckProg, and can also
 * be used if some other pass needs to know the type
 * of something.
 */

#endif
