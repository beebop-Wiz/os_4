#include <string.h>
#include <stdlib.h>

char *strncat(char *dest, const char *src, size_t n) {
  size_t i;
  for(i = 0; (dest[i] = src[i]) != 0 && (i < n); i++) ;
  return dest;
}

size_t strlen(const char *s) {
  size_t r;
  for(r = 0; *s++; r++) ;
  return r;
}

char *strndup(const char *s, size_t n) {
  char *r = malloc(n + 1);
  size_t i;
  for(i = 0; (r[i] = s[i]) != 0 && (i < n); i++) ;
  return r;
}

void *memcpy(void *dest, const void *src, size_t n) {
  char *d = (char *) dest;
  char *s = (char *) src;
  size_t i;
  for(i = 0; i < n; i++) d[i] = s[i];
  return dest;
}
