#include <iostream>
#include <cstdlib>
#include "util.h"
#include "errormsg.h"
using namespace std;

extern int yyparse(void);

void parse(appel_string fname) 
{EM_reset(fname);
 if (yyparse() == 0) /* parsing worked */
   cerr << "Parsing successful!\n";
 else 
   cerr << "Parsing failed\n";
}


int main(int argc, char **argv) {
  if (argc!=2) {cerr << "usage: " << argv[0] << " filename\n"; exit(1);}
 parse(argv[1]);
 return 0;
}
