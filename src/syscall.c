#include "syscall.h"
#include "idt.h"
#include "vgatext.h"

void do_syscall(regs_t r) {
  switch(r->eax) {
  case SYS_WRITE:
    printf("%s", (char *) r->ebx);
    break;
  default: return;
  }
}
