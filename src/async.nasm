global call_usermode;(int stack, void (*func)(int, int), int id, int data)
call_usermode:
	push ebp
	mov ebp,esp

	mov ax, ds
	push ax
	mov ax, es
	push ax
	mov ax, fs
	push ax
	mov ax, gs
	push ax
	mov ax, ss
	push ax
	
	mov eax, 0x2b
	mov ds,ax
	mov es,ax
	mov fs,ax
	mov gs,ax
;	mov ss,ax

	mov eax, esp
	mov esp, [ebp + 8]
	push eax
	mov eax, [ebp + 12]

	push dword [ebp + 20]
	push dword [ebp + 16]
	call eax
	pop eax
	pop eax
	pop esp
	pop ax
	mov ss, ax
	pop ax
	mov gs, ax
	pop ax
	mov fs, ax
	pop ax
	mov es, ax
	pop ax
	mov ds, ax

	pop ebp
	ret