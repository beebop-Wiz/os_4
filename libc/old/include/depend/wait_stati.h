#ifndef WAIT_STATI_H
#define WAIT_STATI_H

#define WCONTINUED 0x10000
#define WNOHANG    0x20000
#define WUNTRACED  0x40000

#define WIFEXITED(stat_val) (((stat_val) & 0xFF00) == 0x0100)
#define WIFSIGNALED(stat_val) (((stat_val) & 0xFF00) == 0x0200)
#define WIFSTOPPED(stat_val) (((stat_val) & 0xFF00) == 0x0300)

#define WEXITSTATUS(stat_val) ((stat_val) & 0xFF)
#define WTERMSIG(stat_val) ((stat_val) & 0xFF)
#define WSTOPSIG(stat_val) ((stat_val) & 0xFF)

#endif
