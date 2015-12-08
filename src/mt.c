#include "mt.h"
#include "malloc.h"
#include "util.h"
#include "idt.h"
#include "user.h"
#include "gdt.h"
#include "vgatext.h"

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
  memset(ptab[i]->fds, 0, FD_MAX * sizeof(int));
  ptab[i]->r = malloc(sizeof(struct registers));
  ptab[i]->r->eip = entry;
  ptab[i]->r->cs  = 0x2b;
  ptab[i]->r->ds = ptab[i]->r->es = ptab[i]->r->fs = ptab[i]->r->gs = ptab[i]->r->ss = 0x33;
  ptab[i]->r->useresp = PROCESS_STACK_TOP - 16;
  ptab[i]->r->eflags = 0x200;
  ptab[i]->pt = malloc(sizeof(struct page_table));
  memset(ptab[i]->pt, 0, sizeof(struct page_table));
  unsigned int j;
  for(j = PROCESS_STACK_BOTTOM; j < PROCESS_STACK_TOP; j += 4096) {
    nonid_page(ptab[i]->pt, j / 4096, 0);
  }
  /*  for(j = KERNEL_STACK_BOTTOM; j < KERNEL_STACK_TOP; j += 4096) {
    nonid_page(ptab[i]->pt, j / 4096, 0);
    } */
  return i;
}

page_table_t get_process_pt(int proc) {
  return ptab[proc]->pt;
}

unsigned short fork(regs_t r) {
  int pid = new_process(r->eip);
  memcpy(ptab[pid]->r, r, sizeof(struct registers));
  page_table_t old;
  old = get_process_pt(cur_ctx);
  unsigned int t_loc;
  while(old) {
    int i;
    printd("Copying PT for idx %x\n", old->idx);
    for(i = 0; i < 1024; i++) {
      mapped_page(ptab[pid]->pt, old->idx * 1024 + i, get_mapping(old, old->idx * 1024 + i), 1);
    }
    for(i = 0; i < 1024; i++) {
      if(!old->table[i]) continue;
      t_loc = nonid_page(ptab[pid]->pt, 0x2000, 1);
      printd("Copying %x to %x\n", (old->idx * 4096 * 1024) + i * 4096, 0x2000000);
      memcpy((void *) 0x2000000, (void *) (old->idx * 4096 * 1024) + i * 4096, 4096);
      mapped_page(ptab[pid]->pt, old->idx * 1024 + i, t_loc, 0);
      mapped_page(ptab[pid]->pt, 0x2000, 0, 0);
      update_page_table(ptab[pid]->pt);	
    }
    old = old->next;
  }
  printd("New pid: %d Old pid %d\n", pid, cur_ctx);
  ptab[pid]->r->ecx = 0;
  return pid;
}

void set_process_entry(int proc, unsigned int entry) {
  ptab[proc]->r->eip = entry;
}

void shutdown() {
  printf("\nNo processes remain.\nShutting down...\n");
  printf("Will now halt.\n");
  for(;;) {
    asm ("cli\nhlt");
  }
}

void switch_ctx(regs_t r) {
  printd("CONTEXT SWITCH\n");
  if(mt_enabled > 1 && ptab[cur_ctx]) 
    memcpy(ptab[cur_ctx]->r, r, sizeof(struct registers));
  int i;
  for(i = 0; i < 65536; i++) {
    if(ptab[i] && i > cur_ctx) goto no_rollover;
  }
  for(i = 0; !ptab[i] && i < 65536; i++);
  if(i > 65535) shutdown();
 no_rollover:
  if(ptab[cur_ctx]) {
    swap_page_table(ptab[cur_ctx]->pt, ptab[i]->pt);
  } else {
    swap_page_table((page_table_t) 0, ptab[i]->pt);
  }
  memcpy(r, ptab[i]->r, sizeof(struct registers));
  cur_ctx = i;
  mt_enabled = 2;		/* init bootstrap complete :) */
  printd("loaded ctx %d\n", i);
}

void free_ptab(page_table_t pt) {
  if(pt->next) free_ptab(pt->next);
  free(pt->table);
  free(pt);
}

void proc_exit(regs_t r) {
  free_ptab(ptab[cur_ctx]->pt);
  free(ptab[cur_ctx]->r);
  free(ptab[cur_ctx]);
  ptab[cur_ctx] = 0;
  switch_ctx(r);
}
