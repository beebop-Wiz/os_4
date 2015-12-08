#include "gdt.h"
#include "paging.h"

struct gdt_entry_bits *gdt;
struct tss_entry_struct tss_entry;
extern page_table_t kernel_pages;

void load_bios_gdt(unsigned int *loc) {
  gdt = (struct gdt_entry_bits *) loc;
  install_tss(gdt + 7);
}

void install_tss(struct gdt_entry_bits *g) {
  unsigned int base = (unsigned int) &tss_entry;
  unsigned int limit = sizeof(tss_entry);

  g->limit_low = limit & 0xFFFF;
  g->base_low = base & 0xFFFFFF;
  g->accessed = 1;
  g->read_write = 0;
  g->conforming = 0;
  g->code = 1;
  g->always_1 = 0;
  g->DPL = 3;
  g->present = 1;
  g->limit_high = (limit & 0xF0000) >> 16;
  g->available = 0;
  g->always_0 = 0;
  g->big = 0;
  g->gran = 0;
  g->base_high = (base & 0xFF000000) >> 24;

  unsigned int i;
  unsigned char *t = (unsigned char *) &tss_entry;
  for(i = 0; i < sizeof(tss_entry); i++) {
    t[i] = 0;
  }

  tss_entry.ss = 0x10;
  tss_entry.esp0 = KERNEL_STACK_TOP;
  for(i = KERNEL_STACK_BOTTOM; i < KERNEL_STACK_TOP; i += 4096) {
    nonid_page(kernel_pages, i / 4096, 1);
  }
}

void set_kernel_stack(unsigned int s) {
  tss_entry.esp0 = s;
}
