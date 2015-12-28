extern main
extern init_stdio
extern alloc_cwd_buf
global _start
_start:
	call alloc_cwd_buf
	call init_stdio
	call main
	mov eax, 1
	int 0x81
