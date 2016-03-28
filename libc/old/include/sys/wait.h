#ifndef SYS_WAIT_H
#define SYS_WAIT_H

#include <depend/types_pid_t.h>
#include <depend/wait_stati.h>

pid_t wait(int *status);
pid_t waitpid(pid_t pid, int *stat_loc, int options);

#endif
