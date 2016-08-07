#include <stdio.h>
#include <sys/call.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

// back > fore
int main(void) {
  printf("Hello, Userspace!\n");
  char *cmdbuf = malloc(2048);
  char **cmd = malloc(2048 * sizeof(char *));
  int argc, i;
  int cmdptr;
  /*  printf("  0123456701234567\n");
  printf("0 \e[40m\e[30m\x80\e[31m\x80\e[32m\x80\e[33m\x80\e[34m\x80\e[35m\x80\e[36m\x80\e[37m\x80\e[1m\e[30m\x80\e[31m\x80\e[32m\x80\e[33m\x80\e[34m\x80\e[35m\x80\e[36m\x80\e[37m\x80\e[0m\n");
  printf("1 \e[41m\e[30m\x80\e[31m\x80\e[32m\x80\e[33m\x80\e[34m\x80\e[35m\x80\e[36m\x80\e[37m\x80\e[1m\e[30m\x80\e[31m\x80\e[32m\x80\e[33m\x80\e[34m\x80\e[35m\x80\e[36m\x80\e[37m\x80\e[0m\n");
  printf("2 \e[42m\e[30m\x80\e[31m\x80\e[32m\x80\e[33m\x80\e[34m\x80\e[35m\x80\e[36m\x80\e[37m\x80\e[1m\e[30m\x80\e[31m\x80\e[32m\x80\e[33m\x80\e[34m\x80\e[35m\x80\e[36m\x80\e[37m\x80\e[0m\n");
  printf("3 \e[43m\e[30m\x80\e[31m\x80\e[32m\x80\e[33m\x80\e[34m\x80\e[35m\x80\e[36m\x80\e[37m\x80\e[1m\e[30m\x80\e[31m\x80\e[32m\x80\e[33m\x80\e[34m\x80\e[35m\x80\e[36m\x80\e[37m\x80\e[0m\n");
  printf("4 \e[44m\e[30m\x80\e[31m\x80\e[32m\x80\e[33m\x80\e[34m\x80\e[35m\x80\e[36m\x80\e[37m\x80\e[1m\e[30m\x80\e[31m\x80\e[32m\x80\e[33m\x80\e[34m\x80\e[35m\x80\e[36m\x80\e[37m\x80\e[0m\n");
  printf("5 \e[45m\e[30m\x80\e[31m\x80\e[32m\x80\e[33m\x80\e[34m\x80\e[35m\x80\e[36m\x80\e[37m\x80\e[1m\e[30m\x80\e[31m\x80\e[32m\x80\e[33m\x80\e[34m\x80\e[35m\x80\e[36m\x80\e[37m\x80\e[0m\n");
  printf("6 \e[46m\e[30m\x80\e[31m\x80\e[32m\x80\e[33m\x80\e[34m\x80\e[35m\x80\e[36m\x80\e[37m\x80\e[1m\e[30m\x80\e[31m\x80\e[32m\x80\e[33m\x80\e[34m\x80\e[35m\x80\e[36m\x80\e[37m\x80\e[0m\n");
  printf("7 \e[47m\e[30m\x80\e[31m\x80\e[32m\x80\e[33m\x80\e[34m\x80\e[35m\x80\e[36m\x80\e[37m\x80\e[1m\e[30m\x80\e[31m\x80\e[32m\x80\e[33m\x80\e[34m\x80\e[35m\x80\e[36m\x80\e[37m\x80\e[0m\n"); */

  while(1) {
    printf("\e[44;37;1mroot\e[0;44;37m\x80\e[47;30;1m/\e[0;47;36m\x80\e[30;46m#\e[30;46m\x80\e[0m ");
    //    printf("\e[44;37;1mroot\e[0;30m\x80\e[0;44;37;1m/\e[0m");
    fflush(stdout);
    cmdptr = 0;
    while((cmdbuf[cmdptr++] = getchar()) != '\n') ;
    cmdbuf[cmdptr - 1] = 0;
    argc = 0;
    char *s;
    while((s = strtok(cmdbuf, " "))) {
      printf("%s\n", s);
      cmd[argc] = malloc(strlen(s));
      strcpy(cmd[argc], s);
      argc++;
    }
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
