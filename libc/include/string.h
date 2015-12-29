#ifndef STRING_H
#define STRING_H

#include <depend/stddef_size_t.h>

void   *memcpy(void *dest, const void *src, size_t n);
#define strcat(d, s) (strncat(d, s, strlen(s)))
#define strdup(s) (strndup((s), strlen(s)))
size_t  strlen(const char *s);
char   *strncat(char *dest, const char *src, size_t n);
char   *strndup(const char *s, size_t n);
int     strncmp(const char *s1, const char *s2, size_t n);
int     strcmp(const char *s1, const char *s2);
char   *strtok(char *str, const char *delim);
char   *strchr(const char *s, int c);
#endif
