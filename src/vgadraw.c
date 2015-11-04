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

extern unsigned char *vga;

void vga_clearcolor(int color) {
  unsigned int b1 = (color & 0xFFFFFF) | ((color & 0xFF) << 24);
  unsigned int b2 = ((color & 0xFFFF00) >> 8) | ((color & 0xFFFF) << 16);
  unsigned int b3 = ((color & 0xFFFFFF) << 8) | ((color & 0xFF0000) >> 16);
  int i;
  for(i = 0; i < 800 * 600 * 3; i += 3) {
    vga[i] = b1;
    vga[i + 1] = b2;
    vga[i + 2] = b3;
  }
}
