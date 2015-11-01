; boot.nasm
; Main kernel entry point. Called by the bootloader.
	
section .bootstrap_stack, nobits
align 4
stack_bottom:
resb 16384
stack_top:

section .text
global _start
_start:
	mov [sector_offset], di
	mov esp, stack_top
extern boot2_main
	call boot2_main
	cli
	hlt	
.hang:
	jmp .hang

section .data
global sector_offset
sector_offset:
	dw 0