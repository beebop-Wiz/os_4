#include "vga.h"

void kernel_main() {
  init_vga();
  vga_write_pix(100, 100, 0xff);
}
