#include "mt.h"
#include "malloc.h"
#include "util.h"
#include "idt.h"
#include "user.h"
#include "gdt.h"
#include "vgatext.h"
#include "log.h"

#ifdef DEBUG_MT
#define printd(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define printd(fmt, ...)
#endif

struct process *ptab[65536];
volatile int cur_ctx;
volatile unsigned short foreground;
int mt_enabled = 0;
extern page_table_t kernel_pages;

void init_mt() {
  int i;
  for(i = 0; i < 65536; i++) ptab[i] = 0;
  cur_ctx = 0;
}

void enable_mt() {
  mt_enabled = 1;
  // fake a IRQ to kick-start it
  tss_flush();
  printf("Started.\n");
  asm volatile ("int $0x20");
}

int new_process(unsigned int entry) {
  unsigned int i;
  for(i = 1; ptab[i]; i++);
  ptab[i] = malloc(sizeof(struct process));
  memset(ptab[i], 0, sizeof(struct process));
  ptab[i]->r = malloc(sizeof(struct registers));
  ptab[i]->r->eip = entry;
  ptab[i]->r->cs  = 0x2b;
  ptab[i]->r->ds = ptab[i]->r->es = ptab[i]->r->fs = ptab[i]->r->gs = ptab[i]->r->ss = 0x33;
  ptab[i]->r->useresp = PROCESS_STACK_TOP - 16;
  ptab[i]->r->eflags = 0x200;
  ptab[i]->pt = malloc(sizeof(struct page_table));
  memset(ptab[i]->pt, 0, sizeof(struct page_table));
  ptab[i]->fds = malloc(sizeof(struct fd) * FD_MAX);
  memset(ptab[i]->fds, 0, sizeof(struct fd) * FD_MAX);
  ptab[i]->suspend = SUS_RUNNING;
  ptab[i]->waitcnt = 0;
  ptab[i]->detach_stack = (unsigned int) malloc_a(DETACH_STACK_SIZE, 4096);
  unsigned int j;
  for(j = PROCESS_STACK_BOTTOM; j < PROCESS_STACK_TOP; j += 4096) {
    nonid_page(ptab[i]->pt, j / 4096, 0);
  }
  for(j = PROCESS_BRK_INITIAL; j < PROCESS_BRK_TOP; j += 4096) {
    nonid_page(ptab[i]->pt, j / 4096, 0);
  }
  ptab[i]->brk = PROCESS_BRK_INITIAL + PROCESS_BRK_INIT_SIZE;
  /*  for(j = KERNEL_STACK_BOTTOM; j < KERNEL_STACK_TOP; j += 4096) {
    nonid_page(ptab[i]->pt, j / 4096, 0);
    } */
  return i;
}

void dump_process_pt(page_table_t p) {
  while(p) {
    printf("|- PDI 0x%x\n", p->idx);
    int i;
    for(i = 0; i < 1024; i++) {
      if(get_mapping(p, p->idx * 1024 + i))
	printf("| |-0x%x -> 0x%x\n", p->idx * 1024 + i, get_mapping(p, p->idx * 1024 + i));
    }
    p = p->next;
  }
}

page_table_t get_process_pt(int proc) {
  return ptab[proc]->pt;
}

