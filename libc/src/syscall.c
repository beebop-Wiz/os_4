// eax = syscall no
// ebx = arg 1
// ecx = ret val
// edx = arg 2
// esi = arg 3

unsigned int syscall(int no, int a1, int a2, int a3) {
  unsigned int r;
  asm volatile("mov %1, %%eax\nmov %2, %%ebx\nmov %3, %%edx\nmov %4, %%esi\nint $0x81\nmov %%ecx, %0"
	       : "=m" (r) : "m" (no), "m" (a1), "m" (a2), "m" (a3) : "eax", "ebx", "ecx", "edx", "esi");
  return r;
}
