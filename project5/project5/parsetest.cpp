//
// Jerry Roth, Gonzaga University, Spring 2001
//

#include <stdio.h>
#include <cstdlib>
#include "util.h"
#include "errormsg.h"
#include "symbol.h"
#include "absyn.h"
#include "parse.h"
#include "typecheck.h"

int main(int argc, char **argv) {
  absyn::Exp* absyn_root;

  if (argc!=2) {fprintf(stderr,"usage: a.out filename\n"); exit(1);}

  absyn_root = parse(argv[1]);
  if (absyn_root != NULL) {
    if (typecheck_prog(absyn_root)) {
      printf("Type checking successful.\n");
      return 0;
    }
    else {
      printf("Type checking failed.\n");
      return 1;
    }
  }
  else {
    printf("Parsing failed.\n");
    return 1;
  }

}
