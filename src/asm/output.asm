_output:
    .puts:
    push eax
    .puts_begin:
    mov al,[bp+di]
    test al,0xFF
    jz .puts_end
    mov [cur_char],al    
    call .putc    
    inc di
    inc byte [col]
    jmp .puts_begin
    .puts_end:
    pop eax
    ret

    .putc:
    push eax
    push ebx
    push ecx
    xor eax,eax
    mov ax,[row]
    mov cl,[width]
    mul cl
    mov bx,[col]
    and bx,0xff
    add ax,bx
    shl eax,1
    xor ebx,ebx
    mov ebx,video_cache
    add ebx,eax    
    mov cl,[cur_char]
    mov ch,[cur_attr]
    mov [ebx],cl
    mov [ebx+1],ch
    pop ecx
    pop ebx
    pop eax
    ret

    .disable_cursor:
	pushf
	push eax
	push edx
 
	mov dx, 0x3D4
	mov al, 0xA	; low cursor shape register
	out dx, al
 
	inc dx
	mov al, 0x20	; bits 6-7 unused, bit 5 disables the cursor, bits 0-4 control the cursor shape
	out dx, al
 
	pop edx
	pop eax
	popf
	ret


    video_cache equ 0xb8000
    width db 80
    height db 25
    col db 0
    row db 0
    cur_char db 0
    cur_attr db 0x0f