#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main(void) {
  pid_t r = fork();
  if(r) {
    printf("I am the parent of pid %d\n", r);
  } else {
    printf("I am the child process\n");
    int fd = open("home/beebop/test.txt", O_RDONLY);
    printf("Opened fd %d\n", fd);
  }
  return 0;
}
