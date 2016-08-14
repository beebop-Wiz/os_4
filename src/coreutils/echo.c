#include <stdio.h>

int main(int argc, char **argv) {
  int i;
  printf("echo: %d %x\n", argc, argv);
  for(i = 1; i < argc; i++) {
    printf("%s ", argv[i]);
  }
  printf("\n");
  return 0;
}
