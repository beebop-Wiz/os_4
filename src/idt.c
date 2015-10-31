#include "idt.h"
#include "vgatext.h"

void c_intr(regs_t r) {
  vga_puts("Recieved interrupt `");
  vga_putchar(r.int_no + '@');
  vga_puts("`\n");
}

idt_gate_t idt[256];

#define LOAD_ISR(i) do {			\
    extern void int_##i();			\
    MAKE_ADDR(idt[i], int_##i);			\
    idt[i].selector = 0x08;			\
    idt[i].res = 0;				\
    idt[i].type = TYPE_USR_32I;			\
  } while(0);

idtr_t idt_r;

void setup_idt() {
  outb(0x21, 0xff);
  outb(0xA1, 0xff);
  int i;
  for(i = 0; i < 256; i++) {
    idt[i].type = 0;
  }
  LOAD_ISR(0);
  LOAD_ISR(1);
  LOAD_ISR(2);
  LOAD_ISR(3);
  LOAD_ISR(4);
  LOAD_ISR(5);
  LOAD_ISR(6);
  LOAD_ISR(7);
  LOAD_ISR(8);
  LOAD_ISR(9);
  LOAD_ISR(10);
  LOAD_ISR(11);
  LOAD_ISR(12);
  LOAD_ISR(13);
  LOAD_ISR(14);
  LOAD_ISR(15);
  LOAD_ISR(16);
  LOAD_ISR(17);
  LOAD_ISR(18);
  idt_r.size = sizeof(idt);
  idt_r.offset = (unsigned int) &idt;
  load_idt(&idt_r);
}
