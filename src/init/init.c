#include <stdio.h>
#include <sys/call.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

// back > fore

int st2_r = 0;

char *strtok2(char *s, char *d) {
  return (st2_r++) ? strtok(0, d) : strtok(s, d);
}

int main(void) {
  printf("Hello, Userspace!\n");
  char *cmdbuf = malloc(2048);
  char **cmd = malloc(2048 * sizeof(char *));
  int argc, i;
  int cmdptr;
  while(1) {
    printf("\e[44;37;1mroot\e[0;44;37m\x80\e[47;30;1m/\e[0;47;36m\x80\e[30;46m#\e[30;46m\x80\e[0m ");
    fflush(stdout);
    cmdptr = 0;
    while((cmdbuf[cmdptr++] = getchar()) != '\n') ;
    cmdbuf[cmdptr - 1] = 0;
    
    argc = 0;
    char *s;
    printf("Command: `%s'\n", cmdbuf);

    while((s = strtok2(cmdbuf, " "))) {
      printf("%s\n", s);
      cmd[argc] = malloc(strlen(s));
      strcpy(cmd[argc], s);
      argc++;
    }
    st2_r = 0;
    
    cmd[argc] = 0;
    pid_t child;
    if(!(child = fork())) {
      execve(cmd[0], cmd, 0);
      return 0;
    }
    for(i = 0; i < argc; i++) {
      free(cmd[i]);
    }
    //    printf("Unknown command %s\n", cmdbuf);
  }
  return 0;
}
