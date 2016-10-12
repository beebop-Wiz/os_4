#ifndef SYS_WAIT_H
#define SYS_WAIT_H

#include <sys/types/pid_t.h>

pid_t waitpid(pid_t pid, int *wstatus, int options);

#endif
