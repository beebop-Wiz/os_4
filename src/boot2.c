#include "real.h"
#include "vbe.h"
#include "vga.h"
#include "vgatext.h"

extern char vbeinfo[512];
extern struct {
  unsigned char size;
  unsigned char res;
  unsigned short nxfer;
  struct {
    unsigned short lo;
    unsigned short hi;
  } __attribute__ ((packed)) addr;
  unsigned int lba;
  unsigned int pad;
} __attribute__ ((packed)) rmsector;
extern short sector_offset;

struct bheader {
  unsigned short nsectors;
  union {
    struct {
      unsigned short memaddr_hi, memaddr_lo;
    } o;
    unsigned int l;
    void (*fn)();
  } addr;
  unsigned char pad[506];
} __attribute__ ((packed)) bh;

void read_sector(unsigned int lba, unsigned char *mem) {
  rmsector.size = 0x10;
  rmsector.res = 0;
  rmsector.nxfer = 1;
  rmsector.addr.lo = OFF(vbeinfo);
  rmsector.addr.hi = SEG(vbeinfo);
  rmsector.lba = lba;
  rmsector.pad = 0;
  rmregs.ax = 0x4200;
  rmregs.dx = 0x0080;
  rmregs.si = OFF(&rmsector);
  rmregs.ds = SEG(&rmsector);
  bios_intr(0x13);
  int i;
  for(i = 0; i < 512; i++) {
    mem[i] = vbeinfo[i];
  }
}

void boot2_main() {
  vbe_load_data();
  init_vga();
  init_vgatext();
  vga_puts("Loaded second stage bootstrap.\n");
  vga_puts("Loading kernel from LBA ");
  vga_itoa(sector_offset);
  vga_puts("\n");
  int lba = sector_offset;
  do {
    read_sector(lba++, (unsigned char *) &bh);
    vga_puts("Read chunk, size = ");
    vga_itoa(bh.nsectors);
    vga_puts(" linear ");
    vga_itoa(bh.addr.l);
    vga_puts(".\n");
    if(!bh.nsectors) break;
    vga_puts("Copying...\n");
    int i;
    for(i = 0; i < bh.nsectors; i++) {
      vga_puts("    LBA ");
      vga_itoa(lba);
      vga_puts("\n");
      read_sector(lba, bh.addr.l);
      lba++;
      bh.addr.l += 512;
    }
  } while(bh.nsectors);
  vga_puts("Booting...\n");
  bh.addr.fn();
}
