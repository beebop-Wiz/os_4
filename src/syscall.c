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
  case SYS_READ: // sys_read(fd, buf); reads 1 FS block (or less) from
		 // fd
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
    } else {
      int n_bytes = 0;
      while(((((char *) SYS_A2)[n_bytes] = keyboard_getchar()) > 0)
	    && (n_bytes <= 1024)) n_bytes++;
      SYS_RET(n_bytes);
    }
    break;
  case SYS_EXEC: // exec(path, argv[])
    printf("exec `%s`\n", (char *) SYS_A1);
    superblock = malloc(sizeof(struct ext2_superblock));
    read_superblock(superblock);
    read_inode(superblock, &inode, get_path_inode(superblock, (char *) SYS_A1));
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
    break;
  default: return;
  }
  printd("COMPLETE\n");
}
