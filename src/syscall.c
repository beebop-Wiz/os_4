#include "syscall.h"
#include "idt.h"
#include "vgatext.h"
#include "mt.h"
#include "malloc.h"
#include "ext2.h"
#include "elf.h"
#include "util.h"
#include "log.h"
#include "keyboard.h"
#include "fs/fs.h"

extern volatile int cur_ctx;
extern struct process *ptab[65536];
volatile int sys_detached = 0;

#define NUM_UNIX_SYSCALLS 190
void (*unix_syscalls[NUM_UNIX_SYSCALLS])(regs_t r);

void usys_exit(regs_t r) {
  proc_exit(r);
}

void usys_fork(regs_t r) {
  r->eax = fork(r);
  if((unsigned) cur_ctx != r->eax)
    switch_ctx(r);
}

void usys_read(regs_t r) {
  r->eax = fs_read(r->ebx, (char *) r->ecx, r->edx);
}

void usys_write(regs_t r) {
  log(LOG_SYSCALL, LOG_INFO, "write(%d, `%s', %d)\n", r->ebx, (char *) r->ecx, r->edx);
  r->eax = fs_write(r->ebx, (char *) r->ecx, r->edx);
}

void usys_open(regs_t r) {
  r->eax = fs_open((char *) r->ebx, r->ecx);
}

void usys_close(regs_t r) {
  r->eax = fs_close(r->ebx);
}

void usys_waitpid(regs_t r) {
  if(r->ebx == (unsigned short) -1) {
    ptab[cur_ctx]->suspend |= SUS_WAIT;
    ptab[cur_ctx]->waitcnt = 0;
    switch_ctx(r);
  } else {
    log(LOG_SYSCALL, LOG_FAILURE, "waitpid() on pid != -1 unsupported (got %d)\n", r->ebx);
  }
}

void usys_execve(regs_t r) {
  struct ext2_superblock *superblock = malloc(sizeof(struct ext2_superblock));
  struct ext2_inode inode;
  int inum;
  unsigned int i;
  read_superblock(superblock);
  inum = get_path_inode(superblock, (char *) r->ebx);
  if(inum < 0) {
    r->eax = 0;
    log(LOG_SYSCALL, LOG_FAILURE, "execve() failed (fname `%s' has inode %d)\n", (char *) r->ebx, inum); 
    return;
  }
  read_inode(superblock, &inode, inum);
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
  // ebx = filename, ecx = argv, edx = envp
  // argc = eax, argv = ebx, envp = esi
  char **argv = (char **) r->ecx;
  for(r->eax = 0; argv[r->eax]; r->eax++);
  r->esi = r->edx;
  r->ebx = r->ecx;
  log(LOG_SYSCALL, LOG_INFO, "exec: argc = %d argv = %x\n", r->eax, r->ebx);
  log(LOG_SYSCALL, LOG_INFO, "execve() complete\n");
}

void usys_brk(regs_t r) {
    if(r->ebx < PROCESS_BRK_INITIAL) {
    r->eax = ptab[cur_ctx]->brk;
    return;
  } else {
    int bdiff = r->ebx - ptab[cur_ctx]->brk, i;
    if(bdiff < 0) { // deallocate
      bdiff = -bdiff;
      for(i = 0; i < bdiff; i += 4096) {
	unmap_page(ptab[cur_ctx]->pt, (ptab[cur_ctx]->brk - i) / 4096);
      }
    } else {
      for(i = 0; i < bdiff; i += 4096) {
	if(!get_mapping(ptab[cur_ctx]->pt, (ptab[cur_ctx]->brk + i) / 4096))
	   nonid_page(ptab[cur_ctx]->pt, (ptab[cur_ctx]->brk + i) / 4096, 1);
      }
      if(!get_mapping(ptab[cur_ctx]->pt, (ptab[cur_ctx]->brk + i) / 4096))
	nonid_page(ptab[cur_ctx]->pt, (ptab[cur_ctx]->brk + i) / 4096, 1);
    }
    r->eax = ptab[cur_ctx]->brk = r->ebx;
  }		  
}

void usys_setpgid(regs_t r) {
  if(!r->eax) r->eax = cur_ctx;
  if(!r->ebx) r->ebx = cur_ctx;
  ptab[r->eax]->pgid = r->ebx;
}

void usys_setsid(regs_t r) {
  ptab[cur_ctx]->sid = cur_ctx;
  r->eax = cur_ctx;
}

void syscall_unix(regs_t r) {
  if(unix_syscalls[r->eax]) unix_syscalls[r->eax](r);
  else log(LOG_SYSCALL, LOG_FAILURE, "Unknown UNIX syscall number %d\n", r->eax);
}

volatile unsigned long sid = 0;

void do_syscall(regs_t r) {
  unsigned long this_id = sid++;
  log(LOG_SYSCALL, LOG_INFO, "%x %s %x %x %x %x %x %x %d %d\n", r, r->int_no == 0x80 ? "unix" : "os4", r->eax, r->ebx, r->ecx, r->edx, r->esi, r->edi, cur_ctx, this_id);
  if(r->int_no == 0x80) {
    syscall_unix(r);
  }
  log(LOG_SYSCALL, LOG_INFO, "COMPLETE %d\n", this_id);
}

// Did you update syscalls.txt?

void init_syscall() {
  unix_syscalls[1] = usys_exit;
  unix_syscalls[2] = usys_fork;
  unix_syscalls[3] = usys_read;
  unix_syscalls[4] = usys_write;
  unix_syscalls[5] = usys_open;
  unix_syscalls[6] = usys_close;
  unix_syscalls[7] = usys_waitpid;
  unix_syscalls[11] = usys_execve;
  unix_syscalls[45] = usys_brk;
  unix_syscalls[57] = usys_setpgid;
  unix_syscalls[66] = usys_setsid;
}
