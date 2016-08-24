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

void sh_exit() {
  _exit(0);
}

struct builtin_desc {
  char *cmd;
  int (*func)(int, char **);
} builtins[] = {
  {"exit", (int(*)(int,char**)) sh_exit},
  {0, 0},
};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
int main(int argc, char **argv, char **envp) {
  char *cmdbuf = malloc(2048);
  char **cmd = malloc(2048 * sizeof(char *));
  int cmd_argc, i;
  int cmdptr;
  while(1) {
    printf("\e[44;37;1mroot\e[0;44;37m\x80\e[47;30;1m/\e[0;47;36m\x80\e[30;46m#\e[30;46m\x80\e[0m ");
    fflush(stdout);
    cmdptr = 0;
    while((cmdbuf[cmdptr++] = getchar()) != '\n') ;
    cmdbuf[cmdptr - 1] = 0;
    
    cmd_argc = 0;
    char *s;
    printf("Command: `%s'\n", cmdbuf);

    while((s = strtok2(cmdbuf, " "))) {
      cmd[cmd_argc] = malloc(strlen(s) + 1);
      printf("`%s' %x\n", s, cmd[cmd_argc]);
      strcpy(cmd[cmd_argc], s);
      cmd_argc++;
    }
    st2_r = 0;
    
    cmd[cmd_argc] = 0;
    for(i = 0; builtins[i].cmd; i++) {
      if(!strcmp(builtins[i].cmd, cmd[0])) {
	builtins[i].func(cmd_argc, cmd);
	goto nextcmd;
      }
    }
    pid_t child;
    if(!(child = fork())) {
      execve(cmd[0], cmd, envp);
      printf("Unknown command %s\n", cmdbuf);
      return 0;
    } else {
      waitpid(-1, 0, 0);
    }
  nextcmd:
    for(i = 0; i < cmd_argc; i++) {
      free(cmd[i]);
    }
  }
  return 0;
}
#pragma GCC diagnostic pop
