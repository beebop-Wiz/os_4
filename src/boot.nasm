; boot.nasm
; Main kernel entry point. Called by the bootloader.

; Set up our multiboot header
	MBALIGN  equ 1 << 0
	MEMINFO  equ 1 << 1
	GFXINFO  equ 1 << 2
	FLAGS    equ MBALIGN | MEMINFO | GFXINFO
	MAGIC    equ 0x1BADB002
	CHECKSUM equ -(MAGIC + FLAGS)

section .multiboot
	jmp _start
align 4
dd MAGIC
dd FLAGS
dd CHECKSUM
dd 0
dd 0
dd 0
dd 0
dd 0
dd 0
dd 0
dd 0
dd 0
	
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
.hang:
	hlt
	jmp .hang