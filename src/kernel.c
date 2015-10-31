#include "vga.h"
#include "vgatext.h"
#include "version.h"
#include "idt.h"
#include "vbe.h"

void kernel_main() {
  init_vga();
  init_vgatext();
  vga_puts("Hello, World!\n");
  vga_puts("Booted os_4 " VERSION ".\n");
  setup_idt();
  asm volatile ("sti");
  asm volatile ("int $0x3");
  asm volatile ("cli");
  vga_puts("Attempting to setup VBE.\n");
  vbe_load_data();
}
