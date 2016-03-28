#ifndef FCNTL_H
#define FCNTL_H

#define F_DUPFD 1
#define F_DUPFD_CLOEXEC 2
#define F_GETFD 3
#define F_SETFD 4
#define F_GETFL 5
#define F_SETFL 6
#define F_GETLK 7
#define F_SETLK 8
#define F_SETLKW 9
#define F_GETOWN 10
#define F_SETOWN 11

#define FD_CLOEXEC 1

#define F_RDLCK 1
#define F_UNLCK 2
#define F_WRLCK 3

#include <depend/stdio_seek.h>

#define O_CLOEXEC   0x01
#define O_CREAT     0x02
#define O_DIRECTORY 0x04
#define O_EXCL      0x08
#define O_NOCTTY    0x10
#define O_NOFOLLOW  0x20
#define O_TRUNC     0x40
#define O_TTY_INIT  0

#define O_APPEND 1
#define O_DSYNC 2
#define O_NONBLOCK 3
#define O_RSYNC 4
#define O_SYNC 5

#define O_ACCMODE 0 // what should this be?

#define O_EXEC 1
#define O_RDONLY 2
#define O_RDWR 3
#define O_SEARCH O_EXEC
#define O_WRONLY 4

// todo

//int  creat(const char *, mode_t);
int  fcntl(int, int, ...);
int  open(const char *, int, ...);
int  openat(int, const char *, int, ...);

#endif
