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

#define SYS_NO (r->eax)
#define SYS_A1 (r->ebx)
#define SYS_A2 (r->edx)
#define SYS_A3 (r->esi)
#define SYS_RET(n) (r->ecx = (n))

extern volatile int cur_ctx;
extern struct process *ptab[65536];

void do_syscall(regs_t r) {
  int i;
  switch(SYS_NO) {
  case SYS_WRITE:
    printf("%s", (char *) SYS_A1);
    break;
  case SYS_EXIT:
    proc_exit(r);
    break;
  case SYS_MALLOC:
    SYS_RET((unsigned int) malloc(SYS_A1));
    break;
  case SYS_FREE:
    free((void *) SYS_A1);
    break;
  case SYS_GETFD:
    for(i = 0; i < FD_MAX; i++) {
      if(!(ptab[cur_ctx]->fds[i] & FD_PRESENT)) {
	ptab[cur_ctx]->fds[i] |= FD_PRESENT;
	SYS_RET(i);
	return;
      }
    }
    SYS_RET(-1);
    break;
  case SYS_FORK:
    r->ecx = fork(r);
    switch_ctx(r);
    break;
  case SYS_BINDFD:
    ptab[cur_ctx]->fds[SYS_A2] |= FD_BOUND;
    ptab[cur_ctx]->bound[SYS_A2].inode = 0;
    ptab[cur_ctx]->bound[SYS_A2].off = 0;
    break;
  default: return;
  }
}
