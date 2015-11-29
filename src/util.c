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
