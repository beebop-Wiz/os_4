#ifndef MT_H
#define MT_H

#define PROCESS_STACK_SIZE 0x10000
#define PROCESS_STACK_BOTTOM 0x0C000000
#define PROCESS_STACK_TOP (PROCESS_STACK_BOTTOM + PROCESS_STACK_SIZE)

#define PROCESS_BRK_INITIAL (PROCESS_STACK_TOP + 1024)
#define PROCESS_BRK_INIT_SIZE 0x10000
#define PROCESS_BRK_TOP (PROCESS_BRK_INITIAL + PROCESS_BRK_INIT_SIZE)

#define DETACH_STACK_SIZE 0x1000

#include "idt.h"
#include "paging.h"

#define FD_MAX 256
#define FD_PRESENT 0x01
#define FD_READ    0x02
#define FD_WRITE   0x04
#define FD_TERM    0x08
#define FD_BOUND   0x10

#define SUS_SCHED   -1
#define SUS_RUNNING 0
#define SUS_STOPPED 0x1
#define SUS_WAIT    0x2

#ifdef USE_ASYNC
#include "async.h"
#endif


#include "fs/fs.h"

struct process {
  page_table_t pt;
  unsigned int brk;
  regs_t r;
  unsigned int regs_cksum;
  unsigned char suspend;
  unsigned short waitpid, waitflags, waitcnt;
  unsigned short ppid, pgid, sid;
  struct fd *fds;
  unsigned int detach_stack;
};

void init_mt();
void enable_mt();
int new_process(unsigned int entry);
page_table_t get_process_pt(int proc);
void set_process_entry(int proc, unsigned int entry);
void switch_ctx(regs_t r);
void proc_exit();
unsigned short fork(regs_t r);
char *get_process_stack(int proc);
void queue_callback(int proc, int cbtype, unsigned int id, unsigned int data);
void set_foreground(unsigned short proc);
unsigned short get_foreground(void);
void signal_foreground(int signum);
void clear_wait(unsigned short proc, unsigned short status);

#endif
