#include "vga.h"
#include "port.h"

unsigned char *vga = (unsigned char *) 0xA0000;

unsigned char palette[256][3] = {
  {0, 7, 0},
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

void write_attr(unsigned char idx, unsigned char val) {
  inb(VGA_IDX_RST);
  outb(VGA_ATTR, idx);
  outb(VGA_ATTR, val);
}

void init_vga(void) {
  write_attr(ATTR_MCTL, MCTL_8BIT | MCTL_ATGE);
  write_attr(ATTR_OSCN, 4);
  write_attr(ATTR_CPE, 0xf);
  write_attr(ATTR_HPAN, 0);
  write_attr(ATTR_CSEL, 0);
  outb(VGA_MOR,
       MOR_HSYNCP | MOR_OEPAGE | MOR_CLOCK(0) | MOR_RAMEN | MOR_IOAS);
  vga_write_reg(VGA_SEQ, SEQ_CMODE, CMODE_98DM);
  vga_write_reg(VGA_SEQ, SEQ_CSEL, CSEL_A(0) | CSEL_B(0));
  vga_write_reg(VGA_SEQ, SEQ_MMODE, MMODE_EXTM | MMODE_OE | MMODE_CH4);
  vga_write_reg(VGA_GC, GC_MODE, MODE_SH256);
  vga_write_reg(VGA_GC, GC_MISC, MISC_MMAP(1) | MISC_ADIS);
  vga_write_reg(VGA_CRTC, CRTC_HTOTAL, 0x5f);
  vga_write_reg(VGA_CRTC, CRTC_HDEE, 0x4f);
  vga_write_reg(VGA_CRTC, CRTC_HBLS, 0x50);
  vga_write_reg(VGA_CRTC, CRTC_HBLE, 0x82);
  vga_write_reg(VGA_CRTC, CRTC_HRTS, 0x54);
  vga_write_reg(VGA_CRTC, CRTC_HRTE, 0x80);
  vga_write_reg(VGA_CRTC, CRTC_VTOTAL, 0xbf);
  vga_write_reg(VGA_CRTC, CRTC_OFREG, 0x1f);
  vga_write_reg(VGA_CRTC, CRTC_PRSCAN, 0);
  vga_write_reg(VGA_CRTC, CRTC_MAXSCN, 0x41);
  vga_write_reg(VGA_CRTC, CRTC_VRTS, 0x9c);
  vga_write_reg(VGA_CRTC, CRTC_VRTE, 0x8e);
  vga_write_reg(VGA_CRTC, CRTC_VDEE, 0x8f);
  vga_write_reg(VGA_CRTC, CRTC_LWIDTH, 0x28);
  vga_write_reg(VGA_CRTC, CRTC_UNDLOC, 0x40);
  vga_write_reg(VGA_CRTC, CRTC_VBLS, 0x96);
  vga_write_reg(VGA_CRTC, CRTC_VBLE, 0xb9);
  vga_write_reg(VGA_CRTC, CRTC_MCTL, 0xa3);

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
  for(x = 0; x < 320; x++) {
    for(y = 0; y < 200; y++) {
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
