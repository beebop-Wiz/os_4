#ifndef STDIO_H
#define STDIO_H

#define BUFSIZ 1024
#define EOF -1

struct __file_struct {
  int fd;
  char *buf;
  int bufwp, bufrp;
  int err;
};

typedef struct __file_struct FILE;

int printf(const char *fmt, ...);
int vprintf(const char *fmt, __builtin_va_list ap);
int putchar(int c);
int fputc(int c, FILE *stream);
int fgetc(FILE *stream);
int getchar();
int fflush(FILE *stream);

#ifndef STDIO_C
extern FILE *stdin, *stdout, *stderr;
#endif

#endif
