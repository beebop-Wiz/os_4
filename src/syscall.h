#ifndef SYSCALL_H
#define SYSCALL_H

enum syscalls {
  SYS_WRITE,			/* 0 */
  SYS_EXIT,			/* 1 */
  SYS_MALLOC,			/* 2 */
  SYS_FREE,			/* 3 */
  SYS_GETFD			/* 4 */
};

#include "idt.h"

void do_syscall(regs_t r);

#endif
