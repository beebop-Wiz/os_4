#include "vbe.h"
#include "vgatext.h"
#include "real.h"

extern struct vbe_info vbeinfo;
extern struct vbe_mode vbemode;
void vbe_load_data() {
  rmregs.ax = 0x4f00;
  rmregs.di = OFF(&vbeinfo);
  rmregs.es = SEG(&vbeinfo);
  bios_intr(0x10);
  unsigned short *modelist = (unsigned short *) LIN((unsigned int) vbeinfo.vmptr[1], (unsigned int) vbeinfo.vmptr[0]);
  int i;
  for(i = 0; modelist[i] != 0xFFFF; i++) {
    rmregs.ax = 0x4f01;
    rmregs.cx = modelist[i];
    rmregs.di = OFF(&vbemode);
    rmregs.es = SEG(&vbemode);
    bios_intr(0x10);
  }
  vga_putchar('\n');
  // we should eventually do something with this.
  // until then, set 0x4118 (24 bits, 1024x768)
  rmregs.ax = 0x4f02;
  rmregs.bx = 0x4118;
  bios_intr(0x10);
  *((unsigned int *) 0xFD000000) = 0xffffffff;
}
