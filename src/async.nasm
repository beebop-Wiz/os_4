global call_usermode;(struct regs *r, int id, int data)
call_usermode:
	push ebp
	mov ebp, esp
	push ebx
	
	mov eax, [ebp+8]	;eax is address of structure
	mov ebx, esp		;save current stack pointer
	mov esp, [eax+60]
	push dword [eax+56]	;eflags
	push dword 0x2b		;cs
	push dword [eax+48]	;eip
	
	push dword [eax+8]	;edi
	push dword [eax+12]	;esi
	push dword [eax+16]	;ebp
	push dword [eax+24]	;ebx
	push dword [eax+28]	;edx
	push dword [eax+32]	;ecx
	push dword [eax+36]	;eax
	push word [eax]		;gs
	push word [eax+2]	;fs
	push word [eax+4]	;es
	push word [eax+6]	;ds

; now build our stack frame
	
	push dword [ebp+16]
	push dword [ebp+12]
	push dword ucall_end
	push ebp

	add esp, 4
	mov [eax+60], esp
	
	mov esp, ebx
	pop ebx
	pop ebp
	ret
	
ucall_end:
	add esp, 8
	pop ax
	mov ds, ax
	pop ax
	mov es, ax
	pop ax
	mov fs, ax
	pop ax
	mov gs, ax
	pop eax
	pop ecx
	pop edx
	pop ebx
	pop ebp
	pop esi
	pop edi

	iret
	
