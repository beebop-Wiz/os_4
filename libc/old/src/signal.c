#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/call.h>

void dfl_term(int s) {
  printf("Killed by signal %d\n", s);
  exit(255);
}

void dfl_stop() {
  syscall(9, 1, 0, 0);
}

void (*signal_handlers[N_SIGNALS + 1])(int) = {
  0,
  dfl_term, 			/* SIGABRT */
  dfl_term,			/* SIGALRM */
  dfl_term,			/* SIGBUS */
  0,				/* SIGCHLD */
  0,				/* SIGCONT, handled by kernel */
  dfl_term,			/* SIGFPE */
  dfl_term,			/* SIGHUP */
  dfl_term,			/* SIGILL */
  dfl_term,			/* SIGINT */
  dfl_term,			/* SIGKILL */
  dfl_term,			/* SIGPIPE */
  dfl_term,			/* SIGQUIT */
  dfl_term,			/* SIGSEGV */
  dfl_stop,			/* SIGSTOP */
  dfl_term,			/* SIGTERM */
  dfl_stop,			/* SIGTSTP */
  dfl_stop,			/* SIGTTIN */
  dfl_stop,			/* SIGTTOU */
  dfl_term,			/* SIGUSR1 */
  dfl_term,			/* SIGUSR2 */
  dfl_term,			/* SIGTRAP */
  0,				/* SIGURG */
  dfl_term,			/* SIGXCPU */
  dfl_term,			/* SIGXFSZ */
};

void (*const dfl_handlers[N_SIGNALS + 1])(int) = {
  0,
  dfl_term, 			/* SIGABRT */
  dfl_term,			/* SIGALRM */
  dfl_term,			/* SIGBUS */
  0,				/* SIGCHLD */
  0,				/* SIGCONT, handled by kernel */
  dfl_term,			/* SIGFPE */
  dfl_term,			/* SIGHUP */
  dfl_term,			/* SIGILL */
  dfl_term,			/* SIGINT */
  dfl_term,			/* SIGKILL */
  dfl_term,			/* SIGPIPE */
  dfl_term,			/* SIGQUIT */
  dfl_term,			/* SIGSEGV */
  dfl_stop,			/* SIGSTOP */
  dfl_term,			/* SIGTERM */
  dfl_stop,			/* SIGTSTP */
  dfl_stop,			/* SIGTTIN */
  dfl_stop,			/* SIGTTOU */
  dfl_term,			/* SIGUSR1 */
  dfl_term,			/* SIGUSR2 */
  dfl_term,			/* SIGTRAP */
  0,				/* SIGURG */
  dfl_term,			/* SIGXCPU */
  dfl_term,			/* SIGXFSZ */
};

void sig_handler(unsigned int id, unsigned int data) {
  if(signal_handlers[data])
    signal_handlers[data](data);
}

int sigaction(int signum, const struct sigaction *act, struct sigaction *old) {
  if(signum == SIGCONT || signum == SIGKILL || signum > N_SIGNALS) return SIG_ERR;
  if(act->sa_handler == (void (*)(int)) SIG_DFL) signal_handlers[signum] = dfl_handlers[signum];
  else signal_handlers[signum] = act->sa_handler;
  return 0;
}

void setup_signals() {
  syscall(10, 2, (int) sig_handler, 0);
}

int kill(pid_t pid, int sig) {
  return syscall(12, pid, sig, 0);
}
