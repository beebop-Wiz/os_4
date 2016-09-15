#include "fs.h"
#include "../mt.h"
#include "../malloc.h"
#include "../util.h"
#include "../vgatext.h"
#include "../real.h"
#include "ext2fs.h"

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

static int ext2fs_id;
extern struct process *ptab[65536];
extern volatile int cur_ctx;

void u_read_sector(unsigned int lba, unsigned char *mem) {
  rmsector.size = 0x10;
  rmsector.res = 0;
  rmsector.nxfer = 1;
  rmsector.addr.lo = OFF(vbeinfo);
  rmsector.addr.hi = SEG(vbeinfo);
  rmsector.lba = lba;
  rmsector.pad = 0;
  rmregs.ax = 0x4200;
  rmregs.dx = 0x0081;
  rmregs.si = OFF(&rmsector);
  rmregs.ds = SEG(&rmsector);
  bios_intr(0x13);
  int i;
  for(i = 0; i < 512; i++) {
    mem[i] = vbeinfo[i];
  }
}

void u_write_sector(unsigned int lba, unsigned char *mem) {
  rmsector.size = 0x10;
  rmsector.res = 0;
  rmsector.nxfer = 1;
  rmsector.addr.lo = OFF(vbeinfo);
  rmsector.addr.hi = SEG(vbeinfo);
  rmsector.lba = lba;
  rmsector.pad = 0;
  int i;
  for(i = 0; i < 512; i++) {
    vbeinfo[i] = mem[i];
  }
  rmregs.ax = 0x4300;
  rmregs.dx = 0x0081;
  rmregs.si = OFF(&rmsector);
  rmregs.ds = SEG(&rmsector);
  bios_intr(0x13);
}

struct e2fs_data {
  //  struct ext2_inode inode;
  unsigned int cur_index;
  unsigned char *blockbuf;
};

struct {
  unsigned char sbuf[512];
  struct e2fs_superblock s;
} e2fs_static;

int ext2fs_init(int id) {
  ext2fs_id = id;
  u_read_sector(2, e2fs_static.sbuf);
  memcpy(&e2fs_static.s, e2fs_static.sbuf, sizeof(struct e2fs_superblock));
  printf("e2fs initialized\n");
  return 0;
}

int ext2fs_check_name(char *fname) {
  // STUB
  return (int) fname;
}

struct fs_driver ext2fs_driver = {
  /* init check open close write read seek */
  ext2fs_init, ext2fs_check_name, 0, 0, 0, 0, 0
};
