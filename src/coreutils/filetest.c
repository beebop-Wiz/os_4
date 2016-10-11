#include <stdio.h>

int main(void) {
  FILE *f = fopen("/test.txt", "r");
  char c;
  while((c = fgetc(f)) != EOF) {
    putchar(c);
  }
  FILE *f2 = fopen("/test2.txt", "w");
  fprintf(f2, "01234567890\n");
  fflush(f2);
  FILE *f3 = fopen("/test2.txt", "r");
  while((c = fgetc(f3)) != EOF) {
    putchar(c);
  }
  return 0;
}