unsigned short fork(regs_t r) {
  int pid = new_process(r->eip);
  memcpy(ptab[pid]->r, r, sizeof(struct registers));
  page_table_t old;
  old = get_process_pt(cur_ctx);
  unsigned long t_loc;
  while(old) {
    int i;
    for(i = 0; i < 1024; i++) {
      mapped_page(ptab[pid]->pt, old->idx * 1024 + i, get_mapping(old, old->idx * 1024 + i), 1);
    }
    for(i = 0; i < 1024; i++) {
      if(!old->table[i]) continue;
      if(!get_mapping(old, old->idx * 1024 + i)) continue;
      t_loc = nonid_page(ptab[pid]->pt, 0x3000, 1);
      memcpy((void *) 0x3000000, (void *) (old->idx * 4096 * 1024) + i * 4096, 4096);
      mapped_page(ptab[pid]->pt, old->idx * 1024 + i, t_loc, 0);
      mapped_page(ptab[pid]->pt, 0x3000, 0, 0);
      update_page_table(ptab[pid]->pt);	
    } 
    old = old->next;
  }
  update_page_table(ptab[pid]->pt);
  int i;
  for(i = 0; i < FD_MAX; i++) {
    memcpy(&ptab[pid]->fds[i], &ptab[cur_ctx]->fds[i], sizeof(struct fd));
  }
  ptab[pid]->ppid = cur_ctx;
  ptab[pid]->pgid = ptab[cur_ctx]->pgid;
  ptab[pid]->sid = ptab[cur_ctx]->sid;
  
  ptab[pid]->suspend = SUS_RUNNING;
  printf("New pid: %d Old pid %d\n", pid, cur_ctx);
  ptab[pid]->r->eax = 0;
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

unsigned int calc_regs_cksum(regs_t r) {
  unsigned int v = 0;
  unsigned int i;
  for(i = 0; i < sizeof(struct registers); i++) {
    v ^= ((char *) r)[i] << (i % 24);
  }
  return v;
}

volatile int idx;

void switch_ctx(regs_t r) {
  if(!mt_enabled) return;
  printd("CONTEXT SWITCH\n");
  vga_statchar((idx++ + ' '), 30);
  idx %= 80;
  if(mt_enabled > 1 && ptab[cur_ctx]) {
    printd("Old cksum (%d) %x => %x\n", cur_ctx, ptab[cur_ctx]->regs_cksum, calc_regs_cksum(r));
    ptab[cur_ctx]->regs_cksum = calc_regs_cksum(r);
    memcpy(ptab[cur_ctx]->r, r, sizeof(struct registers));
  }
  int i, j;
  for(i = 1; i < 65536; i++) {
    if(ptab[i] && !ptab[i]->suspend && i > cur_ctx) goto done;
  }
  for(i = 1; (!ptab[i] || ptab[i]->suspend) && i < 65536; i++);
  if(i > 65535) return;
 done:
  if(i != cur_ctx) {
    if(ptab[cur_ctx]) {
      swap_page_table(ptab[cur_ctx]->pt, ptab[i]->pt);
    } else {
      swap_page_table((page_table_t) 0, ptab[i]->pt);
    }
  }
  for(j = 1; j < 16; j++) {
    vga_statchar('.', j);
    if(ptab[j]) vga_statchar(j+'0', j);
  }
  vga_statchar('*', i);

  memcpy(r, ptab[i]->r, sizeof(struct registers));
  //  log(LOG_MT, LOG_DEBUG, "New cksum (%d) %x => %x\n", i, ptab[i]->regs_cksum, calc_regs_cksum(r));
  ptab[i]->regs_cksum = calc_regs_cksum(r);
  log(LOG_MT, LOG_DEBUG, "loaded ctx #%d (%x)\n", cur_ctx, r->eip);
  if(ptab[i]->waitcnt) {
    ptab[i]->suspend &= ~SUS_WAIT;
    ptab[i]->waitcnt = 0;
  }
  set_kernel_stack(ptab[i]->detach_stack);
  cur_ctx = i;
  mt_enabled = 2;		/* init bootstrap complete :) */
}

void free_ptab(page_table_t pt) {
  if(pt->next) free_ptab(pt->next);
  free(pt->table);
  free(pt);
}

void proc_exit(regs_t r) {
  log(LOG_MT, LOG_INFO, "Process %d exiting...\n", cur_ctx);
  clear_wait(cur_ctx, 0x0100);
  free(ptab[cur_ctx]->fds);
  free_ptab(ptab[cur_ctx]->pt);
  log(LOG_MT, LOG_DEBUG, "Freed page table\n");
  free(ptab[cur_ctx]->r);
  log(LOG_MT, LOG_DEBUG, "Freed regs\n");
  free((void *) ptab[cur_ctx]->detach_stack);
  free(ptab[cur_ctx]);
  ptab[cur_ctx] = 0;
  log(LOG_MT, LOG_INFO, "done\n");
  switch_ctx(r);
}

void set_foreground(unsigned short proc) {
  foreground = proc;
}

unsigned short get_foreground() {
  return foreground;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void signal_foreground(int signum) {
  
  //  queue_callback(foreground, ASYNC_TYPE_PSIG, 1, signum);
}
#pragma GCC diagnostic pop

void clear_wait(unsigned short proc, unsigned short status) {
  if(!ptab[ptab[proc]->ppid]) {
    log(LOG_MT, LOG_INFO, "Can't find parent of process %d\n", proc);
    return;
  }
  ptab[ptab[proc]->ppid]->waitpid = 0;
  ptab[ptab[proc]->ppid]->waitflags = 0;
  if(ptab[ptab[proc]->ppid]->suspend & SUS_WAIT)
    ptab[ptab[proc]->ppid]->r->ecx = (status << 16) | proc;
  ptab[ptab[proc]->ppid]->waitcnt++;
  ptab[ptab[proc]->ppid]->suspend &= ~SUS_WAIT;
      
}
