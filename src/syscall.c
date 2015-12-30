#include "syscall.h"
#include "idt.h"
#include "vgatext.h"
#include "mt.h"
#include "malloc.h"
#include "ext2.h"
#include "elf.h"
#include "util.h"
#include "debug.h"
#include "keyboard.h"

#ifdef DEBUG_SYSCALL
#define printd(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define printd(fmt, ...)
#endif

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
  printd("SYSCALL: %d %x %x %x\n", SYS_NO, SYS_A1, SYS_A2, SYS_A3);
  unsigned int i;
  switch(SYS_NO) {
  case SYS_WRITE: // 0
    printf("%s", (char *) SYS_A1);
    vga_redraw();
    break;
  case SYS_EXIT: // 1
    proc_exit(r);
    break;
  case SYS_MALLOC: // 2
    SYS_RET((unsigned int) malloc(SYS_A1));
    break;
  case SYS_FREE: // 3
    free((void *) SYS_A1);
    break;
  case SYS_GETFD: // 4
    for(i = 0; i < FD_MAX; i++) {
      if(!(ptab[cur_ctx]->fds[i] & FD_PRESENT)) {
	ptab[cur_ctx]->fds[i] |= FD_PRESENT;
	SYS_RET(i);
	return;
      }
    }
    SYS_RET(-1);
    break;
  case SYS_FORK: // 5
    r->ecx = fork(r);
    if((unsigned) cur_ctx != r->ecx)
      switch_ctx(r);
    break;
  case SYS_BINDFD:
    ptab[cur_ctx]->fds[SYS_A2] |= FD_BOUND;
    struct ext2_superblock *superblock = malloc(sizeof(struct ext2_superblock));
    read_superblock(superblock);
    printd("Read superblock\n");
    ptab[cur_ctx]->bound[SYS_A2].inode = get_path_inode(superblock, (char *) SYS_A1);
    struct ext2_inode inode;
    read_inode(superblock, &inode, ptab[cur_ctx]->bound[SYS_A2].inode);
    printd("Read inode (%d)\n", ptab[cur_ctx]->bound[SYS_A2].inode);
    ptab[cur_ctx]->bound[SYS_A2].off = 0;
    ptab[cur_ctx]->bound[SYS_A2].size = inode.size_low;
    free(superblock);
    break;
  case SYS_READ: // sys_read(fd, buf, id); reads 1 FS block (or
		 // less) from fd
    if(SYS_A1) {
      superblock = malloc(sizeof(struct ext2_superblock));
      read_superblock(superblock);
      printd("Read superblock\n");
      read_inode(superblock, &inode, ptab[cur_ctx]->bound[SYS_A1].inode);
      int block_idx = ptab[cur_ctx]->bound[SYS_A1].off / 1024;
      int n_bytes = ptab[cur_ctx]->bound[SYS_A1].size - ptab[cur_ctx]->bound[SYS_A1].off;
      ptab[cur_ctx]->bound[SYS_A1].off += n_bytes;
      get_block(&inode, block_idx, (unsigned char *) SYS_A2);
      if(n_bytes > 1024) n_bytes = 1024;
      else ((char *) SYS_A2)[n_bytes++] = -1;
      SYS_RET(n_bytes);
      printd("Read completed\n");
      free(superblock);
      ptab[cur_ctx]->async_callbacks[ASYNC_TYPE_IO].generated = 0;
      ptab[cur_ctx]->async_callbacks[ASYNC_TYPE_IO].id = SYS_A3;
      queue_callback(cur_ctx, ASYNC_TYPE_IO, SYS_A3, n_bytes);
    } else {
      ptab[cur_ctx]->async_callbacks[ASYNC_TYPE_IO].generated = 0;
      ptab[cur_ctx]->async_callbacks[ASYNC_TYPE_IO].id = SYS_A3;
    }
    break;
  case SYS_EXEC: // exec(path, argv[], envp[])
    printd("exec `%s`\n", (char *) SYS_A1);
    superblock = malloc(sizeof(struct ext2_superblock));
    read_superblock(superblock);
    int inode_num = get_path_inode(superblock, (char *) SYS_A1);
    if(inode_num < 0) {
      SYS_RET(inode_num);
      break;
    }
    read_inode(superblock, &inode, inode_num);
    unsigned char *prog_buf = malloc(inode.n_sectors * 512);
    for(i = 0; i < inode.size_low; i += 1024)
      get_block(&inode, i / 1024, prog_buf + i);
    struct elf_header prog_header;
    struct pheader ph;
    memcpy(&prog_header, prog_buf, sizeof(struct elf_header));
    for(i = 0; i < prog_header.ptidx; i++) {
      memcpy(&ph, prog_buf + prog_header.header_pos + i * prog_header.ptsize, prog_header.ptsize);
      unsigned int j;
      for(j = 0; j < ph.p_memsz; j += 4096) {
	nonid_page(get_process_pt(cur_ctx), (ph.p_vaddr + j) / 4096, 1);
      }
      memcpy((void *) ph.p_vaddr, prog_buf + ph.p_offset, ph.p_filesz);
    }
    r->eip = prog_header.entry;
    r->esi = SYS_A3;
    for(i = 0; i < FD_MAX; i++) ptab[cur_ctx]->fds[i] = 0;
    unsigned int argc;
    char **argv_orig = (char **) SYS_A2;
    for(argc = 0; *argv_orig++; argc++);
    char **argv = (char **) malloc(argc * sizeof(char *));
    argv_orig = (char **) SYS_A2;
    for(i = 0; i < argc; i++) {
      argv[i] = malloc(strlen(argv_orig[i]) + 1);
      memcpy(argv[i], argv_orig[i], strlen(argv_orig[i]) + 1);
      argv[i][strlen(argv[i])] = 0;
    }
    r->eax = argc;
    r->ebx = (unsigned int) argv;
    break;
  case SYS_WAIT:
    if(SYS_A1 == (unsigned int) SUS_SCHED) {
      switch_ctx(r);
    } else {
      ptab[cur_ctx]->suspend |= SYS_A1;
    }
    if(SYS_A2)
      switch_ctx(r);
    break;
  case SYS_SET_HANDLER:
    ptab[cur_ctx]->async_callbacks[SYS_A1].callback = (void (*)(unsigned int, unsigned int)) SYS_A2;
    break;
    /*  case SYS_WAIT_ASYNC:
    ptab[cur_ctx]->async_mask |= 1 << SYS_A1;
    switch_ctx(r);
    break; */
  case SYS_KILL:
    queue_callback(SYS_A1, ASYNC_TYPE_PSIG, 0, SYS_A2);
    break;
  case SYS_TCSETPGRP:
    set_foreground(SYS_A1);
    break;
  case SYS_GETPID:
    SYS_RET(cur_ctx);
    break;
  default: return;
  }
  printd("COMPLETE\n");
}
