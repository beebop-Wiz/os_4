#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main(void) {
  int fd = open("home/beebop/test.txt", O_RDONLY);
  printf("Opened fd %d\n", fd);
  FILE *f = fdopen(fd, "r");
  printf("Here goes nothing... ");
  char c;
  while((c = fgetc(f)) != -1) {
    putchar(c);
  }
  printf("\n");
  if(fork()) {
    printf("Started child process\n");
  } else 
    execv("boot/test.exe", 0);
  return 0;
}
