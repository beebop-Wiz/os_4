#define STDIO_C
#include <stdio.h>
#include <sys/call.h>
#include <string.h>

void init_stdio(void) {
  
}

int printf(char *fmt, ...) {
  syscall4(0x80, 4, 1, (int) fmt, strlen(fmt));
  return 0;
}
