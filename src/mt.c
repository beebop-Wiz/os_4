#include "mt.h"
#include "malloc.h"
#include "util.h"
#include "idt.h"

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
  asm volatile ("int $0x20");
}

int new_process(unsigned int entry) {
  int i;
  for(i = 0; ptab[i]; i++);
  ptab[i] = malloc(sizeof(struct process));
  memset(ptab[i], 0, sizeof(struct process));
  ptab[i]->r = malloc(sizeof(regs_t));
  ptab[i]->r->eip = entry;
  ptab[i]->r->cs  = 0x2b;
  ptab[i]->r->ds = ptab[i]->r->es = ptab[i]->r->fs = ptab[i]->r->gs = ptab[i]->r->ss = 0x33;
  return i;
}

void switch_ctx(regs_t r) {
  printd("CONTEXT SWITCH\n");
  if(mt_enabled > 1) 
    memcpy(ptab[cur_ctx]->r, r, sizeof(regs_t));
  int i;
  for(i = 0; i < 65536; i++) {
    if(ptab[i] && i > cur_ctx) goto no_rollover;
  }
  for(i = 0; !ptab[i]; i++);
 no_rollover:
  swap_page_table(&ptab[cur_ctx]->pt, &ptab[i]->pt);
  memcpy(r, ptab[i]->r, sizeof(struct registers));
  mt_enabled = 2;		/* init bootstrap complete :) */
}
