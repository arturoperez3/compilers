#include <stdio.h>
#include <cstdlib>
#include "util.h"
#include "errormsg.h"
#include "symbol.h"
#include "absyn.h"
#include "parse.h"

int main(int argc, char **argv) {
  absyn::Exp* absyn_root;

  if (argc!=2) {fprintf(stderr,"usage: a.out filename\n"); exit(1);}
  
  absyn_root = parse(argv[1]);
  if (absyn_root != NULL) {
    absyn_root->print(stdout, 0);
    printf("\n");
    return 0;
  }
  else {
    printf("Parsing failed\n");
    return 1;
  }

}
