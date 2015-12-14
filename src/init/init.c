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
  char cmd_buf[128];
  int cmd_ptr;
  pid_t child;
  while(1) {
    printf("%% ");
    fflush(stdout);
    cmd_ptr = 0;
    while((cmd_buf[cmd_ptr++] = getchar()) != '\n') ;
    cmd_buf[cmd_ptr - 1] = 0;
    if(!(child = fork()))
      execv(cmd_buf, 0);
  }
  return 0;
}
