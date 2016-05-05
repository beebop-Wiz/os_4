	[bits 16]
	[org 0]
; Main entry point. First, get CS set properly.
	jmp 0x7c0:start
start:
; set up segments
	mov ax,cs
	mov ds,ax
; enable A20 using BIOS interrupt
	mov ax, 0x2401
	int 0x15
; set screen mode 0x03
;	mov ax,0x03
;	int 0x10

; read one sector (OS information). first word is # of sectors, second
; word is memory address low, third word is memory address high
; Register map:
; di is the current LBA.
; si is the BIOS data structure pointer.
; On exit: ax will contain 0, and a_buf_lo will contain the linear
; address of boot2_main();
	
	mov di,1
read_chunk:
	mov word [a_nsect],1
	mov dword [a_buf_lo],0x09c00000 ; note that this will set a_buf_hi as well. Our buffer will be at 0x07c0:0x2000.
	mov [a_lba_1],di
	inc di
	mov si,address
	mov ah, 0x42
	int 0x13
	jc err

	mov eax,[0x2002]        ; eax contains full address
	mov [a_buf_lo], eax	; as before, sets both
	mov ax,[0x2000]         ; ax contains next sector count
	mov [a_nsect],ax
	test ax,ax
	jz done

	mov [a_lba_1],di        ; set up for actual data read
	add di, ax
	
	mov ah, 0x42
	int 0x13
	jc err
	jmp read_chunk
done:
	push dword [a_buf_lo]	; pushes both lo and hi. will pop later...
	; set screen mode 0x13
	mov al,0x13
	mov ah,0
	int 0x10
	cli	
	lgdt [gdtr]             ; our GDT should have been loaded as part of the 2nd stage bootloader
	pop bx                  ; bx is boot2_main offset
	pop ax                  ; ax is boot2_main segment
	mov edx, cr0            ; enable 32-bit segments
	or dl, 1
	mov cr0, edx
	mov cx, 0x10            ; load seg 0x10 (32-bit kernel data)
	mov ds, cx
	push word 0x8           ; cs
	push pmode+0x7c00       ; eip
	retf                    ; jump to protected mode!
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
	a_size   db 16 ; 0
	a_res    db 0  ; 1
	a_nsect  dw 0  ; 2
	a_buf_lo dw 0  ; 4
	a_buf_hi dw 0  ; 6
	a_lba_1  dw 0  ; 8
	a_lba_2  dw 0  ; 10
	a_lba_3  dw 0  ; 12
	a_lba_4  dw 0  ; 14

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
