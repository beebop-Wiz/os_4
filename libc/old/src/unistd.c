#define UNISTD_C
#include <unistd.h>
#undef UNISTD_C
#include <sys/call.h>
#include <_chdir.h>
#include <string.h>

char **environ;

pid_t fork(void) {
  return syscall(5, 0, 0, 0);
}

int execv(const char *path, char *const argv[]) {
  return execve(path, argv, environ);
}

int execve(const char *path, char *const argv[], char *const envp[]) {
  char *rp = get_full_path(path);
  return syscall(8, (int) rp, (int) argv, (int) envp);
}

char *getcwd(char *buf, size_t size) {
  const char *cwd = get_cwd();
  int len = (strlen(cwd) > size - 1) ? size : strlen(cwd) + 1;
  memcpy(buf, cwd, len);
  buf[size - 1] = 0;
  return buf;
}

int chdir(const char *path) {
  // TODO: stat path first
  set_cwd((char *) path);
  return 0;
}

int tcsetpgrp(int fildes, pid_t pgid_id) {
  syscall(13, pgid_id, 0, 0);
  return 0;
}

pid_t getpid() {
  return (pid_t) syscall(14, 0, 0, 0);
}
