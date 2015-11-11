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
  page_table_t kernel_pages = malloc(sizeof(struct page_table));
  int i;
  for(i = 0; i < 0x2000; i++) {
    id_page(kernel_pages, i);
  }
  nonid_page(kernel_pages, 0x2000);
}
