#include "real.h"
#include "idt.h"
#include "vgatext.h"

void bios_intr(int errupt) {
  int eflags;
  asm volatile("pushf\npop %0\ncli" : "=r" (eflags));
  rmregs.int_no = errupt;
  pmode_to_real();
  extern idtr_t idt_r;
  load_idt(&idt_r);
#ifndef KERNEL_SOURCE
  asm volatile("cli");
#else
  asm volatile("push %0\npopf" : : "r" (eflags));
#endif
}

