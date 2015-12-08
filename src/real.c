#include "real.h"
#include "idt.h"

void bios_intr(int errupt) {
  asm volatile("cli");
  rmregs.int_no = errupt;
  pmode_to_real();
  extern idtr_t idt_r;
  load_idt(&idt_r);
#ifndef KERNEL_SOURCE
  asm volatile("cli");
#endif
}
