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

int strcmp(const char *s1, const char *s2) {
  if(strlen(s1) != strlen(s2)) {
    if(s1[0] < s2[0]) return -1;
    return 1;
  } else return strncmp(s1, s2, strlen(s1));
}

int strncmp(const char *s1, const char *s2, size_t n) {
  size_t i;
  for(i = 0; i < n; i++) {
    if(s1[i] != s2[i]) goto inequal;
    if(s1[i] == 0 || s2[i] == 0) break;
  }
  return 0;
 inequal:
  if(s1[0] < s2[0]) return -1;
  return 1;
}

char *strtok_savep;

char *strtok(char *str, const char *delim) {
  size_t i, j;
  // step 1: skip all tokens in delim
  if(str == NULL) {
    str = strtok_savep;
    if(strtok_savep == 0) return NULL;
  }
  for(i = 0; i < strlen(str); i++)
    for(j = 0; j < strlen(delim); j++)
      if(str[i] != delim[j]) goto stage_2;
 stage_2:
  if(i == strlen(str))
    return NULL;
  for(i = 0; i < strlen(str); i++)
    for(j = 0; j < strlen(delim); j++)
      if(str[i] == delim[j]) goto stage_3;
 stage_3:
  if(str[i]) {
    str[i] = 0;
    strtok_savep = str + i + 1;
  } else strtok_savep = 0;
  return str;
}

char *strchr(const char *s, int c) {
  size_t i, l = strlen(s);
  for(i = 0; i < l; i++) {
    if(s[i] == c) return (char *) &s[i];
  }
  return 0;
}
