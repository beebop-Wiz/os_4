#include <sys/wait.h>
#include <sys/call.h>

pid_t wait(int *status) {
  syscall(9, 2, 1, 0);
  return 0;
}
