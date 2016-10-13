#ifndef STDIO_H
#define STDIO_H

#define BUFSIZ 1024
#define EOF -1

#define O_RDONLY 1
#define O_WRONLY 2
#define O_RDWR   3

#define O_CLOEXEC   4
#define O_CREAT     8
#define O_DIRECTORY 16
#define O_EXCL      32
#define O_NOCTTY    64
#define O_NOFOLLOW  128
#define O_TMPFILE   256
#define O_TRUNC     512

struct __file_struct {
  int fd;
  char *buf;
  int bufwp, bufrp;
  int err;
};

typedef struct __file_struct FILE;

int printf(const char *fmt, ...);
int fprintf(FILE *f, const char *fmt, ...);
int vprintf(const char *fmt, __builtin_va_list ap);
int vfprintf(FILE *f, const char *fmt, __builtin_va_list ap);
int putchar(int c);
int fputc(int c, FILE *stream);
int fgetc(FILE *stream);
int getchar();
int fflush(FILE *stream);
FILE *fopen(const char *path, const char *mode);

#ifndef STDIO_C
extern FILE *stdin, *stdout, *stderr;
#endif

#endif
