#include "real.h"
#include "vbe.h"
#include "vga.h"
#include "vgatext.h"
#include "idt.h"

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
  unsigned int ksize;
  unsigned char pad[502];
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

#define BAR_WIDTH 600
#define BAR_HEIGHT 20
#define BAR_PAD 10
#define PAD_WIDTH (BAR_PAD * 2 + BAR_WIDTH)
#define PAD_HEIGHT (BAR_PAD * 2 + BAR_HEIGHT)
#define BAR_X (400 - (BAR_WIDTH / 2))
#define BAR_Y (400 - (BAR_HEIGHT / 2))
#define PAD_X (400 - (PAD_WIDTH / 2))
#define PAD_Y (400 - (PAD_HEIGHT / 2))

void boot2_main() {
  vbe_load_data();
  init_vga();
  init_vgatext();
  setup_idt();
  vga_rect(0, 0, 800, 600, 0x007766);
  vga_rect(PAD_X, PAD_Y, PAD_WIDTH, PAD_HEIGHT, 0x008877);
  vga_setwin(80, 24, 80, 108);
  vga_set_color(0xffffff, 0x007766);
  vga_clear_text();
  vga_puts("Loaded second stage bootstrap.\n");
  vga_puts("Loading kernel from LBA ");
  vga_itoa(sector_offset);
  vga_puts("\n");
  int lba = sector_offset;
  int knsectors = -1;
  do {
    read_sector(lba++, (unsigned char *) &bh);
    knsectors = bh.ksize;
    vga_puts("Read chunk, size = ");
    vga_itoa(bh.nsectors);
    vga_puts(" linear ");
    vga_itoa(bh.addr.l);
    vga_puts(".\n");
    if(!bh.nsectors) break;
    vga_puts("Copying...\n");
    int i;
    for(i = 0; i < bh.nsectors; i++) {
      printf("    LBA %d (%d%%)\n", lba, ((lba - sector_offset) * 100) / knsectors);
      read_sector(lba, bh.addr.l);
      lba++;
      bh.addr.l += 512;
      vga_rect(BAR_X, BAR_Y, ((lba - sector_offset) * BAR_WIDTH) / knsectors, BAR_HEIGHT, 0x00ffcc);
      int j;
      for(j = 0; j < 10000000; j++) ;
    }
  } while(bh.nsectors);
  vga_puts("Booting...\n");
  bh.addr.fn();
}
