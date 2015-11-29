#ifndef MT_H
#define MT_H

#include "idt.h"
#include "paging.h"

struct process {
  struct page_table pt;
  regs_t r;
};

void init_mt();
void enable_mt();
int new_process(unsigned int entry);
void switch_ctx(regs_t r);
#endif
