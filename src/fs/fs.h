#ifndef FS_FS_H
#define FS_FS_H

#define REGISTER_FS(fsname)

struct fs_driver {
  int (*write)(char *fname, char *c, long len);
  int (*read)(char *fname, char *c, long len);
  int (*seek)(char *fname, long off, int whence);
};

#endif
