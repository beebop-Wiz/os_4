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
  unsigned short *modelist = LIN(vbeinfo.vmptr[1], vbeinfo.vmptr[0]);
  int i;
  for(i = 0; modelist[i] != 0xFFFF; i++) {
    rmregs.ax = 0x4f01;
    rmregs.cx = modelist[i];
    rmregs.di = OFF(&vbemode);
    rmregs.es = SEG(&vbemode);
    bios_intr(0x10);
    if(modelist[i] == 0x115)
      vga_itoa(vbemode.lfb_base);
  }
  vga_putchar('\n');
  // we should eventually do something with this.
  // until then, set 0x4115
  rmregs.ax = 0x4f02;
  rmregs.bx = 0x4115;
  bios_intr(0x10);
  *((unsigned int *) 0xFD000000) = 0xffffffff;
}
