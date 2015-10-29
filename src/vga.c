#include "vga.h"
#include "port.h"

unsigned char *vga = (unsigned char *) 0xA0000;

unsigned char palette[256][3] = {
  {0, 32, 0},
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

unsigned short mode13h[] = {
  0x63, 0x00, 0x70, 0x04, // General
  0x03, 0x01, 0x0f, 0x00, 0x0e, // Sequence
  0x5f, 0x4f, 0x50, 0x82, 0x24, 0x80, 0xbf, 0x1f, 0x00, 0x41,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x31, 0x9c, 0x8e, 0x8f, 0x28,
  0x40, 0x96, 0x89, 0xa3, 0xff, // CRTC
  0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0f, 0xff, // GC
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
  0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x41, 0x00, 0x0f, 0x00,
  0x00 // Attribute
};

void write_attr(unsigned char idx, unsigned char val) {
  inb(VGA_IDX_RST);
  outb(VGA_ATTR, idx);
  outb(VGA_ATTR, val);
}

void init_vga(void) {
  /*  outb(VGA_MOR, mode13h[0]);
  outb(0x03ba, mode13h[1]);
  int i;
  for(i = 0; i < N_SEQ_IDX; i++)
    vga_write_reg(VGA_SEQ, i, mode13h[4 + i]);
  for(i = 0; i < N_CRTC_IDX; i++)
    vga_write_reg(VGA_CRTC, i, mode13h[4 + N_SEQ_IDX + i]);
  for(i = 0; i < N_GC_IDX; i++)
    vga_write_reg(VGA_GC, i, mode13h[4 + N_SEQ_IDX + N_CRTC_IDX] + i);
  for(i = 0; i < N_ATTR_IDX; i++)
    write_attr(i, mode13h[4 + N_SEQ_IDX + N_CRTC_IDX + N_GC_IDX + i]);
  */
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
      vga[320 * y + x] = 1;
    }
  }
  return;
}

void vga_write_reg(unsigned short port, unsigned char idx, unsigned char dat) {
  outb(port, idx);
  outb(port + 1, idx);
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
