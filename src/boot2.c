#include "real.h"
#include "vbe.h"
#include "vga.h"
#include "vgatext.h"
#include "vgadraw.h"
#include "idt.h"
#include "log.h"

volatile unsigned long long jiffy_clock = 0;

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
    unsigned char *l;
    void (*fn)(unsigned int *bdata);
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
  vga_clearcolor(0x002b36);
  vga_rect(PAD_X, PAD_Y, PAD_WIDTH, PAD_HEIGHT, 0x586e75);
  vga_setwin(80, 24, 80, 108);
  vga_set_color(0x93a1a1, 0x002b36);
  vga_clear_text();
  log(LOG_BOOT, LOG_INFO, "Loaded second stage bootstrap.\n");
  log(LOG_BOOT, LOG_INFO, "Loading kernel from LBA %d\n", sector_offset);
  int lba = sector_offset;
  int knsectors = -1;
  do {
    read_sector(lba++, (unsigned char *) &bh);
    knsectors = bh.ksize;
    log(LOG_BOOT, LOG_INFO, "Read chunk, size = %d linear %x.\n", bh.nsectors, bh.addr.l);
    if(!bh.nsectors) break;
    log(LOG_BOOT, LOG_INFO, "Copying...\n");
    int i;
    for(i = 0; i < bh.nsectors; i++) {
      log(LOG_BOOT, LOG_INFO, "    LBA %d (%d%%)\n", lba, ((lba - sector_offset) * 100) / knsectors);
      read_sector(lba, bh.addr.l);
      lba++;
      bh.addr.l += 512;
      vga_rect(BAR_X, BAR_Y, ((lba - sector_offset) * BAR_WIDTH) / knsectors, BAR_HEIGHT, 0x2aa198);
    }
  } while(bh.nsectors);
  log(LOG_BOOT, LOG_INFO, "Booting (%x)\n", bh.addr.l);
  bh.addr.fn((unsigned int *) &sector_offset);
  asm("cli");
  for(;;) {
    asm("hlt");
  }
}
