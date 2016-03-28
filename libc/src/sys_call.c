#include <sys/call.h>

void syscall(int errupt, int eax, int ebx, int ecx, int edx, int esi, int edi) {
  if(errupt == 0x80)
    asm volatile ("mov %[eax], %%eax\n\tmov %[ebx], %%ebx\n\tmov %[ecx], %%ecx\n\tmov %[edx], %%edx\n\tmov %[esi], %%esi\n\tmov %[edi], %%edi\n\tint $0x80" : : [eax] "m" (eax), [ebx] "m" (ebx), [ecx] "m" (ecx), [edx] "m" (edx), [esi] "m" (esi), [edi] "m" (edi) : "eax", "ebx", "ecx", "edx", "esi", "edi");
  else
    asm volatile ("mov %[eax], %%eax\n\tmov %[ebx], %%ebx\n\tmov %[ecx], %%ecx\n\tmov %[edx], %%edx\n\tmov %[esi], %%esi\n\tmov %[edi], %%edi\n\tint $0x81" : : [eax] "m" (eax), [ebx] "m" (ebx), [ecx] "m" (ecx), [edx] "m" (edx), [esi] "m" (esi), [edi] "m" (edi) : "eax", "ebx", "ecx", "edx", "esi", "edi");
  
}
