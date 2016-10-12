#include <fcntl.h>
#include <sys/call.h>
#include <_chdir.h>

int open(const char *path, int flags, ...) {
  int fd = syscall(4, 0, 0, 0);
  char *rp = get_full_path(path);
  syscall(6, (int) rp, fd, flags);
  return fd;
}
