#include <sys/wait.h>
#include <sys/call.h>

pid_t wait(int *status) {
  return waitpid(-1, status, 0);
}

pid_t waitpid(pid_t pid, int *status, int options) {
  int stat = syscall(9, 2, 1, pid | options);
  *status = (stat & 0xFFFF0000) >> 16;
  return stat & 0xFFFF;
}
