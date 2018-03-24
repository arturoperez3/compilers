#include <cstdio>
#include <string>
#include <cstring>
using namespace std;
#include "util.h"
#include "symbol.h"
#include "table.h"

static S_symbol mksymbol(appel_string name, S_symbol next)
{S_symbol s = (S_symbol) checked_malloc(sizeof(*s));
 s->name=name; s->next=next;
 return s;
}

#define SIZE 109  /* should be prime */

static S_symbol hashtable[SIZE];

static unsigned int appel_hash(char *s0)
{unsigned int h=0; char *s;
 for(s=s0; *s; s++)  
       h = h*65599 + *s;
 return h;
}
 
static int streq(appel_string a, appel_string b)
{
 return !strcmp(a,b);
}

S_symbol S_Symbol(string *name1)
{appel_string name = Appel_String(name1);
 int index= appel_hash(name) % SIZE;
 S_symbol syms = hashtable[index], sym;
 for(sym=syms; sym; sym=sym->next)
   if (streq(sym->name,name)) return sym;
 sym = mksymbol(name,syms);
 hashtable[index]=sym;
 return sym;
}
 
S_symbol S_Symbol(appel_string name)
{int index= appel_hash(name) % SIZE;
 S_symbol syms = hashtable[index], sym;
 for(sym=syms; sym; sym=sym->next)
   if (streq(sym->name,name)) return sym;
 sym = mksymbol(name,syms);
 hashtable[index]=sym;
 return sym;
}
 
appel_string S_name(S_symbol sym)
{
 return sym->name;
}

S_table S_empty(void) 
{ 
 return TAB_empty();
}

void S_enter(S_table t, S_symbol sym, void *value) {
  TAB_enter(t,sym,value);
}

void *S_look(S_table t, S_symbol sym) {
  return TAB_look(t,sym);
}

static struct S_symbol_ marksym = {"<mark>",0};

void S_beginScope(S_table t)
{ S_enter(t,&marksym,NULL);
}

void S_endScope(S_table t)
{S_symbol s;
  do s = (S_symbol) TAB_pop(t);
  while (s != &marksym);
}

void S_dump(S_table t, void (*show)(S_symbol sym, void *binding)) {
  TAB_dump(t, (void (*)(void *, void *)) show);
}

