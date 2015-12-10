#include "real.h"
#include "vbe.h"
#include "ext2.h"
#include "util.h"
#include "vgatext.h"
#include "malloc.h"

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
  memcpy((void *) desc, sector + (t_idx * 32), 32);
}

void read_inode(struct ext2_superblock *s, struct ext2_inode *inode, int inode_idx) {
  int block_group = (inode_idx - 1) / s->ig_size;
  int local_index = (inode_idx - 1) % s->ig_size;
  struct ext2_bg_desc desc;
  read_block_group(s, &desc, block_group);
  int block_offset = local_index * sizeof(struct ext2_inode);
  int sector_offset = block_offset % 512;
  int it_sector = desc.inode_table * 2 + (block_offset / 512);
  unsigned char sector[512];
  read_sector(it_sector, sector);
  memcpy((void *) inode, sector + sector_offset, sizeof(struct ext2_inode));
}

void get_block(struct ext2_inode *inode, int block_idx, unsigned char *block) {
  unsigned int block_addr;
  if(block_idx < 12) {
    block_addr = inode->bp[block_idx];
  } else {
    printf("EXT2 error: you need to fix indirection\n");
  }
  read_sector(block_addr * 2, block);
  read_sector(block_addr * 2 + 1, block + 512);
}

ext2_dirstate_t opendir(struct ext2_inode *inode) {
  ext2_dirstate_t r = malloc(sizeof(struct ext2_dirstate));
  r->inode = malloc(sizeof(struct ext2_inode));
  r->ent_idx = 0;
  r->last = 0;
  memcpy(r->inode, inode, sizeof(struct ext2_inode));
  return r;
}

ext2_dirent_t dirwalk(ext2_dirstate_t s) {
  if(s->last) {
    free(s->last->name);
    free(s->last);
  }
  unsigned int i, block_idx = 0, boff = 0, size;
  unsigned char block[1024];
  for(i = 0; i < s->ent_idx; i++) {
    get_block(s->inode, block_idx, block);
    size = block[boff + 4];
    size += block[boff + 5] << 8;
    if(size == 0) return 0;
    boff += size;
    if(boff > 1024) {
      boff = 0;
      block_idx++;
    }
  }
  if(block_idx > s->inode->n_sectors / 2) {
    return 0;
  }
  ext2_dirent_t r = malloc(sizeof(struct ext2_dirent));
  s->ent_idx++;
  get_block(s->inode, block_idx, block);
  memcpy(r, block + boff, sizeof(struct ext2_dirent) - 4);
  r->name = malloc(r->nlen + 1);
  r->name[r->nlen] = 0;
  memcpy(r->name, block + boff + sizeof(struct ext2_dirent) - 4, r->nlen);
  return r;
}

void closedir(ext2_dirstate_t s) {
  if(s->last) {
    free(s->last->name);
    free(s->last);
  }
  free(s->inode);
  free(s);
}

void parse_inode_type(unsigned short type, char *out) {
  if(type & 0x1000) out[0] = 'f';
  else if(type & 0x2000) out[0] = 'c';
  else if(type & 0x4000) out[0] = 'd';
  else if(type & 0x6000) out[0] = 'b';
  else if(type & 0x8000) out[0] = '-';
  else if(type & 0xA000) out[0] = 'l';
  else if(type & 0xC000) out[0] = 's';
  int i;
  for(i = 0; i < 9; i++) {
    out[i + 1] = '-';
  }
  if(type & 0x100) out[1] = 'r';
  if(type & 0x80) out[2] = 'w';
  if(type & 0x40) out[3] = 'x';
  if(type & 0x20) out[4] = 'r';
  if(type & 0x10) out[5] = 'w';
  if(type & 0x8) out[6] = 'x';
  if(type & 0x4) out[7] = 'r';
  if(type & 0x2) out[8] = 'w';
  if(type & 0x1) out[9] = 'x';
  
  if(type & 0x200) out[10] = 's';
  if(type & 0x400) out[10] = 'G';
  if(type & 0x800) out[10] = 'U';
  if(type & 0xC00) out[10] = 'S';
}

int get_file_inode(struct ext2_superblock *s, int dir_inode, const char *name) {
  struct ext2_inode dir;
  read_inode(s, &dir, dir_inode);
  ext2_dirstate_t ds = opendir(&dir);
  ext2_dirent_t d;
  while((d = dirwalk(ds)))
    if(d->nlen && streq(d->name, name)) break;
  int r = d->inode;
  closedir(ds);
  return r;
}

char *path_tokenize(char *path, char **loc) {
  if(!*path) return 0;
  char *r;
  // skip initial slashes
  while(*path == '/') path++;
  r = (char *) path; // save current position
  while(*path != '/' && *path) path++;
  *path = 0; 
  *loc = path + 1;
  return r;
}

void list_directory(struct ext2_superblock *s, struct ext2_inode *i) {
  ext2_dirstate_t root = opendir(i);
  ext2_dirent_t d;
  struct ext2_inode sub;
  char out[11];
  while((d = dirwalk(root))) {
    if(d->nlen) {
      d->name[d->nlen] = 0;
      read_inode(s, &sub, d->inode);
      parse_inode_type(sub.type, out);
      printf("%s %s %d %d\n", out, d->name, sub.size_low, d->inode);
    }
  }
  closedir(root);
}

int get_path_inode(struct ext2_superblock *s, const char *p) {
  char *token;
  char *path = malloc(strlen(p) + 2);
  char *p_orig = path;
  memcpy(path, p, strlen(p) + 2);
  path[strlen(p) + 1] = 0;
  int dir_inode = 2;
  struct ext2_inode dir;
  while((token = path_tokenize(path, &path))) {
    if(!strlen(token)) break;
    read_inode(s, &dir, dir_inode);
    list_directory(s, &dir);
    dir_inode = get_file_inode(s, dir_inode, token);
  }
  free(p_orig);
  return dir_inode;
}
