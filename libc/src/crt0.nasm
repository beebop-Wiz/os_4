extern main
extern init_stdio
global _start
_start:
	call init_stdio
	call main
	mov eax, 1
	int 0x81
