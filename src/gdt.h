#ifndef GDT_H
#define GDT_H

#define KERNEL_STACK_BOTTOM 0xA0000000
#define KERNEL_STACK_LIMIT  0x00010000
#define KERNEL_STACK_TOP (KERNEL_STACK_BOTTOM + KERNEL_STACK_LIMIT)

struct gdt_entry_bits {
  unsigned int limit_low  :16;
  unsigned int base_low   :24;
  unsigned int accessed   :1;
  unsigned int read_write :1;
  unsigned int conforming :1;
  unsigned int code       :1;
  unsigned int always_1   :1;
  unsigned int DPL        :2;
  unsigned int present    :1;
  unsigned int limit_high :4;
  unsigned int available  :1;
  unsigned int always_0   :1;
  unsigned int big        :1;
  unsigned int gran       :1;
  unsigned int base_high  :8;
} __attribute__ ((packed));

struct tss_entry_struct {
  unsigned int prev_tss;
  unsigned int esp0;
  unsigned int ss;
  unsigned int dont_care[23];
} __attribute__ ((packed));

void load_bios_gdt(unsigned int *loc);
void install_tss(struct gdt_entry_bits *g);

#endif
