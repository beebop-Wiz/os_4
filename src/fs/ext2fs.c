#include "fs.h"
#include "../mt.h"
#include "../malloc.h"
#include "../util.h"
#include "../vgatext.h"
#include "../real.h"
#include "../disk.h"
#include "ext2fs.h"

static int ext2fs_id;
extern struct process *ptab[65536];
extern volatile int cur_ctx;

struct e2fs_data {
  struct e2fs_inode inode;
  int inum;
  unsigned int offset;
  unsigned char blockbuf[1024];
};

struct {
  unsigned char sbuf[512];
  struct e2fs_superblock s;
  unsigned char block[1024];
  char mbuf[12];
} e2fs_static;

#define BLOCK_SIZE (1024 << e2fs_static.s.s_log_block_size)

void read_within_sector(int sector, int offset, int len, unsigned char *mem) {
  read_sector(sector, e2fs_static.sbuf);
  memcpy(mem, e2fs_static.sbuf + offset, len);
}

void write_within_sector(int sector, int offset, int len, unsigned char *mem) {
  read_sector(sector, e2fs_static.sbuf);
  memcpy(e2fs_static.sbuf + offset, mem, len);
  write_sector(sector, e2fs_static.sbuf);
}

void e2fs_read_superblock(void) {
  read_sector(2, e2fs_static.sbuf);
  memcpy(&e2fs_static.s, e2fs_static.sbuf, sizeof(struct e2fs_superblock));
}

void read_block_raw(int n, unsigned char *mem) {
  int n_sectors = BLOCK_SIZE / 512;
  int sid = n * n_sectors;
  int i;
  for(i = 0; i < n_sectors; i++) {
    read_sector(sid + i, mem + i * 512);
  }
}

void write_block_raw(int n, unsigned char *mem) {
  int n_sectors = BLOCK_SIZE / 512;
  int sid = n * n_sectors;
  int i;
  for(i = 0; i < n_sectors; i++) {
    write_sector(sid + i, mem + i * 512);
  }
}

void read_within_block(int block, int offset, int len, unsigned char *mem) {
  read_block_raw(block, e2fs_static.block);
  memcpy(mem, e2fs_static.block + offset, len);
}

void write_within_block(int block, int offset, int len, unsigned char *mem) {
  read_block_raw(block, e2fs_static.block);
  memcpy(e2fs_static.block + offset, mem, len);
  write_block_raw(block, e2fs_static.block);
}

char bmread(int start_block, int offt) {
  int byte = (offt / 8) % BLOCK_SIZE;
  int bit = offt % 8;
  int block = (offt / 8) / BLOCK_SIZE + start_block;
  unsigned char bm;
  read_within_block(block, byte, 1, &bm);
  return !!(bm & 1 << bit);
}

void bmwrite(int start_block, int offt, int val) {
  int byte = (offt / 8) % BLOCK_SIZE;
  int bit = offt % 8;
  int block = (offt / 8) / BLOCK_SIZE + start_block;
  unsigned char bm;
  read_within_block(block, byte, 1, &bm);
  bm &= ~(1 << bit);
  bm |= (!!val << bit);
  write_within_block(block, byte, 1, &bm);
}

void read_bgd(int n, struct e2fs_bdesc *d) {
  int block = n / (BLOCK_SIZE / sizeof(struct e2fs_bdesc)) + 2;
  int offt  = n % (BLOCK_SIZE / sizeof(struct e2fs_bdesc)) * sizeof(struct e2fs_bdesc);
  read_within_block(block, offt, sizeof(struct e2fs_bdesc), (unsigned char *) d);
}

void write_bgd(int n, struct e2fs_bdesc *d) {
  int block = n / (BLOCK_SIZE / sizeof(struct e2fs_bdesc)) + 2;
  int offt  = n % (BLOCK_SIZE / sizeof(struct e2fs_bdesc)) * sizeof(struct e2fs_bdesc);
  write_within_block(block, offt, sizeof(struct e2fs_bdesc), (unsigned char *) d);
}

void e2fs_read_inode(int idx, struct e2fs_inode *i) {
  int bg = (idx - 1) / e2fs_static.s.s_inodes_per_group;
  int bi = (idx - 1) % e2fs_static.s.s_inodes_per_group;
  //  printf("Inode %d is at (%d, %d) ", idx, bg, bi);
  struct e2fs_bdesc b;
  read_bgd(bg, &b);
  int bk = (bi * sizeof(struct e2fs_inode)) / BLOCK_SIZE + b.bg_inode_table;
  int bo = (bi * sizeof(struct e2fs_inode)) % BLOCK_SIZE;
  //  printf("block (%d, %d)\n", bk, bo);
  read_within_block(bk, bo, sizeof(struct e2fs_inode), (unsigned char *) i);
}

