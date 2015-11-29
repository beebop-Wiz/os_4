#ifndef SYSCALL_H
#define SYSCALL_H

enum syscalls {
  SYS_WRITE,
};

#include "idt.h"

void do_syscall(regs_t r);

#endif
