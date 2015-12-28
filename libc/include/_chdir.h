#ifndef _CHDIR_H
#define _CHDIR_H

#ifndef _CHDIR_C
extern char *cwd;
#endif

#define CWD_BUFSIZ 1024

void alloc_cwd_buf();
void set_cwd(char *cwd);
char *get_full_path(const char *rel);

#endif
