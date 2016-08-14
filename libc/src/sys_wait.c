#include <sys/wait.h>
#include <sys/call.h>

pid_t waitpid(pid_t pid, int *wstatus, int options) {
  return syscall4(0x80, 7, pid, (int) wstatus, options);
}

