; boot.nasm
; Main 2nd stage boot entry point. Called by the bootloader.
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
	mov ax, 0x10
	mov es,ax
	mov fs,ax
	mov gs,ax
	mov ss,ax
extern boot2_main
	call boot2_main
	cli
	hlt	
.hang:
	jmp .hang

section .data
global sector_offset
sector_offset:
dd 0
global gdt_address
gdt_address:
dd 0xec00