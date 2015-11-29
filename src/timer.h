#ifndef TIMER_H
#define TIMER_H

void PIC_init();
void PIC_remap(int off1, int off2);
void PIC_mask_line(unsigned char line);
void PIC_clear_line(unsigned char line);
#include "idt.h"
void handle_irq(regs_t r);
void timer_init(int freq);

#endif
