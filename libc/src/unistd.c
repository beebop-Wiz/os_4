#include <unistd.h>
#include <sys/call.h>
#include <_chdir.h>

pid_t fork(void) {
  return syscall(5, 0, 0, 0);
}

int execv(const char *path, char *const argv[]) {
  char *rp = get_full_path(path);
  return syscall(8, (int) rp, (int) argv, 0);
}
