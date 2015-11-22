#ifndef REAL_H
#define REAL_H

struct realmode_regs {
  unsigned short ax, bx, cx, dx, si, di, sp, bp;
  unsigned short ds, es, fs, gs;
  unsigned char int_no;
} __attribute__ ((packed));

extern struct realmode_regs rmregs;

#define OFF(addr) ((unsigned short) ((unsigned int) (addr)))
#define SEG(addr) ((unsigned short) (((unsigned int) (addr)) & 0xF0000) >> 1)
#define LIN(seg, off) (((seg) << 1) | (off))
/**
 * Jumps to real mode. Sets registers from the global rmregs and
 * executes the selected interrupt.
 */
void pmode_to_real();

/**
 * Executes an interrupt.
 * @param[in] errupt The interrupt number.
 */
void bios_intr(int errupt);

#endif
