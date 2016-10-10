#include <stdio.h>

int main(void) {
  FILE *f = fopen("/test.txt", "r");
  char c;
  while((c = fgetc(f)) != EOF) {
    putchar(c);
  }
  f = fopen("/test.txt", "r");
  while((c = fgetc(f)) != EOF) {
    putchar(c);
  }
  return 0;
}
