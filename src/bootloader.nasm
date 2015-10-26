	[bits 16]
	[org 0]
	nop
	nop
	nop
	mov ax,ax
	jmp 0x7c0:start

start:
; set up segments
	mov ax,cs
	mov ds,ax
	mov es,ax
; enable A20
	mov ax, 0x2401
	int 0x15
; set screen mode 3h
	mov al,0x03
	mov ah,0
	int 0x10

; print a message
	mov si,msg
	call print
	
; read one sector (OS information). first byte is the number of
; sectors the OS occupies.
	mov ax,0x0100
	mov es,ax
	mov bx,0x0000
	
	mov ah,2
	mov al,1
	mov ch,0
	mov cl,2
	mov dh,0
	mov dl,0x80
	int 0x13
	jc err
	mov si,roi
	call print

load:
; load the kernel
	mov ax,0x0100
	mov es,ax
	mov al,[es:0000]
	mov ah,0x0e
	int 0x10
	
	mov dx,0x1000
	mov es,dx
	mov bx,0x0000	
	mov ax,0x0100
	mov es,ax
	mov al,[es:0000]
	mov ah,2
	mov ch,0
	mov cl,2
	mov dh,0
	mov dl,0x80
	int 0x13
	jc err
	mov si,ld
	call print
	jmp 0x1000:0000

err:
	mov cl,al
	mov al,ah
	mov ah,0x0e
	int 0x10
	mov al,cl
	mov ah,0x0e
	int 0x10
	mov ax,0x0100
	mov es,ax
	mov al,[es:0000]
	mov ah,0x0e
	int 0x10
	mov si,erm
	call print
	mov ah,0
	int 0x16
	mov al,0
	jmp load

print:
	pusha
	mov bp,sp
cont:
	lodsb
	or al,al
	jz dne
	mov ah,0x0e
	mov bx,0
	int 0x10
	jmp cont
dne:
	mov sp,bp
	popa
	ret
	
msg db "Booting...",10,13,0
roi db "Read OS information",10,13,0
erm db "Error loading sector!",10,13,"Press any key to try again",10,13,0
ld  db "Loaded OS.",10,13,0	
times 510-($-$$) db 0
db 0x55
db 0xAA
	