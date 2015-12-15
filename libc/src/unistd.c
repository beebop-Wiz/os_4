#include <unistd.h>
#include <sys/call.h>

pid_t fork(void) {
  return syscall(5, 0, 0, 0);
}

int execv(const char *path, char *const argv[]) {
  return syscall(8, (int) path, (int) argv, 0);
}
