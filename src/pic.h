#ifndef PIC_H
#define PIC_H

void PIC_init();
void PIC_remap(int off1, int off2);
void PIC_mask_line(unsigned char line);
void PIC_clear_line(unsigned char line);
#include "idt.h"
void register_irq_handler(int irq, void (*handler)(regs_t r));
void handle_irq(regs_t r);
void check_pic_reset();

#endif
