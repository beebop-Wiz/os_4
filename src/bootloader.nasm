	[bits 16]
	[org 0]
%macro debugb 1
	push ax
	mov al,%1
	mov ah,0x0e
	int 0x10
	pop ax
%endmacro

%macro debugw 1 ; prints little-endian
	push ax
	mov ax,%1
	mov ah,0x0e
	int 0x10
	mov ax,%1
	mov al,ah
	mov ah,0x0e
	int 0x10
	pop ax
%endmacro
	
	jmp 0x7c0:start

start:
; set up segments
	mov ax,cs
	mov ds,ax
	mov es,ax
	
; enable A20
	mov ax, 0x2401
	int 0x15
; set screen mode 0x03
	mov al,0x03
	mov ah,0
	int 0x10

; check for int 13h extensions
	mov ah, 0x41
	mov bx, 0x55aa
	mov dl, 0x80
	int 0x13
	jc err
	mov al,ah
	mov ah,0x0e
	int 0x10
	add cx,0x20
	mov al,cl
	mov ah,0x0e
	int 0x10
	jmp ssm
err:
	mov al,'E'
	mov ah,0x0e
	int 0x10
	hlt
	jmp err
ssm:
; read one sector (OS information). first word is # of sectors, second
; word is memory address high, third word is memory address low
	mov di,1
read_chunk:
	debugw 'o'
	debugw di
	mov ax,cs
	mov ds,ax
	mov bx,0x0000
	mov byte [a_size],16
	mov byte [a_res],0
	mov word [a_nsect],1
	mov word [a_buf_lo],0x0000
	mov word [a_buf_hi],0x0100
	mov [a_lba_1],di
	inc di
	mov word [a_lba_2],0
	mov word [a_lba_3],0
	mov word [a_lba_4],0
	mov si,address
	mov dl,0x80
	mov ah, 0x42
	int 0x13
	jc err

	mov ax,0x0100
	mov fs,ax

	mov cx,[fs:0]
	mov bx,[fs:2]
	mov ax,[fs:4]
	debugb 'c'
	debugw cx
	debugb 'h'
	debugw ax
	debugb 'l'
	debugw bx
	test cx,cx
	jz done

	mov byte [a_size],16
	mov byte [a_res],0
	mov [a_nsect],cx
	mov [a_buf_lo],bx
	mov [a_buf_hi],ax
	mov [a_lba_1],di
	mov word [a_lba_2],0
	mov word [a_lba_3],0
	mov word [a_lba_4],0
	add di, cx
	mov si,address
	mov dl,0x80
	
	mov ah, 0x42
	int 0x13

	jc err

	mov al,'C'
	mov ah,0x0e
	int 0x10
	jmp read_chunk
done:
	mov ax, 0x0e00
	mov al, 'F'
	int 0x10
halt:
	hlt
	jmp halt

align 4
address:
	a_size   db 0
	a_res    db 0
	a_nsect  dw 0
	a_buf_lo dw 0
	a_buf_hi dw 0
	a_lba_1  dw 0
	a_lba_2  dw 0
	a_lba_3  dw 0
	a_lba_4  dw 0
	
times 510-($-$$) db 0
db 0x55
db 0xAA
