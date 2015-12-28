#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/call.h>

int main(void) {
  char cmd_buf[128];
  int cmd_ptr;
  pid_t child;
  while(1) {
    printf("# ");
    fflush(stdout);
    cmd_ptr = 0;
    while((cmd_buf[cmd_ptr++] = getchar()) != '\n') ;
    if(cmd_ptr == 1) continue;
    cmd_buf[cmd_ptr - 1] = 0;
    if(!(child = fork())) {
      if(execv(cmd_buf, 0) < 0) {
	printf("%s: command not found\n", cmd_buf);
	exit(127);
      }
    } else wait(0);
  }
  return 0;
}
