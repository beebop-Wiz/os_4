#include "syscall.h"
#include "idt.h"
#include "vgatext.h"
#include "mt.h"

void do_syscall(regs_t r) {
  switch(r->eax) {
  case SYS_WRITE:
    printf("%s", (char *) r->ebx);
    break;
  case SYS_EXIT:
    proc_exit(r);
    break;
  default: return;
  }
}
