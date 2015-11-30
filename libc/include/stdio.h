#ifndef STDIO_H
#define STDIO_H

#define BUFSIZ 1024

struct __file_struct {
  int fd;
  char buf[BUFSIZ];
  int bufwp, bufrp;
};

typedef struct __file_struct FILE;
typedef unsigned long fpos_t;
#include <depend/types_off_t.h>
#include <depend/stddef_size_t.h>
#include <depend/types_ssize_t.h>
#include <depend/stdarg_va_list.h>

#define L_ctermid 0
#define L_tmpnam 0

#define _IOFBF 0
#define _IOLBF 1
#define _IONBF 2

#define SEEK_CUR 0
#define SEEK_END 1
#define SEEK_SET 2

#define FILENAME_MAX 255
#define FOPEN_MAX 255
#define TMP_MAX 0

#define EOF -1

#include <depend/stddef_null.h>
#define P_tmpdir "/tmp/"

#define stderr f_stderr
#define stdin f_stdin
#define stdout f_stdout

#ifndef STDIO_C
extern FILE *f_stderr, *f_stdin, *f_stdout;
#endif

void     clearerr(FILE *);
char    *ctermid(char *);
int      dprintf(int, const char *, ...);
int      fclose(FILE *);
FILE    *fdopen(int, const char *);
int      feof(FILE *);
int      ferror(FILE *);
int      fflush(FILE *);
int      fgetc(FILE *);
int      fgetpos(FILE *, fpos_t *);
char    *fgets(char *, int, FILE *);
int      fileno(FILE *);
void     flockfile(FILE *);
FILE    *fmemopen(void *, size_t, const char *);
FILE    *fopen(const char *, const char *);
int      fprintf(FILE *, const char *, ...);
int      fputc(int, FILE *);
int      fputs(const char *, FILE *);
size_t   fread(void *, size_t, size_t, FILE *);
FILE    *freopen(const char *, const char *, FILE *);
int      fscanf(FILE *, const char *, ...);
int      fseek(FILE *, long, int);
int      fseeko(FILE *, off_t, int);
int      fsetpos(FILE *, const fpos_t *);
long     ftell(FILE *);
off_t    ftello(FILE *);
int      ftrylockfile(FILE *);
void     funlockfile(FILE *);
size_t   fwrite(const void *, size_t, size_t, FILE *);
int      getc(FILE *);
int      getchar(void);
int      getc_unlocked(FILE *);
int      getchar_unlocked(void);
ssize_t  getdelim(char **, size_t *, int, FILE *);
ssize_t  getline(char **, size_t *, FILE *);
char    *gets(char *);
FILE    *open_memstream(char **, size_t *);
int      pclose(FILE *);
void     perror(const char *);
FILE    *popen(const char *, const char *);
int      printf(const char *, ...);
int      putc(int, FILE *);
int      putchar(int);
int      putc_unlocked(int, FILE *);
int      putchar_unlocked(int);
int      puts(const char *);
int      remove(const char *);
int      rename(const char *, const char *);
int      renameat(int, const char *, int, const char *);
void     rewind(FILE *);
int      scanf(const char *, ...);
void     setbuf(FILE *, char *);
int      setvbuf(FILE *, char *, int, size_t);
int      snprintf(char *, size_t, const char *, ...);
int      sprintf(char *, const char *, ...);
int      sscanf(const char *, const char *, ...);
char    *tempnam(const char *, const char *);
FILE    *tmpfile(void);
char    *tmpnam(char *);
int      ungetc(int, FILE *);
int      vdprintf(int, const char *, va_list);
int      vfprintf(FILE *, const char *, va_list);
int      vfscanf(FILE *, const char *, va_list);
int      vprintf(const char *, va_list);
int      vscanf(const char *, va_list);
int      vsnprintf(char *, size_t, const char *, va_list);
int      vsprintf(char *, const char *, va_list);
int      vsscanf(const char *, const char *, va_list);

#endif
