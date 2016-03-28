#ifndef STDIO_H
#define STDIO_H

#define BUFSIZ 1024

struct __file_struct {
  int fd;
  char buf[BUFSIZ];
  int bufwp, bufrp;
  int err;
};

typedef struct __file_struct FILE;

int printf(char *fmt, ...);

#ifndef STDIO_C
extern FILE *stdin, *stdout, *stderr;
#endif

#endif
