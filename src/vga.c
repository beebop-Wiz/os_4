#include "vga.h"
#include "vgatext.h"
#include "port.h"

#define VGA_WIDTH 800
#define VGA_HEIGHT 600

#define VGA_LOC(x, y) ((unsigned int *) (vga + (VGA_WIDTH * y + x) * 3))

unsigned char *vga = (unsigned char *) 0xFD000000;

void init_vga(void) {
  int x, y;
  for(y = 0; y < VGA_HEIGHT; y++) {
    for(x = 0; x < VGA_WIDTH; x++) {
      *VGA_LOC(x, y) = 0;
    }
  }
  return;
}

#ifdef KERNEL_SOURCE

void vga_clear() {
  int x, y;
  for(y = 0; y < 600; y++) {
    for(x = 0; x < 800; x++) {
      *VGA_LOC(x, y) = 0;
    }
  }
}

#endif

void vga_write_pix(int x, int y, int color) {
  unsigned int *begin = VGA_LOC(x, y);
  *begin &= 0xFF000000;
  *begin |= color;
}

#ifdef KERNEL_SOURCE

int vga_get_pix(int x, int y) {
  unsigned char *b = (vga + (VGA_WIDTH * y + x) * 3);
  unsigned char *g = b + 1;
  unsigned char *r = b + 2;
  int c = *b;
  c |= *g << 8;
  c |= *r << 16;
  return c;
}

void vga_set_palette(int idx, unsigned char r, unsigned char g, unsigned char b) {
  outb(0x3c8, idx);
  outb(0x3c9, r);
  outb(0x3c9, g);
  outb(0x3c9, b);
}

#endif
