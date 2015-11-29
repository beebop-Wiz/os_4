#include "real.h"
#include "vbe.h"
#include "ext2.h"
#include "util.h"

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

void read_sector(unsigned int lba, unsigned char *mem) {
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

void read_superblock(struct ext2_superblock *s) {
  memset(s, 0, 1024);
  read_sector(2, (unsigned char *) s);
  s->block_size = 1024 << s->block_size;
}

void read_block_group(struct ext2_superblock *s, struct ext2_bg_desc *desc, int bg_idx) {
  memset(desc, 0, 32);
  int b_idx = bg_idx / (s->block_size / 32);
  int t_idx = bg_idx % 32;
  int s_idx = b_idx * 2 + 4;
  unsigned char sector[512];
  read_sector(s_idx, sector);
  memcpy((void *) desc, sector + t_idx, 32);
}

void read_inode(struct ext2_superblock *s, struct ext2_inode *inode, int inode_idx) {
  int bg_idx = (inode_idx - 1) / s->ig_size;
  struct ext2_bg_desc desc;
  read_block_group(s, &desc, bg_idx);
  // TODO
}
