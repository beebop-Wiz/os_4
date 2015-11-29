global tss_flush
tss_flush:
	mov ax, 0x3b
	ltr ax
	ret

global jump_usermode
extern ring3_main
jump_usermode:
	call tss_flush
	mov ax,0x33
	mov ds,ax
	mov es,ax
	mov fs,ax
	mov gs,ax
	mov eax,esp
	push 0x33
	push eax
	pushf
	push 0x2b
	push ring3_main
	iret