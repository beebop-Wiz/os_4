#include <string.h>
#include <stdio.h>

int strlen(const char *s) {
  int i = 0;
  while(*s++) i++;
  return i;
}

int strtok_pos;
int strtok_ol;
char *strtok_os = 0;

char *strtok(char *str, const char *delim) {
  if(!strtok_os || str){
    strtok_pos = 0;
    strtok_os = str;
    strtok_ol = strlen(str);
  }
  if(!str) {
    str = strtok_os;
  }
  if(strtok_pos >= strtok_ol) return 0;
  str += strtok_pos;
  int i, j;
  for(i = 0; i < strlen(str); i++) {
    for(j = 0; j < strlen(delim); j++) {
      if(str[i] == delim[j]) {
	str[i] = 0;
	strtok_pos += i + 1;
	return str;
      }
    }
  }
  str[i] = 0;
  strtok_pos += i + 1;
  return str;
}

char *strcpy(char *dest, const char *src) {
  int i;
  for(i = 0; i < strlen(src); i++)
    dest[i] = src[i];
  dest[i] = 0;
  return dest;
}
