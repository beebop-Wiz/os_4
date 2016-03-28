#include "syscall.h"
#include "idt.h"
#include "vgatext.h"
#include "mt.h"
#include "malloc.h"
#include "ext2.h"
#include "elf.h"
#include "util.h"
#include "log.h"
#include "keyboard.h"

// eax = syscall no
// ebx = arg 1
// ecx = return
// edx = arg 2
// esi = arg 3

#define SYS_NO (r->eax)
#define SYS_A1 (r->ebx)
#define SYS_A2 (r->edx)
#define SYS_A3 (r->esi)
#define SYS_RET(n) (r->ecx = (n))

extern volatile int cur_ctx;
extern struct process *ptab[65536];

#define NUM_UNIX_SYSCALLS 190
void (*unix_syscalls[NUM_UNIX_SYSCALLS])(regs_t r);

void usys_exit() {
  proc_exit();
}

void usys_write(regs_t r) {
  printf("%s\n", r->ecx);
}

void syscall_unix(regs_t r) {
  if(unix_syscalls[r->eax]) unix_syscalls[r->eax](r);
}

void do_syscall(regs_t r) {
  log(LOG_SYSCALL, LOG_INFO, "%s %x %x %x %x %x %x\n", r->int_no == 0x80 ? "unix" : "os4", r->eax, r->ebx, r->ecx, r->edx, r->esi, r->edi);
  if(r->int_no == 0x80) {
    syscall_unix(r);
  }
  log(LOG_SYSCALL, LOG_INFO, "COMPLETE\n");
}

void init_syscall() {
  unix_syscalls[1] = usys_exit;
  unix_syscalls[4] = usys_write;
}
