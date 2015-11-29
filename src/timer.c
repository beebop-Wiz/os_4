#include "timer.h"
#include "port.h"
#include "vgatext.h"
#include "mt.h"

void PIC_init() {
  PIC_remap(0x20, 0x28);
  int i;
  for(i = 0; i < 16; i++) PIC_mask_line(i);
}

void PIC_remap(int off1, int off2) {
  outb(0x20, 0x11);
  outb(0xa0, 0x11);
  outb(0x21, off1);
  outb(0xa1, off2);
  outb(0x21, 0x04);
  outb(0xa1, 0x02);
  outb(0x21, 0x01);
  outb(0xa1, 0x01);
  outb(0x21, 0xfe);
  outb(0xa1, 0xff);
}

void PIC_mask_line(unsigned char line) {
  unsigned short port;
  unsigned char data;
  if(line < 8) {
    port = 0x21;
  } else {
    port = 0xa1;
    line -= 8;
  }
  data = inb(port) | (1 << line);
  outb(port, data);
}

void PIC_clear_line(unsigned char line) {
  unsigned short port;
  unsigned char data;
  if(line < 8) {
    port = 0x21;
  } else {
    port = 0xa1;
    line -= 8;
  }
  data = inb(port) & ~(1 << line);
  outb(port, data);
}

void handle_irq(regs_t r) {
  if(r.int_no > 40)
    outb(0xa0, 0x20);
  outb(0x20, 0x20);
  extern int mt_enabled;
  if(r.int_no == 0x20 && mt_enabled) {
    switch_ctx(r);
  }
}

void timer_init(int freq) {
  int div = 1193180 / freq;
  outb(0x43, 0x36);
  outb(0x40, div & 0xff);
  outb(0x40, div >> 8);
  PIC_clear_line(0);
}
