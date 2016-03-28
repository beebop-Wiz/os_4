#include "timer.h"
#include "pic.h"
#include "port.h"
#include "vgatext.h"
#include "mt.h"
#include "keyboard.h"

volatile int tn = 0;
extern volatile int cs;
extern unsigned int cx, cy;

volatile unsigned long long jiffy_clock = 0;

void timer_intr(regs_t r) {
  jiffy_clock++;
  tn++;
  if((tn % 10) == 9) {
    cs = !cs;
    vga_redraw();
  }
  switch_ctx(r);
}

void timer_init(int freq) {
  int div = 1193180 / freq;
  outb(0x43, 0x36);
  outb(0x40, div & 0xff);
  outb(0x40, div >> 8);
  PIC_clear_line(0);
  register_irq_handler(0, timer_intr);
}
