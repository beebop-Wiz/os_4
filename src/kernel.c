#include "vga.h"

void kernel_main() {
  *((unsigned short *) 0xBF000) = 0x7777;
  /*  init_vga();
      vga_write_pix(100, 100, 0xff);*/
}
