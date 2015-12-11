#include <fcntl.h>
#include <sys/call.h>

int open(const char *path, int flags, ...) {
  int fd = syscall(4, 0, 0, 0);
  syscall(6, (int) path, fd, flags);
  return fd;
}
