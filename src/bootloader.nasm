	[bits 16]
	[org 0]
	jmp 0x7c0:start
start:
; set up segments
	mov ax,cs
	mov ds,ax	
; enable A20
	mov ax, 0x2401
	int 0x15
; set screen mode 0x03
	mov al,0x03
	mov ah,0
	int 0x10

; check for int 13h extensions
; no point - it won't work either way
;	mov ah, 0x41
;	mov bx, 0x55aa
;	mov dl, 0x80
;	int 0x13
;	jc err
; read one sector (OS information). first word is # of sectors, second
; word is memory address high, third word is memory address low
	mov di,1
read_chunk:
	mov word [a_nsect],1
	mov word [a_buf_lo],0x0000
	mov word [a_buf_hi],0x0100
	mov [a_lba_1],di
	inc di
	mov si,address
	mov ah, 0x42
	int 0x13
	jc err

	mov ax,0x0100
	push ds
	mov ds,ax

	mov cx,[0]
	mov ax,[2]
	mov bx,[4]
	pop ds
	test cx,cx
	jz done

	mov [a_nsect],cx
	mov [a_buf_lo],bx
	mov [a_buf_hi],ax
	mov [a_lba_1],di
	add di, cx
	
	mov ah, 0x42
	int 0x13
	jc err
	jmp read_chunk
done:
	push ax
	push bx
	; set screen mode 0x13
	mov al,0x13
	mov ah,0
	int 0x10
	cli
	lgdt [gdtr]
	mov eax, cr0
	or al, 1
	mov cr0, eax
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	mov bx, pmode
	add bx,0x7c00
	push word 0x8
	push bx
	retf
	[bits 32]
pmode:
	xor ebx, ebx
	pop bx
	mov ecx, gdt ; so the OS can use it
	add ecx, 0x7c00
	pop ax
	shl eax, 4
	add eax, ebx
	jmp eax
halt:
	cli
	hlt
	jmp halt
	[bits 16]
err:
	mov al,'E'
	mov ah,0x0e
	int 0x10
	hlt
	jmp err

address:
	a_size   db 16
	a_res    db 0
	a_nsect  dw 0
	a_buf_lo dw 0
	a_buf_hi dw 0
	a_lba_1  dw 0
	a_lba_2  dw 0
	a_lba_3  dw 0
	a_lba_4  dw 0

gdtr:
	lim dw (gdt_end - gdt - 1) + 8
	base dd 0x7c00 + gdt

gdt:
; null descriptor
dw 0
dw 0
db 0
db 0
db 0
db 0
; 0x08: kcode
dw 0xffff
dw 0
db 0
db 0x9a
db 0xcf
db 0
; 0x10: kdata
dw 0xffff
dw 0
db 0
db 0x92
db 0xcf
db 0
; 0x18: 16-bit pmode code
dw 0xffff
dw 0
db 0
db 0x9a
db 0x8f
db 0
; 0x20: 16-bit pmode data
dw 0xffff
dw 0
db 0
db 0x92
db 0x8f
db 0
; 0x28: ucode
dw 0xffff
dw 0
db 0
db 0xfa
db 0xcf
db 0
; 0x30: udata
dw 0xffff
dw 0
db 0
db 0xf2
db 0xcf
db 0
gdt_end:
dw 0