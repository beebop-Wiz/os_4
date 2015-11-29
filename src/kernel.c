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
  new_process((unsigned int) test_mt);
  new_process((unsigned int) test_mt_2);
  printf("Enabling MT\n");
  enable_mt();
  //jump_usermode();
}
