#ifndef FS_FS_H
#define FS_FS_H

struct fs_driver {
  int (*init)(int id);
  int (*check_name)(char *fname);
  int (*open)(char *fname, int mode);
  int (*close)(int f);
  int (*write)(int f, char *c, long len);
  int (*read)(int f, char *c, long len);
  int (*seek)(int f, long off, int whence);
};

#define DRIVER_MAX 256
#define FD_MAX 256

#define O_RDONLY 1
#define O_WRONLY 2
#define O_RDWR   3

#define O_CLOEXEC   4
#define O_CREAT     8
#define O_DIRECTORY 16
#define O_EXCL      32
#define O_NOCTTY    64
#define O_NOFOLLOW  128
#define O_TMPFILE   256
#define O_TRUNC     512

int fs_init();
int fs_open(char *fname, int mode);
int fs_close(int f);
int fs_write(int f, char *c, long len);
int fs_read(int f, char *c, long len);
int fs_seek(int f, long off, int whence);

int _fs_getid(int id);

struct fd {
  char alloc;
  char flags;
  int driver;
  char *fname;
  unsigned long driver_data;
};

#endif
