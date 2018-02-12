#if ! defined _ERRORMSG_H
#define _ERRORMSG_H

extern bool EM_anyErrors;

void EM_newline(void);

extern int EM_tokPos;

void EM_error(int, appel_string,...);
void EM_impossible(appel_string,...);
void EM_reset(appel_string filename);

#endif
