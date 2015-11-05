#include "vga.h"
#include "vgatext.h"
#include "version.h"
#include "idt.h"
#include "vbe.h"
#include "malloc.h"
#include "paging.h"

void kernel_main() {
  vbe_load_data();
  init_vga();
  init_vgatext();
  printf("Hello, World!\n");
  init_malloc();
  printf("Booted os_4 %s\n", VERSION);
  setup_idt();
  printf("Testing malloc() and free()\n");
  void *a = malloc(20);
  printf("A = %x\n", a);
  void *b = malloc(20);
  printf("B = %x\n", b);
  free(a);
  a = malloc(10);
  printf("A = %x\n", a);
  b = malloc_a(10, 4096);
  printf("B = %x\n", b);
  init_paging();
  get_page_block(0);
  get_page_block(14);
  get_page_block(2);
  get_page_block(17);
}
