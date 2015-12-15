#include "util.h"

void *memset(void *s, int c, unsigned long n) {
  unsigned long i;
  for(i = 0; i < n; i++) {
    ((char *) s)[i] = c;
  }
  return s;
}

void *memcpy(void *dest, const void *src, unsigned long n) {
  unsigned long i;
  for(i = 0; i < n; i++) {
    ((char *) dest)[i] = ((char *) src)[i];
  }
  return dest;
}

int strlen(const char *s) {
  int i = 0;
  while(*s++) i++;
  return i;
}

int streq(const char *a, const char *b) {
  int la = strlen(a);
  int lb = strlen(b);
  if(la != lb) return 0;
  int i;
  for(i = 0; i < la; i++)
    if(a[i] != b[i]) return 0;
  return 1;
}
