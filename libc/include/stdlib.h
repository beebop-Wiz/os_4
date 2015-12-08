#ifndef STDLIB_H
#define STDLIB_H

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0
#define RAND_MAX 32767

#define MB_CUR_MAX 4

#include <depend/stddef_null.h>

struct __div_t {
  int quot;
  int rem;
};

typedef struct __div_t div_t;

struct __ldiv_t {
  long quot;
  long rem;
};

typedef struct __ldiv_t ldiv_t;

struct __lldiv_t {
  long long quot;
  long long rem;
};

typedef struct __lldiv_t lldiv_t;

#include <depend/stddef_size_t.h>
#include <depend/stddef_wchar_t.h>
#include <depend/wait_stati.h>

void          _Exit(int);
long          a64l(const char *);
void          abort(void);
int           abs(int);
int           atexit(void (*)(void));
double        atof(const char *);
int           atoi(const char *);
long          atol(const char *);
long long     atoll(const char *);
void         *bsearch(const void *, const void *, size_t, size_t,
                  int (*)(const void *, const void *));
void         *calloc(size_t, size_t);
div_t         div(int, int);
double        drand48(void);
double        erand48(unsigned short [3]);
void          exit(int);
void          free(void *);
char         *getenv(const char *);
int           getsubopt(char **, char *const *, char **);
int           grantpt(int);
char         *initstate(unsigned, char *, size_t);
long          jrand48(unsigned short [3]);
char         *l64a(long);
long          labs(long);
void          lcong48(unsigned short [7]);
ldiv_t        ldiv(long, long);
long long     llabs(long long);
lldiv_t       lldiv(long long, long long);
long          lrand48(void);
void         *malloc(size_t);
int           mblen(const char *, size_t);
size_t        mbstowcs(wchar_t *, const char *, size_t);
int           mbtowc(wchar_t *, const char *, size_t);
char         *mkdtemp(char *);
int           mkstemp(char *);
long          mrand48(void);
long          nrand48(unsigned short [3]);
int           posix_memalign(void **, size_t, size_t);
int           posix_openpt(int);
char         *ptsname(int);
int           putenv(char *);
void          qsort(void *, size_t, size_t, int (*)(const void *,
                  const void *));
int           rand(void);
int           rand_r(unsigned *);
long          random(void);
void         *realloc(void *, size_t);
char         *realpath(const char *, char *);
unsigned short *seed48(unsigned short [3]);
int           setenv(const char *, const char *, int);
void          setkey(const char *);
char         *setstate(char *);
void          srand(unsigned);
void          srand48(long);
void          srandom(unsigned);
double        strtod(const char *, char **);
float         strtof(const char *, char **);
long          strtol(const char *, char **, int);
long double   strtold(const char *, char **);
long long     strtoll(const char *, char **, int);
unsigned long strtoul(const char *, char **, int);
unsigned long long
              strtoull(const char *, char **, int);
int           system(const char *);
int           unlockpt(int);
int           unsetenv(const char *);
size_t        wcstombs(char *, const wchar_t *, size_t);
int           wctomb(char *, wchar_t);

#endif
