#include <stdio.h>

unsigned char data[2049];

int main(void) {
  /*  FILE *f = fopen("/test.txt", "r");
  char c;
  while((c = fgetc(f)) != EOF) {
    putchar(c);
    } */
  int i;
  char c;
  printf("Initializing buffer\n");
  for(i = 0; i < 1024; i++) data[i] = 'a' + (i / (2048 / 26));
  data[2049] = 0;
  FILE *f2 = fopen("/test2.txt", "w");
  printf("Writing data\n");
  fprintf(f2, "%s", data);
  fflush(f2);
  FILE *f3 = fopen("/test2.txt", "r");
  printf("Reading data\n");
  while((c = fgetc(f3)) != EOF) {
    putchar(c);
  }
  return 0;
}
