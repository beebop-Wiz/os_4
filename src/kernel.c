#include "vga.h"
#include "vgatext.h"
#include "version.h"
#include "idt.h"
#include "vbe.h"

void kernel_main() {
  vbe_load_data();
  init_vga();
  init_vgatext();
  *((unsigned int *) 0xFD000000) = 0xFFFFFFFF;
  vga_write_pix(10, 10, 0xeeeeee);
  vga_puts("000000000011111111112222222222333333333344444444445555555555666666666677777777778888888888999999999\n");
  vga_puts("012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678\n");
  vga_puts("Hello, World!\n");
  vga_puts("Booted os_4 " VERSION ".\n");
  setup_idt();
  asm volatile ("sti");
  asm volatile ("int $0x3");
  asm volatile ("cli");
  for(;;);
}