void e2fs_write_inode(int idx, struct e2fs_inode *i) {
  int bg = (idx - 1) / e2fs_static.s.s_inodes_per_group;
  int bi = (idx - 1) % e2fs_static.s.s_inodes_per_group;
  //  printf("Inode %d is at (%d, %d) ", idx, bg, bi);
  struct e2fs_bdesc b;
  read_bgd(bg, &b);
  int bk = (bi * sizeof(struct e2fs_inode)) / BLOCK_SIZE + b.bg_inode_table;
  int bo = (bi * sizeof(struct e2fs_inode)) % BLOCK_SIZE;
  //  printf("block (%d, %d)\n", bk, bo);
  write_within_block(bk, bo, sizeof(struct e2fs_inode), (unsigned char *) i);
}						      

void get_inode_block(struct e2fs_inode *i, int block, unsigned char *mem) {
  int block_addr;
  if(block < 12) {
    block_addr = i->i_bp0[block];
  } else {
    printf("e2fs error: you need to fix indirection on read\n");
  }
  read_block_raw(block_addr, mem);
}

void put_inode_block(struct e2fs_inode *i, int block, unsigned char *mem) {
  int block_addr;
  if(block < 12) {
    block_addr = i->i_bp0[block];
  } else {
    printf("e2fs error: you need to fix indirection on write\n");
  }
  write_block_raw(block_addr, mem);
}

int get_first_free_block() {
  unsigned int bgn;
  struct e2fs_bdesc b;
  unsigned int bi;
  for(bgn = 0; bgn < e2fs_static.s.s_blocks_count / e2fs_static.s.s_blocks_per_group; bgn++) {
    read_bgd(bgn, &b);
    for(bi = 0; bi < e2fs_static.s.s_blocks_per_group; bi++) {
      if(!bmread(b.bg_block_bitmap, bi)) {
	return (bgn * e2fs_static.s.s_blocks_per_group) + bi;
      }
    }
  }
  return -1;
}

int get_first_free_inode() {
  unsigned int bgn;
  struct e2fs_bdesc b;
  unsigned int ii;
  for(bgn = 0; bgn < e2fs_static.s.s_inodes_count / e2fs_static.s.s_inodes_per_group; bgn++) {
    read_bgd(bgn, &b);
    for(ii = 0; ii < e2fs_static.s.s_inodes_per_group; ii++) {
      if(!bmread(b.bg_inode_bitmap, ii)) {
	return (bgn * e2fs_static.s.s_inodes_per_group) + ii + 1;
      }
    }
  }
  return -1;
}

int allocate_block() {
  int bi = get_first_free_block();
  struct e2fs_bdesc b;
  read_bgd(bi / e2fs_static.s.s_blocks_per_group, &b);
  bmwrite(b.bg_block_bitmap, bi % e2fs_static.s.s_blocks_per_group, 1);
  return bi;
}

void add_block_to_inode(int idx, struct e2fs_inode *i) {
  int bi = (i->i_size / BLOCK_SIZE);
  if(bi < 12) {
    i->i_bp0[bi] = allocate_block();
  } else {
    printf("e2fs error: you need to fix indirection on append\n");
  }
  e2fs_write_inode(idx, i);
}

void guarantee_inode_size(int idx, struct e2fs_inode *i, int len) {
  int cur_size = i->i_size;
  if(len < cur_size) goto end;
  int diff = len - cur_size;
  int block_space = (BLOCK_SIZE - (cur_size % BLOCK_SIZE)) % BLOCK_SIZE;
  if(block_space > diff) goto end;
  diff -= block_space;
  while(diff > 0) {
    add_block_to_inode(idx, i);
    diff -= BLOCK_SIZE;
  }
 end:
  i->i_size = len;
  e2fs_write_inode(idx, i);
}

