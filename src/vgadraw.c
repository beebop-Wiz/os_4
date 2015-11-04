#include "vgadraw.h"
#include "vga.h"

void vga_rect(int x, int y, int w, int h, int color) {
  int i, j;
  for(i = x; i < x + w; i++) {
    for(j = y; j < y + h; j++) {
      vga_write_pix(i, j, color);
    }
  }
}
