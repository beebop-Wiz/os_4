#include "mt.h"
#include "malloc.h"
#include "util.h"
#include "idt.h"

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
}

int new_process(unsigned int entry) {
  int i;
  for(i = 0; ptab[i]; i++);
  ptab[i] = malloc(sizeof(struct process));
  memset(ptab[i], 0, sizeof(struct process));
  ptab[i]->r.eip = entry;
  ptab[i]->r.cs  = 0x2b;
  return i;
}

void switch_ctx(regs_t r) {
  memcpy(&ptab[cur_ctx]->r, &r, sizeof(regs_t));
  int i;
  for(i = 0; i < 65536; i++) {
    if(ptab[i] && i > cur_ctx) goto no_rollover;
  }
  for(i = 0; ptab[i]; i++);
 no_rollover:
  swap_page_table(&ptab[cur_ctx]->pt, &ptab[i]->pt);
  memcpy(&r, &ptab[i]->r, sizeof(regs_t));
}
