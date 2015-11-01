#include "vga.h"
#include "port.h"

#define VGA_WIDTH 800
#define VGA_HEIGHT 600

#define VGA_LOC(x, y) ((unsigned int *) (vga + (VGA_WIDTH * y + x) * 3))

unsigned char *vga = (unsigned char *) 0xFD000000;

void init_vga(void) {
  vga_itoa(VGA_LOC(1, 0));
  int x, y;
  for(y = 0; y < VGA_HEIGHT; y++) {
    for(x = 0; x < VGA_WIDTH; x++) {
      *VGA_LOC(x, y) = 0;
    }
  }
  return;
}

void vga_clear() {
    int x, y;
  for(y = 0; y < 600; y++) {
    for(x = 0; x < 800; x++) {
      *VGA_LOC(x, y) = 0;
    }
  }
}

void vga_write_pix(int x, int y, int color) {
  *VGA_LOC(x, y) = color;
}

void vga_set_palette(int idx, unsigned char r, unsigned char g, unsigned char b) {
  outb(0x3c8, idx);
  outb(0x3c9, r);
  outb(0x3c9, g);
  outb(0x3c9, b);
}
