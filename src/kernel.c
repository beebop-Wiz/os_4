#include "vga.h"

void kernel_main() {
  init_vga();
  int t, x, i;
  while(1) {
    for(t = 0; t < 128; t++) {
      for(x = 0; x < 128; x++) {
	vga_write_pix(x, x^t, 15);
      }
      for(i = 0; i < 10000000; i++) ;
    }
    vga_clear();
  }
}
