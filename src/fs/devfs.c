#include "fs.h"
#include "../util.h"
#include "../malloc.h"
#include "../vgatext.h"
#include "../keyboard.h"
#include "../mt.h"

static int devfs_id;
extern struct process *ptab[65536];
extern volatile int cur_ctx;


#define TYPE_STDIO 1

int devfs_init(int id) {
  devfs_id = id;
  return 0;
}

int devfs_check_name(char *fname) {
  char nbuf[6];
  memcpy(nbuf, fname, 5);
  nbuf[5] = 0;
  return streq(nbuf, "/dev/");
}

int devfs_open(char *fname, int mode) {
  int id = _fs_getid(devfs_id);
  ptab[cur_ctx]->fds[id].flags = mode;
  ptab[cur_ctx]->fds[id].fname = malloc(strlen(fname));
  memcpy(ptab[cur_ctx]->fds[id].fname, fname, strlen(fname));
  if(streq(fname, "/dev/stdout") || streq(fname, "/dev/stdin")) {
    ptab[cur_ctx]->fds[id].driver_data = TYPE_STDIO;
  }   
  return id;
}

int devfs_close(int id) {
  free(ptab[cur_ctx]->fds[id].fname);
  ptab[cur_ctx]->fds[id].alloc = 0;
  return 0;
}

int devfs_write(int f, char *c, long len) {
  int i;
  for(i = 0; i < len; i++) {
    if(ptab[cur_ctx]->fds[f].driver_data == TYPE_STDIO) {
      vga_putchar(c[i]);
    }
  }
  vga_redraw();
  return len;
}

extern unsigned char *kbdbuf;
extern volatile int kbreadp, kbwritep;

int devfs_read(int f, char *s, long len) {
  int i;
  unsigned char c;
  for(i = 0; i < len; i++) {
    if(ptab[cur_ctx]->fds[f].driver_data == TYPE_STDIO) {
      asm volatile("sti");
      while(kbreadp == kbwritep) ;
      c = kbdbuf[kbreadp++];
      if(kbreadp > KBD_BUFSIZ) kbreadp = 0;
      if(c == 255) break; // This is separate from the \n test for a
			  // reason - we should write the newline to
			  // the string but not the EOF.
      s[i] = c;
      if(c == '\n') { i++; break; }
      if(c == '\b') { i -= 2; if(i < -1) i = -1;} // will be set to 0 in the next loop iteration
      asm volatile("cli");
    }
  }
  s[i] = 0;
  return i;
}

struct fs_driver devfs_driver = {
  devfs_init, devfs_check_name, devfs_open, devfs_close, devfs_write, devfs_read, 0
};
