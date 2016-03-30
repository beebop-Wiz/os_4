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
	mov ax,0x03
	int 0x10

; read one sector (OS information). first word is # of sectors, second
; word is memory address low, third word is memory address high
	mov di,1
read_chunk:
	mov word [a_size + 2],1
	mov dword [a_size + 4],0x09c00000
	mov [a_size + 8],di
	inc di
	mov si,address
	mov ah, 0x42
	int 0x13
	jc err

	mov eax,[0x2002]
	mov [a_size + 4], eax
	mov ax,[0x2000]
	mov [a_size + 2],ax
	test ax,ax
	jz done

	mov [a_size + 8],di
	add di, ax
	
	mov ah, 0x42
	int 0x13
	jc err
	jmp read_chunk
done:
	push dword [a_size + 4]
	; set screen mode 0x13
	mov al,0x13
	mov ah,0
	int 0x10
	cli	
	lgdt [gdtr]
	pop bx
	pop ax
	mov edx, cr0
	or dl, 1
	mov cr0, edx
	mov cx, 0x10
	mov ds, cx
	push word 0x8
	push pmode+0x7c00
	retf
	[bits 32]
pmode:
	shl eax, 4
	add eax, ebx
	pusha
	call demo
	popa
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
	jmp err

gdtr:
	lim dw 512
	base dd 0xec00

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

	[bits 32]

	; graphics code begins here (0xad)
demo:
	mov ch, 0xfa
wl:
	mov byte [0xA0000 + ecx], 8
	loop wl
	
	mov cl, 24
	shl ecx, cl
l:
	dec ecx
	cmp ecx,300000000
	je d1
	cmp ecx,200000000
	je d2
	
dr:
	test ecx, ecx
	jnz l
	ret

d1:
	push ecx
	mov eax, 320
d1l2:
	mov bx, 200
d1l3:
	lea ecx, [eax - 120]
	imul ecx, ecx
	lea edx, [ebx - 100]
	imul edx, edx
	add ecx, edx
	cmp ecx, 2500
	jg nsp
	call csetpix
nsp:
	dec ebx
	jnz d1l3
	dec eax
	jnz d1l2
	pop ecx
	jmp dr

csetpix:
	imul esi, ebx, 80
	mov byte [eax + esi*4 + 0xA0028], 15
	ret
	
d2:
	push ecx
	xor eax, eax
d2l1:
	xor ebx, ebx
d2l2:
	mov edx, eax
	shr edx, 3
	mov ch, [edx + ebx*4 + img + 0x7c00]
	mov cl, al
	and cl, 7
	mov dl, 1
	shl dl, cl
	test ch, dl
	jnz csetpix2
cs2r:
	inc ebx
	cmp ebx, 16
	jl d2l2
	inc eax
	cmp eax, 32
	jl d2l1
	pop ecx
	jmp dr
	
csetpix2:
	imul esi, ebx, 80
	mov byte [eax + esi*4 + 0xa7110], 0
	jmp cs2r
img:
dq 4035225266929270784
dq 3891178515063635968
dq 9151330262037062499
dq 3458873791478773859
dq 71776119866539582
dq 142410876333326336
dq 27287610864972592
dq 100756977832964912
