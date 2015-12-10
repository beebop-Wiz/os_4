#ifndef UTIL_H
#define UTIL_H

void *memset(void *s, int c, unsigned long n);
void *memcpy(void *dest, const void *src, unsigned long n);
int strlen(const char *s);
int streq(const char *a, const char *b);
#endif
