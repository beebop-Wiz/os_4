#ifndef STRING_H
#define STRING_H

int strlen(const char *s);
char *strtok(char *str, const char *delim);
char *strcpy(char *dest, const char *src);

#include <sys/types/size_t.h>
int strncmp(const char *s1, const char *s2, size_t n);
int strcmp(const char *s1, const char *s2);
#endif
