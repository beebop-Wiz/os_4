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
	mov esp, stack_top
extern kernel_main
	call kernel_main
	cli
	hlt	
.hang:
	jmp .hang