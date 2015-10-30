#include "vga.h"
#include "vgatext.h"
#include "version.h"

void kernel_main() {
  init_vga();
  init_vgatext();
  vga_puts("Hello, World!\n");
  vga_puts("Booted os_4 " VERSION ".\n");
}