int iread(struct e2fs_inode *i, unsigned char *buf, int offset, int count) {
  //  printf("iread: %d %d %d\n", offset, count, i->i_size);
  if((unsigned)(offset + count) > i->i_size) {
    return iread(i, buf, offset, i->i_size - offset);
  }
  // read up to the block boundary
  get_inode_block(i, offset / BLOCK_SIZE, e2fs_static.block);
  int dist = BLOCK_SIZE - (offset % BLOCK_SIZE);
  int n_read = 0;
  memcpy(buf, e2fs_static.block + offset % BLOCK_SIZE, count);
  if(dist >= count)
    return count;
  n_read = dist;
  int bp = offset / BLOCK_SIZE + 1;
  for(; n_read < count; n_read += BLOCK_SIZE) {
    get_inode_block(i, bp++, e2fs_static.block);
    if(n_read + BLOCK_SIZE > count) {
      memcpy(buf + n_read, e2fs_static.block, count - n_read);
    } else {
      memcpy(buf + n_read, e2fs_static.block, BLOCK_SIZE);
    }
  }
  return count;
}

#define MIN(x, y) (((x)<(y))?(x):(y))

int iwrite(int idx, struct e2fs_inode *i, unsigned char *buf, int offset, int count) {
  printf("[e2fs] iwrite: %d %d %d\n", offset, count, i->i_size);
  guarantee_inode_size(idx, i, offset + count);
  get_inode_block(i, offset / BLOCK_SIZE, e2fs_static.block);
  int dist = BLOCK_SIZE - (offset % BLOCK_SIZE);
  int n_written = 0;
  memcpy(e2fs_static.block + offset % BLOCK_SIZE, buf, MIN(dist, count));
  put_inode_block(i, offset / BLOCK_SIZE, e2fs_static.block);
  if(dist >= count) return count;
  n_written = dist;
  int bp = offset / BLOCK_SIZE + 1;
  for(; n_written < count; n_written += BLOCK_SIZE) {
    get_inode_block(i, bp, e2fs_static.block);
    if(n_written + BLOCK_SIZE > count) {
      memcpy(e2fs_static.block, buf + n_written, count - n_written);
    } else {
      memcpy(e2fs_static.block, buf + n_written, BLOCK_SIZE);
    }
    put_inode_block(i, bp++, e2fs_static.block);
  }
  //  printf("[e2fs] iwrite: can't write more than a block at a time");
  return count;
}

void getdent(struct e2fs_inode *i, struct e2fs_dirent *buf, int offset) {
  if(iread(i, (unsigned char *) buf, offset, 7) == 0) {
    memset(buf, 0, sizeof(struct e2fs_dirent));
    return;
  }
  if(buf->d_inode) {
    if(buf->d_rec_len > sizeof(struct e2fs_dirent) )
      iread(i, (unsigned char *) buf, offset, sizeof(struct e2fs_dirent));
    else
      iread(i, (unsigned char *) buf, offset, buf->d_rec_len);
    buf->d_name[buf->d_name_len] = 0;
  }
}

int new_file(struct e2fs_inode *d, char *name) {
  return (int)d + (int)name;
}

void prettyprint_mode(unsigned short mode, char *s) {
  switch(mode & 0xF000) {
  case 0xC000: s[0] = 's'; break;
  case 0xA000: s[0] = 'l'; break;
  case 0x8000: s[0] = '-'; break;
  case 0x6000: s[0] = 'c'; break;
  case 0x4000: s[0] = 'd'; break;
  case 0x2000: s[0] = 'b'; break;
  case 0x1000: s[0] = 'p'; break;
  default: s[0] = '?'; break;
  }
  char xc = 'x', nc = '-';
  if(mode & 0x800) {
    xc = 's';
    nc = 'S';
  }
  if(mode & 0x400) {
    xc = 's';
    nc = 'S';
  }
  if(mode & 0x200) {
    xc = 't';
    nc = 'T';
  }
  s[1] = (mode & 0x0100) ? 'r' : '-';
  s[2] = (mode & 0x0080) ? 'w' : '-';
  s[3] = (mode & 0x0040) ? xc : nc;
  s[4] = (mode & 0x0020) ? 'r' : '-';
  s[5] = (mode & 0x0010) ? 'w' : '-';
  s[6] = (mode & 0x0008) ? 'x' : '-';
  s[7] = (mode & 0x0004) ? 'r' : '-';
  s[8] = (mode & 0x0002) ? 'w' : '-';
  s[9] = (mode & 0x0001) ? 'x' : '-';
  s[10] = 0;
}

