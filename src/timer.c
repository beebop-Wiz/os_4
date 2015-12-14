#include "timer.h"
#include "pic.h"
#include "port.h"
#include "vgatext.h"
#include "mt.h"
#include "keyboard.h"

void timer_init(int freq) {
  int div = 1193180 / freq;
  outb(0x43, 0x36);
  outb(0x40, div & 0xff);
  outb(0x40, div >> 8);
  PIC_clear_line(0);
  register_irq_handler(0, switch_ctx);
}
