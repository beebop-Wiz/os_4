#include "idt.h"
#include "vgatext.h"
#include "vgadraw.h"
#include "port.h"
#include "syscall.h"
#include "timer.h"
#include "pic.h"
#include "log.h"

char *exc[] = {
  "Divide by zero",
  "Debug",
  "NMI",
  "Breakpoint",
  "Overflow",
  "Bound Range Exceeded",
  "Invalid Opcode",
  "Device Not Available",
  "Double Fault",
  "Coprocessor Segment Overrun (CH)",
  "Invalid TSS",
  "Segment Not Present",
  "Stack-Segment Fault",
  "General Protection Fault",
  "Page Fault",
  "Reserved Exception (CH)",
  "x87 Floating Point Exception",
  "Alignment Check",
  "Machine Check"
};

#define BLUE 0x002b36
#define WHITE 0x268bd2

#ifdef KERNEL_SOURCE
extern volatile int cur_ctx;
#endif

volatile int order = 0;

void bsod(regs_t r) {
  if(order++) {
    printf("\n\n");
    log(LOG_GENERAL, LOG_CRITICAL, "Error detected in error handler. Giving up.\n");
    goto halt;
  }
  asm volatile ("cli");
  asm volatile ("mov $0x1800000, %esp");
  vga_setwin(90, 70, 30, 30);
  vga_clearcolor(0x000000);
  vga_set_color(0xFF0000, 0x000000);
  vga_clear_text();
  log(LOG_GENERAL, LOG_CRITICAL, "Kernel panic: err 0x%x (%s)\n", r->int_no, exc[r->int_no]);
#ifdef KERNEL_SOURCE
  log(LOG_GENERAL, LOG_CRITICAL, "Current process: %d\n", cur_ctx);
#endif
  log(LOG_GENERAL, LOG_CRITICAL, "\tCS:EIP %x:%x\n", r->cs, r->eip);
  log(LOG_GENERAL, LOG_CRITICAL, "\tEDI: %x ESI: %x EBP: %x ESP: %x (%x)\n", r->edi, r->esi, r->ebp, r->esp, r->useresp);
  log(LOG_GENERAL, LOG_CRITICAL, "\tEBX: %x EDX: %x ECX: %x EAX: %x\n", r->ebx, r->edx, r->ecx, r->eax);
  unsigned int cr;
  asm volatile("mov %%cr2, %0" : "=r" (cr));
  log(LOG_GENERAL, LOG_CRITICAL, "\tCR2: %x\n", cr);
  log(LOG_GENERAL, LOG_CRITICAL, "Err: %x flags %x\n", r->err, r->eflags);
  log(LOG_GENERAL, LOG_CRITICAL, "\n");
  log(LOG_GENERAL, LOG_CRITICAL, "Stacktrace:\n");
  log(LOG_GENERAL, LOG_CRITICAL, "%x\n", r->eip);
  while(r->ebp) {
    log(LOG_GENERAL, LOG_CRITICAL, "%x\n", *((unsigned int *) r->ebp + 1));
    r->ebp = *((unsigned int *) r->ebp);
  }
 halt:
  printf("\n\n\nWill now halt.\n");
  for(;;) asm volatile ("cli\nhlt");
}

void c_intr(regs_t r) {
  r->int_no &= 0xff;
  //printf("Recieved interrupt 0x%x\n", r->int_no);
  if(r->int_no < 19) {
    bsod(r);
#ifdef KERNEL_SOURCE
  } else if(r->int_no > 31 && r->int_no < 48) {
    handle_irq(r);
  } else if(r->int_no == 0x80 || r->int_no == 0x81) {
    do_syscall(r);
#endif
  }
#ifdef KERNEL_SOURCE
  check_pic_reset(r);
#endif
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
  int i;
  outb(0x21, (unsigned char) 0xff);
  outb(0xA1, (unsigned char) 0xff);
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

  LOAD_ISR(32);
  LOAD_ISR(33);
  LOAD_ISR(34);
  LOAD_ISR(35);
  LOAD_ISR(36);
  LOAD_ISR(37);
  LOAD_ISR(38);
  LOAD_ISR(39);
  LOAD_ISR(40);
  LOAD_ISR(41);
  LOAD_ISR(42);
  LOAD_ISR(43);
  LOAD_ISR(44);
  LOAD_ISR(45);
  LOAD_ISR(46);
  LOAD_ISR(47);

  LOAD_ISR(128);
  LOAD_ISR(129);
  idt_r.size = sizeof(idt);
  idt_r.offset = (unsigned int) &idt;
  load_idt(&idt_r);
}
