#include <stdio.h>
#include <unistd.h>

int main(void) {
  pid_t r = fork();
  if(r) {
    printf("I am the parent of pid %d\n", r);
  } else {
    printf("I am the child process\n");
  }
  return 0;
}
