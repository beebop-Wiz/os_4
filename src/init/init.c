#include <stdio.h>
#include <sys/call.h>
#include <unistd.h>
#include <stdlib.h>

int main(void) {
  printf("Hello, Userspace!\n");
  char *cmdbuf = malloc(2048);
  int cmdptr;
  while(1) {
    printf("/ > ");
    fflush(stdout);
    cmdptr = 0;
    while((cmdbuf[cmdptr++] = getchar()) != '\n') ;
    cmdbuf[cmdptr - 1] = 0;
    printf("Unknown command %s\n", cmdbuf);
  }

  return 0;
}
