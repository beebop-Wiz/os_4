#ifndef DEBUG_H
#define DEBUG_H

/*

#ifdef DEBUG_whatever
#define printd(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define printd(fmt, ...)
#endif

*/

#undef DEBUG_MALLOC
#undef DEBUG_PAGING
#undef DEBUG_PAGING_MEMORY
#undef DEBUG_MT
#undef DEBUG_SYSCALL

int dummy_print(char *f, ...);

#endif
