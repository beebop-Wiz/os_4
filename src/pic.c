#include "pic.h"
#include "port.h"
#include "vgatext.h"

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
  outb(0x21, 0xff);
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

void (*irq_handlers[16])(regs_t r);
void register_irq_handler(int irq, void (*handler)(regs_t r)) {
  irq_handlers[irq] = handler;
}

void handle_irq(regs_t r) {
  if(r->int_no > 40)
    outb(0xa0, 0x20);
  outb(0x20, 0x20);
  if(irq_handlers[r->int_no - 0x20])
    irq_handlers[r->int_no - 0x20](r);
}

unsigned short get_irq_reg(int ocw3) {
  outb(0x20, ocw3);
  outb(0xa0, ocw3);
  return (inb(0xa0) << 8) | inb(0x20);
}

void check_pic_reset(regs_t r) {
  unsigned short s, i;
  if((s = get_irq_reg(0x0b))) {
    for(i = 0; i < 16; i++) {
      if(s & (1 << i)) {
	if(irq_handlers[i])
	  irq_handlers[i](r);
	if(i > 8)
	  outb(0xa0, 0x20);
	outb(0x20, 0x20);
      }
    }
  }
}
