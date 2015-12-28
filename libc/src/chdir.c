#include <_chdir.h>
#include <stdlib.h>
#include <string.h>

char *cwd;

void alloc_cwd_buf() {
  cwd = malloc(CWD_BUFSIZ);
  cwd[0] = '/';
}

void set_cwd(char *newcwd) {
  int i;
  for(i = 0; (cwd[i] = newcwd[i]) && (i < CWD_BUFSIZ); i++) ;
}

char *get_full_path(const char *rel) {
  if(rel[0] == '/') {
    return strdup(rel);
  } else {
    char *r = malloc(strlen(cwd) + strlen(rel) + 1);
    memcpy(r, cwd, strlen(cwd));
    memcpy(r + strlen(cwd), rel, strlen(rel));
    r[strlen(cwd) + strlen(rel)] = 0;
    r[strlen(cwd) + strlen(rel) + 1] = 0;
    return r;
  }
}
