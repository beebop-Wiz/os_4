extern c_intr
global load_idt
align 4
load_idt:
	mov eax, [esp + 4]
	lidt [eax]
	ret

%macro isr_noerr 1
global int_%1
	int_%1:
	cli
	push byte 0
	push byte %1
	jmp asm_intr
%endmacro
%macro isr_err 1
global int_%1
	int_%1:
	cli
	push byte %1
	jmp asm_intr
%endmacro

	isr_noerr 0
	isr_noerr 1
	isr_noerr 2
	isr_noerr 3
	isr_noerr 4
	isr_noerr 5
	isr_noerr 6
	isr_noerr 7
	isr_err 8
	isr_noerr 9
	isr_err 10
	isr_err 11
	isr_err 12
	isr_err 13
	isr_err 14
	isr_noerr 15
	isr_noerr 16
	isr_noerr 17
	isr_noerr 18
	
asm_intr:
	pusha
	mov ax, ds
	push ax
	mov ax, 0x10 ; 0x10 = kdata
	mov ds,ax
	mov es,ax
	mov fs,ax
	mov gs,ax
	call c_intr
	pop ax
	mov ds,ax
	mov es,ax
	mov fs,ax
	mov gs,ax
	popa
	add esp,8
	sti
	iret