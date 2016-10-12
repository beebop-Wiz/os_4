#ifndef SIGNAL_H
#define SIGNAL_H

#define SIGABRT 1
#define SIGALRM 2
#define SIGBUS 3
#define SIGCHLD 4
#define SIGCONT 5
#define SIGFPE 6
#define SIGHUP 7
#define SIGILL 8
#define SIGINT 9
#define SIGKILL 10
#define SIGPIPE 11
#define SIGQUIT 12
#define SIGSEGV 13
#define SIGSTOP 14
#define SIGTERM 15
#define SIGTSTP 16
#define SIGTTIN 17
#define SIGTTOU 18
#define SIGUSR1 19
#define SIGUSR2 20
#define SIGTRAP 21
#define SIGURG  22
#define SIGXCPU 23
#define SIGXFSZ 24

#define N_SIGNALS 24

#define SIG_DFL -2
#define SIG_IGN 0
#define SIG_ERR -1

typedef unsigned int sigset_t;

union sigval {
  int sival_int;
  void *sival_ptr;
};

#include <depend/types_pid_t.h>

struct __siginfo {
  int si_signo;
  int si_code;
  pid_t si_pid;
  void *si_addr;
  int si_status;
  union sigval si_value;
};

typedef struct __siginfo siginfo_t;

struct sigaction {
  void (*sa_handler)(int);
  sigset_t sa_mask;
  int sa_flags;
  void (*sa_sigaction)(int, siginfo_t *, void *);
};

int sigaction(int signum, const struct sigaction *act, struct sigaction *old);
int kill(pid_t pid, int sig);
#endif
