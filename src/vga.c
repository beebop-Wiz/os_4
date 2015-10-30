#include "vga.h"
#include "port.h"

unsigned char *vga = (unsigned char *) 0xA0000;

unsigned char palette[256][3] = {
  {0, 0, 0},
  {32, 0, 0},
  {0, 32, 0},
  {32, 32, 0},
  {0, 0, 32},
  {32, 0, 32},
  {0, 32, 32},
  {48, 48, 48},
  {32, 32, 32},
  {63, 0, 0},
  {0, 63, 0},
  {63, 63, 0},
  {0, 0, 63},
  {63, 0, 63},
  {0, 63, 63},
  {63, 63, 63}
};

void init_vga(void) {
// Load palette
  int pi;
  for(pi = 0; pi < 256; pi++) {
    outb(VGA_DAC1, pi);
    outb(VGA_DAC2, palette[pi][0]);
    outb(VGA_DAC2, palette[pi][1]);
    outb(VGA_DAC2, palette[pi][2]);
  }
  outb(VGA_ATTR, 0x20);
  int x, y;
  for(y = 0; y < 200; y++) {
    for(x = 0; x < 320; x++) {
      vga[320 * y + x] = 0;
    }
  }
  return;
}

void vga_clear() {
    int x, y;
  for(y = 0; y < 200; y++) {
    for(x = 0; x < 320; x++) {
      vga[320 * y + x] = 0;
    }
  }
}

void vga_write_pix(int x, int y, int color) {
  vga[320 * y + x] = color;
}

void vga_set_palette(int idx, unsigned char r, unsigned char g, unsigned char b) {
  outb(0x3c8, idx);
  outb(0x3c9, r);
  outb(0x3c9, g);
  outb(0x3c9, b);
}
