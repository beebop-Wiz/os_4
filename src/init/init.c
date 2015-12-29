/*
 * "Init", actually a basic sorta-POSIX shell.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/call.h>

char *initial_environment[] = {
  "PATH=/bin:/usr/bin:/usr/local/bin:/usr/sbin:/sbin",
  "HOME=/",
  "USER=root",
  NULL
};

int main(void) {
  environ = (char **) initial_environment;
  setenv("SHELL", "/boot/init.exe", 1);
  char cmd_buf[128];
  char *cmd_tok[64];
  char cwd_buf[256];
  char cmd_path[256];
  int cmd_ptr;
  pid_t child;
  while(1) {
    printf("root@localhost:%s# ", getcwd(cwd_buf, 256));
    fflush(stdout);
    cmd_ptr = 0;
    while((cmd_buf[cmd_ptr++] = getchar()) != '\n') ;
    if(cmd_ptr == 1) continue;
    cmd_buf[cmd_ptr - 1] = 0;
    cmd_tok[0] = strtok(cmd_buf, " ");
    int i;
    for(i = 1; i < 64; i++) cmd_tok[i] = 0;
    i = 1;
    while((cmd_tok[i++] = strtok(NULL, " ")));
    // check for builtin commands
    if(!strncmp(cmd_tok[0], "cd", 128)) {
      if(cmd_tok[1]) {
	chdir(cmd_tok[1]);
      } else {
	chdir("/");
      }
    } else { // not a builtin
      if(!(child = fork())) {
	if(!strchr(cmd_tok[0], '/')) { // relative path
	  char *path_orig = strdup(getenv("PATH"));
	  char *path = path_orig;
	  char *test_path;
	  for(test_path = strtok(path, ":"); test_path; test_path = strtok(NULL, ":")) {
	    memcpy(cmd_path, test_path, strlen(test_path));
	    cmd_path[strlen(test_path)] = '/';
	    memcpy(cmd_path + strlen(test_path) + 1, cmd_tok[0], strlen(cmd_tok[0]));
	    cmd_path[strlen(test_path) + strlen(cmd_tok[0]) + 1] = 0;
	    execve(cmd_path, cmd_tok, environ);
	  }
	  printf("%s: command not found\n", cmd_buf);
	  exit(127);
	} else {
	  if(execve(cmd_tok[0], cmd_tok, environ) < 0) {
	    printf("%s: command not found\n", cmd_buf);
	    exit(127);
	  }
	}
      } else wait(0);
    }
  }
  return 0;
}