int ext2fs_init(int id) {
  ext2fs_id = id;
  memset(&e2fs_static, 0, sizeof(e2fs_static));
  e2fs_read_superblock();
  printf("e2fs initialized\n");
  printf("e2fs data:\n");
  printf("  %d inodes\n", e2fs_static.s.s_inodes_count);
  printf("  %d free\n", e2fs_static.s.s_free_inodes_count);
  printf("  %d bytes/block\n", 1024<<e2fs_static.s.s_log_block_size);
  printf("  %d blocks/group\n", e2fs_static.s.s_blocks_per_group);
  printf("  %d block groups\n", e2fs_static.s.s_blocks_count / e2fs_static.s.s_blocks_per_group);
  printf("reading BGD #0...\n");
  struct e2fs_bdesc b;
  read_bgd(0, &b);
  printf("  block map %d\n", b.bg_block_bitmap);
  printf("  inode map %d\n", b.bg_inode_bitmap);
  printf("  inode table %d\n", b.bg_inode_table);
  struct e2fs_inode i;
  e2fs_read_inode(2, &i);
  prettyprint_mode(i.i_mode, e2fs_static.mbuf);
  printf("  Root inode: mode %s (0x%x) size %d\n", e2fs_static.mbuf, i.i_mode, i.i_size);
  struct e2fs_dirent d;
  struct e2fs_inode i2;
  int offt = 0;
  getdent(&i, &d, offt);
  while(d.d_inode) {
    e2fs_read_inode(d.d_inode, &i2);
    prettyprint_mode(i2.i_mode, e2fs_static.mbuf);
    printf("  %s %d %d %d %s %d\n", e2fs_static.mbuf, d.d_inode, d.d_rec_len, d.d_name_len, d.d_name, offt);
    offt += d.d_rec_len;
    getdent(&i, &d, offt);
  }
  printf("  First free block: %d\n", get_first_free_block());
  printf("  First free inode: %d\n", get_first_free_inode());
  return 0;
}

int ext2fs_check_name(char *fname) {
  // STUB
  return (int) fname;
}

char *e2fs_path_tokenize(char *path, char **loc) {
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

int ext2fs_open(char *fname, int mode) {
  int id = _fs_getid(ext2fs_id);
  ptab[cur_ctx]->fds[id].flags = mode;
  ptab[cur_ctx]->fds[id].fname = malloc(strlen(fname));
  memcpy(ptab[cur_ctx]->fds[id].fname, fname, strlen(fname));
  char *path = malloc(strlen(fname) + 20);
  char *po = path;
  char *token;
  memcpy(path, fname, strlen(fname) + 2);
  path[strlen(fname) + 1] = 0;
  int dir_inode = 2;
  struct e2fs_inode dir;
  struct e2fs_dirent de;
  int offt = 0;
  while((token = e2fs_path_tokenize(path, &path))) {
    if(!strlen(token)) break;
    e2fs_read_inode(dir_inode, &dir);
    do {
      getdent(&dir, &de, offt);
      if(streq((char *) de.d_name, token)) break;
      offt += de.d_rec_len;
    } while(de.d_inode);
    if(!streq((char *) de.d_name, token)) de.d_inode = new_file(&dir, token);
    dir_inode = de.d_inode;
  }
  printf("[e2fs] %s is at inode %d\n", fname, dir_inode);
  struct e2fs_data *data = malloc(sizeof(struct e2fs_data));
  e2fs_read_inode(dir_inode, &data->inode);
  data->inum = dir_inode;
  ptab[cur_ctx]->fds[id].driver_data = (unsigned long) data;
  printf("[e2fs] open %s -> %d\n", fname, id);
  free(po);
  return id;
}

int ext2fs_close(int f) {
  free((void *) ptab[cur_ctx]->fds[f].driver_data);
  printf("[e2fs] close %s\n");
  return 0;
}

int e2fs_read(int f, char *c, long len) {
  struct e2fs_data *data = (struct e2fs_data *) ptab[cur_ctx]->fds[f].driver_data;
  int nread = iread(&data->inode, (unsigned char *) c, data->offset, len);
  if(nread < len) c[nread] = 255;
  data->offset += nread;
  printf("[e2fs] read %d %d -> %d\n", f, len, nread);
  return nread;
}

int e2fs_write(int f, char *c, long len) {
  struct e2fs_data *data = (struct e2fs_data *) ptab[cur_ctx]->fds[f].driver_data;
  int nwrite = iwrite(data->inum, &data->inode, (unsigned char *) c, data->offset, len);
  data->offset += nwrite;
  printf("[e2fs] write %d %d \"%s\" -> %d\n", f, len, c, nwrite);
  return nwrite;
}

struct fs_driver ext2fs_driver = {
  /* init check open close write read seek */
  ext2fs_init, ext2fs_check_name, ext2fs_open, ext2fs_close, e2fs_write, e2fs_read, 0
};
