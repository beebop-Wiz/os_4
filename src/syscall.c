#include "syscall.h"
#include "idt.h"
#include "vgatext.h"
#include "mt.h"
#include "malloc.h"

// eax = syscall no
// ebx = arg 1
// ecx = return
// edx = arg 2
// esi = arg 3

extern volatile int cur_ctx;
extern struct process *ptab[65536];

void do_syscall(regs_t r) {
  int i;
  switch(r->eax) {
  case SYS_WRITE:
    printf("%s", (char *) r->ebx);
    break;
  case SYS_EXIT:
    proc_exit(r);
    break;
  case SYS_MALLOC:
    r->ecx = (unsigned int) malloc(r->ebx);
    break;
  case SYS_FREE:
    free((void *) r->ebx);
    break;
  case SYS_GETFD:
    for(i = 0; i < FD_MAX; i++) {
      if(!(ptab[cur_ctx]->fds[i] & FD_PRESENT)) {
	r->ecx = i;
	return;
      }
    }
    r->ecx = -1;
    break;
  default: return;
  }
}
