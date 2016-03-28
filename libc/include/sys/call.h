#ifndef SYS_CALL_H
#define SYS_CALL_H

#define syscall1(a, b) syscall(a, b, 0, 0, 0, 0, 0)
#define syscall2(a, b, c) syscall(a, b, c, 0, 0, 0, 0)
#define syscall3(a, b, c, d) syscall(a, b, c, d, 0, 0, 0)
#define syscall4(a, b, c, d, e) syscall(a, b, c, d, e, 0, 0)
#define syscall5(a, b, c, d, e, f) syscall(a, b, c, d, e, f, 0)
#define syscall6(a, b, c, d, e, f, g) syscall(a, b, c, d, e, f, g

void syscall(int errupt, int eax, int ebx, int ecx, int edx, int esi, int edi);

#endif
