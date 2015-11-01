#include "vga.h"
#include "vgatext.h"
#include "version.h"
#include "idt.h"
#include "vbe.h"
#include "malloc.h"

void kernel_main() {
  vbe_load_data();
  init_vga();
  init_vgatext();
  vga_puts("Hello, World!\n");
  init_malloc();
  vga_puts("Booted os_4 " VERSION ".\n");
  setup_idt();
  asm volatile ("sti");
  asm volatile ("int $0x3");
  asm volatile ("cli");
  vga_puts("Attempting some MALLOC tests.\n");
  void *a = malloc(20);
  vga_itoa(a);
  vga_puts("\n");
  void *b = malloc(20);
  vga_itoa(b);
  vga_puts("\n");
  free(a);
  a = malloc(10);
  vga_itoa(a);
  vga_puts("\n");
  for(;;);
}
