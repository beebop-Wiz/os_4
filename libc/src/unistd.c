#include <unistd.h>
#include <sys/call.h>

pid_t fork(void) {
  return syscall1(0x80, 2);
}

int execve(const char *filename, char *const argv[], char *const envp[]) {
  return syscall4(0x80, 11, (int) filename, (int) argv, (int) envp);
}

int cur_brk = 0;

int brk(void *addr) {
  syscall2(0x80, 45, (int) addr);
  return 0;
}

void *sbrk(int increment) {
  if(!cur_brk) cur_brk = syscall2(0x80, 45, 0);
  unsigned int old_brk = cur_brk;
  cur_brk += increment;
  syscall2(0x80, 45, cur_brk);
  return (void *) old_brk;
}

int setpgid(pid_t pid, pid_t pgid) {
  return syscall3(0x80, 57, pid, pgid);
}

pid_t setsid(void) {
  return syscall1(0x80, 66);
}

void _exit(int val) {
  syscall2(0x80, 1, val);
}
