section .rmcode
	[bits 32]
global rmregs
rmregs:
dw 0 ; 0 - ax
dw 0 ; 2 - bx
dw 0 ; 4 - cx
dw 0 ; 6 - dx
dw 0 ; 8 - si
dw 0 ; 10 - di
dw 0 ; 12 - sp
dw 0 ; 14 - bp
dw 0 ; 16 - ds
dw 0 ; 18 - es
dw 0 ; 20 - fs
dw 0 ; 22 - gs
db 0 ; 24 - int no
global pmode_to_real
pmode_to_real:
	mov [ssp],esp
	mov [sbp],ebp
	jmp 0x18:entry_16

	[bits 16]
idt_real:
dw 0x3ff
dd 0
scr:
dd 0
ssp:
dd 0
sbp:
dd 0	
entry_16:
	cli
	mov ax,0x20
	mov ds,ax
	mov es,ax
	mov fs,ax
	mov gs,ax
	mov ss,ax
	mov eax,cr0
	mov [scr],eax
	and eax, 0xFFFFFFFE
	mov cr0,eax
	jmp 0:goreal
goreal:
	mov sp,0x8000
	mov ax,0
	mov ds,ax
	mov es,ax
	mov fs,ax
	mov gs,ax
	mov ss,ax
	lidt [idt_real]
	push ds
	pusha
	mov ax,[rmregs+0]
	mov bx,[rmregs+2]
;       mov cx,[rmregs+4]
	mov dx,[rmregs+6]
	mov si,[rmregs+8]
	mov di,[rmregs+10]
	mov cx,[rmregs+18]
	mov es,cx
	xor cx,cx
	mov cl,[rmregs+24]
	cmp cx,0x10
	je int_10
	cmp cx,0x13
	je int_13
	jmp ie
int_10:
	mov cx,[rmregs+16]
	mov ds,cx
	mov cx,[rmregs+4]
	sti
	int 0x10
	cli
	jmp ie
int_13:
	mov cx,[rmregs+16]
	mov ds,cx
	mov cx,[rmregs+4]
	sti
	int 0x13
	cli
	jmp ie
ie:
	mov [rmregs],ax
	mov [rmregs+2],bx
	mov [rmregs+4],cx
	mov [rmregs+6],dx
	popa
	pop ds
	mov eax,[scr]
	mov cr0,eax
	mov eax,0x10
	mov ds,ax
	mov es,ax
	mov fs,ax
	mov gs,ax
	mov ss,ax
	mov ebx,pmode
	push word 0x8
	push bx
	retf
	[bits 32]
pmode:
	mov esp,[ssp]
	mov ebp,[sbp]
	ret

section .rmdata nobits
global vbeinfo
vbeinfo:
resb 512
global vbemode
vbemode:
resb 256
global rmsector
rmsector:
resb 10
	
	