extern c_intr
global load_idt
align 4
load_idt:
	mov eax, [esp + 4]
	lidt [eax]
	sti
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

	isr_noerr 32
	isr_noerr 33
	isr_noerr 34
	isr_noerr 35
	isr_noerr 36
	isr_noerr 37
	isr_noerr 38
	isr_noerr 39
	isr_noerr 40
	isr_noerr 41
	isr_noerr 42
	isr_noerr 43
	isr_noerr 44
	isr_noerr 45
	isr_noerr 46
	isr_noerr 47
	
	isr_noerr 129
	
asm_intr:
	pusha
	mov ax, ds
	push ax
	mov ax, es
	push ax
	mov ax, fs
	push ax
	mov ax, gs
	push ax
	mov ax, 0x10 ; 0x10 = kdata
	mov ds,ax
	mov es,ax
	mov fs,ax
	mov gs,ax
	push esp
	call c_intr
	pop eax
	pop ax
	movzx eax, ax
	mov ds,ax
	pop ax
	mov es,ax
	pop ax
	mov fs,ax
	pop ax
	mov gs,ax
	popa
	add esp,8
	sti
	iret