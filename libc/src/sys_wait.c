#include <sys/wait.h>
#include <sys/call.h>

pid_t wait(int *status) {
  while(syscall(9, -1, (int) status, 0) == 0) {
    int i;
    for(i = 0; i < 100000; i++) ;
  }
  return 0;
}
