/*
 * util.c - commonly used utility functions.
 */

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <cstring>
#include "util.h"
using namespace std;

void *checked_malloc(int len)
{
  void *p = malloc(len);
  if (!p)
   error("Ran out of memory!");
 
  return p;
}

appel_string Appel_String(char *s)
{appel_string p = (appel_string) checked_malloc(strlen(s)+1);
 strcpy(p,s);
 return p;
}

appel_string Appel_String(string *s)
{appel_string p = (appel_string) checked_malloc(s->length()+1);
 strncpy(p,s->data(),s->length());
 p[s->length()] = '\0';
 return p;
}

U_boolList U_BoolList(bool head, U_boolList tail)
{ U_boolList list = (U_boolList) checked_malloc(sizeof(*list));
  list->head = head;
  list->tail = tail;
  return list;
}

void error(const string &s)
{
  cerr << endl << "Error: " << s << endl;
  exit(1);
}
