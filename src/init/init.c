#include <stdio.h>
#include <sys/call.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

// back > fore

int st2_r = 0;

char *strtok2(char *s, char *d) {
  return (st2_r++) ? strtok(0, d) : strtok(s, d);
}

char *default_env[] = {
  "PATH=/bin:/usr/bin",
  "USER=root"
};

int main(void) {		/* init is always called with no args and no environment */
  printf("Hello, Userspace!\n");
  while(1) {
    printf("Starting shell... (/bin/sh)\n");
    pid_t shell;
    if(!(shell = fork())) {
      execve("/bin/sh", 0, default_env);
      return 0;
    } else {
      waitpid(-1, 0, 0);
    }
    printf("Shell exited, restarting\n");
  }
}
