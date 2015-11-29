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

page_table_t kernel_pages = 0;

void test_mt() {
  char *foo = "foo\n";
  int i;
  for(;;) {
    asm ("mov $0, %%eax\nmov %0, %%ebx\nint $0x81" : : "m" (foo) : "eax", "ebx");
    for(i = 0; i < 100000000; i++) ;
  // no way to return to kcode right now
  }
}

void test_mt_2() {
  char *foo = "bar\n";
  int i;
  for(;;) {
    asm ("mov $0, %%eax\nmov %0, %%ebx\nint $0x81" : : "m" (foo) : "eax", "ebx");
    for(i = 0; i < 100000000; i++) ;
  // no way to return to kcode right now
  }
}


void kernel_main(unsigned int **bdata) {
  vbe_load_data();
  init_vga();
  init_vgatext();
  printf("Hello, World!\n");
  init_malloc();
  setup_idt();
  PIC_init();
  init_paging();
  kernel_pages = malloc(sizeof(struct page_table));
  int i;
  for(i = 0; i < 0x2000; i++) {
    id_page(kernel_pages, i);
  }
  for(i = 0xFD000; i < 0xFFFFF; i++) {
    id_page(kernel_pages, i);
  }
  nonid_page(kernel_pages, 0x2000);
  // now to set up the TSS for multitasking
  load_bios_gdt(bdata[1]);
  page_all_allocations();
  load_page_directory();
  timer_init(1);
  printf("Booted os_4 %s\n", VERSION);
  init_mt();
  //  new_process((unsigned int) test_mt);
  //  new_process((unsigned int) test_mt_2);
  //  printf("Enabling MT\n");
  //  enable_mt();
  struct ext2_superblock s;
  struct ext2_bg_desc desc;
  struct ext2_inode rd;
  read_superblock(&s);
  printf("Read EXT2 superblock\n");
  printf("\tversion %d.%d magic %x n_blocks %d\n\tblock size %d (%d sectors)\n", s.vers_major, s.vers_minor, s.magic, s.n_blocks, s.block_size, s.block_size / 2);
  printf("Attempting to read BGD 0\n");
  read_block_group(&s, &desc, 0);
  printf("\titab idx %d\n", desc.inode_table);
  printf("Attempting to read inode 2\n");
  read_inode(&s, &rd, 2);
  printf("\ttype %x atime %d n_sectors %d\n", rd.type, rd.atime, rd.n_sectors);
  for(;;) {
    asm("hlt");
    asm("cli");
  }
}
