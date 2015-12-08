#include <unistd.h>
#include <sys/call.h>

pid_t fork(void) {
  return syscall(5, 0, 0, 0);
}
