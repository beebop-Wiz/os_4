#ifndef MT_H
#define MT_H

#define PROCESS_STACK_SIZE 0x10000

#include "idt.h"
#include "paging.h"

#define FD_MAX 256
#define FD_PRESENT 0x1
#define FD_READ    0x2
#define FD_WRITE   0x4
#define FD_TERM    0x8

struct process {
  page_table_t pt;
  regs_t r;
  int fds[FD_MAX];
  unsigned char *stack_bot;
};

void init_mt();
void enable_mt();
int new_process(unsigned int entry);
page_table_t get_process_pt(int proc);
void set_process_entry(int proc, unsigned int entry);
void switch_ctx(regs_t r);
void proc_exit();
#endif
