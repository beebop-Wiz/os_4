#define STDIO_C
#include <stdio.h>
#undef STDIO_C
#include <stdlib.h>
#include <sys/call.h>

FILE *f_stderr, *f_stdin, *f_stdout;

int sys_getfd() {
  return syscall(4, 0, 0, 0);
}

void init_stdio(void) {
  f_stderr = malloc(sizeof(struct __file_struct));
  f_stdin = malloc(sizeof(struct __file_struct));
  f_stdout = malloc(sizeof(struct __file_struct));
  f_stdin->fd = sys_getfd();
  f_stdout->fd = sys_getfd();
  f_stderr->fd = sys_getfd();
  f_stdin->bufwp = f_stdin->bufrp = f_stdout->bufwp = f_stdout->bufrp = f_stderr->bufwp = f_stderr->bufrp = 0;
}

FILE *fdopen(int fd, const char *mode) {
  FILE *r = malloc(sizeof(struct __file_struct));
  r->fd = fd;
  r->bufrp = r->bufwp = 0;
  return r;
}

int fflush(FILE *f) {
  f->buf[f->bufwp] = 0;
  char * b = f->buf + f->bufrp;
  syscall(0, (int) b, 0, 0);
  f->bufrp = f->bufwp = 0;
  return 0;
}

int fputc(int c, FILE *f) {
  f->buf[f->bufwp++] = c;
  if(f->bufwp >= BUFSIZ) fflush(f);
  if(c == '\n') fflush(f);
  return c;
}

int printf(const char *fmt, ...) {
  __builtin_va_list ap;
  __builtin_va_start(ap, fmt);
  int r = vfprintf(f_stdout, fmt, ap);
  __builtin_va_end(ap);
  return r;
}

int vprintf(const char *fmt, __builtin_va_list ap) {
  return vfprintf(f_stdout, fmt, ap);
}

#define gc(f) (*((f)++))
#define ugc(f) --(f)

#define LEN_8  0
#define LEN_16 1
#define LEN_32 2
#define LEN_64 3

const char digits[] = "0123456789abcdef";

void itoa_u(FILE *f, unsigned long i, unsigned int radix) {
  if(i >= radix)
    itoa_u(f, i/radix, radix);
  fputc(digits[i % radix], f);
}

void itoa_s(FILE *f, signed long i, unsigned int radix) {
  if(i < 0) {
    i = -i;
    fputc('-', f);
  }
  itoa_u(f, (unsigned long) i, radix);
}

void fputs_nonl(FILE *f, char *s) {
  while(*s) fputc(*s++, f);
}

int vfprintf(FILE *f, const char *fmt, __builtin_va_list ap) {
  char c;
  int length = -1;
  while(*fmt) {
    if(*fmt == '%') {
      fmt++;
      switch(c = gc(fmt)) {
      case 'h':
	if((c = gc(fmt)) == 'h') {
	  length = LEN_8;
	} else {
	  length = LEN_16;
	  ugc(fmt);
	}
	break;
      case 'l':
	if((c = gc(fmt)) == 'l') {
	  length = LEN_64;
	} else {
	  length = LEN_32;
	  ugc(fmt);
	}
	break;
      case 'd':
      case 'i':
	switch(length) {
	case LEN_64:
	  itoa_s(f, __builtin_va_arg(ap, signed long), 10);
	  break;
	default:
	  itoa_s(f, __builtin_va_arg(ap, signed int), 10);
	  break;
	}
	break;
	      case 'u':
	switch(length) {
	case LEN_64:
	  itoa_u(f, __builtin_va_arg(ap, unsigned long), 10);
	  break;
	default:
	  itoa_u(f, __builtin_va_arg(ap, unsigned int), 10);
	  break;
	}
	break;
      case 'o':
	switch(length) {
	case LEN_64:
	  itoa_u(f, __builtin_va_arg(ap, unsigned long), 8);
	  break;
	default:
	  itoa_u(f, __builtin_va_arg(ap, unsigned int), 8);
	  break;
	}
	break;
      case 'x':
	switch(length) {
	case LEN_64:
	  itoa_u(f, __builtin_va_arg(ap, unsigned long), 16);
	  break;
	default:
	  itoa_u(f, __builtin_va_arg(ap, unsigned int), 16);
	  break;
	}
	break;
      case 'c':
	fputc(__builtin_va_arg(ap, unsigned int), f);
	break;
      case 's':
	fputs_nonl(f, __builtin_va_arg(ap, char *));
	break;
      case '%':
	fputc('%', f);
	break;
      }
    } else {
      fputc(gc(fmt), f);
    }
  }
  return 0;
}

int fgetc(FILE *f) {
  int r;
  if((f->bufrp != f->bufwp) &&
     (f->bufrp < (BUFSIZ - 1))) { // data available
    r = f->buf[f->bufrp++];
    if(f->bufrp >= BUFSIZ) f->bufrp = 0;
  } else { // data not available, refill buffer
    f->bufrp = 0;
    f->bufwp = syscall(7, f->fd, (int) f->buf, 0);
    r = f->buf[f->bufrp++];
  }
  return r;
}
