extern main
extern init_stdio
global _start
_start:
	mov [environ], esi
	mov [argc], eax
	mov [argv], ebx
	call init_stdio
	push dword [argv]
	push dword [argc]
	call main
_start_exit:
	mov ebx, eax
	mov eax, 1
	int 0x80
	jmp _start_exit ; didn't work, try again

section .bss
environ: resd 1
argc: resd 1
argv: resd 1