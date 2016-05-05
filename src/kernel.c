#include "vga.h"
#include "vgatext.h"
#include "version.h"
#include "idt.h"
#include "vbe.h"
#include "malloc.h"
#include "paging.h"
#include "gdt.h"
#include "user.h"
#include "timer.h"
#include "mt.h"
#include "ext2.h"
#include "util.h"
#include "elf.h"
#include "pic.h"
#include "keyboard.h"
#include "syscall.h"
#include "log.h"
#include "fs/fs.h"

page_table_t kernel_pages = 0;

void kernel_main(unsigned int **bdata) {
  vbe_load_data();
  init_vga();
  init_vgatext();
  //  log(LOG_BOOT, LOG_INFO, "Hello, World!\n");
  init_malloc();
  setup_idt();
  PIC_init();
  init_paging();
  kernel_pages = malloc(sizeof(struct page_table));
  unsigned int i;
  for(i = 0; i < 0x2000; i++) {
    id_page(kernel_pages, i);
  }
  for(i = 0xFD000; i < 0xFFFFF; i++) {
    id_page(kernel_pages, i);
  }
  // now to set up the TSS for multitasking
  load_bios_gdt(bdata[1]);
  page_all_allocations();
  load_page_directory();
  timer_init(100);
  log(LOG_BOOT, LOG_INFO, "Booted os_4 %s\n", VERSION);
  init_mt();
  struct ext2_bg_desc desc;
  struct ext2_inode rd, sub;
  struct ext2_superblock superblock;
  int init_inode;
  read_superblock(&superblock);
  log(LOG_BOOT, LOG_INFO, "Read EXT2 superblock\n");
  log(LOG_BOOT, LOG_INFO, "\tversion %d.%d magic %x n_blocks %d\n\tblock size %d (%d sectors)\n",
	 superblock.vers_major,
	 superblock.vers_minor,
	 superblock.magic,
	 superblock.n_blocks,
	 superblock.block_size,
	 superblock.block_size / 2);
  log(LOG_BOOT, LOG_INFO, "Attempting to read BGD 0\n");
  read_block_group(&superblock, &desc, 0);
  log(LOG_BOOT, LOG_INFO, "\titab idx %d\n", desc.inode_table);
  log(LOG_BOOT, LOG_INFO, "Attempting to read inode 2\n");
  read_inode(&superblock, &rd, 2);
  char out[11];
  parse_inode_type(rd.type, out);
  log(LOG_BOOT, LOG_INFO, "\ttype %s atime %d n_sectors %d\n", out, rd.atime, rd.n_sectors);
  log(LOG_BOOT, LOG_INFO, "Looking for init... \n");
  read_inode(&superblock, &sub, init_inode = get_path_inode(&superblock, "boot/init.exe"));
  log(LOG_BOOT, LOG_INFO, "\tinit at %d\n", init_inode);
  /*printf("\n\nListing:\n");
    ext2_dirstate_t root = opendir(&rd);
  ext2_dirent_t d;
  while((d = dirwalk(root))) {
    if(d->nlen) {
      d->name[d->nlen] = 0;
      read_inode(&s, &sub, d->inode);
      parse_inode_type(sub.type, out);
      printf("%s %s %d\n", out, d->name, sub.size_low);
      if(streq(d->name, "init.exe")) break;
    }
  }
  closedir(root); */
  int init_pid = new_process(0);
  log(LOG_BOOT, LOG_INFO, "Init size: %d\n", sub.size_low);
  unsigned char *init_buf = malloc(sub.n_sectors * 512);
  log(LOG_BOOT, LOG_DEBUG, "Allocated init buffer\n");
  for(i = 0; i < sub.size_low; i += 1024) {
    get_block(&sub, i / 1024, init_buf + i);
  }
  struct elf_header init_header;
  struct pheader ph;
  memcpy(&init_header, init_buf, sizeof(struct elf_header));
  log(LOG_BOOT, LOG_INFO, "Read init ELF header\n");
  for(i = 0; i < init_header.ptidx; i++) {
    memcpy(&ph, init_buf + init_header.header_pos + i * init_header.ptsize, init_header.ptsize);
    log(LOG_BOOT, LOG_INFO, "\tProgram header %d: type %d memsz %x vaddr %x\n", i, ph.type, ph.p_memsz, ph.p_vaddr);
    unsigned int j;
    for(j = 0; j < ph.p_memsz; j += 4096) {
      nonid_page(get_process_pt(init_pid), (ph.p_vaddr + j) / 4096, 1);
      log(LOG_BOOT, LOG_INFO, "Paging in page %x (idx %x)\n", (j + ph.p_vaddr) / 4096, (j + ph.p_vaddr) / 4096 / 1024);
    }
    memcpy((void *) ph.p_vaddr, init_buf + ph.p_offset, ph.p_filesz);
  }
  set_process_entry(init_pid, init_header.entry);
  log(LOG_BOOT, LOG_INFO, "Init at %x\n", init_header.entry);
  vga_clear_text();
#ifdef USE_ASYNC
  init_async();
#endif
  setup_keyboard();
  log(LOG_BOOT, LOG_INFO, "Initializing system call handler\n");
  init_syscall();
  log(LOG_BOOT, LOG_INFO, "Initializing filesystem\n");
  fs_init();
  log(LOG_BOOT, LOG_INFO, "Starting MT.\n");
  enable_mt();
  for(;;) {
    asm("hlt");
    asm("cli");
  }
}
