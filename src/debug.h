#ifndef DEBUG_H
#define DEBUG_H

/*

#ifdef DEBUG_whatever
#define printd(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define printd(fmt, ...)
#endif

*/

#define DEBUG_MALLOC
#undef DEBUG_PAGING
#undef DEBUG_PAGING_MEMORY
#undef DEBUG_MT

int dummy_print(char *f, ...);

#endif
