#ifndef _CHDIR_H
#define _CHDIR_H

#define CWD_BUFSIZ 1024

void alloc_cwd_buf();
void set_cwd(char *cwd);
const char *get_cwd();
char *get_full_path(const char *rel);

#endif
