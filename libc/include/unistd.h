#ifndef UNISTD_H
#define UNISTD_H

#define _POSIX_VERSION 0 // 200809L
#define _POSIX2_VERSION -1

#include <depend/stddef_null.h>

#define F_OK 0
#define R_OK 1
#define W_OK 2
#define X_OK 4

#define F_LOCK 0
#define F_TEST 1
#define F_TLOCK 2
#define F_ULOCK 3

#define STDERR_FILENO 2
#define STDIN_FILENO 0
#define STDOUT_FILENO 1

#include <depend/types_pid_t.h>

pid_t fork(void);

#endif

