#include <sys/call.h>

int syscall(int errupt, int eax, int ebx, int ecx, int edx, int esi, int edi) {
  int retval = 0;
  if(errupt == 0x80)
    asm volatile ("mov %[eax], %%eax\n\tmov %[ebx], %%ebx\n\tmov %[ecx], %%ecx\n\tmov %[edx], %%edx\n\tmov %[esi], %%esi\n\tmov %[edi], %%edi\n\tint $0x80\n\t mov %%eax, %[ret]" : [ret] "=m" (retval) : [eax] "m" (eax), [ebx] "m" (ebx), [ecx] "m" (ecx), [edx] "m" (edx), [esi] "m" (esi), [edi] "m" (edi) : "eax", "ebx", "ecx", "edx", "esi", "edi");
  else
    asm volatile ("mov %[eax], %%eax\n\tmov %[ebx], %%ebx\n\tmov %[ecx], %%ecx\n\tmov %[edx], %%edx\n\tmov %[esi], %%esi\n\tmov %[edi], %%edi\n\tint $0x81" : : [eax] "m" (eax), [ebx] "m" (ebx), [ecx] "m" (ecx), [edx] "m" (edx), [esi] "m" (esi), [edi] "m" (edi) : "eax", "ebx", "ecx", "edx", "esi", "edi");
  return retval;
}
