#include <stdio.h>

int main(void) {
  printf("cat started\n");
  char c;
  while((c = getchar()) != EOF) {
    putchar(c);
  }
  return 0;
}
