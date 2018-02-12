#if ! defined UTIL_H
#define UTIL_H

#include <cassert>
#include <string>
using namespace std;

typedef char *appel_string;
appel_string Appel_String(char *);
appel_string Appel_String(string *);

#if defined __cplusplus
#define TRUE true
#define FALSE false
#else
typedef char bool;
#define TRUE 1
#define FALSE 0
#endif

void *checked_malloc(int);

typedef struct U_boolList_ *U_boolList;
struct U_boolList_ {bool head; U_boolList tail;};
U_boolList U_BoolList(bool head, U_boolList tail);


void error(const string &s);

#endif
