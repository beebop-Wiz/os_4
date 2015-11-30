#include <stdio.h>
#include <unistd.h>

int main(void) {
  char *str = "INIT v0.1 loaded\n";
  fork();
  printf("%s\n", str);
  //  asm volatile("mov $0, %%eax\nmov %0, %%ebx\nint $0x81" : : "m" (str) : "eax", "ebx");
  return 0;
}
