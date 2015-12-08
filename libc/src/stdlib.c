#include <stdlib.h>
#include <sys/call.h>

void *malloc(size_t sz) {
  return (void *) syscall(2, sz, 0, 0);
}
