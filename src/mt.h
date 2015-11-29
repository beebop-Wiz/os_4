#ifndef MT_H
#define MT_H

#define PROCESS_STACK_SIZE 0x10000

#include "idt.h"
#include "paging.h"

struct process {
  page_table_t pt;
  regs_t r;
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
