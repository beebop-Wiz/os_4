#include "mt.h"
#include "malloc.h"
#include "util.h"
#include "idt.h"
#include "user.h"
#include "gdt.h"

#ifdef DEBUG_MT
#define printd(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define printd(fmt, ...)
#endif

struct process *ptab[65536];
volatile int cur_ctx;
int mt_enabled = 0;

void init_mt() {
  int i;
  for(i = 0; i < 65536; i++) ptab[i] = 0;
  cur_ctx = 0;
}

void enable_mt() {
  mt_enabled = 1;
  // fake a IRQ to kick-start it
  tss_flush();
  asm volatile ("int $0x20");
}

int new_process(unsigned int entry) {
  unsigned int i;
  for(i = 0; ptab[i]; i++);
  ptab[i] = malloc(sizeof(struct process));
  memset(ptab[i], 0, sizeof(struct process));
  ptab[i]->r = malloc(sizeof(struct registers));
  ptab[i]->r->eip = entry;
  ptab[i]->r->cs  = 0x2b;
  ptab[i]->r->ds = ptab[i]->r->es = ptab[i]->r->fs = ptab[i]->r->gs = ptab[i]->r->ss = 0x33;
  ptab[i]->stack_bot = malloc(PROCESS_STACK_SIZE);
  ptab[i]->r->useresp = (unsigned int) ptab[i]->stack_bot + PROCESS_STACK_SIZE - 16;
  ptab[i]->r->eflags = 0x200;
  ptab[i]->pt = malloc(sizeof(struct page_table));
  memset(ptab[i]->pt, 0, sizeof(struct page_table));
  unsigned int j;
  for(j = KERNEL_STACK_BOTTOM; j < KERNEL_STACK_TOP; j += 4096) {
    nonid_page(ptab[i]->pt, j / 4096);
  }
  return i;
}

void switch_ctx(regs_t r) {
  printd("CONTEXT SWITCH\n");
  if(mt_enabled > 1) 
    memcpy(ptab[cur_ctx]->r, r, sizeof(struct registers));
  int i;
  for(i = 0; i < 65536; i++) {
    if(ptab[i] && i > cur_ctx) goto no_rollover;
  }
  for(i = 0; !ptab[i]; i++);
 no_rollover:
  swap_page_table(ptab[cur_ctx]->pt, ptab[i]->pt);
  memcpy(r, ptab[i]->r, sizeof(struct registers));
  cur_ctx = i;
  mt_enabled = 2;		/* init bootstrap complete :) */
}
