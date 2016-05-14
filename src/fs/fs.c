#define FS_C
#include "fs.h"
#include "../util.h"
#include "../mt.h"
#include "../vgatext.h"
#include "../log.h"

struct fs_driver *fs_drivers[DRIVER_MAX];
int ndrv = 0;

extern struct process *ptab[65536];
extern volatile int cur_ctx;

#define REGISTER_FS(fsname) do{extern struct fs_driver fsname##_driver; fs_drivers[ndrv] = &fsname##_driver; fsname##_driver.init(ndrv++);}while(0)

void register_filesystems() {
  REGISTER_FS(devfs);
}

int fs_init() {
  register_filesystems();
  return 0;
}

int fs_open(char *fname, int mode) {
  int i;
  for(i = 0; i < ndrv; i++) {
    if(fs_drivers[i]->check_name(fname)) return fs_drivers[i]->open(fname, mode);
  }
  return -1;
}

int fs_close(int id) {
  return fs_drivers[ptab[cur_ctx]->fds[id].driver]->close(id);
}

int fs_write(int id, char *s, long len) {
  return fs_drivers[ptab[cur_ctx]->fds[id].driver]->write(id, s, len);
}

int fs_read(int id, char *s, long len) {
  log(LOG_SYSCALL, LOG_INFO, "fs_read(%d, %x, %d)\n", id, s, len);
  return fs_drivers[ptab[cur_ctx]->fds[id].driver]->read(id, s, len);
}

int _fs_getid(int id) {
  int i;
  for(i = 0; i < FD_MAX && ptab[cur_ctx]->fds[i].alloc; i++) ;
  if(i > FD_MAX) return -1;
  ptab[cur_ctx]->fds[i].alloc = 1;
  ptab[cur_ctx]->fds[i].driver = id;
  return i;
}
