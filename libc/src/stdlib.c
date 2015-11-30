#include <stdlib.h>

void *malloc(size_t sz) {
  void *r;
  asm volatile("mov $2, %%eax\nmov %1, %%ebx\nint $0x81\nmov %%ecx, %0" : "=m" (r) : "m" (sz) : "eax", "ebx", "ecx");
  return r;
}
