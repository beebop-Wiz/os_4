extern main
extern init_stdio
extern alloc_cwd_buf
extern environ
global _start
_start:
	mov [environ], esi
	mov [argc], eax
	mov [argv], ebx
	call alloc_cwd_buf
	call init_stdio
	push dword [argv]
	push dword [argc]
	call main
	mov eax, 1
	int 0x81

section .bss
argc: resd 1
argv: resd 1