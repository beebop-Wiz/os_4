#include <unistd.h>

pid_t fork(void) {
  pid_t r;
  asm volatile("mov $5, %%eax\nint $0x81\nmov %%cx, %0" : "=m" (r) : : "eax", "cx");
  return r;
}
